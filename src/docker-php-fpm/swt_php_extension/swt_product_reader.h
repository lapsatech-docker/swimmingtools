#if !defined(SWT_PRODUCT_READER)
#define SWT_PRODUCT_READER
#include <memory>
#include "fit_device_info_mesg_listener.hpp"
#include "fit_file_id_mesg_listener.hpp"
#include "fit_activity_mesg_listener.hpp"
#include "fit_session_mesg_listener.hpp"
#include "swt_swim_file.h"

namespace swt
{
  class ProductReader : public fit::DeviceInfoMesgListener, public fit::FileIdMesgListener,
  public fit::ActivityMesgListener, public fit::SessionMesgListener
  {
    public:
      ProductReader();
      std::unique_ptr<SwimFile> Read(std::istream& istream);
      void OnMesg(fit::FileIdMesg& fileId);
      void OnMesg(fit::DeviceInfoMesg& deviceInfo);
      void OnMesg(fit::ActivityMesg& activity);
      void OnMesg(fit::SessionMesg& activity);

    private:
      const std::string GetSportAsString(FIT_ENUM sport, FIT_ENUM sub_sport) const;

      FIT_FILE file_type_;
      FIT_MANUFACTURER manufacturer_;
      FIT_UINT16 product_;
      std::unique_ptr<fit::ActivityMesg> activity_;
      std::unique_ptr<fit::SessionMesg> session_;

  };
}
#endif

