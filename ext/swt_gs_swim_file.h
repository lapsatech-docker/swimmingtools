#if !defined(SWT_GARMIN_SWIM_FILE)
#define SWT_GARMIN_SWIM_FILE
#include "swt_swim_file.h"

namespace swt
{
  class GarminSwimFile: public SwimFile
  {
    public:
      GarminSwimFile() : current_timestamp_(0) {};
      Product GetProduct() const {return Product::kGarminSwim;}
      void CreateNewFile(FIT_DATE_TIME reated, FIT_UINT32Z serial_number, 
          FIT_FLOAT32 pool_length, FIT_DISPLAY_MEASURE pool_length_unit);
      void AddLength(FIT_FLOAT32 time, FIT_UINT16 stroke_count, FIT_SWIM_STROKE stroke, FIT_LENGTH_TYPE type);
      void Pause(FIT_FLOAT32 time, FIT_UINT16 calories);
      void CloseNewFile();
      void Delete(FIT_MESSAGE_INDEX length_index);
      void Save(const std::string &filename, bool convert=false) const;

    private:
      static const FIT_UINT8 kLapAvgStrokeCountFieldNum = 72;
      static const FIT_UINT8 kLapAvgStrokeDistanceFieldNum = 37;
      static const FIT_UINT8 kLapMaxSpeedFieldNum = 14;
      static const FIT_UINT8 kLapMovingTimeFieldNnum = 70;
      static const FIT_UINT8 kLapSwolfFieldNum = 73;
      static const FIT_UINT8 kSessionAvgStrokeCountFieldNum = 79;
      static const FIT_UINT8 kSessionMovingTimeFieldNum = 78;
      static const FIT_UINT8 kSessionNumActiveLengthsFieldNum = 33;
      static const FIT_UINT8 kSessionSwolfFieldNum = 80;
      static const FIT_UINT8 kFileIdLclMesgNum = 0;
      static const FIT_UINT8 kActivityLclMesgNum = 0;
      static const FIT_UINT8 kSessionLclMesgNum = 1;
      static const FIT_UINT8 kEventLclMesgNum = 3;
      static const FIT_UINT8 kLengthLclMesgNum = 5;
      static const FIT_UINT8 kLapLclMesgNum = 2;
      static const FIT_UINT8 kDeviceInfoLclMesgNum = 0;

      std::unique_ptr<fit::LapMesg> current_lap_;
      FIT_DATE_TIME current_timestamp_; 
      fit::LapMesg GetLapTemplate();
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

