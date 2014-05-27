#if !defined(SWT_FENIX2_SWIM_FILE)
#define SWT_FENIX2_SWIM_FILE
#include "swt_swim_file.h"
#include "fit_record_mesg.hpp"

namespace swt
{
  class Fenix2SwimFile: public SwimFile
  {
    public:
      Fenix2SwimFile() : record_(nullptr) {}; 
      void AddMesg(const void *mesg);
      Device GetDevice() const {return Device::kGarminFenix2;}
      void Initialize();
      void Delete(FIT_MESSAGE_INDEX length_index);
      void Save(const std::string &filename) const;

    private:
      static const FIT_UINT8 kLapSwolfFieldNum = 73;
      static const FIT_UINT8 kSessionAvgStrokeCountFieldNum = 79;
      static const FIT_UINT8 kSessionNumLengthsFieldNum = 33;
      static const FIT_UINT8 kSessionSwolfFieldNum = 80;

      void LapSetSwolf(fit::LapMesg *lap, FIT_UINT16 swolf);
      void SessionSetAvgStrokeCount(FIT_FLOAT32 avg_stroke_count);
      void SessionSetNumLengths(FIT_UINT16 num_lengths);
      void SessionSetSwolf(FIT_UINT16 swolf);
      void UpdateLap(fit::LapMesg *lap);
      void UpdateSession();

      fit::RecordMesg *record_;
  };
}
#endif

