#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "fit_encode.hpp"
#include "fit_event_mesg.hpp"
#include "fit_record_mesg.hpp"
#include "swt_tomtom_swim_file.h"

bool swt::TomtomSwimFile::CanMerge(FIT_MESSAGE_INDEX length_index, std::string *error) const {
  *error ="";

  // -2 because we merge 2 lengths (length_index being the first) so the last length can't be merged
  if (length_index > (lengths_.size() - 2)) {
    *error = "Length doesn't exist or is the last length";
  } else {
    fit::LengthMesg *first_length = lengths_.at(length_index);
    fit::LengthMesg *second_length = lengths_.at(length_index + 1);


    for (FIT_DATE_TIME timer_stop_timestamp : timer_stop_timestamps_) {
      if ((timer_stop_timestamp > first_length->GetStartTime()) &&
          (timer_stop_timestamp < second_length->GetStartTime()))
        *error = "Timer was stopped between lengths";
    }
  }
  return error->empty();
}

void swt::TomtomSwimFile::Delete(FIT_MESSAGE_INDEX length_index) {
  std::string error;

  if (!CanSplitChangeStrokeDelete(length_index, &error))
    throw std::runtime_error(error);

  fit::LengthMesg *length = lengths_.at(length_index);
  
  std::unique_ptr<fit::EventMesg> stop(new fit::EventMesg());
  stop->SetTimestamp(length->GetStartTime());
  stop->SetEvent(FIT_EVENT_TIMER);
  stop->SetEventType(FIT_EVENT_TYPE_STOP_ALL);
  std::unique_ptr<fit::EventMesg> start(new fit::EventMesg());
  start->SetTimestamp(length->GetStartTime() + static_cast<FIT_UINT32>(length->GetTotalTimerTime()));
  start->SetEvent(FIT_EVENT_TIMER);
  start->SetEventType(FIT_EVENT_TYPE_START);

  lengths_.erase(lengths_.begin() + length_index);

  for (fit::LengthMesg *length : lengths_) {
    if (length->GetMessageIndex() > length_index)
      length->SetMessageIndex(static_cast<FIT_MESSAGE_INDEX>(length->GetMessageIndex() - 1));
  }

  std::list<std::unique_ptr<fit::Mesg>>::iterator l = 
    std::find_if(mesgs_.begin(), mesgs_.end(), 
        [length] (std::unique_ptr<fit::Mesg> &mesg) {return mesg.get() == length;});

  mesgs_.insert(l, move(stop));
  mesgs_.insert(l, move(start));
  mesgs_.erase(l);

  fit::LapMesg *the_lap = GetLap(length_index);
  the_lap->SetNumLengths(the_lap->GetNumLengths() - 1);
  for (fit::LapMesg *lap : laps_) {
    if ((lap->GetMessageIndex() > the_lap->GetMessageIndex()) &&
        (lap->GetFirstLengthIndex() != FIT_UINT16_INVALID)) {
      lap->SetFirstLengthIndex(static_cast<FIT_UINT16>(lap->GetFirstLengthIndex() - 1));
    }
  }
  UpdateLap(the_lap);
  UpdateSession();
  UpdateStopTimestamps();
}

void swt::TomtomSwimFile::FixRestWithinLength(fit::LengthMesg * length) {
  fit::EventMesg *stop_event = NULL;
  fit::EventMesg *start_event = NULL;
  FIT_DATE_TIME length_start = length->GetStartTime();
  FIT_DATE_TIME length_end = length_start + static_cast<FIT_DATE_TIME>(length->GetTotalElapsedTime());
  for (const std::unique_ptr<fit::Mesg> &mesg : mesgs_) {
    if (typeid(*mesg) == typeid(fit::EventMesg)) {
      fit::EventMesg *event = dynamic_cast<fit::EventMesg *>(mesg.get());
      if (event->GetEvent() == FIT_EVENT_TIMER &&
          event->GetEventType() == FIT_EVENT_TYPE_STOP_ALL &&
          event->GetTimestamp() >= length_start && 
          event->GetTimestamp() < length_end) {
        stop_event = event;
      }
      if (event->GetEvent() == FIT_EVENT_TIMER &&
          event->GetEventType() == FIT_EVENT_TYPE_START &&
          event->GetTimestamp() > length_start && 
          event->GetTimestamp() <= length_end) {
        start_event = event;
      }
    }
  }
  
  if (stop_event == NULL || start_event == NULL)
    throw std::runtime_error("Tomtom can't find Rest within length");

  FIT_DATE_TIME before = stop_event->GetTimestamp() - length_start;
  FIT_DATE_TIME after = length_end - start_event->GetTimestamp();

  if (before > after) {
    start_event->SetTimestamp(length_end);
    length->SetTotalTimerTime(before);
    length->SetTotalElapsedTime(before);
  } else {
    length->SetStartTime(start_event->GetTimestamp());
    stop_event->SetTimestamp(length_start);
    length->SetTotalTimerTime(after);
    length->SetTotalElapsedTime(after);
  }
  UpdateStopTimestamps();

}



