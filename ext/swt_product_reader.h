#if !defined(SWT_PRODUCT_READER)
#define SWT_PRODUCT_READER
#include "fit_device_info_mesg_listener.hpp"
#include "fit_file_id_mesg_listener.hpp"
#include "swt_swim_file.h"

namespace swt
{
  class ProductReader : public fit::DeviceInfoMesgListener, public fit::FileIdMesgListener
  {
    public:
      ProductReader();
      std::unique_ptr<SwimFile> Read(std::istream& istream);
      void OnMesg(fit::FileIdMesg& fileId);
      void OnMesg(fit::DeviceInfoMesg& deviceInfo);

    private:
      FIT_FILE file_type_;
      FIT_MANUFACTURER manufacturer_;
      FIT_UINT16 product_;

  };
}
#endif

