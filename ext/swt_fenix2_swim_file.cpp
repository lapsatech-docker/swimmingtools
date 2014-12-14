#include "swt_fenix2_swim_file.h"
#include <cmath>
#include <algorithm>
#include <fstream>
#include "fit_encode.hpp"

void swt::Fenix2SwimFile::AddMesg(const void *mesg) {

  // Save first record as a template and  bookmark for where to
  // insert recordi mesgs in Save,
  const fit::Mesg *fit_mesg = reinterpret_cast<const fit::Mesg*>(mesg);
  if (fit_mesg->GetNum() == FIT_MESG_NUM_RECORD && record_ == nullptr) {
    std::unique_ptr<fit::RecordMesg> record(new fit::RecordMesg(*fit_mesg));
    record_ = record.get();
    mesgs_.push_back(move(record));
  }

  SwimFile::AddMesg(mesg);
}

void swt::Fenix2SwimFile::Initialize() {

  RepairPoolLength();
  RepairLapNumLengths();

  for (fit::LengthMesg * length : lengths_) {
    if (length->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE) {
      length->SetAvgSpeed(session_->GetPoolLength() / length->GetTotalTimerTime());
      length->SetAvgSwimmingCadence(
          static_cast<FIT_UINT8>(round(length->GetTotalStrokes() / length->GetTotalTimerTime() * 60)));
    }
  }
  Recalculate();
}


void swt::Fenix2SwimFile::Delete(FIT_MESSAGE_INDEX length_index) {
  std::string error;

  if (!CanSplitChangeStrokeDelete(length_index, &error))
    throw std::runtime_error(error);

  fit::LengthMesg *length = lengths_.at(length_index);
  length->SetAvgSpeed(0);
  length->SetAvgSwimmingCadence(0);
  length->SetEventType(FIT_EVENT_TYPE_MARKER);
  length->SetLengthType(FIT_LENGTH_TYPE_IDLE);
  length->SetSwimStroke(FIT_SWIM_STROKE_INVALID);
  length->SetTotalStrokes(0);

  UpdateLap(GetLap(length_index));
  UpdateSession();
}

// This is require because as of firmware 3.1, the num_lengths field of lap
// containing rest lengths (when the user rest without pressing the lap
// button) doesn't include those length 
void swt::Fenix2SwimFile::RepairLapNumLengths() {

  if (laps_.size() > 1 && lengths_.size() > 0) {
    std::size_t nbr_laps = laps_.size();
    for (std::size_t i = 0; i < nbr_laps - 1; ++i) {
      fit::LapMesg *lap = laps_[i];
      fit::LapMesg *next_lap = laps_[i+1];

      if (lap->GetFirstLengthIndex() == FIT_UINT16_INVALID || 
          next_lap->GetFirstLengthIndex() == FIT_UINT16_INVALID) {
        throw std::runtime_error("First length index is invalid"); 
      }

      lap->SetNumLengths(next_lap->GetFirstLengthIndex() - lap->GetFirstLengthIndex());
    }
  }

  if (laps_.size() > 0) {
    fit::LapMesg *last_lap = laps_.back();
    laps_.back()->SetNumLengths(lengths_.size() - last_lap->GetFirstLengthIndex());
  }
}

void swt::Fenix2SwimFile::RepairPoolLength() {
 FIT_FLOAT32 pool_length = session_->GetPoolLength();

  if (pool_length == FIT_FLOAT32_INVALID || pool_length < 1) {
    session_->SetPoolLength(25);
    session_->SetPoolLengthUnit(FIT_DISPLAY_MEASURE_METRIC);
  }
}

void swt::Fenix2SwimFile::Save(const std::string &filename, bool convert/*=false*/) const {
  unsigned short active_length_counter = 0;
  FIT_FLOAT32 pool_length = session_->GetPoolLength();

  fit::Encode encode;
  std::fstream fit_file(filename, 
      std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);

  if (!fit_file.is_open())
    throw std::runtime_error("Error opening file"); 

  encode.Open(fit_file);
  for (const std::unique_ptr<fit::Mesg> &mesg : mesgs_) {
    if (typeid(*mesg) == typeid(fit::RecordMesg)) {
      for (fit::LengthMesg *length : lengths_) {
        fit::LapMesg *lap = GetLap(length->GetMessageIndex());
        if (lap->GetNumActiveLengths() > 0) {
          if (length->GetMessageIndex() == lap->GetFirstLengthIndex()) {
            record_->SetTimestamp(lap->GetStartTime());
              record_->SetDistance(static_cast<FIT_FLOAT32>(active_length_counter) * pool_length);
            encode.Write(*record_);
          }

          if (length->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE) 
            ++active_length_counter;

          record_->SetTimestamp(length->GetTimestamp());
          record_->SetDistance(active_length_counter * pool_length);
          encode.Write(*record_);
        }
      }
    } else { 
      encode.Write(*mesg);
    }
  }

  if (!encode.Close())
    throw std::runtime_error("Error writing file");

  fit_file.close();
}

