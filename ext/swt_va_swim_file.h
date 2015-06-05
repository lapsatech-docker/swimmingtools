#if !defined(SWT_VA_SWIM_FILE)
#define SWT_VA_SWIM_FILE
#include "swt_fr920_swim_file.h"

namespace swt
{
  class VaSwimFile: public Fr920SwimFile
  {
    public:
      Product GetProduct() const {return Product::kGarminVivoActive;}

    private:
      void UpdateLap(fit::LapMesg *lap);
      void UpdateSession();
  };
}
#endif

