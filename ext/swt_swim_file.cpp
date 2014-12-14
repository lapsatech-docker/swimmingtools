#include "swt_swim_file.h"
#include <cmath>
#include <ctime>
#include <algorithm>
#include "fit_device_info_mesg.hpp"
#include "fit_event_mesg.hpp"

swt::SwimFile::SwimFile()
: session_(nullptr), serial_number_(FIT_UINT32Z_INVALID),
  software_version_(FIT_UINT16_INVALID)
{}


void swt::SwimFile::AddMesg(const void *mesg)
{
  const fit::Mesg* fit_mesg = reinterpret_cast<const fit::Mesg *>(mesg);

  switch(fit_mesg->GetNum())
  {
    case FIT_MESG_NUM_ACTIVITY:
      {
        std::unique_ptr<fit::ActivityMesg> activity(new fit::ActivityMesg(*fit_mesg));
        this->activity_ = activity.get();
        mesgs_.push_back(move(activity));
        break;
      }
    case FIT_MESG_NUM_SESSION:
      {
        std::unique_ptr<fit::SessionMesg> session(new fit::SessionMesg(*fit_mesg));
        this->session_ = session.get();
        mesgs_.push_back(move(session));
        break;
      }
    case FIT_MESG_NUM_LAP:
      {
        std::unique_ptr<fit::LapMesg> lap(new fit::LapMesg(*fit_mesg));
        laps_.push_back(lap.get());
        mesgs_.push_back(move(lap));
        break;
      }
    case FIT_MESG_NUM_LENGTH:
      {
        std::unique_ptr<fit::LengthMesg> length(new fit::LengthMesg(*fit_mesg));
        lengths_.push_back(length.get());
        mesgs_.push_back(move(length));
        break;
      }
    case FIT_MESG_NUM_DEVICE_INFO:
      {
        std::unique_ptr<fit::DeviceInfoMesg> device_info(new fit::DeviceInfoMesg(*fit_mesg));
        if (device_info->GetDeviceIndex() == 0) {
          software_version_ = device_info->GetFieldUINT16Value(kSoftwareVersionFieldNum);
          serial_number_  = device_info->GetSerialNumber();
        }
        mesgs_.push_back(move(device_info));
        break;
      }
    case FIT_MESG_NUM_EVENT:
      {
        std::unique_ptr<fit::EventMesg> event(new fit::EventMesg(*fit_mesg));
        if (event->GetEvent() == FIT_EVENT_TIMER && event->GetEventType() == FIT_EVENT_TYPE_STOP_ALL)
          timer_stop_timestamps_.push_back(event->GetTimestamp());
        mesgs_.push_back(move(event));
        break;
      }
    case FIT_MESG_NUM_RECORD:
      // Do nothing, we'll recreate Record mesgs when saving file
      break;

    default:
      mesgs_.push_back(std::unique_ptr<fit::Mesg>(new fit::Mesg(*fit_mesg)));

  }
}

bool swt::SwimFile::CanMerge(FIT_MESSAGE_INDEX length_index, std::string *error) const {
  *error ="";

  // -2 because we merge 2 lengths (length_index being the first) so the last length can't be merged
  if (length_index > (lengths_.size() - 2)) {
    *error = "Length doesn't exist or is the last length";
  } else {
    fit::LengthMesg *first_length = lengths_.at(length_index);
    fit::LengthMesg *second_length = lengths_.at(length_index + 1);

    if ((first_length->GetLengthType() != FIT_LENGTH_TYPE_ACTIVE) ||
        (second_length->GetLengthType() != FIT_LENGTH_TYPE_ACTIVE)) {
      *error = "Both Length to be merge must be an Active length (not rest)";
    } else if ((first_length->GetSwimStroke() == FIT_SWIM_STROKE_DRILL) ||
        (second_length->GetSwimStroke() == FIT_SWIM_STROKE_DRILL)) {
      *error = "Cannot merge drill lengths";
    } else if (GetLap(length_index) !=
        GetLap(static_cast<FIT_MESSAGE_INDEX>(length_index + 1))) {
      *error = "Both length must be in the same lap";
    } else {

      for (FIT_DATE_TIME timer_stop_timestamp : timer_stop_timestamps_) {
        if ((timer_stop_timestamp > first_length->GetStartTime()) &&
            (timer_stop_timestamp < second_length->GetStartTime()))
          *error = "Timer was stopped between lengths";
      }
    }
  }
  return error->empty();
}

