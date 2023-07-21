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



