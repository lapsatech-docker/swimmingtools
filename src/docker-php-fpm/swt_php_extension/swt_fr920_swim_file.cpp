#include <cmath>
#include <algorithm>
#include <fstream>
#include <utility>
#include "fit_encode.hpp"
#include "fit_record_mesg.hpp"
#include "swt_fr920_swim_file.h"

swt::Fr920SwimFile::Fr920SwimFile()
{}


void swt::Fr920SwimFile::AddMesg(const void *mesg) {

  const fit::Mesg *fit_mesg = reinterpret_cast<const fit::Mesg*>(mesg);

   if (fit_mesg->GetNum() == FIT_MESG_NUM_RECORD) {
     std::unique_ptr<fit::RecordMesg> record(new fit::RecordMesg(*fit_mesg));
     mesgs_.push_back(move(record));
   } else {
     SwimFile::AddMesg(mesg);
   }   
}

void swt::Fr920SwimFile::Delete(FIT_MESSAGE_INDEX length_index) {
  std::string error;

  if (!CanSplitChangeStrokeDelete(length_index, &error))
    throw std::runtime_error(error);

  fit::LengthMesg *length = lengths_.at(length_index);
  length->SetFieldUINT16Value(kLengthAvgSpeedFieldNum, FIT_UINT16_INVALID);
  length->SetAvgSwimmingCadence(FIT_UINT8_INVALID);
  length->SetEventType(FIT_EVENT_TYPE_STOP);
  length->SetLengthType(FIT_LENGTH_TYPE_IDLE);
  length->SetSwimStroke(FIT_SWIM_STROKE_INVALID);
  length->SetTotalCalories(FIT_UINT16_INVALID);
  length->SetTotalStrokes(FIT_UINT16_INVALID);

  UpdateLap(GetLap(length_index));
  UpdateSession();
}

void swt::Fr920SwimFile::Initialize() {

  CheckIndexes();
}

void swt::Fr920SwimFile::Split(FIT_MESSAGE_INDEX length_index) {
  std::string error;

  if (!CanSplitChangeStrokeDelete(length_index, &error))
    throw std::runtime_error(error);

  fit::LengthMesg *existing_length = lengths_.at(length_index);
  std::unique_ptr<fit::LengthMesg> added_length(new fit::LengthMesg(*existing_length));

  FIT_FLOAT32 total_elapsed_time = existing_length->GetTotalElapsedTime() / 2;
  FIT_FLOAT32 total_timer_time = existing_length->GetTotalTimerTime() / 2;
  FIT_UINT16 total_strokes = existing_length->GetTotalStrokes();

  added_length->SetTotalElapsedTime(total_elapsed_time);
  added_length->SetTotalTimerTime(total_timer_time);
  added_length->SetAvgSpeed(session_->GetPoolLength() / total_timer_time);
  added_length->SetTotalStrokes(total_strokes / 2); // Integer Division
  added_length->SetAvgSwimmingCadence(
      static_cast<FIT_UINT8>(round(60.0 * added_length->GetTotalStrokes() / total_timer_time)));


  existing_length->SetStartTime(added_length->GetStartTime() +
      static_cast<FIT_DATE_TIME>(added_length->GetTotalTimerTime()));  // second length start when first end
  existing_length->SetTotalElapsedTime(total_elapsed_time);
  existing_length->SetTotalTimerTime(total_timer_time);
  existing_length->SetAvgSpeed(session_->GetPoolLength() / total_timer_time);

  existing_length->SetTotalStrokes(static_cast<FIT_UINT16>((total_strokes / 2) +
        (total_strokes % 2))); // Add mod to preserve stroke count when odd
  existing_length->SetAvgSwimmingCadence(static_cast<FIT_UINT8>
      (round(60.0 * existing_length->GetTotalStrokes() / total_timer_time)));

  for (fit::LengthMesg *length : lengths_) {
    if (length->GetMessageIndex() >= length_index) {
      length->SetMessageIndex(static_cast<FIT_MESSAGE_INDEX>
          (length->GetMessageIndex() + 1));
    }
  }

  std::list<std::unique_ptr<fit::Mesg>>::iterator it;


  it = std::find_if(mesgs_.begin(), mesgs_.end(),
      [existing_length] (const std::unique_ptr<fit::Mesg> &mesg) {
      return mesg.get() == existing_length;});

  lengths_.insert(lengths_.begin() + length_index, added_length.get());
  mesgs_.insert(it, move(added_length));

  fit::LapMesg *the_lap = GetLap(length_index);
  the_lap->SetNumLengths(static_cast<FIT_UINT16>(the_lap->GetNumLengths() + 1));
  UpdateLap(the_lap);

  for (fit::LapMesg *lap: laps_) {
    if ((lap->GetMessageIndex() > the_lap->GetMessageIndex()) &&
        (lap->GetFirstLengthIndex() != FIT_UINT16_INVALID)) {
      lap->SetFirstLengthIndex(static_cast<FIT_UINT16>(lap->GetFirstLengthIndex() + 1));
    }
  }
  UpdateSession();
}