bool swt::SwimFile::CanSplitChangeStrokeDelete(FIT_MESSAGE_INDEX length_index, std::string *error) const {
  *error = "";

  if (length_index > (lengths_.size() - 1)) {
    *error = "Length doesn't exist";
  } else {
    if (lengths_.at(length_index)->GetLengthType() != FIT_LENGTH_TYPE_ACTIVE)
      *error = "Length to be edited must be an Active length (not rest)";
    else if (lengths_.at(length_index)->GetSwimStroke() == FIT_SWIM_STROKE_DRILL)
      *error = "Cannot edit  drill lengths";
  }
  return error->empty();
}

void swt::SwimFile::ChangeStroke(FIT_MESSAGE_INDEX length_index, FIT_SWIM_STROKE new_stroke, ChangeStrokeOption option) {
  std::string error;
  if (!CanSplitChangeStrokeDelete(length_index, &error))
    throw std::runtime_error(error);

  fit::LapMesg *lap = GetLap(length_index);

  switch (option) {
    case ChangeStrokeOption::kLengthOnly:
      {
        lengths_.at(length_index)->SetSwimStroke(new_stroke);

        // if we change the stroke for a length, the lap stroke may be impacted,
        // if all the lengths in the same lap are of the same stroke as the new stroke,
        // lap Stroke will be updated to the new stroke, if not lap stroke will
        // be set to mixed
        FIT_SWIM_STROKE lap_stroke = new_stroke;

        for (int index = lap->GetFirstLengthIndex();
            index < lap->GetFirstLengthIndex() + lap->GetNumLengths(); index++) {
          if (lengths_.at(index)->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE) {
            if (lengths_.at(index)->GetSwimStroke() != new_stroke)
              lap_stroke = FIT_SWIM_STROKE_MIXED;
          }
        }
        lap->SetSwimStroke(lap_stroke);
      }
      break;

    case ChangeStrokeOption::kLap:

      for (int index = lap->GetFirstLengthIndex();
          index < lap->GetFirstLengthIndex() + lap->GetNumLengths(); index++) {
        if (lengths_.at(index)->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE)
          lengths_.at(index)->SetSwimStroke(new_stroke);
      }
      lap->SetSwimStroke(new_stroke);
      break;

    case ChangeStrokeOption::kAll:

      for (fit::LengthMesg *length : lengths_) {
        if (length->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE) {
          // we never change the stroke for drill lengths
          if (length->GetSwimStroke() != FIT_SWIM_STROKE_DRILL)
            length->SetSwimStroke(new_stroke);
        }
      }

      for (fit::LapMesg *lap : laps_) {
        if (lap->GetNumActiveLengths() > 0) {
          // We never change the stroke for drill Lap
          if (lap->GetSwimStroke() != FIT_SWIM_STROKE_DRILL)
            lap->SetSwimStroke(new_stroke);
        }
      }
      break;
  }

  // Adjust Session Swim stroke
  FIT_SWIM_STROKE session_stroke = new_stroke;

  for (fit::LapMesg *lap : laps_) {
    if (lap->GetNumActiveLengths() > 0) {
      if (session_stroke != lap->GetSwimStroke())
        session_stroke = FIT_SWIM_STROKE_MIXED;
    }
  }
  session_->SetSwimStroke(session_stroke);
}

void swt::SwimFile::ChangePoolSize(FIT_FLOAT32 new_size_metric, FIT_DISPLAY_MEASURE display_measure) {

  if (new_size_metric < 16.45 || new_size_metric > 150)
    throw std::runtime_error("Pool length must be within 17-150m or 18-159y");

  session_->SetPoolLength(new_size_metric);
  session_->SetPoolLengthUnit(display_measure);

  for (fit::LengthMesg *length : lengths_) {
    if (length->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE &&
        length->GetSwimStroke() != FIT_SWIM_STROKE_DRILL) {
      length->SetAvgSpeed(new_size_metric / length->GetTotalTimerTime());
    }
  }

  for(fit::LapMesg * lap : laps_) {
    if (lap->GetNumActiveLengths() > 0)
      UpdateLap(lap);
  }
  UpdateSession();
}

