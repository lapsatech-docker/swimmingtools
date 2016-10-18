#include <cmath>
#include <algorithm>
#include <fstream>
#include "fit_encode.hpp"
#include "fit_record_mesg.hpp"
#include "swt_fr920_swim_file.h"

void swt::Fr920SwimFile::AddMesg(const void *mesg) {

  const fit::Mesg *fit_mesg = reinterpret_cast<const fit::Mesg*>(mesg);

  // keep record messages with only a timestamp. Those are used with new
  // heart rate data Garmin has added September 2015
  if (fit_mesg->GetNum() == FIT_MESG_NUM_RECORD &&
      fit_mesg->GetNumFields() == 1 &&
      fit_mesg->HasField(kTimestampFieldNum)) {

      mesgs_.push_back(std::unique_ptr<fit::Mesg>(new fit::Mesg(*fit_mesg)));
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
  RepairMissingLaps();
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
  
  FIT_DATE_TIME timestamp_lag = 0;
  if (existing_length->GetTimestamp() > (existing_length->GetStartTime() + 
        static_cast<FIT_DATE_TIME>(existing_length->GetTotalTimerTime())))
    timestamp_lag = existing_length->GetTimestamp() - (existing_length->GetStartTime() + 
        static_cast<FIT_DATE_TIME>(existing_length->GetTotalTimerTime()));

  LengthSetTimestamp(added_length.get(), existing_length->GetStartTime() +
     static_cast<FIT_DATE_TIME>(total_timer_time) + timestamp_lag);
  added_length->SetTotalElapsedTime(total_elapsed_time);
  added_length->SetTotalTimerTime(total_timer_time);
  added_length->SetAvgSpeed(session_->GetPoolLength() / total_timer_time);
  added_length->SetTotalStrokes(total_strokes / 2); // Integer Division
  added_length->SetAvgSwimmingCadence(
      static_cast<FIT_UINT8>(round(60.0 * added_length->GetTotalStrokes() / total_timer_time)));

  for (fit::LengthMesg *length : lengths_) {
    if (length->GetMessageIndex() >= length_index) {
      length->SetMessageIndex(static_cast<FIT_MESSAGE_INDEX>
          (length->GetMessageIndex() + 1));
    }
  }

  existing_length->SetStartTime(added_length->GetStartTime() + 
      static_cast<FIT_DATE_TIME>(added_length->GetTotalTimerTime()));  // second length start when first end
  existing_length->SetTotalElapsedTime(total_elapsed_time);
  existing_length->SetTotalTimerTime(total_timer_time);
  existing_length->SetAvgSpeed(session_->GetPoolLength() / total_timer_time);

  existing_length->SetTotalStrokes(static_cast<FIT_UINT16>((total_strokes / 2) +
        (total_strokes % 2))); // Add mod to preserve stroke count when odd
  existing_length->SetAvgSwimmingCadence(static_cast<FIT_UINT8>
      (round(60.0 * existing_length->GetTotalStrokes() / total_timer_time)));

  lengths_.insert(lengths_.begin() + length_index, added_length.get());
  std::list<std::unique_ptr<fit::Mesg>>::iterator it;
  it = std::find_if(mesgs_.begin(), mesgs_.end(),
      [existing_length] (const std::unique_ptr<fit::Mesg> &mesg) {
      return mesg.get() == existing_length;});

  fit::LengthMesg * preceding_length = NULL;
  std::list<std::unique_ptr<fit::Mesg>>::iterator preceding_length_it  = mesgs_.begin();
  
  if (length_index > 0) {
    preceding_length = lengths_.at(length_index -1);
    preceding_length_it = std::find_if(mesgs_.begin(), it,
        [preceding_length] (const std::unique_ptr<fit::Mesg> &mesg) {
      return mesg.get() == preceding_length;});
  }

  if (added_length->GetFieldUINT32Value(kTimestampFieldNum) == FIT_UINT32_INVALID)
    throw std::runtime_error("Fr920 Split, added length timestamp invalid"); 


  while((it != preceding_length_it) &&
      ((it->get()->GetFieldUINT32Value(kTimestampFieldNum) > added_length->GetTimestamp() &&
       it->get()->GetFieldUINT32Value(kTimestampFieldNum) != FIT_UINT32_INVALID) ||
       it->get()->GetFieldUINT32Value(kTimestampFieldNum) == FIT_UINT32_INVALID))
    it--;
  
  mesgs_.insert(++it, move(added_length));

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

void swt::Fr920SwimFile::Save(const std::string &filename, bool convert/*=false*/) const {
  unsigned int active_length_counter = 0;
  fit::Encode encode(fit_protocol_version_);
  std::fstream fit_file(filename,
      std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);

  if (!fit_file.is_open())
    throw std::runtime_error("Error opening file");

  encode.Open(fit_file);

  for (const std::unique_ptr<fit::Mesg> &mesg : mesgs_) {
    if (typeid(*mesg) == typeid(fit::LengthMesg)) {
      fit::LengthMesg *length = dynamic_cast<fit::LengthMesg*>(mesg.get());
      fit::RecordMesg record;
      record.SetTimestamp(length->GetTimestamp());

      if (length->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE) {
        active_length_counter++;
        record.SetDistance(static_cast<FIT_FLOAT32>(active_length_counter) *
            session_->GetPoolLength());
        record.SetSpeed(length->GetAvgSpeed());
        record.SetCadence(length->GetAvgSwimmingCadence());
      } else if (length->GetLengthType() == FIT_LENGTH_TYPE_IDLE) {
        record.SetDistance(static_cast<FIT_FLOAT32>(active_length_counter) *
            session_->GetPoolLength());
        record.SetFieldUINT16Value(kRecordAvgSpeedFieldNum, FIT_UINT16_INVALID);
        record.SetCadence(FIT_UINT8_INVALID);
      }
      encode.Write(record);
      encode.Write(*length);
    } else {
      encode.Write(*mesg);
    }
  }


  if (!encode.Close())
    throw std::runtime_error("Error writing file");

  for (int i = 0; i < hr_data_.size(); i++) {
    fit_file << hr_data_[i];
  }

  fit_file.close();
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

void swt::Fr920SwimFile::RepairMissingLaps() {

  for(FIT_MESSAGE_INDEX i = 0; i < laps_.size(); i++) {
    if (laps_.at(i)->GetMessageIndex() == (i + 1)) {

      std::unique_ptr<fit::LapMesg> lap(new fit::LapMesg(*laps_.at(i)));
      fit::LapMesg *lap_after = laps_.at(i);
      lap->SetMessageIndex(i);
      FIT_MESSAGE_INDEX first_length_index = FIT_MESSAGE_INDEX_INVALID;
      FIT_MESSAGE_INDEX last_length_index = FIT_MESSAGE_INDEX_INVALID;


      if (i == 0) {
        first_length_index = 0;
      } else {

        fit::LapMesg *lap_before = laps_.at(i - 1);

        if (lap_before->GetNumActiveLengths() == 0) {
          first_length_index = lap_before->GetFirstLengthIndex() + 1;
        } else {
          first_length_index = lap_before->GetFirstLengthIndex() + lap_before->GetNumLengths();
        }
      }
      last_length_index = lap_after->GetFirstLengthIndex() - 1;

      lap->SetFirstLengthIndex(first_length_index);
      lap->SetNumLengths(last_length_index - first_length_index + 1);
      lap->SetStartTime(lengths_.at(first_length_index)->GetStartTime());
      lap->SetTimestamp(lengths_.at(last_length_index)->GetTimestamp());

      FIT_FLOAT32 timer_time = 0;
      for (FIT_MESSAGE_INDEX j = first_length_index; j <= last_length_index; j++) {
        timer_time += lengths_.at(j)->GetTotalTimerTime();
      }
      lap->SetTotalElapsedTime(timer_time);
      lap->SetTotalTimerTime(timer_time);

      UpdateLap(lap.get());

      fit::LengthMesg *last_length = lengths_.at(last_length_index);

      laps_.insert(laps_.begin() + i, lap.get());
      std::list<std::unique_ptr<fit::Mesg>>::iterator it;
      it = std::find_if(mesgs_.begin(), mesgs_.end(),
          [last_length] (const std::unique_ptr<fit::Mesg> &mesg) {
          return mesg.get() == last_length;});
      ++it;
      mesgs_.insert(it, move(lap));

      UpdateSession();
    }
  }
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
      // Prior to version 2.50, calories were cumulative
      if (software_version_ >= 250)
        total_calories = static_cast<FIT_UINT16>(total_calories + length->GetTotalCalories());
      else
        total_calories = length->GetTotalCalories();

      if (length->GetAvgSpeed() != FIT_FLOAT32_INVALID &&
          max_speed < length->GetAvgSpeed()) {
        max_speed = length->GetAvgSpeed();
      }
    }
  }

  if (num_active_lengths == 0) {// Active lap must be converted to Rest Lap
    lap->SetNumLengths(0);
    lap->SetNumActiveLengths(0);
    LapSetMovingTime(lap, 0);
    lap->SetTotalCycles(0);
    lap->SetSwimStroke(FIT_SWIM_STROKE_INVALID);

    lap->SetAvgCadence(0);
    lap->SetAvgSpeed(0);
    LapSetAvgStrokeCount(lap, 0);
    lap->SetAvgStrokeDistance(0);
    lap->SetMaxSpeed(0);
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
      lap->SetAvgSpeed(total_distance / moving_time);
      FIT_FLOAT32 avg_stroke_count = static_cast<FIT_FLOAT32>(total_cycles) /
        num_active_lengths;
      LapSetAvgStrokeCount(lap, avg_stroke_count);
      lap->SetAvgStrokeDistance(total_distance / static_cast<FIT_FLOAT32>(total_cycles));
      lap->SetMaxSpeed(max_speed);
      FIT_FLOAT32 avg_time_per_length = moving_time / num_active_lengths;
      LapSetSwolf(lap, static_cast<FIT_UINT16>(round(avg_stroke_count + avg_time_per_length)));
      lap->SetTotalCalories(total_calories);
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
      if (length->GetAvgSpeed() != FIT_FLOAT32_INVALID &&
          max_speed < length->GetAvgSpeed()) {
        max_speed = length->GetAvgSpeed();
      }

    }
  }

  for (fit::LapMesg *lap: laps_) {
    if (lap->GetNumActiveLengths() > 0) {
      num_lengths_in_active_laps += lap->GetNumLengths();

      if (lap->GetTotalCalories() != FIT_UINT16_INVALID)
        total_calories = static_cast<FIT_UINT16>(total_calories + lap->GetTotalCalories());

    }
  }

  if (num_active_lengths_without_drills == 0)
    throw std::runtime_error("Session is empty (no lengths)");

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
  session_->SetAvgSpeed(total_distance_without_drills / moving_time_without_drills);
  FIT_FLOAT32 avg_stroke_count = static_cast<FIT_FLOAT32>(total_cycles) /
    static_cast<FIT_FLOAT32>(num_active_lengths_without_drills);
  SessionSetAvgStrokeCount(avg_stroke_count);
  session_->SetAvgStrokeDistance(total_distance_without_drills / static_cast<FIT_FLOAT32>(total_cycles));
  session_->SetMaxSpeed(max_speed);
  FIT_FLOAT32 avgTimePerLength = moving_time_without_drills / num_active_lengths_without_drills;
  SessionSetSwolf(static_cast<FIT_UINT16>(round(avg_stroke_count + avgTimePerLength)));
  session_->SetTotalCalories(total_calories);
  session_->SetTotalDistance(total_distance);
}