void swt::Fr920SwimFile::Save(const std::string &filename, bool convert/*=false*/) {

  UpdateRecords();

  fit::Encode encode(fit_protocol_version_);
  std::fstream fit_file(filename,
      std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);

  if (!fit_file.is_open())
    throw std::runtime_error("Error opening file");

  encode.Open(fit_file);

  for (const std::unique_ptr<fit::Mesg> &mesg : mesgs_) {
    encode.Write(*mesg);
  }

  if (!encode.Close())
    throw std::runtime_error("Error writing file");

  for (int i = 0; i < hr_data_.size(); i++) {
    fit_file << hr_data_[i];
  }

  fit_file.close();
}

// The first_length_index field of lap message is corrupted in some files.
// The field is used by the code to associate lengths to their corresponding
// laps (GetLap Function). If the field is corrupted an attempt is made
// to fix it (RepairLapsIndexes)
void swt::Fr920SwimFile::CheckIndexes() {

  FIT_UINT16 length_message_index = 0;

  for(fit::LengthMesg *length: lengths_) {
    if (length->GetMessageIndex() == length_message_index) {
      length_message_index++;
    }  else {
      RepairIndexes();
    }
  }

  FIT_UINT16 lap_message_index = 0;

  for(fit::LapMesg *lap: laps_) {
    if (lap->GetMessageIndex() == lap_message_index) {
      lap_message_index++;
    }  else {
      RepairIndexes();
    }
  }

  FIT_UINT16 current_first_index = 0;

  for(fit::LapMesg *lap: laps_) {

    FIT_UINT16 first_length_index = lap->GetFirstLengthIndex();
    FIT_UINT16 num_lengths = lap->GetNumLengths();

    if (first_length_index != FIT_UINT16_INVALID &&
        num_lengths != FIT_UINT16_INVALID) {
      if (first_length_index >= current_first_index) {
        current_first_index = first_length_index + num_lengths;
      } else {
        RepairIndexes();
        break;
      }
    }
  }
}

void swt::Fr920SwimFile::LapSetAvgStrokeCount(fit::LapMesg *lap, FIT_FLOAT32 avg_stroke_count) {
  lap->SetFieldUINT16Value(kLapAvgStrokeCountFieldNnum,
      static_cast<FIT_UINT16>(round(avg_stroke_count * 10)));
}

void swt::Fr920SwimFile::LapSetMovingTime(fit::LapMesg *lap, FIT_FLOAT32 moving_time) {
  lap->SetFieldUINT32Value(kLapMovingTimeFieldNnum,
      static_cast<FIT_UINT32>(round(moving_time * 1000)));
}

void swt::Fr920SwimFile::LapSetSwolf(fit::LapMesg *lap, FIT_UINT16 swolf) {
  lap->SetFieldUINT16Value(kLapSwolfFieldNum, swolf);
}


