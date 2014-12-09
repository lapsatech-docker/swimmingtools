#if !defined(SWT_TOMTOM_FILE)
#define SWT_TOMTOM_FILE
#include "swt_swim_file.h"
#include "fit_activity_mesg.hpp"


namespace swt
{
  class TomtomSwimFile: public SwimFile
  {
    public:
      TomtomSwimFile() : activity_(nullptr) ,lap_num_fields_(0), session_num_fields_(0) {};

      Product GetProduct() const {return Product::kTomtom;}
      bool CanMerge(FIT_MESSAGE_INDEX length_index, std::string *error) const;
      void Delete(FIT_MESSAGE_INDEX length_index);
      void Initialize();
      void Merge(FIT_MESSAGE_INDEX length_index);
      void Save(const std::string &filename) const;

    private:
      void FixRestWithinLength(fit::LengthMesg * length);
      void LengthSetTimestamp(fit::LengthMesg *length, FIT_DATE_TIME timestamp) {};
      void UpdateLap(fit::LapMesg *lap);
      void UpdateSession();
      void UpdateStopTimestamps();

      int lap_num_fields_;
      int session_num_fields_;
      fit::ActivityMesg *activity_;

  };
}
#endif

