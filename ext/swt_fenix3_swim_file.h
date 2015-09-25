#if !defined(SWT_FENIX3_SWIM_FILE)
#define SWT_FENIX3_SWIM_FILE
#include "swt_fr920_swim_file.h"

namespace swt
{
  class Fenix3SwimFile: public Fr920SwimFile
  {
    public:
      Product GetProduct() const {return Product::kGarminFenix3;}

    private:
      void UpdateLap(fit::LapMesg *lap);
      void UpdateSession();
  };
}
#endif