fit::LapMesg* swt::SwimFile::GetLap(FIT_MESSAGE_INDEX length_index) const {
  for (fit::LapMesg *lap : laps_) {
    if (lap->GetFirstLengthIndex() != FIT_UINT16_INVALID)  {
      if ((length_index >= lap->GetFirstLengthIndex()) &&
          (length_index < (lap->GetFirstLengthIndex() + lap->GetNumLengths()))) {
        return lap;
      }
    }
  }
  std::string error = "Cannot Get Lap for length " + std::to_string(length_index);
  throw std::runtime_error(error);
}

// Compute rest time. Some watches compute rest time and add rest
// lenths to the file, this rest is confirmed. If the watch doesn't
// compute rest, we have to estimate rest time between lengths by
// substracting  the time the length ends from the time the next
// length starts
double swt::SwimFile::GetRestTime(FIT_MESSAGE_INDEX length_index) const {

  fit::LengthMesg *length = lengths_.at(length_index);
  FIT_MESSAGE_INDEX next_length = length_index + 1;
  bool confirmed = false;
  double confirmed_rest = 0;
  double estimated_rest = 0;
  double rest = 0;
  while (next_length < lengths_.size() &&
      (lengths_.at(next_length)->GetLengthType() == FIT_LENGTH_TYPE_IDLE)) {
    confirmed_rest += lengths_.at(next_length)->GetTotalTimerTime();
    confirmed = true;
    next_length++;
  }
  if (next_length < lengths_.size()) {
    estimated_rest  = static_cast<double>(lengths_.at(next_length)->GetStartTime()) -
      (static_cast<double>(length->GetStartTime()) +
       static_cast<double>(length->GetTotalTimerTime()));
  }
  // stop button was pressed betwean the two lengths_, even if there
  // are no rest length, the rest is confirmed
  for (FIT_DATE_TIME timer_stop_timestamp : timer_stop_timestamps_) {
    if (next_length < lengths_.size() &&
        (timer_stop_timestamp > length->GetStartTime()) &&
        (timer_stop_timestamp < lengths_.at(next_length)->GetStartTime()))
      confirmed = true;
  }

  // Next length is not in the same lap, so lap button was pressed.
  // Even if there is no rest length, the rest is confirmed
  if (next_length < lengths_.size() &&
      GetLap(next_length) != GetLap(next_length))
    confirmed = true;

  // Drill length by definition ara always consecutive and never
  // have rest in between. In Garmin swim all drill from an
  // interval have same start time so estimated rest value
  // is irrelevant
  if (length->GetSwimStroke() == FIT_SWIM_STROKE_DRILL) {
    confirmed_rest = estimated_rest = 0;
    confirmed = true;
  }

  if (confirmed)
    rest = estimated_rest > confirmed_rest ? estimated_rest : confirmed_rest;
  else
    rest = 0;

  return rest;
}

const std::string swt::SwimFile::GetSportAsString(FIT_ENUM sport, FIT_ENUM sub_sport) const {
  std::string sport_string;
  switch (sport) {
    case FIT_SPORT_RUNNING:
      sport_string = "Running";
      break;
    case FIT_SPORT_CYCLING:
      sport_string = "Cycling";
      break;
    case FIT_SPORT_TRANSITION:
      sport_string = "Transition";
      break;
    default:
      sport_string = "Unknown";
      break;
  }
  if (sport == FIT_SPORT_SWIMMING && sub_sport == FIT_SUB_SPORT_OPEN_WATER)
    sport_string = "Open Water Swimming";
  return sport_string;
}

bool swt::SwimFile::IsValid(std::string *error) const {
  *error = "";

  if (session_->GetSport() != FIT_SPORT_SWIMMING ||
      session_->GetSubSport() != FIT_SUB_SPORT_LAP_SWIMMING) {
    *error = "File is not a lap swimming file ("
      + GetSportAsString(session_->GetSport(), session_->GetSubSport())
      + ")";
  } else {

    bool hasActiveLength = false;

    for (fit::LengthMesg *length : lengths_)
    {
      if (length->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE) {
        hasActiveLength = true;
        break;
      }
    }

    if (!hasActiveLength)
      *error = "File is empty (no lengths)";
    else if (session_->GetSwimStroke() == FIT_SWIM_STROKE_DRILL)
      *error = "File contains drills only (cannot be edited)";
    else if (lengths_.size() != (lengths_.at(lengths_.size()-1)->GetMessageIndex() + 1))
      *error = "File failed Validation";
  }
  return error->empty();
}


