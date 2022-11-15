#include "swt_garmin_generic_swim_file.h"

swt::GarminGenericSwimFile::GarminGenericSwimFile()
{}



void swt::GarminGenericSwimFile::ChangePoolSize(FIT_FLOAT32 new_size_metric, FIT_DISPLAY_MEASURE display_measure) {
  Fr920SwimFile::ChangePoolSize(new_size_metric, display_measure);
  DeletePersonalRecords();
}

void swt::GarminGenericSwimFile::DeletePersonalRecords() {

  const FIT_MESG_NUM kPersonalRecordMsgNum = 113;
  const FIT_UINT8 kPersonalRecordDistanceFiedldNum = 2;
  const FIT_UINT8 kPersonalRecordTimeFieldNum = 3;

  mesgs_.remove_if(
    [] (std::unique_ptr<fit::Mesg> &mesg) {
      if (mesg->GetNum() == kPersonalRecordMsgNum &&
        mesg->HasField(kPersonalRecordDistanceFiedldNum) &&
        mesg->HasField(kPersonalRecordTimeFieldNum))
        return true;
      else
        return false;
    }
  );

}

void swt::GarminGenericSwimFile::Merge(FIT_MESSAGE_INDEX length_index) {
  Fr920SwimFile::Merge(length_index);
  DeletePersonalRecords();

}

void swt::GarminGenericSwimFile::UpdateLap(fit::LapMesg *lap) {
  Fr920SwimFile::UpdateLap(lap);


  // Calculation of calories are a bit different than FR920
  // Everything else is the same
  FIT_UINT16 total_calories = 0;
  FIT_UINT16 first_length_index = lap->GetFirstLengthIndex();
  FIT_UINT16 last_length_index = static_cast<FIT_UINT16>(lap->GetFirstLengthIndex() + 
      lap->GetNumLengths() - 1);
  FIT_UINT16 length_total_calories = FIT_UINT16_INVALID;
  for (int index = first_length_index; index <= last_length_index; index++) {
    fit::LengthMesg *length = lengths_.at(index);
    length_total_calories = length->GetTotalCalories() ;
    if (length_total_calories != FIT_UINT16_INVALID)
      total_calories += length_total_calories;
  }  

  lap->SetTotalCalories(total_calories);
}
 
void swt::GarminGenericSwimFile::UpdateSession() {

  Fr920SwimFile::UpdateSession();
}

