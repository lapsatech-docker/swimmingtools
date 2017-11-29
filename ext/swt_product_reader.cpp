#include "swt_product_reader.h"
#include <fstream>
#include <iostream>
#include "fit_decode.hpp"
#include "fit_mesg_broadcaster.hpp"
#include "fit_file_id_mesg.hpp"
#include "swt_fenix2_swim_file.h"
#include "swt_fr910_swim_file.h"
#include "swt_fr920_swim_file.h"
#include "swt_garmin_generic_swim_file.h"
#include "swt_gs_swim_file.h"
#include "swt_tomtom_swim_file.h"

swt::ProductReader::ProductReader()
: file_type_(FIT_FILE_INVALID), manufacturer_(FIT_MANUFACTURER_INVALID),
  product_(FIT_UINT16_INVALID)
{}

std::unique_ptr<swt::SwimFile> swt::ProductReader::Read(std::istream &istream) 
{
  std::unique_ptr<SwimFile> swim_file;
  fit::MesgBroadcaster broadcaster;

  broadcaster.AddListener(dynamic_cast<fit::FileIdMesgListener&>(*this));
  broadcaster.AddListener(dynamic_cast<fit::DeviceInfoMesgListener&>(*this));

  broadcaster.Run(istream);

  if (file_type_ != FIT_FILE_ACTIVITY) {
    throw FileNotValidException("File is not an activity file");
  }
  if (manufacturer_ == FIT_MANUFACTURER_GARMIN && 
      product_ == FIT_GARMIN_PRODUCT_SWIM) {
    swim_file.reset(new GarminSwimFile());
  } else if (manufacturer_ == FIT_MANUFACTURER_GARMIN 
      && ((product_ == FIT_GARMIN_PRODUCT_FR910XT) ||
        (product_ == FIT_GARMIN_PRODUCT_FR910XT_CHINA) ||
        (product_ == FIT_GARMIN_PRODUCT_FR910XT_JAPAN) ||
        (product_ == FIT_GARMIN_PRODUCT_FR910XT_KOREA))) {
    swim_file.reset(new Fr910SwimFile());
  } else if (manufacturer_ == FIT_MANUFACTURER_GARMIN 
      && product_ == FIT_GARMIN_PRODUCT_FENIX2) {
    swim_file.reset(new Fenix2SwimFile());
  } else if (manufacturer_ == FIT_MANUFACTURER_GARMIN 
      && ((product_ == FIT_GARMIN_PRODUCT_FR920XT) ||
        (product_ == FIT_GARMIN_PRODUCT_FR920XT_TAIWAN) ||
        (product_ == FIT_GARMIN_PRODUCT_FR920XT_CHINA) ||
        (product_ == FIT_GARMIN_PRODUCT_FR920XT_JAPAN))) {
    swim_file.reset(new Fr920SwimFile());
  } else if (manufacturer_ == FIT_MANUFACTURER_TOMTOM) {
    swim_file.reset(new TomtomSwimFile());
  } else if (manufacturer_ == FIT_MANUFACTURER_GARMIN 
      && ((product_ == FIT_GARMIN_PRODUCT_VIVO_ACTIVE) ||
       (product_ == FIT_GARMIN_PRODUCT_VIVO_ACTIVE_APAC) ||
       (product_ == FIT_GARMIN_PRODUCT_VIVO_ACTIVE_HR) ||
       (product_ == FIT_GARMIN_PRODUCT_VIVO_ACTIVE_HR_ASIA))) {
    swim_file.reset(new GarminGenericSwimFile(kGarminVivoActive));
  } else if (manufacturer_ == FIT_MANUFACTURER_GARMIN 
      && ((product_ == FIT_GARMIN_PRODUCT_FENIX3) ||
       (product_ == FIT_GARMIN_PRODUCT_FENIX3_CHINA) ||
       (product_ == FIT_GARMIN_PRODUCT_FENIX3_TWN) ||
       (product_ == FIT_GARMIN_PRODUCT_FENIX3_HR))) {
    swim_file.reset(new GarminGenericSwimFile(kGarminFenix3));
  } else if (manufacturer_ == FIT_MANUFACTURER_GARMIN 
      && product_ == FIT_GARMIN_PRODUCT_EPIX) {
    swim_file.reset(new GarminGenericSwimFile(kGarminEpix));
  } else if (manufacturer_ == FIT_MANUFACTURER_GARMIN 
      && ((product_ == FIT_GARMIN_PRODUCT_FR735) ||
       (product_ == FIT_GARMIN_PRODUCT_FR735_TAIWAN))) {
    swim_file.reset(new GarminGenericSwimFile(kGarminFr735));
  } else if (manufacturer_ == FIT_MANUFACTURER_GARMIN
      && ((product_ == FIT_GARMIN_PRODUCT_FENIX5) ||
        (product_ == FIT_GARMIN_PRODUCT_FENIX5X) ||
        (product_ == FIT_GARMIN_PRODUCT_FENIX5_SAPHIRE) ||
        (product_ == FIT_GARMIN_PRODUCT_FENIX5_UNKNOWN1) ||
        (product_ == FIT_GARMIN_PRODUCT_FENIX5_UNKNOWN2) ||
        (product_ == FIT_GARMIN_PRODUCT_FENIX5S))) {
    swim_file.reset(new GarminGenericSwimFile(kGarminFenix5));
  } else if (manufacturer_ == FIT_MANUFACTURER_GARMIN 
      && ((product_ == FIT_GARMIN_PRODUCT_FR935) ||
        (product_ == FIT_GARMIN_PRODUCT_FR935_UNKNOWN))) {
    swim_file.reset(new GarminGenericSwimFile(kGarminFr935));
  } else if (manufacturer_ == FIT_MANUFACTURER_GARMIN 
      && product_ == FIT_GARMIN_PRODUCT_VIVOACTIVE3) {
    swim_file.reset(new GarminGenericSwimFile(kGarminVivoActive3));
  } else {
    std::string message = "This Device is not supported. See list of supported devices above ("
      + std::to_string(manufacturer_) + "/" 
      + std::to_string(product_) + ")";
    throw FileNotValidException(message);
  }
  return move(swim_file);
}

void swt::ProductReader::OnMesg(fit::FileIdMesg& fileId) {
  file_type_ = fileId.GetType();
  manufacturer_ = fileId.GetManufacturer();
}
void swt::ProductReader::OnMesg(fit::DeviceInfoMesg& deviceInfo) {
  if (deviceInfo.GetDeviceIndex() == 0) {
    manufacturer_ = deviceInfo.GetManufacturer();
    product_ = deviceInfo.GetProduct();
  }
}