void swt::Fenix2SwimFile::LapSetSwolf(fit::LapMesg *lap, FIT_UINT16 swolf) {
    lap->SetFieldUINT16Value(kLapSwolfFieldNum, swolf);
}

void swt::Fenix2SwimFile::SessionSetAvgStrokeCount(FIT_FLOAT32 avg_stroke_count) {
    session_->SetFieldUINT16Value(kSessionAvgStrokeCountFieldNum, 
        static_cast<FIT_UINT16>(round(avg_stroke_count * 10)));
}

void swt::Fenix2SwimFile::SessionSetNumLengths(FIT_UINT16 num_lengths) {
  session_->SetFieldUINT16Value(kSessionNumLengthsFieldNum, num_lengths);
}

void swt::Fenix2SwimFile::SessionSetSwolf(FIT_UINT16 swolf) {
    session_->SetFieldUINT16Value(kSessionSwolfFieldNum, swolf);
}

void swt::Fenix2SwimFile::UpdateLap(fit::LapMesg *lap) {
  FIT_UINT16 num_active_lengths = 0;
  FIT_FLOAT32 moving_time = 0;
  FIT_UINT32 total_cycles = 0;
  bool is_swim_stroke_init = false; // Allows to perform swimStroke initialization on first length
  FIT_SWIM_STROKE swim_stroke = FIT_SWIM_STROKE_INVALID;

  FIT_UINT8 max_cadence = 0;
  FIT_FLOAT32 max_speed = 0;
  FIT_FLOAT32 total_distance = 0;

  FIT_UINT16 first_length_index = lap->GetFirstLengthIndex();
  FIT_UINT16 last_length_index = static_cast<FIT_UINT16>(lap->GetFirstLengthIndex() + 
      lap->GetNumLengths() - 1);

  for (int index = first_length_index; index <= last_length_index; index++) {
    fit::LengthMesg *length = lengths_.at(index);

    if (length->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE) {
      num_active_lengths++;
      moving_time += length->GetTotalTimerTime();
      total_cycles += length->GetTotalStrokes();

      if (!is_swim_stroke_init) {
        swim_stroke = length->GetSwimStroke();
        is_swim_stroke_init = true;
      } else {
        if (swim_stroke != length->GetSwimStroke())
          swim_stroke = FIT_SWIM_STROKE_MIXED;
      }
      if (length->GetAvgSwimmingCadence() != FIT_UINT8_INVALID && 
          max_cadence < length->GetAvgSwimmingCadence()) {
        max_cadence = length->GetAvgSwimmingCadence();
      }
      if (length->GetAvgSpeed() != FIT_FLOAT32_INVALID && 
          max_speed < length->GetAvgSpeed()) {
        max_speed = length->GetAvgSpeed();
      }
    }
  }

  if (num_active_lengths == 0) {// Active lap must be converted to Rest Lap
    lap->SetNumActiveLengths(0);
    lap->SetTotalCycles(0);
    lap->SetSwimStroke(FIT_SWIM_STROKE_INVALID);

    lap->SetAvgCadence(0);
    lap->SetAvgSpeed(0);
    lap->SetMaxCadence(0);
    lap->SetMaxSpeed(0);
    LapSetSwolf(lap, 0);
    lap->SetTotalCalories(0);
    lap->SetTotalDistance(0);
  
  } else {
    total_distance = num_active_lengths * session_->GetPoolLength();
    lap->SetNumActiveLengths(num_active_lengths);
    lap->SetTotalDistance(total_distance);

    if (swim_stroke != FIT_SWIM_STROKE_DRILL) {
      lap->SetTotalCycles(total_cycles);
      lap->SetSwimStroke(swim_stroke);

      lap->SetAvgCadence(static_cast<FIT_UINT8>
          (round(static_cast<FIT_FLOAT32>(total_cycles) / moving_time * 60)));
      lap->SetAvgSpeed(total_distance / moving_time);
      FIT_FLOAT32 avg_stroke_count = static_cast<FIT_FLOAT32>(total_cycles) / 
        num_active_lengths;
      lap->SetMaxCadence(max_cadence);
      lap->SetMaxSpeed(max_speed);
      FIT_FLOAT32 avg_time_per_length = moving_time / num_active_lengths;
      LapSetSwolf(lap, static_cast<FIT_UINT16>(round(avg_stroke_count + avg_time_per_length)));
    }
  }
}