void swt::TomtomSwimFile::Initialize() {

  if (session_->GetTotalCycles() != lengths_.at(lengths_.size() - 1)->GetTotalStrokes())
    throw std::runtime_error("Tomtom num strokes is not cumulative");

  FIT_UINT32 cumulative_total_strokes = lengths_.at(0)->GetTotalStrokes();

  for (unsigned int i = 1; i < lengths_.size(); i++) {
    fit::LengthMesg *length = lengths_.at(i);
    if (length->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE) {
      FIT_UINT32 total_strokes = length->GetTotalStrokes();
      if (total_strokes != FIT_UINT32_INVALID) {
        length->SetTotalStrokes(total_strokes - cumulative_total_strokes);
        cumulative_total_strokes += (total_strokes - cumulative_total_strokes);
      }

      if (length->GetTotalElapsedTime() > length->GetTotalTimerTime()) {
        FixRestWithinLength(length);
      }
    } else {
      throw std::runtime_error("Rest length in tomtom file");
    }
  }
  FIT_UINT16 first_length_index = FIT_UINT16_INVALID;
  FIT_MESSAGE_INDEX lap_message_index = 0;
  for (const std::unique_ptr<fit::Mesg> &mesg : mesgs_) {
    if (typeid(*mesg) == typeid(fit::LengthMesg)) {
      if (first_length_index == FIT_UINT16_INVALID) {
        fit::LengthMesg *length = dynamic_cast<fit::LengthMesg*>(mesg.get());
        first_length_index = length->GetMessageIndex();
      }
    } else if (typeid(*mesg) == typeid(fit::LapMesg)) {
      lap_num_fields_ = mesg->GetNumFields();
      fit::LapMesg *lap = dynamic_cast<fit::LapMesg*>(mesg.get());
      lap->SetMessageIndex(lap_message_index++);
      lap->SetFirstLengthIndex(first_length_index);
      first_length_index = FIT_UINT16_INVALID;
    } else if (typeid(*mesg) == typeid(fit::SessionMesg)) {
      session_num_fields_ = mesg->GetNumFields();
    } else if (mesg->GetNum() == FIT_MESG_NUM_ACTIVITY) {
      activity_ = reinterpret_cast<fit::ActivityMesg*>(mesg.get());
    }
  }
  Recalculate();
}

void swt::TomtomSwimFile::Merge(FIT_MESSAGE_INDEX length_index) {
  std::string error;
  if (!CanMerge(length_index, &error))
    throw std::runtime_error(error);

  fit::LengthMesg *first_length = lengths_.at(length_index);
  fit::LengthMesg *second_length = lengths_.at(length_index + 1);
  fit::LapMesg *first_length_lap = GetLap(length_index);
  fit::LapMesg *second_length_lap = GetLap(length_index + 1);


  LengthSetTimestamp(first_length, second_length->GetTimestamp());
  first_length->SetTotalElapsedTime(first_length->GetTotalElapsedTime() +
      second_length->GetTotalElapsedTime());
  first_length->SetTotalTimerTime(first_length->GetTotalTimerTime() +
      second_length->GetTotalTimerTime());
  first_length->SetTotalStrokes(static_cast<FIT_UINT16>
      (first_length->GetTotalStrokes() + second_length->GetTotalStrokes()));
  first_length->SetAvgSpeed(session_->GetPoolLength() / first_length->GetTotalTimerTime());
  first_length->SetAvgSwimmingCadence(static_cast<FIT_UINT8>
      (round(60.0 * first_length->GetTotalStrokes() /
             first_length->GetTotalTimerTime())));

  mesgs_.remove_if([second_length] (std::unique_ptr<fit::Mesg> &mesg)
      {return mesg.get() == second_length;});
  lengths_.erase(lengths_.begin() + length_index + 1);

  for (fit::LengthMesg *length : lengths_) {
    if (length->GetMessageIndex() > length_index)
      length->SetMessageIndex(static_cast<FIT_MESSAGE_INDEX>(length->GetMessageIndex() - 1));
  }

  fit::LapMesg *affected_lap = first_length_lap;
  if (first_length_lap != second_length_lap) 
    affected_lap = second_length_lap;

  affected_lap->SetNumLengths(static_cast<FIT_UINT16>(affected_lap->GetNumLengths() - 1));

  for (fit::LapMesg *lap : laps_) {
    if ((lap->GetMessageIndex() > affected_lap->GetMessageIndex()) &&
        (lap->GetFirstLengthIndex() != FIT_UINT16_INVALID)) {
      lap->SetFirstLengthIndex(static_cast<FIT_UINT16>(lap->GetFirstLengthIndex() - 1));
    }
  }
  UpdateLap(first_length_lap);
  UpdateLap(second_length_lap);
  UpdateSession();
}

