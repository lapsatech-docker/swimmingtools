#if !defined(SWT_FR920_SWIM_FILE)
#define SWT_FR920_SWIM_FILE
#include "swt_swim_file.h"

namespace swt
{
  class Fr920SwimFile: public SwimFile
  {
    public:
      Device GetDevice() const {return Device::kGarminFr920;}
      void Delete(FIT_MESSAGE_INDEX length_index);
      void Save(const std::string &filename) const;

    private:
      static const FIT_UINT8 kLapAvgStrokeCountFieldNnum = 90;
      static const FIT_UINT8 kLapAvgStrokeDistanceFieldNum = 37;
      static const FIT_UINT8 kLapMaxSpeedFieldNum = 14;
      static const FIT_UINT8 kLapMovingTimeFieldNnum = 70;
      static const FIT_UINT8 kLapSwolfFieldNum = 73;
      static const FIT_UINT8 kLengthAvgSpeedFieldNum = 6;
      static const FIT_UINT8 kRecordAvgSpeedFieldNum = 6;
      static const FIT_UINT8 kSessionAvgStrokeCountFieldNum = 79;
      static const FIT_UINT8 kSessionMovingTimeFieldNum = 78;
      static const FIT_UINT8 kSessionNumActiveLengthsFieldNum = 33;
      static const FIT_UINT8 kSessionSwolfFieldNum = 80;

      void LapSetAvgStrokeCount(fit::LapMesg *lap, FIT_FLOAT32 avg_stroke_count);
      void LapSetMovingTime(fit::LapMesg *lap, FIT_FLOAT32 moving_time);
      void LapSetSwolf(fit::LapMesg *lap, FIT_UINT16 swolf);
      void SessionSetAvgStrokeCount(FIT_FLOAT32 avg_stroke_count);
      void SessionSetMovingTime(FIT_FLOAT32 moving_time);
      void SessionSetNumActiveLengths(FIT_UINT16 num_active_lengths);
      void SessionSetSwolf(FIT_UINT16 swolf);
      void UpdateLap(fit::LapMesg *lap);
      void UpdateSession();
  };
}
#endif

