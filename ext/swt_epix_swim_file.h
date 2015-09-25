#if !defined(SWT_EPIX_SWIM_FILE)
#define SWT_EPIX_SWIM_FILE
#include "swt_fr920_swim_file.h"

namespace swt
{
  class EpixSwimFile: public Fr920SwimFile
  {
    public:
      Product GetProduct() const {return Product::kGarminEpix;}

    private:
      void UpdateLap(fit::LapMesg *lap);
      void UpdateSession();
  };
}
#endif

