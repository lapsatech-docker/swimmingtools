#if !defined(SWT_FR910_SWIM_FILE)
#define SWT_FR910_SWIM_FILE
#include "swt_swim_file.h"


namespace swt
{
  class Fr910SwimFile: public SwimFile
  {
    public:
      Fr910SwimFile() : length_count_(0) {};

      void AddMesg(const void *mesg);
      void Delete(FIT_MESSAGE_INDEX length_index);
      void Initialize();
      void Save(const std::string &filename, bool convert=false) const;
      bool IsDuplicate(FIT_MESSAGE_INDEX length_index) const;

    private:
      static const FIT_UINT8 kLapAvgStrokeDistanceFieldNum = 37;
      static const FIT_UINT8 kLapMaxSpeedFieldNum = 14;
      static const FIT_UINT8 kLengthAvgSpeedFieldNum = 6;
      static const FIT_UINT8 kSessionNumLengthsFieldNum = 33;

      unsigned short length_count_;

      void LengthSetTimestamp(fit::LengthMesg *length, FIT_DATE_TIME timestamp);
      void RepairMissingLengths();
      void SessionSetNumLengths(FIT_UINT16 num_lengths);
      void UpdateLap(fit::LapMesg *lap);
      void UpdateSession();
  };
}
#endif