void swt::TomtomSwimFile::Save(const std::string &filename) const {
  fit::Encode encode;
  std::fstream fit_file(filename, 
      std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);

  if (!fit_file.is_open())
    throw std::runtime_error("Error opening file"); 

  encode.Open(fit_file);

  FIT_FLOAT32 moving_time = 0;
  for(fit::LengthMesg *length : lengths_) {
    if (length->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE)
      moving_time += (length->GetTotalTimerTime() + 1);
  }

  FIT_FLOAT32 calories_increment = moving_time / session_->GetTotalCalories();
  FIT_FLOAT32 cumulative_distance = 0;
  FIT_UINT16 cumulative_moving_time = 0;
  FIT_UINT16 cumulative_strokes = 0;
  FIT_DATE_TIME current_timestamp = 0 ;

  for (const std::unique_ptr<fit::Mesg> &mesg : mesgs_) {
    if (typeid(*mesg) == typeid(fit::LengthMesg)) {
      fit::LengthMesg *length = dynamic_cast<fit::LengthMesg*>(mesg.get());

      cumulative_strokes += length->GetTotalStrokes();
      length->SetTotalStrokes(cumulative_strokes);

      FIT_FLOAT32 speed  = length->GetAvgSpeed();
      current_timestamp = current_timestamp > length->GetStartTime() ? current_timestamp : length->GetStartTime();
      FIT_DATE_TIME end_time = current_timestamp + static_cast<FIT_UINT32>(length->GetTotalTimerTime());

      FIT_FLOAT32 distance_increment = session_->GetPoolLength() / (end_time - current_timestamp + 1);

      while (current_timestamp <= end_time) {

        cumulative_distance += distance_increment;
        cumulative_moving_time++;
        fit::RecordMesg record;
        record.SetTimestamp(current_timestamp++);
        record.SetDistance(cumulative_distance);
        record.SetSpeed(speed);
        record.SetCalories(cumulative_moving_time / calories_increment);
        encode.Write(record);
      }
      encode.Write(*length);
    } else if (typeid(*mesg) == typeid(fit::LapMesg)) {
      fit::LapMesg tomtom_lap(mesg->GetNum());
      tomtom_lap.SetLocalNum(mesg->GetLocalNum());
      for (int i = 0; i < lap_num_fields_; i++) {
        tomtom_lap.AddField(*(mesg->GetFieldByIndex(i)));
      }
      tomtom_lap.SetTotalCycles(cumulative_strokes);
      encode.Write(tomtom_lap);
    } else if (typeid(*mesg) == typeid(fit::SessionMesg)) {
      fit::SessionMesg tomtom_session(mesg->GetNum());
      tomtom_session.SetLocalNum(mesg->GetLocalNum());
      for (int i = 0; i < session_num_fields_; i++) {
        tomtom_session.AddField(*(mesg->GetFieldByIndex(i)));
      }
      encode.Write(tomtom_session);
    } else { 
      encode.Write(*mesg);
    }
  }

  if (!encode.Close())
    throw std::runtime_error("Error writing file");

  fit_file.close();
}

