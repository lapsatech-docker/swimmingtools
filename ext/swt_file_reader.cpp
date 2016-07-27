#include "swt_file_reader.h"
#include <fstream>
#include "fit_decode.hpp"
#include "swt_product_reader.h"

std::unique_ptr<swt::SwimFile> swt::FileReader::Read(const std::string &filename) {
  std::ifstream istream;
  istream.open(filename, std::ios::in | std::ios::binary);
  fit::Decode decode;

  if (!istream.is_open())
    throw std::runtime_error("Error opening file"); 

  if (!decode.CheckIntegrity(istream))
    throw FileNotValidException("File is not a FIT file or is corrupted");

  ProductReader pr;
  swim_file = pr.Read(istream);
  
  if (swim_file) {
    decode.Read(istream, *this);

    // Garmin store heart rate Data after the end of the activity file
    // Hearth Rate Data appear as fit file following the activity file
    // within the same file, so if we are not at the end of the file after
    // reading the activity file. It probably means there is Heart rate
    // Data following, so we will load it so that we can restore when 
    // saving the file back after editing.
    if (!istream.eof())
    {
      swim_file->LoadHrData(istream);
    }
    std::string error;
    if (!swim_file->IsValid(&error))
      throw FileNotValidException(error);
    swim_file->Initialize();
  }

  return move(swim_file);
}

void swt::FileReader::OnMesg(fit::Mesg& mesg) {
    swim_file->AddMesg(&mesg);
}
