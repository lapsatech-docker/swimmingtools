#include "swt_gs_swim_file.h"
#include <cmath>
#include <algorithm>
#include <fstream>
#include "fit_encode.hpp"
#include "fit_device_info_mesg.hpp"
#include "fit_event_mesg.hpp"
#include "fit_file_id_mesg.hpp"
#include "fit_record_mesg.hpp"

void swt::GarminSwimFile::CreateNewFile(FIT_DATE_TIME time_created, FIT_UINT32Z serial_number,  
    FIT_FLOAT32 pool_length, FIT_DISPLAY_MEASURE pool_length_unit) {
  lengths_.empty();
  laps_.empty();
  session_ = nullptr;
  activity_ = nullptr;
  mesgs_.empty();
  current_timestamp_ = time_created;
  serial_number_ = serial_number;

  // FIT_UINT8 file_id_field_nums[6] = {1,2,3,4,5,0};
  std::unique_ptr<fit::FileIdMesg> file_id(new fit::FileIdMesg());
  file_id->SetLocalNum(kFileIdLclMesgNum);
  file_id->SetManufacturer(FIT_MANUFACTURER_GARMIN);
  file_id->SetGarminProduct(FIT_GARMIN_PRODUCT_SWIM);
  file_id->SetSerialNumber(serial_number_);
  file_id->SetTimeCreated(current_timestamp_);
  file_id->SetNumber(FIT_UINT16_INVALID);
  file_id->SetType(FIT_FILE_ACTIVITY);

  mesgs_.push_back(move(file_id));

  //FIT_UINT8 activity_field_nums[5] = {253,5,2,3,4};
  std::unique_ptr<fit::ActivityMesg> activity(new fit::ActivityMesg());

  activity->SetLocalNum(kActivityLclMesgNum);
  activity->SetTimestamp(FIT_DATE_TIME_INVALID);
  activity->SetLocalTimestamp(FIT_DATE_TIME_INVALID);
  activity->SetType(FIT_ACTIVITY_MANUAL);
  activity->SetEvent(FIT_EVENT_ACTIVITY);
  activity->SetEventType(FIT_EVENT_TYPE_STOP);
  activity_ = activity.get();
  mesgs_.push_back(move(activity));


  //FIT_UINT8 session_field_nums[28] = {254,253,2,7,8,9,10,11,13,14,15,25,26,33,44,78,79,80,0,1,5,6,18,19,27,28,43,46};
  std::unique_ptr<fit::SessionMesg> session(new fit::SessionMesg());

  session->SetLocalNum(kSessionLclMesgNum);
  session->SetMessageIndex(0);
  session->SetTimestamp(FIT_DATE_TIME_INVALID);
  session->SetStartTime(current_timestamp_);
  session->SetTotalElapsedTime(0);
  session->SetTotalTimerTime(0);
  session->SetTotalDistance(0);
  session->SetTotalCycles(0);
  session->SetTotalCalories(FIT_UINT16_INVALID);
  session->SetTotalFatCalories(FIT_UINT16_INVALID);
  session->SetAvgSpeed(FIT_FLOAT32_INVALID);

  fit::Field max_speed("session", "max_speed"); 
  max_speed.SetUINT32Value(FIT_UINT32_INVALID);
  session->AddField(max_speed);

  session->SetFirstLapIndex(0);
  session->SetNumLaps(1);

  fit::Field num_active_lengths(FIT_MESG_NUM_SESSION, kSessionNumActiveLengthsFieldNum, FIT_BASE_TYPE_UINT16);
  num_active_lengths.SetUINT16Value(FIT_UINT16_INVALID);
  session->AddField(num_active_lengths);

  session->SetPoolLength(pool_length);

  fit::Field moving_time(FIT_MESG_NUM_SESSION, kSessionMovingTimeFieldNum, FIT_BASE_TYPE_UINT32);
  moving_time.SetUINT32Value(FIT_UINT32_INVALID);
  session->AddField(moving_time);

  fit::Field avg_stroke_count(FIT_MESG_NUM_SESSION, kSessionAvgStrokeCountFieldNum, FIT_BASE_TYPE_UINT16);
  avg_stroke_count.SetUINT16Value(FIT_UINT16_INVALID);
  session->AddField(avg_stroke_count);

  fit::Field swolf(FIT_MESG_NUM_SESSION, kSessionSwolfFieldNum, FIT_BASE_TYPE_UINT16);
  swolf.SetUINT16Value(FIT_UINT16_INVALID);
  session->AddField(swolf);

  session->SetEvent(FIT_EVENT_SESSION);
  session->SetEventType(FIT_EVENT_TYPE_STOP);
  session->SetSport(FIT_SPORT_SWIMMING);
  session->SetSubSport(FIT_SUB_SPORT_LAP_SWIMMING);
  session->SetAvgCadence(FIT_UINT8_INVALID);
  session->SetMaxCadence(FIT_UINT8_INVALID);
  session->SetEventGroup(0);
  session->SetTrigger(FIT_SESSION_TRIGGER_ACTIVITY_END);
  session->SetSwimStroke(FIT_SWIM_STROKE_INVALID);
  session->SetPoolLengthUnit(pool_length_unit);

  session_ = session.get();
  mesgs_.push_back(move(session));


  //FIT_UINT8 event_field_nums[5] = {253,3,0,1,4};
  std::unique_ptr<fit::EventMesg> event(new fit::EventMesg());

  event->SetLocalNum(kEventLclMesgNum);
  event->SetTimestamp(current_timestamp_);
  event->SetData(FIT_TIMER_TRIGGER_MANUAL);
  event->SetEvent(FIT_EVENT_TIMER);
  event->SetEventType(FIT_EVENT_TYPE_START);
  event->SetEventGroup(0);
  mesgs_.push_back(move(event));

  current_lap_.reset(new fit::LapMesg(GetLapTemplate()));
}

