#include <cmath>
#include <algorithm>
#include <fstream>
#include "fit_encode.hpp"
#include "fit_record_mesg.hpp"
#include "swt_fr910_swim_file.h"

   
void swt::Fr910SwimFile::AddMesg(const void *mesg) {

  const fit::Mesg *fit_mesg = reinterpret_cast<const fit::Mesg*>(mesg);
  if (fit_mesg->GetNum() == FIT_MESG_NUM_LENGTH) {
    std::unique_ptr<fit::LengthMesg> length(new fit::LengthMesg(*fit_mesg));

    while (length_count_ < length->GetMessageIndex()) {
      std::unique_ptr<fit::LengthMesg> dummy_length(new fit::LengthMesg(*length));
      dummy_length->SetMessageIndex(length_count_);
      dummy_length->SetLengthType(FIT_LENGTH_TYPE_INVALID);
      lengths_.push_back(dummy_length.get());
      mesgs_.push_back(std::move(dummy_length));
      length_count_++;
    }
    length_count_++;
  }
  SwimFile::AddMesg(mesg);
} 


void swt::Fr910SwimFile::Delete(FIT_MESSAGE_INDEX length_index) {
  // if length is not a duplicate but following length is, delete following length
  // because the duplicate length has 0 strokes, and is the one that should be deleted
  if (length_index < lengths_.size() - 1) {
    if (IsDuplicate(static_cast<FIT_MESSAGE_INDEX>(length_index + 1)))
      length_index = static_cast<FIT_MESSAGE_INDEX>(length_index + 1);
  }
   
  std::string error;
  if (!CanSplitChangeStrokeDelete(length_index, &error))
    throw std::runtime_error(error);

  fit::LengthMesg *length = lengths_.at(length_index);
  fit::LapMesg *lap = GetLap(length_index);
  
  if (IsDuplicate(length_index)) {

    mesgs_.remove_if([length] (std::unique_ptr<fit::Mesg> &mesg) 
        {return mesg.get() == length;});
    lengths_.erase(lengths_.begin() + length_index);

    for (fit::LengthMesg *length : lengths_) {
      if (length->GetMessageIndex() > length_index)
        length->SetMessageIndex(static_cast<FIT_MESSAGE_INDEX>(length->GetMessageIndex() - 1));
    }
    lap->SetNumLengths(static_cast<FIT_UINT16>(lap->GetNumLengths() - 1));

    for(unsigned int i = lap->GetMessageIndex() + 1; i < laps_.size(); ++i) {
      if (laps_[i]->GetFirstLengthIndex() != FIT_UINT16_INVALID) {
        laps_[i]->SetFirstLengthIndex(static_cast<FIT_UINT16>(laps_[i]->GetFirstLengthIndex() - 1));
      }
    }
  } else {
    length->SetFieldUINT16Value(kLengthAvgSpeedFieldNum, FIT_UINT16_INVALID);
    length->SetAvgSwimmingCadence(FIT_UINT8_INVALID);
    length->SetEventType(FIT_EVENT_TYPE_STOP);
    length->SetLengthType(FIT_LENGTH_TYPE_IDLE);
    length->SetSwimStroke(FIT_SWIM_STROKE_INVALID);
    length->SetTotalCalories(FIT_UINT16_INVALID);
    length->SetTotalStrokes(FIT_UINT16_INVALID);
  }
  UpdateLap(lap);
  UpdateSession();
}

bool swt::Fr910SwimFile::IsDuplicate(FIT_MESSAGE_INDEX length_index) const {
  bool is_duplicate = false;
  if (length_index > 0 && length_index < lengths_.size()) {
    fit::LengthMesg *length = lengths_.at(length_index);
    if (length->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE) {
      if (length->GetTotalStrokes() == 0) {
        fit::LengthMesg *prev_length = lengths_.at(length_index -1);
        if (length->GetTotalTimerTime() == prev_length->GetTotalTimerTime()) {
          is_duplicate = true;
        }
      }
    }
  }
  return is_duplicate;
}

void swt::Fr910SwimFile::LengthSetTimestamp(fit::LengthMesg *length, FIT_DATE_TIME timestamp)
{
  // Do Nothing Fr910 has this field but is not used (set to invalid)
}

