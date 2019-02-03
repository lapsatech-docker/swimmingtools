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
  product_(FIT_UINT16_INVALID), activity_(nullptr), session_(nullptr)
{}

const std::string swt::ProductReader::GetSportAsString(FIT_ENUM sport, FIT_ENUM sub_sport) const {
  std::string sport_string;
  switch (sport) {
    case FIT_SPORT_RUNNING:
      sport_string = "Running";
      break;
    case FIT_SPORT_CYCLING:
      sport_string = "Cycling";
      break;
    case FIT_SPORT_TRANSITION:
      sport_string = "Transition";
      break;
    default:
      sport_string = "Unknown";
      break;
  }
  if (sport == FIT_SPORT_SWIMMING && sub_sport == FIT_SUB_SPORT_OPEN_WATER)
    sport_string = "Open Water Swimming";
  return sport_string;
}


std::unique_ptr<swt::SwimFile> swt::ProductReader::Read(std::istream &istream) 
{
  std::unique_ptr<SwimFile> swim_file;
  fit::MesgBroadcaster broadcaster;

  broadcaster.AddListener(dynamic_cast<fit::FileIdMesgListener&>(*this));
  broadcaster.AddListener(dynamic_cast<fit::DeviceInfoMesgListener&>(*this));
  broadcaster.AddListener(dynamic_cast<fit::ActivityMesgListener&>(*this));
  broadcaster.AddListener(dynamic_cast<fit::SessionMesgListener&>(*this));

  broadcaster.Run(istream);

  if (file_type_ != FIT_FILE_ACTIVITY) {
    throw FileNotValidException("File is not an activity file");
  }

  FIT_UINT16 numSessions = activity_->GetNumSessions();
  if (numSessions != FIT_UINT16_INVALID && numSessions > 1)
    throw FileNotValidException("Multi sport Files are not supported");

  if (session_->GetSport() != FIT_SPORT_SWIMMING ||
      session_->GetSubSport() != FIT_SUB_SPORT_LAP_SWIMMING) {
    std::string message = "File is not a lap swimming file ("
      + GetSportAsString(session_->GetSport(), session_->GetSubSport())
      + ")";
    throw FileNotValidException(message);

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
  } else if (manufacturer_ == FIT_MANUFACTURER_GARMIN) {
    swim_file.reset(new GarminGenericSwimFile());
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
  // Required to work with Tomtom files which do not contain a deviceInfo mesg
  manufacturer_ = fileId.GetManufacturer();

}

void swt::ProductReader::OnMesg(fit::DeviceInfoMesg& deviceInfo) {
  if (deviceInfo.GetDeviceIndex() == 0) {
    manufacturer_ = deviceInfo.GetManufacturer();
    product_ = deviceInfo.GetProduct();
  }
}

void swt::ProductReader::OnMesg(fit::ActivityMesg& activity) {
  activity_.reset(new fit::ActivityMesg(activity));

}

void swt::ProductReader::OnMesg(fit::SessionMesg& session) {
  session_.reset(new fit::SessionMesg(session));

}