fit::LapMesg swt::GarminSwimFile::GetLapTemplate() {
    //FIT_UINT8 lap_field_nums[26] = {254,253,2,7,8,9,10,11,12,13,14,32,35,37,40,70,72,73,0,1,17,18,24,25,38,64};
    fit::LapMesg lap;

    lap.SetLocalNum(kLapLclMesgNum);
    lap.SetMessageIndex(laps_.size());
    lap.SetTimestamp(FIT_DATE_TIME_INVALID);
    lap.SetStartTime(current_timestamp_);
    lap.SetTotalElapsedTime(0);
    lap.SetTotalTimerTime(0);
    lap.SetTotalDistance(FIT_FLOAT32_INVALID);
    lap.SetTotalCycles(FIT_UINT32_INVALID);
    lap.SetTotalCalories(FIT_UINT16_INVALID);
    lap.SetTotalFatCalories(FIT_UINT16_INVALID);
    lap.SetAvgSpeed(FIT_FLOAT32_INVALID);
    lap.SetMaxSpeed(FIT_FLOAT32_INVALID);
    lap.SetNumLengths(0);
    lap.SetFirstLengthIndex(lengths_.size());
    lap.SetAvgStrokeDistance(FIT_FLOAT32_INVALID);
    lap.SetNumActiveLengths(FIT_UINT16_INVALID);

    fit::Field moving_time(FIT_MESG_NUM_LAP, kLapMovingTimeFieldNnum, FIT_BASE_TYPE_UINT32);
    moving_time.SetUINT32Value(FIT_UINT32_INVALID);
    lap.AddField(moving_time);

    fit::Field avg_stroke_count(FIT_MESG_NUM_LAP, kLapAvgStrokeCountFieldNum, FIT_BASE_TYPE_UINT16);
    avg_stroke_count.SetUINT16Value(FIT_UINT16_INVALID);
    lap.AddField(avg_stroke_count);

    fit::Field swolf(FIT_MESG_NUM_LAP, kLapSwolfFieldNum, FIT_BASE_TYPE_UINT16);
    swolf.SetUINT16Value(FIT_UINT16_INVALID);
    lap.AddField(swolf);

    lap.SetEvent(FIT_EVENT_LAP);
    lap.SetEventType(FIT_EVENT_TYPE_STOP);
    lap.SetAvgCadence(FIT_UINT8_INVALID);
    lap.SetMaxCadence(FIT_UINT8_INVALID);
    lap.SetLapTrigger(FIT_LAP_TRIGGER_MANUAL);
    lap.SetSport(FIT_SPORT_SWIMMING);
    lap.SetSwimStroke(FIT_SWIM_STROKE_INVALID);

    return lap;
}