void swt::SwimFile::LengthSetTimestamp(fit::LengthMesg *length, FIT_DATE_TIME timestamp)
{
  length->SetTimestamp(timestamp);
}


void swt::SwimFile::Merge(FIT_MESSAGE_INDEX length_index) {
  std::string error;
  if (!CanMerge(length_index, &error))
    throw std::runtime_error(error);

  fit::LengthMesg *first_length = lengths_.at(length_index);
  fit::LengthMesg *second_length = lengths_.at(length_index + 1);

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

  fit::LapMesg *the_lap = GetLap(length_index);
  the_lap->SetNumLengths(static_cast<FIT_UINT16>(the_lap->GetNumLengths() - 1));
  UpdateLap(the_lap);

  for (fit::LapMesg *lap : laps_) {
    if ((lap->GetMessageIndex() > the_lap->GetMessageIndex()) &&
        (lap->GetFirstLengthIndex() != FIT_UINT16_INVALID)) {
      lap->SetFirstLengthIndex(static_cast<FIT_UINT16>(lap->GetFirstLengthIndex() - 1));
    }
  }
  UpdateSession();
}

void swt::SwimFile::Recalculate()
{
  for (fit::LapMesg* lap : laps_) {
    if (lap->GetNumActiveLengths() > 0) {
      UpdateLap(lap);
    }
  }
  UpdateSession();
}






void swt::SwimFile::Split(FIT_MESSAGE_INDEX length_index) {
  std::string error;

  if (!CanSplitChangeStrokeDelete(length_index, &error))
    throw std::runtime_error(error);

  fit::LengthMesg *first_length = lengths_.at(length_index);
  first_length->SetTotalElapsedTime(first_length->GetTotalElapsedTime() / 2);
  first_length->SetTotalTimerTime(first_length->GetTotalTimerTime() / 2);
  FIT_DATE_TIME timestamp = first_length->GetStartTime() +
    static_cast<FIT_DATE_TIME>(first_length->GetTotalTimerTime());
  LengthSetTimestamp(first_length, timestamp);
  FIT_UINT16 totalStrokes = first_length->GetTotalStrokes();
  first_length->SetTotalStrokes(totalStrokes / 2); // Integer Division
  first_length->SetAvgSpeed(session_->GetPoolLength() / first_length->GetTotalTimerTime());
  first_length->SetAvgSwimmingCadence(static_cast<FIT_UINT8>
      (round(60.0 * first_length->GetTotalStrokes() /
             first_length->GetTotalTimerTime())));

  for (fit::LengthMesg *length : lengths_) {
    if (length->GetMessageIndex() > length_index) {
      length->SetMessageIndex(static_cast<FIT_MESSAGE_INDEX>
          (length->GetMessageIndex() + 1));
    }
  }

  std::unique_ptr<fit::LengthMesg> second_length(new fit::LengthMesg(*first_length));
  second_length->SetMessageIndex(static_cast<FIT_MESSAGE_INDEX>
      (first_length->GetMessageIndex() + 1));
  second_length->SetStartTime(timestamp);  // second length start when first end
  timestamp = second_length->GetStartTime() +
    static_cast<FIT_DATE_TIME>(second_length->GetTotalTimerTime());
  LengthSetTimestamp(second_length.get(), timestamp);
  second_length->SetTotalStrokes(static_cast<FIT_UINT16>((totalStrokes / 2) +
        (totalStrokes % 2))); // Add mod to preserve stroke count when odd
  second_length->SetAvgSwimmingCadence(static_cast<FIT_UINT8>
      (round(60.0 * second_length->GetTotalStrokes() /
             second_length->GetTotalTimerTime())));

  lengths_.insert(lengths_.begin() + length_index + 1, second_length.get());
  std::list<std::unique_ptr<fit::Mesg>>::iterator it;
  it = std::find_if(mesgs_.begin(), mesgs_.end(),
      [first_length] (const std::unique_ptr<fit::Mesg> &mesg) {
      return mesg.get() == first_length;});
  ++it;
  mesgs_.insert(it, move(second_length));

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
