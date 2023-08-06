#if !defined(SWT_GARMIN_GENERIC_SWIM_FILE)
#define SWT_GARMIN_GENERIC_SWIM_FILE
#include "swt_fr920_swim_file.h"

namespace swt
{
  class GarminGenericSwimFile: public Fr920SwimFile
  {
    public:
      GarminGenericSwimFile();
      void Merge(FIT_MESSAGE_INDEX length_index);
      void ChangePoolSize(FIT_FLOAT32 new_size_metric, FIT_DISPLAY_MEASURE display_measure);


    private:
      void DeletePersonalRecords();
  };
}
#endif