void swt::GarminSwimFile::AddLength(FIT_FLOAT32 time, FIT_UINT16 stroke_count, FIT_SWIM_STROKE stroke, FIT_LENGTH_TYPE type) {
  std::unique_ptr<fit::LengthMesg> length(new fit::LengthMesg());
  // 254,253,2,3,4,5,6,0,1,7,9,12,13
  if (type == FIT_LENGTH_TYPE_ACTIVE) {
    length->SetLocalNum(kLengthLclMesgNum);
    length->SetMessageIndex(lengths_.size());
    length->SetTimestamp(FIT_DATE_TIME_INVALID);
    length->SetStartTime(current_timestamp_);
    length->SetTotalElapsedTime(time);
    length->SetTotalTimerTime(time);
    length->SetTotalStrokes(stroke_count);
    length->SetAvgSpeed(session_->GetPoolLength() / time);
    length->SetEvent(FIT_EVENT_LENGTH);
    length->SetEventType(FIT_EVENT_TYPE_MARKER);
    length->SetSwimStroke(stroke);
    length->SetAvgSwimmingCadence(static_cast<FIT_UINT8>(round(60 * stroke_count / time)));
    length->SetLengthType(type);
  } else {
    length->SetLocalNum(kLengthLclMesgNum);
    length->SetMessageIndex(lengths_.size());
    length->SetTimestamp(FIT_DATE_TIME_INVALID);
    length->SetStartTime(current_timestamp_);
    length->SetTotalElapsedTime(time);
    length->SetTotalTimerTime(time);
    length->SetTotalStrokes(FIT_UINT16_INVALID);
    length->SetAvgSpeed(0);
    length->SetEvent(FIT_EVENT_LENGTH);
    length->SetEventType(FIT_EVENT_TYPE_STOP);
    length->SetSwimStroke(FIT_SWIM_STROKE_INVALID);
    length->SetAvgSwimmingCadence(FIT_UINT8_INVALID);
    length->SetLengthType(type);
  }
  current_timestamp_ +=  static_cast<FIT_DATE_TIME>(round(time));
  length->SetTimestamp(current_timestamp_);
  current_lap_->SetNumLengths(current_lap_->GetNumLengths() + 1);
  current_lap_->SetTotalElapsedTime(current_lap_->GetTotalElapsedTime() + time);
  current_lap_->SetTotalTimerTime(current_lap_->GetTotalTimerTime() + time);
  session_->SetTotalElapsedTime(session_->GetTotalElapsedTime() + time);
  session_->SetTotalTimerTime(session_->GetTotalTimerTime() + time);

  lengths_.push_back(length.get());
  mesgs_.push_back( move(length));
}

void swt::GarminSwimFile::Pause(FIT_FLOAT32 time, FIT_UINT16 calories) {

  if (time > 0) {
    std::unique_ptr<fit::EventMesg> event(new fit::EventMesg());

    event->SetLocalNum(kEventLclMesgNum);
    event->SetTimestamp(current_timestamp_);
    event->SetData(FIT_TIMER_TRIGGER_MANUAL);
    event->SetEvent(FIT_EVENT_TIMER);
    event->SetEventType(FIT_EVENT_TYPE_STOP_ALL);
    event->SetEventGroup(0);
    mesgs_.push_back(move(event));

    current_lap_->SetTimestamp(current_timestamp_);
    current_lap_->SetTotalCalories(calories);
    current_lap_->SetTotalFatCalories(calories);
    laps_.push_back(current_lap_.get());
    mesgs_.push_back(move(current_lap_));

    event.reset(new fit::EventMesg());
    event->SetLocalNum(kEventLclMesgNum);
    event->SetTimestamp(current_timestamp_ + static_cast<FIT_DATE_TIME>(round(time)));
    event->SetData(FIT_TIMER_TRIGGER_MANUAL);
    event->SetEvent(FIT_EVENT_TIMER);
    event->SetEventType(FIT_EVENT_TYPE_START);
    event->SetEventGroup(0);
    mesgs_.push_back(move(event));

    current_lap_.reset(new fit::LapMesg(GetLapTemplate()));

    AddLength(time, FIT_UINT16_INVALID, FIT_SWIM_STROKE_INVALID, FIT_LENGTH_TYPE_IDLE);
    current_lap_->SetTimestamp(current_timestamp_);
    current_lap_->SetNumLengths(0);
    current_lap_->SetFirstLengthIndex(FIT_UINT16_INVALID);

    laps_.push_back(current_lap_.get());
    mesgs_.push_back(move(current_lap_));

  } else {
    current_lap_->SetTimestamp(current_timestamp_);
    current_lap_->SetTotalCalories(calories);
    current_lap_->SetTotalFatCalories(calories);
    laps_.push_back(current_lap_.get());
    mesgs_.push_back(move(current_lap_));

  }
  current_lap_.reset(new fit::LapMesg(GetLapTemplate()));
}
  
