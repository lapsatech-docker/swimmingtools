#if !defined(SWT_FILE_READER)
#define SWT_FILE_READER
#include "fit_mesg_listener.hpp"
#include "swt_swim_file.h"

namespace swt
{
  class FileReader : public fit::MesgListener
  {
    public:
      std::unique_ptr<SwimFile> Read(const std::string& filename);
      void OnMesg(fit::Mesg& mesg);

    private:
      std::unique_ptr<SwimFile> swim_file;
  };
}
#endif

