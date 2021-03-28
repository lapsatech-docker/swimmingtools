#if !defined(SWT_TOMTOM_FILE)
#define SWT_TOMTOM_FILE
#include "swt_swim_file.h"

namespace swt
{
  
  class TomtomSwimFile: public SwimFile
  {
    public:
      TomtomSwimFile() : lap_num_fields_(0), session_num_fields_(0) {};

      void AddMesg(const void *mesg);
      bool CanMerge(FIT_MESSAGE_INDEX length_index, std::string *error) const;
      void Delete(FIT_MESSAGE_INDEX length_index);
      void Initialize();

      FIT_UINT16 GetProduct() const {return kTomtom;}
      void Merge(FIT_MESSAGE_INDEX length_index);
      void Save(const std::string &filename, bool convert=false);

    private:
      const FIT_UINT16 kTomtom = 5;
      void FixRestWithinLength(fit::LengthMesg * length);
      void LengthSetTimestamp(fit::LengthMesg *length, FIT_DATE_TIME timestamp) {};
      void UpdateLap(fit::LapMesg *lap);
      void UpdateSession();
      void UpdateStopTimestamps();

      int lap_num_fields_;
      int session_num_fields_;

  };
}
#endif