void swt::GarminSwimFile::CloseNewFile() {

  if (current_lap_->GetNumLengths() > 0) {
    current_lap_->SetTimestamp(current_timestamp_);
    laps_.push_back(current_lap_.get());
    mesgs_.push_back(move(current_lap_));
  }
  //FIT_UINT8 device_info_field_nums[26] = {253,2,3,4,5,7,10,13,14,15,16,0,1,6,9,11};
  std::unique_ptr<fit::DeviceInfoMesg> device_info(new fit::DeviceInfoMesg());

  device_info->SetLocalNum(kDeviceInfoLclMesgNum);
  device_info->SetTimestamp(current_timestamp_);
  device_info->SetManufacturer(FIT_MANUFACTURER_GARMIN);
  device_info->SetSerialNumber(serial_number_);
  device_info->SetProduct(FIT_GARMIN_PRODUCT_SWIM);
  device_info->SetSoftwareVersion(3.6);
  device_info->SetDeviceIndex(0);
  device_info->SetDeviceType(FIT_ANTPLUS_DEVICE_TYPE_ANTFS);
  mesgs_.push_back(move(device_info));

  session_->SetTimestamp(current_timestamp_);
  session_->SetNumLaps(laps_.size());
  activity_->SetTimestamp(current_timestamp_);
  activity_->SetLocalTimestamp(current_timestamp_);
  Recalculate();
}

void swt::GarminSwimFile::Delete(FIT_MESSAGE_INDEX length_index) {
  std::string error;

  if (!CanSplitChangeStrokeDelete(length_index, &error))
    throw std::runtime_error(error);

  fit::LengthMesg *length = lengths_.at(length_index);
  length->SetAvgSpeed(0);
  length->SetAvgSwimmingCadence(FIT_UINT8_INVALID);
  length->SetEventType(FIT_EVENT_TYPE_STOP);
  length->SetLengthType(FIT_LENGTH_TYPE_IDLE);
  length->SetSwimStroke(FIT_SWIM_STROKE_INVALID);
  length->SetTotalStrokes(FIT_UINT16_INVALID);

  UpdateLap(GetLap(length_index));
  UpdateSession();
}