// If the field first_length_index is corrupted (CheckLapsIndexex). An attempt is made to
// fix it. The function assumes that lengths and laps messages are ordered by timestamp.
// All lengths messages preceding a lap message will be associated to this lap.
//
// Length 1
// length 2
// Lap 1 first_length_index = 1, num_lengths = 2
// length 3
// length 4
// lenght 5
// lap 2 first_length_index = 3, num_lengths = 3
void swt::Fr920SwimFile::RepairIndexes() {

  FIT_MESSAGE_INDEX length_message_index = 0;
  FIT_MESSAGE_INDEX lap_message_index = 0;
  FIT_UINT16 num_active_lengths = 0;
  FIT_UINT16 num_idle_lengths = 0;
  FIT_FLOAT32 lap_timer_time = 0;
  FIT_DATE_TIME lap_start_time = FIT_DATE_TIME_INVALID;

  for (auto mesg = mesgs_.begin(); mesg != mesgs_.end();) {

    bool mesg_was_erased = false;

    if (typeid(**mesg) == typeid(fit::LengthMesg)) {
      fit::LengthMesg *length = dynamic_cast<fit::LengthMesg*>((*mesg).get());
      length->SetMessageIndex(length_message_index);
      length_message_index++;

      if (num_active_lengths == 0 && num_idle_lengths == 0) {
        lap_start_time = length->GetStartTime();
      }

      if (length->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE) {
        num_active_lengths++;
      } else {
        num_idle_lengths++;
      }

      FIT_FLOAT32 length_timer_time = length->GetTotalTimerTime();

      if (length_timer_time != FIT_FLOAT32_INVALID)
        lap_timer_time += length_timer_time;

    } else if (typeid(**mesg) == typeid(fit::LapMesg)) {

      fit::LapMesg *lap = dynamic_cast<fit::LapMesg*>((*mesg).get());
      if (num_active_lengths > 0 || num_idle_lengths > 0) {
        lap->SetMessageIndex(lap_message_index);
        lap->SetNumActiveLengths(num_active_lengths);
        lap->SetNumLengths(num_active_lengths + num_idle_lengths);
        lap->SetFirstLengthIndex(length_message_index - num_active_lengths - num_idle_lengths);
        lap->SetStartTime(lap_start_time);
        lap->SetTotalTimerTime(lap_timer_time);
        lap->SetTotalElapsedTime(lap_timer_time);

        UpdateLap(lap);

        num_active_lengths = 0;
        num_idle_lengths = 0;
        lap_timer_time = 0;
        lap_message_index++;

      } else {

        laps_.erase(laps_.begin()+ lap_message_index);
        mesg = mesgs_.erase(mesg);
        mesg_was_erased = true;
      }
    }
    if (!mesg_was_erased) mesg++;
  }

  if (num_active_lengths > 0 || num_idle_lengths > 0)
    throw std::runtime_error("Last lap missing");

  session_->SetNumLaps(lap_message_index);
}


void swt::Fr920SwimFile::SessionSetAvgStrokeCount(FIT_FLOAT32 avg_stroke_count) {
  session_->SetFieldUINT16Value(kSessionAvgStrokeCountFieldNum,
      static_cast<FIT_UINT16>(round(avg_stroke_count * 10)));
}

void swt::Fr920SwimFile::SessionSetNumLengthsInActiveLaps(FIT_UINT16 num_lengths_in_active_laps) {
  session_->SetFieldUINT16Value(kSessionNumLengthsInActiveLapsFieldNum, num_lengths_in_active_laps);
}

void swt::Fr920SwimFile::SessionSetMovingTime(FIT_FLOAT32 moving_time) {
  session_->SetFieldUINT32Value(kSessionMovingTimeFieldNum,
      static_cast<FIT_UINT32>(round(moving_time * 1000)));
}

void swt::Fr920SwimFile::SessionSetSwolf(FIT_UINT16 swolf) {
  session_->SetFieldUINT16Value(kSessionSwolfFieldNum, swolf);
}