void swt::Fenix2SwimFile::UpdateSession() {
  FIT_UINT16 num_lengths = 0;
  FIT_UINT16 num_active_lengths = 0;
  FIT_UINT16 num_active_lengths_without_drills = 0;
  FIT_FLOAT32 moving_time = 0;
  FIT_FLOAT32 moving_time_without_drills = 0;
  FIT_UINT32 total_cycles = 0;
  bool is_swim_stroke_init = false; // Allows to perform swimStroke initialization on first length
  FIT_SWIM_STROKE swim_stroke = FIT_SWIM_STROKE_INVALID;
  FIT_UINT8 max_cadence = 0;
  FIT_UINT16 total_calories = 0;
  FIT_FLOAT32 total_distance = 0;
  FIT_FLOAT32 total_distance_without_drills = 0;

  for (fit::LengthMesg *length: lengths_)
  {
    num_lengths++;

    if (length->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE) {
      num_active_lengths++;
      moving_time += length->GetTotalTimerTime();
      total_cycles += length->GetTotalStrokes();
      
      if (length->GetSwimStroke() != FIT_SWIM_STROKE_DRILL) {
        num_active_lengths_without_drills++;
        moving_time_without_drills += length->GetTotalTimerTime();
      }
      if (!is_swim_stroke_init) {
        swim_stroke = length->GetSwimStroke();
        is_swim_stroke_init = true;
      } else {
        if (swim_stroke != length->GetSwimStroke()) 
          swim_stroke = FIT_SWIM_STROKE_MIXED;
      }
      if (length->GetAvgSwimmingCadence() != FIT_UINT8_INVALID && 
          max_cadence < length->GetAvgSwimmingCadence()) {
        max_cadence = length->GetAvgSwimmingCadence();
      }
    }
  }

  for (fit::LapMesg *lap: laps_) {
    // GS Drill lengths have those fields set to invalid
    if (lap->GetNumActiveLengths() > 0 && lap->GetSwimStroke() != FIT_SWIM_STROKE_DRILL) {
      total_calories = static_cast<FIT_UINT16>(total_calories + lap->GetTotalCalories());
    }
  }

  if (num_active_lengths_without_drills == 0)
    throw std::runtime_error("Session is empty (no lengths)");

  total_distance = num_active_lengths * session_->GetPoolLength();
  total_distance_without_drills = num_active_lengths_without_drills * session_->GetPoolLength();

  SessionSetNumLengths(num_lengths);
  session_->SetNumActiveLengths(num_active_lengths);
  session_->SetTotalCycles(total_cycles);
  session_->SetSwimStroke(swim_stroke);

  session_->SetAvgCadence(static_cast<FIT_UINT8>
      (round(static_cast<FIT_FLOAT32>(total_cycles) /
             moving_time_without_drills * 60)));
  session_->SetAvgSpeed(total_distance_without_drills / moving_time_without_drills);
  FIT_FLOAT32 avg_stroke_count = static_cast<FIT_FLOAT32>(total_cycles) / 
    static_cast<FIT_FLOAT32>(num_active_lengths_without_drills);
  SessionSetAvgStrokeCount(avg_stroke_count);
  session_->SetMaxCadence(max_cadence);
  FIT_FLOAT32 avgTimePerLength = moving_time_without_drills / num_active_lengths_without_drills;
  SessionSetSwolf(static_cast<FIT_UINT16>(round(avg_stroke_count + avgTimePerLength))); // GS only
  session_->SetTotalCalories(total_calories);
  session_->SetTotalDistance(total_distance);
}