void swt::GarminSwimFile::Save(const std::string &filename, bool convert/*=false*/) const {
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

      if (length->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE) {
        active_length_counter++;
        fit::RecordMesg record;
        record.SetTimestamp(length->GetTimestamp());
        record.SetDistance(static_cast<FIT_FLOAT32>(active_length_counter) * 
            session_->GetPoolLength());
        record.SetSpeed(length->GetAvgSpeed());
        encode.Write(*length);
        encode.Write(record);
      } else if (length->GetLengthType() == FIT_LENGTH_TYPE_IDLE) { 
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


void swt::GarminSwimFile::LapSetAvgStrokeCount(fit::LapMesg *lap, FIT_FLOAT32 avg_stroke_count) {
  if (software_version_ > 300) {
    lap->SetFieldUINT16Value(kLapAvgStrokeCountFieldNum, 
        static_cast<FIT_UINT16>(round(avg_stroke_count * 10)));
  }
}

void swt::GarminSwimFile::LapSetMovingTime(fit::LapMesg *lap, FIT_FLOAT32 moving_time) {
  lap->SetFieldUINT32Value(kLapMovingTimeFieldNnum, 
      static_cast<FIT_UINT32>(round(moving_time * 1000)));
}

void swt::GarminSwimFile::LapSetSwolf(fit::LapMesg *lap, FIT_UINT16 swolf) {
  if (software_version_ > 300) 
    lap->SetFieldUINT16Value(kLapSwolfFieldNum, swolf);
}

void swt::GarminSwimFile::SessionSetAvgStrokeCount(FIT_FLOAT32 avg_stroke_count) {
  if (software_version_ > 300) {
    session_->SetFieldUINT16Value(kSessionAvgStrokeCountFieldNum, 
        static_cast<FIT_UINT16>(round(avg_stroke_count * 10)));
  }
}

void swt::GarminSwimFile::SessionSetNumActiveLengths(FIT_UINT16 num_active_lengths) {
  session_->SetFieldUINT16Value(kSessionNumActiveLengthsFieldNum, num_active_lengths);
}

void swt::GarminSwimFile::SessionSetMovingTime(FIT_FLOAT32 moving_time) {
  session_->SetFieldUINT32Value(kSessionMovingTimeFieldNum, 
      static_cast<FIT_UINT32>(round(moving_time * 1000)));
}

void swt::GarminSwimFile::SessionSetSwolf(FIT_UINT16 swolf) {
  if (software_version_ > 300) 
    session_->SetFieldUINT16Value(kSessionSwolfFieldNum, swolf);
}

void swt::GarminSwimFile::UpdateLap(fit::LapMesg *lap) {
  FIT_UINT16 num_active_lengths = 0;
  FIT_FLOAT32 moving_time = 0;
  FIT_UINT32 total_cycles = 0;
  bool is_swim_stroke_init = false ; // Allows to perform swimStroke initialization on first length
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

  if (num_active_lengths == 0) {// Active lap must be converted to Rest Lap
    lap->SetNumActiveLengths(0);
    LapSetMovingTime(lap, 0);
    lap->SetTotalCycles(FIT_UINT32_INVALID);
    lap->SetSwimStroke(FIT_SWIM_STROKE_INVALID);

    lap->SetAvgCadence(0);
    lap->SetAvgSpeed(0);
    LapSetAvgStrokeCount(lap, 0);
    lap->SetFieldUINT16Value(kLapAvgStrokeDistanceFieldNum, FIT_UINT16_INVALID);
    lap->SetMaxCadence(0);
    lap->SetFieldUINT16Value(kLapMaxSpeedFieldNum, FIT_UINT16_INVALID);
    LapSetSwolf(lap, 0);
    lap->SetTotalCalories(FIT_UINT16_INVALID);
    lap->SetTotalDistance(0);
    lap->SetTotalFatCalories(FIT_UINT16_INVALID);

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
      lap->SetMaxCadence(max_cadence);
      lap->SetMaxSpeed(max_speed);
      FIT_FLOAT32 avg_time_per_length = moving_time / num_active_lengths;
      LapSetSwolf(lap, static_cast<FIT_UINT16>(round(avg_stroke_count + avg_time_per_length)));
    }
  }
}

void swt::GarminSwimFile::UpdateSession() {
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
  FIT_UINT16 total_fat_calories = 0;

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
      total_fat_calories =  static_cast<FIT_UINT16>(total_fat_calories + 
          lap->GetTotalFatCalories());
    }
  }

  if (num_active_lengths_without_drills == 0)
    throw std::runtime_error("Session is empty (no lengths)");

  total_distance = num_active_lengths * session_->GetPoolLength();
  total_distance_without_drills = num_active_lengths_without_drills * session_->GetPoolLength();

  SessionSetNumActiveLengths(num_active_lengths);
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
  session_->SetMaxCadence(max_cadence);
  FIT_FLOAT32 avgTimePerLength = moving_time_without_drills / num_active_lengths_without_drills;
  SessionSetSwolf(static_cast<FIT_UINT16>(round(avg_stroke_count + avgTimePerLength))); // GS only
  session_->SetTotalCalories(total_calories);
  session_->SetTotalDistance(total_distance);
  session_->SetTotalFatCalories(total_fat_calories);
}