void swt::Fr920SwimFile::UpdateLap(fit::LapMesg *lap) {
  FIT_UINT16 num_active_lengths = 0;
  FIT_FLOAT32 moving_time = 0;
  FIT_UINT32 total_cycles = 0;
  bool is_swim_stroke_init = false ; // Allows to perform swimStroke initialization on first length
  FIT_SWIM_STROKE swim_stroke = FIT_SWIM_STROKE_INVALID;

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

      if (length->GetAvgSpeed() != FIT_FLOAT32_INVALID &&
          max_speed < length->GetAvgSpeed()) {
        max_speed = length->GetAvgSpeed();
      }
    }
  }

  if (num_active_lengths == 0) {// Active lap must be converted to Rest Lap
    lap->SetNumActiveLengths(0);
    LapSetMovingTime(lap, 0);
    lap->SetTotalCycles(0);
    lap->SetSwimStroke(FIT_SWIM_STROKE_INVALID);

    lap->SetAvgCadence(0);
    if (lap->HasField(kLapAvgSpeedFieldNum))  lap->SetAvgSpeed(0);
    if (lap->HasField(kLapEnhancedAvgSpeedFieldNum)) lap->SetEnhancedAvgSpeed(0);
    LapSetAvgStrokeCount(lap, 0);
    lap->SetAvgStrokeDistance(0);
    if (lap->HasField(kLapMaxSpeedFieldNum)) lap->SetMaxSpeed(0);
    if (lap->HasField(kLapEnhancedMaxSpeedFieldNum)) lap->SetEnhancedMaxSpeed(0);
    LapSetSwolf(lap, 0);
    lap->SetTotalCalories(0);
    lap->SetTotalDistance(0);

  } else {
    total_distance = num_active_lengths * session_->GetPoolLength();
    lap->SetNumActiveLengths(num_active_lengths);
    lap->SetTotalDistance(total_distance);

    if (swim_stroke != FIT_SWIM_STROKE_DRILL) {
      LapSetMovingTime(lap, moving_time);
      lap->SetTotalCycles(total_cycles);
      lap->SetSwimStroke(swim_stroke);

      lap->SetAvgCadence(static_cast<FIT_UINT8>
          (round(static_cast<FIT_FLOAT32>(total_cycles) / moving_time * 60)));
      if (lap->HasField(kLapAvgSpeedFieldNum)) lap->SetAvgSpeed(total_distance / moving_time);
      if (lap->HasField(kLapEnhancedAvgSpeedFieldNum)) lap->SetEnhancedAvgSpeed(total_distance / moving_time);
      FIT_FLOAT32 avg_stroke_count = static_cast<FIT_FLOAT32>(total_cycles) /
        num_active_lengths;
      LapSetAvgStrokeCount(lap, avg_stroke_count);
      lap->SetAvgStrokeDistance(total_distance / static_cast<FIT_FLOAT32>(total_cycles));
      if (lap->HasField(kLapMaxSpeedFieldNum)) lap->SetMaxSpeed(max_speed);
      if (lap->HasField(kLapEnhancedMaxSpeedFieldNum)) lap->SetEnhancedMaxSpeed(max_speed);
      FIT_FLOAT32 avg_time_per_length = moving_time / num_active_lengths;
      LapSetSwolf(lap, static_cast<FIT_UINT16>(round(avg_stroke_count + avg_time_per_length)));
    }
  }
}