void swt::TomtomSwimFile::UpdateLap(fit::LapMesg *lap) {
  FIT_UINT16 num_active_lengths = 0;
  FIT_FLOAT32 moving_time = 0;
  FIT_UINT32 total_cycles = 0;

  FIT_UINT8 max_cadence = 0;
  FIT_FLOAT32 max_speed = 0;
  FIT_FLOAT32 total_distance = 0;
  FIT_UINT16 total_calories = 0;

  if (lap->GetNumLengths() == 0) {
    FIT_MESSAGE_INDEX lap_message_index = lap->GetMessageIndex();
    laps_.erase(laps_.begin() + lap_message_index);

    for (fit::LapMesg *lap : laps_) {
      if (lap->GetMessageIndex() > lap_message_index)
        lap->SetMessageIndex(static_cast<FIT_MESSAGE_INDEX>(lap->GetMessageIndex() - 1));
    }

    std::list<std::unique_ptr<fit::Mesg>>::iterator it = 
      std::find_if(mesgs_.begin(), mesgs_.end(), 
          [lap] (std::unique_ptr<fit::Mesg> &mesg) {return mesg.get() == lap;});

    mesgs_.erase(it);
  } else {

    FIT_UINT16 first_length_index = lap->GetFirstLengthIndex();
    FIT_UINT16 last_length_index = static_cast<FIT_UINT16>(lap->GetFirstLengthIndex() + 
        lap->GetNumLengths() - 1);

    for (int index = first_length_index; index <= last_length_index; index++) {
      fit::LengthMesg *length = lengths_.at(index);

      num_active_lengths++;
      moving_time += length->GetTotalTimerTime();
      total_cycles += length->GetTotalStrokes();

      if (length->GetAvgSpeed() != FIT_FLOAT32_INVALID && 
          max_speed < length->GetAvgSpeed()) {
        max_speed = length->GetAvgSpeed();
      }
      if (length->GetAvgSwimmingCadence() != FIT_UINT8_INVALID && 
          max_cadence < length->GetAvgSwimmingCadence()) {
        max_cadence = length->GetAvgSwimmingCadence();
      }
      if (length->GetTotalCalories() != FIT_UINT16_INVALID) 
        total_calories += length->GetTotalCalories();
    }

    total_distance = num_active_lengths * session_->GetPoolLength();
    lap->SetTotalTimerTime(moving_time);
    lap->SetNumActiveLengths(num_active_lengths);
    lap->SetTotalCycles(total_cycles);
    lap->SetTotalDistance(total_distance);

    lap->SetAvgCadence(static_cast<FIT_UINT8>
        (round(static_cast<FIT_FLOAT32>(total_cycles) / moving_time * 60)));
    lap->SetAvgSpeed(total_distance / moving_time);
    lap->SetAvgStrokeDistance(total_distance / static_cast<FIT_FLOAT32>(total_cycles));
    lap->SetMaxCadence(max_cadence);
    lap->SetMaxSpeed(max_speed);
    lap->SetTotalCalories(total_calories);
  }
}


void swt::TomtomSwimFile::UpdateSession() {
  FIT_UINT16 num_active_lengths = 0;
  FIT_FLOAT32 moving_time = 0;
  FIT_UINT32 total_cycles = 0;
  FIT_FLOAT32 max_speed = 0;
  FIT_UINT8 max_cadence = 0;
  FIT_UINT16 total_calories = 0;
  FIT_FLOAT32 total_distance = 0;

  for (fit::LengthMesg *length: lengths_)
  {
    num_active_lengths++;
    moving_time += length->GetTotalTimerTime();
    total_cycles += length->GetTotalStrokes();

    if (length->GetAvgSpeed() != FIT_FLOAT32_INVALID && 
        max_speed < length->GetAvgSpeed()) {
      max_speed = length->GetAvgSpeed();
    }
    if (length->GetAvgSwimmingCadence() != FIT_UINT8_INVALID && 
        max_cadence < length->GetAvgSwimmingCadence()) {
      max_cadence = length->GetAvgSwimmingCadence();
    }
    if (length->GetTotalCalories() != FIT_UINT16_INVALID) 
      total_calories += length->GetTotalCalories();
  }

  if (num_active_lengths == 0)
    throw std::runtime_error("Session is empty (no lengths)");

  total_distance = num_active_lengths * session_->GetPoolLength();
  session_->SetTotalTimerTime(moving_time);
  session_->SetNumActiveLengths(num_active_lengths);
  session_->SetTotalCycles(total_cycles);

  session_->SetAvgCadence(static_cast<FIT_UINT8>
      (round(static_cast<FIT_FLOAT32>(total_cycles) /
             moving_time * 60)));
  session_->SetAvgSpeed(total_distance / moving_time);
  FIT_FLOAT32 avg_stroke_count = static_cast<FIT_FLOAT32>(total_cycles) / 
    static_cast<FIT_FLOAT32>(num_active_lengths);
  session_->SetAvgStrokeCount(avg_stroke_count);
  session_->SetAvgStrokeDistance(total_distance / static_cast<FIT_FLOAT32>(total_cycles));
  session_->SetMaxCadence(max_cadence);
  session_->SetMaxSpeed(max_speed);
  session_->SetNumLaps(laps_.size());
  session_->SetTotalCalories(total_calories);
  session_->SetTotalDistance(total_distance);

  activity_->SetTotalTimerTime(moving_time);
}


void swt::TomtomSwimFile::UpdateStopTimestamps() {
  timer_stop_timestamps_.empty();
  for (const std::unique_ptr<fit::Mesg> &mesg : mesgs_) {
    if (typeid(*mesg) == typeid(fit::EventMesg)) {
      fit::EventMesg *event = dynamic_cast<fit::EventMesg *>(mesg.get());
      if (event->GetEvent() == FIT_EVENT_TIMER && event->GetEventType() == FIT_EVENT_TYPE_STOP_ALL) {
        timer_stop_timestamps_.push_back(event->GetTimestamp());
      }
    }
  }
}