void swt::Fr910SwimFile::RepairMissingLengths() {

  bool recalculate = false;
  for (int i = static_cast<int>(lengths_.size()) - 1; i >= 0; --i) {
    fit::LengthMesg *length = lengths_.at(i);

    if (length->GetLengthType() == FIT_LENGTH_TYPE_INVALID) {

      recalculate = true;
      for (unsigned short j = static_cast<unsigned short>(i + 1); j < lengths_.size(); ++j) {
        lengths_.at(j)->SetMessageIndex(
            static_cast<FIT_MESSAGE_INDEX>(lengths_.at(j)->GetMessageIndex() - 1));
      }

      fit::LapMesg *lap = GetLap(length->GetMessageIndex());
      lap->SetNumLengths(static_cast<FIT_UINT16>(lap->GetNumLengths() - 1));

      mesgs_.remove_if([length] (std::unique_ptr<fit::Mesg> &mesg) 
      {return mesg.get() == length;});
      lengths_.erase(lengths_.begin() + i);

      for (unsigned short j = static_cast<unsigned short>(lap->GetMessageIndex() + 1); j < laps_.size(); ++j)  {
        laps_[j]->SetFirstLengthIndex(
            static_cast<FIT_UINT16>(laps_[j]->GetFirstLengthIndex() - 1));
      }
    }
  }

  if (recalculate) 
    Recalculate();
}

void swt::Fr910SwimFile::Initialize() {
  RepairMissingLengths();
}

void swt::Fr910SwimFile::Save(const std::string &filename) const {
  unsigned short active_length_counter = 0;
  FIT_FLOAT32 pool_length = session_->GetPoolLength();

  fit::Encode encode;
  std::fstream fit_file(filename, 
      std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);

  if (!fit_file.is_open())
    throw std::runtime_error("Error opening file"); 

  encode.Open(fit_file);

  for (const std::unique_ptr<fit::Mesg> &mesg : mesgs_) {
    if (typeid(*mesg) == typeid(fit::LengthMesg)) {
      fit::LengthMesg *length = dynamic_cast<fit::LengthMesg*>(mesg.get());

      if (length->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE) {
        active_length_counter++;
        fit::RecordMesg record;
        record.SetTimestamp(FIT_DATE_TIME_INVALID);
        record.SetDistance(active_length_counter * pool_length);
        record.SetSpeed(length->GetAvgSpeed());

        FIT_MESSAGE_INDEX length_index = length->GetMessageIndex();
        fit::LapMesg * lap = GetLap(length_index);
        if (length_index == (lap->GetFirstLengthIndex() + lap->GetNumLengths() - 1)) 
          record.SetFieldUINT16Value(kLengthAvgSpeedFieldNum, FIT_UINT16_INVALID);

        encode.Write(record);
        encode.Write(*length);
      } else if (length->GetLengthType() == FIT_LENGTH_TYPE_IDLE) { 
        fit::RecordMesg record;
        record.SetTimestamp(FIT_DATE_TIME_INVALID);
        record.SetDistance(active_length_counter * pool_length);
        record.SetSpeed(0);
        record.SetFieldUINT16Value(kLengthAvgSpeedFieldNum, FIT_UINT16_INVALID);
        encode.Write(record);
        encode.Write(*length);
      }
    } else { 
      encode.Write(*mesg);
    }
  }

  if (!encode.Close())
    throw std::runtime_error("Error writing file");

  fit_file.close();
}


void swt::Fr910SwimFile::SessionSetNumLengths(FIT_UINT16 num__lengths) {
  session_->SetFieldUINT16Value(kSessionNumLengthsFieldNum, num__lengths);
}

