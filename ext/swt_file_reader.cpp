#include "swt_file_reader.h"
#include <fstream>
#include "fit_decode.hpp"
#include "fit_file_id_mesg.hpp"
#include "swt_fenix2_swim_file.h"
#include "swt_fr910_swim_file.h"
#include "swt_fr920_swim_file.h"
#include "swt_gs_swim_file.h"
#include "swt_product_reader.h"
#include "swt_tomtom_swim_file.h"
#include "swt_va_swim_file.h"

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
    swim_file->AddMesg(&mesg);
}