void swt::Fr920SwimFile::UpdateSession() {
  FIT_UINT16 num_lengths = 0;
  FIT_UINT16 num_active_lengths = 0;
  FIT_UINT16 num_active_lengths_without_drills = 0;
  // There is a mysterious custom field in FR920, this field seems to be the number
  // of lengths mesgs (active and rest) in active laps. Active laps may contain
  // rest lengths if a user stop swimming without hitting the pause button.
  FIT_UINT16 num_lengths_in_active_laps = 0;
  FIT_FLOAT32 moving_time = 0;
  FIT_FLOAT32 moving_time_without_drills = 0;
  FIT_UINT32 total_cycles = 0;
  bool is_swim_stroke_init = false; // Allows to perform swimStroke initialization on first length
  FIT_SWIM_STROKE swim_stroke = FIT_SWIM_STROKE_INVALID;

  FIT_FLOAT32 max_speed = 0;
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
      if (length->GetAvgSpeed() != FIT_FLOAT32_INVALID &&
          max_speed < length->GetAvgSpeed()) {
        max_speed = length->GetAvgSpeed();
      }

    }
  }

  for (fit::LapMesg *lap: laps_) {
    if (lap->GetNumActiveLengths() > 0) {
      num_lengths_in_active_laps += lap->GetNumLengths();
    }
  }

  if (num_active_lengths_without_drills == 0)
    throw std::runtime_error("File is empty (no lengths or only drills)");

  total_distance = num_active_lengths * session_->GetPoolLength();
  total_distance_without_drills = num_active_lengths_without_drills * session_->GetPoolLength();

  session_->SetNumActiveLengths(num_active_lengths);
  SessionSetNumLengthsInActiveLaps(num_lengths_in_active_laps);
  SessionSetMovingTime(moving_time);
  session_->SetTotalCycles(total_cycles);
  session_->SetSwimStroke(swim_stroke);

  session_->SetAvgCadence(static_cast<FIT_UINT8>
      (round(static_cast<FIT_FLOAT32>(total_cycles) /
             moving_time_without_drills * 60)));
  if (session_->HasField(kSessionAvgSpeedFieldNum)) session_->SetAvgSpeed(total_distance_without_drills / moving_time_without_drills);
  if (session_->HasField(kSessionEnhancedAvgSpeedFieldNum)) session_->SetEnhancedAvgSpeed(total_distance_without_drills / moving_time_without_drills);
  FIT_FLOAT32 avg_stroke_count = static_cast<FIT_FLOAT32>(total_cycles) /
    static_cast<FIT_FLOAT32>(num_active_lengths_without_drills);
  SessionSetAvgStrokeCount(avg_stroke_count);
  session_->SetAvgStrokeDistance(total_distance_without_drills / static_cast<FIT_FLOAT32>(total_cycles));
  if (session_->HasField(kSessionMaxSpeedFieldNum)) session_->SetMaxSpeed(max_speed);
  if (session_->HasField(kSessionEnhancedMaxSpeedFieldNum))  session_->SetEnhancedMaxSpeed(max_speed);
  FIT_FLOAT32 avgTimePerLength = moving_time_without_drills / num_active_lengths_without_drills;
  SessionSetSwolf(static_cast<FIT_UINT16>(round(avg_stroke_count + avgTimePerLength)));
  session_->SetTotalDistance(total_distance);
}

void swt::Fr920SwimFile::UpdateRecords() {

  auto length = lengths_.begin();
  auto first_length = lengths_.begin();
  FIT_FLOAT32 distance = 0;
  FIT_FLOAT32 pool_length = session_->GetPoolLength();

  for (const std::unique_ptr<fit::Mesg> &mesg : mesgs_) {
    if (typeid(*mesg) == typeid(fit::RecordMesg)) {
      fit::RecordMesg *record = dynamic_cast<fit::RecordMesg*>(mesg.get());

      if (record->IsDistanceValid()) {

        if (record->GetTimestamp() < 
            ((*first_length)->GetStartTime() + static_cast<FIT_DATE_TIME>((*first_length)->GetTotalTimerTime()))) {
          record->SetDistance(0);
          record->SetFieldUINT16Value(kRecordAvgSpeedFieldNum, FIT_UINT16_INVALID);
          record->SetCadence(FIT_UINT8_INVALID);
        } else {
          while (length != lengths_.end()) {

            if (record->GetTimestamp() >= ((*length)->GetStartTime() + static_cast<FIT_DATE_TIME>((*length)->GetTotalTimerTime()))) {

              if ((*length)->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE)
                distance += pool_length;

              length++;
            } else {
              break;
            } 
          }   
          if ((*(length -1))->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE) {
            record->SetDistance(distance);
            record->SetSpeed((*(length-1))->GetAvgSpeed());
            record->SetCadence((*(length-1))->GetAvgSwimmingCadence());
          } else {
            record->SetDistance(distance);
            record->SetFieldUINT16Value(kRecordAvgSpeedFieldNum, FIT_UINT16_INVALID);
            record->SetCadence(FIT_UINT8_INVALID);
          }
        }
      }
    }
  }
}