void swt::Fr910SwimFile::UpdateLap(fit::LapMesg *lap) {
  FIT_UINT16 num_active_lengths = 0;
  FIT_FLOAT32 moving_time = 0;
  FIT_UINT32 total_cycles = 0;
  bool is_swim_stroke_init = false ; // Allows to perform swimStroke initialization on first length
  FIT_SWIM_STROKE swim_stroke = FIT_SWIM_STROKE_INVALID;

  FIT_UINT8 max_cadence = 0;
  FIT_FLOAT32 max_speed = 0;
  FIT_FLOAT32 total_distance = 0;
  FIT_UINT16 total_calories = 0;

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
      total_calories = static_cast<FIT_UINT16>(total_calories + length->GetTotalCalories());
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
    lap->SetFieldUINT16Value(kLapAvgStrokeDistanceFieldNum, FIT_UINT16_INVALID);
    lap->SetMaxCadence(0);
    lap->SetFieldUINT16Value(kLapMaxSpeedFieldNum, FIT_UINT16_INVALID);
    lap->SetTotalCalories(0);
    lap->SetTotalDistance(0);
    lap->SetTotalFatCalories(0);
  
  } else {
    if (software_version_ < 250)
      moving_time = lap->GetTotalTimerTime();

    total_distance = num_active_lengths * session_->GetPoolLength();
    lap->SetNumActiveLengths(num_active_lengths);
    lap->SetTotalCycles(total_cycles);
    lap->SetSwimStroke(swim_stroke);

    lap->SetAvgCadence(static_cast<FIT_UINT8>
        (round(static_cast<FIT_FLOAT32>(total_cycles) / lap->GetTotalTimerTime() * 60)));
    lap->SetAvgSpeed(total_distance / moving_time);
    
    if (total_cycles > 0)
      lap->SetAvgStrokeDistance(total_distance / static_cast<FIT_FLOAT32>(total_cycles));
    else
      lap->SetFieldUINT16Value(kLapAvgStrokeDistanceFieldNum, FIT_UINT16_INVALID);
      
    lap->SetMaxCadence(max_cadence);
    lap->SetMaxSpeed(max_speed);
    lap->SetTotalCalories(total_calories); 
    lap->SetTotalDistance(total_distance);
    lap->SetTotalFatCalories(0);
  }
}

void swt::Fr910SwimFile::UpdateSession() {
  FIT_UINT16 num_lengths = 0;
  FIT_UINT16 num_active_lengths = 0;
  FIT_FLOAT32 moving_time = 0;
  FIT_UINT32 total_cycles = 0;
  bool is_swim_stroke_init = false; // Allows to perform swimStroke initialization on first length
  FIT_SWIM_STROKE swim_stroke = FIT_SWIM_STROKE_INVALID;
  FIT_UINT8 max_cadence = 0;
  FIT_FLOAT32 max_speed = 0;
  FIT_UINT16 total_calories = 0;
  FIT_FLOAT32 total_distance = 0;
  FIT_UINT16 total_fat_calories = 0;

  for (fit::LengthMesg *length: lengths_)
  {
    num_lengths++;

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

  for (fit::LapMesg *lap: laps_) {
    if (lap->GetNumActiveLengths() > 0) {
      total_calories = static_cast<FIT_UINT16>(total_calories + lap->GetTotalCalories());
      total_fat_calories =  static_cast<FIT_UINT16>(total_fat_calories + 
          lap->GetTotalFatCalories());
    }
  }

  if (num_active_lengths == 0)
    throw std::runtime_error("Session is empty (no lengths)");
  
  if (software_version_ < 250)
    moving_time = session_->GetTotalTimerTime();

  total_distance = num_active_lengths * session_->GetPoolLength();

  SessionSetNumLengths(num_lengths);
  session_->SetNumActiveLengths(num_active_lengths);
  session_->SetTotalCycles(total_cycles);
  session_->SetSwimStroke(swim_stroke);

  session_->SetAvgCadence(static_cast<FIT_UINT8>
      (round(static_cast<FIT_FLOAT32>(total_cycles) / session_->GetTotalTimerTime() * 60)));
  session_->SetAvgSpeed(total_distance / moving_time);
  FIT_FLOAT32 avg_stroke_count = static_cast<FIT_FLOAT32>(total_cycles) / 
    static_cast<FIT_FLOAT32>(num_active_lengths);
  session_->SetAvgStrokeCount(avg_stroke_count);
  session_->SetAvgStrokeDistance(total_distance / static_cast<FIT_FLOAT32>(total_cycles));
  session_->SetMaxCadence(max_cadence);
  session_->SetMaxSpeed(max_speed);
  session_->SetTotalCalories(total_calories);
  session_->SetTotalDistance(total_distance);
  session_->SetTotalFatCalories(total_fat_calories);
}

