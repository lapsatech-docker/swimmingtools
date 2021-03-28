#if !defined(SWT_FR920_SWIM_FILE)
#define SWT_FR920_SWIM_FILE
#include "swt_swim_file.h"
#include "fit_record_mesg.hpp"

namespace swt
{
  class Fr920SwimFile: public SwimFile
  {
    public:
      Fr920SwimFile();
      void AddMesg(const void *mesg);
      void Delete(FIT_MESSAGE_INDEX length_index);
      void Initialize();
      void Save(const std::string &filename, bool convert=false);
      void Split(FIT_MESSAGE_INDEX length_index);

    private:
      static const FIT_UINT8 kLapAvgStrokeCountFieldNnum = 90;
      static const FIT_UINT8 kLapAvgStrokeDistanceFieldNum = 37;
      static const FIT_UINT8 kLapAvgSpeedFieldNum = 14;
      static const FIT_UINT8 kLapEnhancedAvgSpeedFieldNum = 124;
      static const FIT_UINT8 kLapMaxSpeedFieldNum = 15;
      static const FIT_UINT8 kLapEnhancedMaxSpeedFieldNum = 125;
      static const FIT_UINT8 kLapMovingTimeFieldNnum = 70;
      static const FIT_UINT8 kLapSwolfFieldNum = 73;
      static const FIT_UINT8 kLengthAvgSpeedFieldNum = 6;
      static const FIT_UINT8 kRecordAvgSpeedFieldNum = 6;
      static const FIT_UINT8 kRecordTemperatureFieldNum = 13;
      static const FIT_UINT8 kSessionAvgSpeedFieldNum = 14;
      static const FIT_UINT8 kSessionEnhancedAvgSpeedFieldNum = 124;
      static const FIT_UINT8 kSessionMaxSpeedFieldNum = 15;
      static const FIT_UINT8 kSessionEnhancedMaxSpeedFieldNum = 125 ;
      static const FIT_UINT8 kSessionAvgStrokeCountFieldNum = 79;
      static const FIT_UINT8 kSessionMovingTimeFieldNum = 78;
      static const FIT_UINT8 kSessionNumLengthsInActiveLapsFieldNum = 33;
      static const FIT_UINT8 kSessionSwolfFieldNum = 80;
      static const FIT_UINT8 kTimestampFieldNum = 253;

      void CheckIndexes();
      void LapSetAvgStrokeCount(fit::LapMesg *lap, FIT_FLOAT32 avg_stroke_count);
      void LapSetMovingTime(fit::LapMesg *lap, FIT_FLOAT32 moving_time);
      void LapSetSwolf(fit::LapMesg *lap, FIT_UINT16 swolf);
      void RepairIndexes();
      void SessionSetAvgStrokeCount(FIT_FLOAT32 avg_stroke_count);
      void SessionSetMovingTime(FIT_FLOAT32 moving_time);
      void SessionSetNumLengthsInActiveLaps(FIT_UINT16 num_lengths_in_active_laps);
      void SessionSetSwolf(FIT_UINT16 swolf);
      void UpdateRecords();

    protected:
      void UpdateLap(fit::LapMesg *lap);
      void UpdateSession();

  };
}
#endif
