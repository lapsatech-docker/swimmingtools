#include "swt_file_reader.h"
#include <fstream>
#include "fit_decode.hpp"
#include "fit_file_id_mesg.hpp"
#include "swt_fr910_swim_file.h"
#include "swt_gs_swim_file.h"
#include "swt_fenix2_swim_file.h"

std::unique_ptr<swt::SwimFile> swt::FileReader::Read(const std::string &filename) {
  std::ifstream istream;
  istream.open(filename, std::ios::in | std::ios::binary);
  fit::Decode decode;

  if (!istream.is_open())
    throw std::runtime_error("Error opening file"); 

  if (!decode.CheckIntegrity(istream))
    throw FileNotValidException("File is not a FIT file or is corrupted");

  swim_file.reset();
  decode.Read(istream, *this);
  if (swim_file) {
    std::string error;
    if (!swim_file->IsValid(&error))
      throw FileNotValidException(error);
    swim_file->Initialize();
  }

  return move(swim_file);
}

void swt::FileReader::OnMesg(fit::Mesg& mesg) {
  if (swim_file != nullptr) {
    swim_file->AddMesg(&mesg);
  } else if (swim_file == nullptr && mesg.GetNum() == FIT_MESG_NUM_FILE_ID) {

    fit::FileIdMesg fileId(mesg);

    if (fileId.GetType() != FIT_FILE_ACTIVITY) {
      throw FileNotValidException("File is not an activity file");
    }
    if (fileId.GetProduct() == Device::kGarminSwim) {
      swim_file.reset(new GarminSwimFile());
      swim_file->AddMesg(&mesg);
    } else if (fileId.GetProduct() == Device::kGarminFr910) {
      swim_file.reset(new Fr910SwimFile());
      swim_file->AddMesg(&mesg);
    } else if (fileId.GetProduct() == Device::kGarminFenix2) {
      swim_file.reset(new Fenix2SwimFile());
      swim_file->AddMesg(&mesg);
    } else {
      std::string message = "This application is compatible with Garmin Swim/FR910/Fenix 2 only ("
        + std::to_string(fileId.GetManufacturer()) + "/" 
        + std::to_string(fileId.GetProduct()) + ")";
      throw FileNotValidException(message);
    }
  } 
  else if (swim_file == nullptr && mesg.GetNum() != FIT_MESG_NUM_FILE_ID) {
    throw FileNotValidException("File is not a FIT file or is corrupted");
  }
}
