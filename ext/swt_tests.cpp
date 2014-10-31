#include "swt_tests.h"
#include <cmath>
#include <memory>
#include <string>
#include <iostream>
#include <dirent.h>
#include "fit_decode.hpp"
#include "fit_device_info_mesg.hpp"
#include "fit_file_id_mesg.hpp"
#include "fit_profile.hpp"
#include "libgen.h"
#include "swt_swim_file.h"
#include "swt_file_reader.h"

using std::endl;

swt::Tests::Tests() {
  log.open("test.csv");

  TestSwimFile();

  //DIR *dp;
  //struct dirent *ep;

  //dp = opendir("../fit_files/fr920");
  //if (dp != NULL) {
  //  
  //  while (ep = readdir(dp)) 
  //  {
  //    if (ep->d_type == DT_REG) {
  //      std::string filename = "../fit_files/fr920/";
  //      filename += ep->d_name;
  //      std::cout << filename << std::endl;
  //      CheckUpdateLapAndSession(filename);
  //   }
  //  }
  //  closedir(dp);
  //}
}

swt::Tests::~Tests() {
  log.close();
}

void swt::Tests::CheckUpdateLapAndSession(std::string file) {
  bool error = false;    
  std::string filename(file);
  size_t last_slash = filename.find_last_of('/');
  if (last_slash != std::string::npos) 
    filename = filename.substr(last_slash + 1);
  FileReader file_reader;
  std::unique_ptr<SwimFile> fit_file; 

  FIT_UINT16 device  = FIT_UINT16_INVALID;
  FIT_FLOAT32 version = FIT_FLOAT32_INVALID;
  fit::SessionMesg session_before;
  std::vector<fit::LapMesg> laps_before;
  int  length_count = 0;
  ReadFile(file, &device, &version);

  for (fit::Mesg mesg : mesgs_) {
    if (mesg.GetNum() == FIT_MESG_NUM_SESSION) {
      session_before = fit::SessionMesg(mesg);
    } else if (mesg.GetNum() == FIT_MESG_NUM_LAP) {
      laps_before.push_back(fit::LapMesg(mesg));
    } else if (mesg.GetNum() == FIT_MESG_NUM_LENGTH) {
        length_count++;
    } 
  }

  bool length_count_error = false;
  // Check for missing length bug in Fr910
  if (device == kGarminFr910 && length_count != session_before.GetFieldUINT16Value(33)) {
    log << filename << "," << device << "," << version << ",Num length check failed," 
      << "," << session_before.GetFieldUINT16Value(33) << "," << length_count << std::endl;
    length_count_error = true;
  }

  try {
    fit_file = file_reader.Read(file); 
  }
  catch (const std::exception &e)     {
    log << filename << "," << device << "," << version << "," << e.what() << endl;
  }

  if (fit_file && !length_count_error) {

    std::string msg;

    fit_file->Recalculate();

    const fit::SessionMesg *session_after = fit_file->GetSession();
    const std::vector<fit::LapMesg*>  &laps_after = fit_file->GetLaps();

    if (abs(session_before.GetAvgCadence() - session_after->GetAvgCadence()) > 1) {

      log << filename << "," << device << "," << version << ",session,avg_cadence," 
        << static_cast<int>(session_before.GetAvgCadence()) << "," 
        << static_cast<int>(session_after->GetAvgCadence()) << endl;
    }
    if (fabs(session_before.GetAvgSpeed() - session_after->GetAvgSpeed()) > .01) {

      log << filename << "," << device << "," << version << ",session,avg_speed," 
        << session_before.GetAvgSpeed() << "," 
        << session_after->GetAvgSpeed() << endl;
    }
    if (fabs(session_before.GetAvgStrokeCount() - session_after->GetAvgStrokeCount()) > .11) { 

      log << filename << "," << device << "," << version << ",session,avg_stroke_count," 
        << session_before.GetAvgStrokeCount() << ","  << session_after->GetAvgStrokeCount() 
        << endl;
    }
    if (fabs(session_before.GetAvgStrokeDistance() - session_after->GetAvgStrokeDistance()) > .011) { 

      log << filename << "," << device << "," << version << ",session,avg_stroke_distance," 
        << session_before.GetAvgStrokeDistance() << ","  << session_after->GetAvgStrokeDistance() 
        << endl;
    }
    if (session_before.GetFirstLapIndex() != session_after->GetFirstLapIndex()) {

      log << filename << "," << device << "," << version << ",session,first_lap_index," 
        << session_before.GetFirstLapIndex() << ","  << session_after->GetFirstLapIndex() 
        << endl;
    }
    if (abs((session_before.GetMaxCadence() -  session_after->GetMaxCadence())) > 0) {

      log << filename << "," << device << "," << version << ",session,max_cadence," 
        << static_cast<int>(session_before.GetMaxCadence()) << "," 
        << static_cast<int>(session_after->GetMaxCadence()) << endl;
    }
    if (fabs(session_before.GetMaxSpeed() - session_after->GetMaxSpeed()) > .01) {

      log << filename << "," << device << "," << version << ",session ,max_speed," 
        << session_before.GetMaxSpeed() << "," << session_after->GetMaxSpeed() << endl;
    }
    if (session_before.GetNumActiveLengths() != session_after->GetNumActiveLengths()) {

      log << filename << "," << device << "," << version << ",session,num_active_lengths," 
        << session_before.GetNumActiveLengths() << ","  << session_after->GetNumActiveLengths() 
        << endl;
    }
    if (session_before.GetNumLaps() != session_after->GetNumLaps()) {
      log << filename << "," << device << "," << version << ",session,num_laps," 
        << session_before.GetNumLaps() << ","  << session_after->GetNumLaps() << endl;
    }
    if (fabs(session_before.GetPoolLength() - session_after->GetPoolLength()) > .01) {
      log << filename << "," << device << "," << version << ",session,pool_length," 
        << session_before.GetPoolLength() << ","  << session_after->GetPoolLength() << endl;
    }
    if (session_before.GetPoolLengthUnit() != session_after->GetPoolLengthUnit()) {

      log << filename << "," << device << "," << version << ",session,pool_length_unit," 
        << static_cast<int>(session_before.GetPoolLengthUnit()) << "," 
        << static_cast<int>(session_after->GetPoolLengthUnit()) << endl;
    }
    if (session_before.GetSwimStroke() != session_after->GetSwimStroke()) {

      log << filename << "," << device << "," << version << ",session,swim_stroke," 
        << static_cast<int>(session_before.GetSwimStroke()) << "," 
        << static_cast<int>(session_after->GetSwimStroke()) << endl;
    }
    if (abs(session_before.GetTotalCalories() - session_after->GetTotalCalories()) > 1) {

      log << filename << "," << device << "," << version << ",session,total_calories," 
        << session_before.GetTotalCalories() << "," << session_after->GetTotalCalories() 
        << endl;
    }
    if (abs(session_before.GetTotalCycles() - session_after->GetTotalCycles()) > 0 ) {

      log << filename << "," << device << "," << version << ",session,total_cycles," 
        << session_before.GetTotalCycles() << "," << session_after->GetTotalCycles() 
        << endl;
    }
    if (fabs(session_before.GetTotalDistance() - session_after->GetTotalDistance()) > .01)
    {
      log << filename << "," << device << "," << version << ",session,total_distance," 
        << session_before.GetTotalDistance() << "," << session_after->GetTotalDistance() 
        << endl;
    }
    if (abs(session_before.GetTotalFatCalories() - session_after->GetTotalFatCalories()) > 1)
    {
      log << filename << "," << device << "," << version << ",session,total_fat_falories," 
        << session_before.GetTotalFatCalories() << "," << session_after->GetTotalFatCalories() 
        << endl;
    }

    if (fit_file->GetDevice() == kGarminSwim || fit_file->GetDevice() == kGarminFr920)
    { 
      const FIT_UINT8 kSessionAvgStrokeCountFieldNum = 79;
      const FIT_UINT8 kSessionMovingTimeFieldNum = 78;
      const FIT_UINT8 kSessionNumActiveLengthsFieldNum = 33;
      const FIT_UINT8 kSessionSwolfFieldNum = 80;

      if (abs(session_before.GetFieldUINT16Value(kSessionAvgStrokeCountFieldNum) - 
            session_after->GetFieldUINT16Value(kSessionAvgStrokeCountFieldNum)) > 1) {

        log << filename << "," << device << "," << version << ",session,avg_stroke_count,"
          << session_before.GetFieldUINT16Value(kSessionAvgStrokeCountFieldNum) << "," 
          << session_after->GetFieldUINT16Value(kSessionAvgStrokeCountFieldNum) << endl;
      }
      if (abs(session_before.GetFieldUINT32Value(kSessionMovingTimeFieldNum) -
            session_after->GetFieldUINT32Value(kSessionMovingTimeFieldNum)) > 2) {

        log << filename << "," << device << "," << version << ",session,moving_time,"
          << session_before.GetFieldUINT32Value(kSessionMovingTimeFieldNum) << "," 
          << session_after->GetFieldUINT32Value(kSessionMovingTimeFieldNum) << endl;
      }
      if (session_before.GetFieldUINT16Value(kSessionNumActiveLengthsFieldNum) != 
          session_after->GetFieldUINT16Value(kSessionNumActiveLengthsFieldNum)) {

        log << filename << "," << device << "," << version << ",session,num_active_lengths,"
          << session_before.GetFieldUINT16Value(kSessionNumActiveLengthsFieldNum) << "," 
          << session_after->GetFieldUINT16Value(kSessionNumActiveLengthsFieldNum) << endl;
      }
      if (abs(session_before.GetFieldUINT16Value(kSessionSwolfFieldNum) - 
            session_after->GetFieldUINT16Value(kSessionSwolfFieldNum)) > 1) {

        log << filename << "," << device << "," << version << ",session,swolf,"
          << session_before.GetFieldUINT16Value(kSessionSwolfFieldNum) << "," 
          << session_after->GetFieldUINT16Value(kSessionSwolfFieldNum) << endl;
      }

    } else if (fit_file->GetDevice() == kGarminFr910) {
      const FIT_UINT8 kSessionNumLengthsFieldNum = 33;

      if (session_before.GetFieldUINT16Value(kSessionNumLengthsFieldNum) != 
          session_after->GetFieldUINT16Value(kSessionNumLengthsFieldNum)) {

        log << filename << "," << device << "," << version << ",session,num_lengths,"
          << session_before.GetFieldUINT16Value(kSessionNumLengthsFieldNum) << "," 
          << session_after->GetFieldUINT16Value(kSessionNumLengthsFieldNum) << endl;
      }
    } else if (fit_file->GetDevice() == kGarminFenix2) { 
      const FIT_UINT8 kSessionAvgStrokeCountFieldNum = 79;
      const FIT_UINT8 kSessionNumLengthsFieldNum = 33;
      const FIT_UINT8 kSessionSwolfFieldNum = 80;

      if (abs(session_before.GetFieldUINT16Value(kSessionAvgStrokeCountFieldNum) - 
            session_after->GetFieldUINT16Value(kSessionAvgStrokeCountFieldNum)) > 1) {

        log << filename << "," << device << "," << version << ",session,avg_stroke_count,"
          << session_before.GetFieldUINT16Value(kSessionAvgStrokeCountFieldNum) << "," 
          << session_after->GetFieldUINT16Value(kSessionAvgStrokeCountFieldNum) << endl;
      }
      if (session_before.GetFieldUINT16Value(kSessionNumLengthsFieldNum) != 
          session_after->GetFieldUINT16Value(kSessionNumLengthsFieldNum)) {

        log << filename << "," << device << "," << version << ",session,num_lengths,"
          << session_before.GetFieldUINT16Value(kSessionNumLengthsFieldNum) << "," 
          << session_after->GetFieldUINT16Value(kSessionNumLengthsFieldNum) << endl;
      }
      if (abs(session_before.GetFieldUINT16Value(kSessionSwolfFieldNum) - 
            session_after->GetFieldUINT16Value(kSessionSwolfFieldNum)) > 1) {

        log << filename << "," << device << "," << version << ",session,swolf,"
          << session_before.GetFieldUINT16Value(kSessionSwolfFieldNum) << "," 
          << session_after->GetFieldUINT16Value(kSessionSwolfFieldNum) << endl;
      }
    }

    for (unsigned int i = 0 ; i < laps_before.size(); ++i) {

      if (abs(laps_before[i].GetAvgCadence() - laps_after[i]->GetAvgCadence()) > 1) {

        log << filename << "," << device << "," << version
          << ",lap" + std::to_string(i) + ",avg_cadence," 
          << static_cast<int>(laps_before[i].GetAvgCadence()) << "," 
          << static_cast<int>(laps_after[i]->GetAvgCadence()) << endl;
      }
      if (fabs(laps_before[i].GetAvgSpeed() - laps_after[i]->GetAvgSpeed()) > .01) {

        log << filename << "," << device << "," << version
          << ",lap" + std::to_string(i) + ",avg_speed," 
          << laps_before[i].GetAvgSpeed() << "," << laps_after[i]->GetAvgSpeed() << endl;
      }
      if (fabs(laps_before[i].GetAvgStrokeDistance() - laps_after[i]->GetAvgStrokeDistance()) > .011) { 

        log << filename << "," << device << "," << version
          << ",lap" + std::to_string(i) + ",avg_stroke_distance," 
          << laps_before[i].GetAvgStrokeDistance() << "," << laps_after[i]->GetAvgStrokeDistance() << endl;
      }
      if (laps_before[i].GetFirstLengthIndex() != laps_after[i]->GetFirstLengthIndex()) {

        log << filename << "," << device << "," << version
          << ",lap" + std::to_string(i) + ",first_length_index," 
          << laps_before[i].GetFirstLengthIndex() << "," << laps_after[i]->GetFirstLengthIndex() << endl;
      }
      if (abs((laps_before[i].GetMaxCadence() -  laps_after[i]->GetMaxCadence())) > 0) {

        log << filename << "," << device << "," << version
          << ",lap" + std::to_string(i) + ",max_cadence," 
          << static_cast<int>(laps_before[i].GetMaxCadence()) << "," 
          << static_cast<int>(laps_after[i]->GetMaxCadence()) << endl;
      }
      if (fabs(laps_before[i].GetMaxSpeed() - laps_after[i]->GetMaxSpeed()) > 0.01f) {

        log << filename << "," << device << "," << version
          << ",lap" + std::to_string(i) + ",max_speed," 
          << laps_before[i].GetMaxSpeed() << "," << laps_after[i]->GetMaxSpeed() << endl;
      }
      if (laps_before[i].GetNumActiveLengths() != laps_after[i]->GetNumActiveLengths()) {

        log << filename << "," << device << "," << version 
          << ",lap" + std::to_string(i) + ",num_active_lengths," 
          << laps_before[i].GetNumActiveLengths() << "," << laps_after[i]->GetNumActiveLengths() 
          << endl;
      }
      if (laps_before[i].GetNumLengths() != laps_after[i]->GetNumLengths()) {
        log << filename << "," << device << "," << version 
          << ",lap" + std::to_string(i) + ",num_lengths," 
          << laps_before[i].GetNumLengths() << "," << laps_after[i]->GetNumLengths() 
          << endl;
      }
      if (laps_before[i].GetSwimStroke() != laps_after[i]->GetSwimStroke()) {

        log << filename << "," << device << "," << version 
          << ",lap" + std::to_string(i) + ",swim_stroke," 
          << static_cast<int>(laps_before[i].GetSwimStroke()) << "," 
          << static_cast<int>(laps_after[i]->GetSwimStroke()) << endl;
      }
      if (abs(laps_before[i].GetTotalCalories() - laps_after[i]->GetTotalCalories()) > 1) {

        log << filename << "," << device << "," << version 
          << ",lap" + std::to_string(i) + ",total_calories," 
          << laps_before[i].GetTotalCalories() << "," << laps_after[i]->GetTotalCalories() 
          << endl;
      }
      if (laps_before[i].GetTotalCycles() != laps_after[i]->GetTotalCycles()) {

        log << filename << "," << device << "," << version 
          << ",lap" + std::to_string(i) + ",total_cycles," 
          << laps_before[i].GetTotalCycles() << "," << laps_after[i]->GetTotalCycles() << endl;
      }
      if (fabs(laps_before[i].GetTotalDistance() - laps_after[i]->GetTotalDistance()) > .01)
      {
        log << filename << "," << device << "," << version 
          << ",lap" + std::to_string(i) + ",total_distance," 
          << laps_before[i].GetTotalDistance() << "," << laps_after[i]->GetTotalDistance() 
          << endl;
      }
      if (abs(laps_before[i].GetTotalFatCalories() - laps_after[i]->GetTotalFatCalories()) > 1)
      {
        log << filename << "," << device << "," << version 
          << ",lap" + std::to_string(i) + ",Total_fat_falories," 
          << laps_before[i].GetTotalFatCalories() << "," << laps_after[i]->GetTotalFatCalories() 
          << endl;
      }

      if (fit_file->GetDevice() == kGarminSwim)
      { 
        const FIT_UINT8 kLapAvgStrokeCountFieldNum = 72;
        const FIT_UINT8 kLapMovingTimeFieldNum = 70;
        const FIT_UINT8 kLapSwolfFieldNum = 73;

        if (abs(laps_before[i].GetFieldUINT16Value(kLapAvgStrokeCountFieldNum) - 
              laps_after[i]->GetFieldUINT16Value(kLapAvgStrokeCountFieldNum)) > 1) {

          log << filename << "," << device << "," << version 
            << ",lap" + std::to_string(i) + ",avg_stroke_count,"
            << laps_before[i].GetFieldUINT16Value(kLapAvgStrokeCountFieldNum) << "," 
            << laps_after[i]->GetFieldUINT16Value(kLapAvgStrokeCountFieldNum) << endl;
        }
        if (abs(laps_before[i].GetFieldUINT32Value(kLapMovingTimeFieldNum) -
              laps_after[i]->GetFieldUINT32Value(kLapMovingTimeFieldNum)) > 1) {

          log << filename << "," << device << "," << version 
            << ",lap" + std::to_string(i) + ",moving_time,"
            << laps_before[i].GetFieldUINT32Value(kLapMovingTimeFieldNum) << "," 
            << laps_after[i]->GetFieldUINT32Value(kLapMovingTimeFieldNum) << endl;
        }
        if (abs(laps_before[i].GetFieldUINT16Value(kLapSwolfFieldNum) - 
              laps_after[i]->GetFieldUINT16Value(kLapSwolfFieldNum)) > 1) {

          log << filename << "," << device << "," << version 
            << ",lap" + std::to_string(i) + ",swolf,"
            << laps_before[i].GetFieldUINT16Value(kLapSwolfFieldNum) << "," 
            << laps_after[i]->GetFieldUINT16Value(kLapSwolfFieldNum) << endl;
        }

      } else if (fit_file->GetDevice() == kGarminFr910) {
        // fr910 has no custom lap fields
      } else if (fit_file->GetDevice() == kGarminFenix2) {
        const FIT_UINT8 kLapSwolfFieldNum = 73;
        if (abs(laps_before[i].GetFieldUINT16Value(kLapSwolfFieldNum) - 
              laps_after[i]->GetFieldUINT16Value(kLapSwolfFieldNum)) > 1) {

          log << filename << "," << device << "," << version 
            << ",lap" + std::to_string(i) + ",swolf,"
            << laps_before[i].GetFieldUINT16Value(kLapSwolfFieldNum) << "," 
            << laps_after[i]->GetFieldUINT16Value(kLapSwolfFieldNum) << endl;
        }
      } else if (fit_file->GetDevice() == kGarminFr920) { 
        const FIT_UINT8 kLapAvgStrokeCountFieldNum = 90;
        const FIT_UINT8 kLapMovingTimeFieldNum = 70;
        const FIT_UINT8 kLapSwolfFieldNum = 73;

        if (abs(laps_before[i].GetFieldUINT16Value(kLapAvgStrokeCountFieldNum) - 
              laps_after[i]->GetFieldUINT16Value(kLapAvgStrokeCountFieldNum)) > 1) {

          log << filename << "," << device << "," << version 
            << ",lap" + std::to_string(i) + ",avg_stroke_count,"
            << laps_before[i].GetFieldUINT16Value(kLapAvgStrokeCountFieldNum) << "," 
            << laps_after[i]->GetFieldUINT16Value(kLapAvgStrokeCountFieldNum) << endl;
        }
        if (abs(laps_before[i].GetFieldUINT32Value(kLapMovingTimeFieldNum) -
              laps_after[i]->GetFieldUINT32Value(kLapMovingTimeFieldNum)) > 1) {

          log << filename << "," << device << "," << version 
            << ",lap" + std::to_string(i) + ",moving_time,"
            << laps_before[i].GetFieldUINT32Value(kLapMovingTimeFieldNum) << "," 
            << laps_after[i]->GetFieldUINT32Value(kLapMovingTimeFieldNum) << endl;
        }
        if (abs(laps_before[i].GetFieldUINT16Value(kLapSwolfFieldNum) - 
              laps_after[i]->GetFieldUINT16Value(kLapSwolfFieldNum)) > 1) {

          log << filename << "," << device << "," << version 
            << ",lap" + std::to_string(i) + ",swolf,"
            << laps_before[i].GetFieldUINT16Value(kLapSwolfFieldNum) << "," 
            << laps_after[i]->GetFieldUINT16Value(kLapSwolfFieldNum) << endl;
        }

      }
    }
  }
}


void swt::Tests::OnMesg(fit::Mesg& mesg) {
  mesgs_.push_back(mesg);
}

void swt::Tests::OnMesgDefinition(fit::MesgDefinition& mesgDef) {
  mesg_defs_.push_back(mesgDef);
}


void swt::Tests::ReadDefs(const std::string &filename) {
  FIT_UINT16 product = FIT_UINT16_INVALID;
  FIT_FLOAT32 version = FIT_FLOAT32_INVALID;

  ReadFile(filename, &product, &version);

  for (fit::MesgDefinition mesg_def : mesg_defs_) {

    std::string name;
    const fit::Profile::MESG *profile = fit::Profile::GetMesg(mesg_def.GetNum());
    if (profile != nullptr) 
      name = profile->name;
    else
      name = "unknown" + std::to_string(mesg_def.GetNum());

    log << basename(const_cast<char *>(filename.c_str())) << "," << product << "," << version << ","
      << name ;

    for (fit::FieldDefinition field_def : mesg_def.GetFields()) {
      log << "," << std::to_string(field_def.GetNum()); 

    }
    log << std::endl;
  }
}

// allow to copy Fenix files to a working directory
void swt::Tests::CopyFenixFile(const std::string &filename) {

  FIT_UINT16 product;
  FIT_FLOAT32 version;
  ReadFile(filename, &product, &version);
  std::string dest_filename = "/home/stephane/swt/fit_files/fenix2/";
  dest_filename += basename(const_cast<char*>(filename.c_str()));
  if (product == swt::kGarminFenix2) {
    std::ifstream source(filename, std::fstream::binary);
    std::ofstream dest(dest_filename, std::fstream::trunc|std::fstream::binary);
    dest << source.rdbuf();
  }
}

void swt::Tests::ReadFile(const std::string &filename, FIT_UINT16 *product, 
    FIT_FLOAT32 *version ) {

  std::ifstream istream;
  istream.open(filename, std::ios::in | std::ios::binary);
  fit::Decode decode;

  mesgs_.clear();
  mesg_defs_.clear();

  *product = FIT_UINT16_INVALID;
  *version = FIT_FLOAT32_INVALID;

  if (!istream.is_open())
    throw std::runtime_error("Error opening file"); 

  if (!decode.IsFIT(istream))
    throw FileNotValidException("File is not a FIT file or is corrupted");

  decode.Read(istream, *this, *this);

  for (fit::Mesg mesg : mesgs_) {
    if (mesg.GetNum() == FIT_MESG_NUM_FILE_ID) {
      fit::FileIdMesg file_id(mesg);
      *product = file_id.GetProduct();
    }
    if (mesg.GetNum() == FIT_MESG_NUM_DEVICE_INFO) {
      fit::DeviceInfoMesg device_info(mesg);
      if (device_info.GetProduct() == *product) {
        *version = device_info.GetSoftwareVersion();
      }
    }
    if (*product != FIT_UINT16_INVALID && *version != FIT_FLOAT32_INVALID) {
      break;
    }
  }
}

void swt::Tests::ReadSessionLapLength(const std::string &filename) {
  FIT_UINT16 product = FIT_UINT16_INVALID;
  FIT_FLOAT32 version = FIT_FLOAT32_INVALID;

  ReadFile(filename, &product, &version);

  bool active_length_done = false;
  bool rest_length_done = false;
  bool active_lap_done = false;
  bool rest_lap_done = false;
  bool session_done = false;
  bool record_done = false;

  for (fit::Mesg &mesg : mesgs_) {
    switch (mesg.GetNum()) {
      case FIT_MESG_NUM_SESSION:
        if (!session_done) {
          WriteMesg(filename, product, version, mesg, "session");
          session_done = true;
        }
        break;
      case FIT_MESG_NUM_LAP:
        {
          fit::LapMesg lap(mesg);

          if (!active_lap_done && lap.GetNumActiveLengths() > 0) {
            WriteMesg(filename, product, version, mesg, "lap_active");
            active_lap_done = true;
          }
          if (!rest_lap_done && lap.GetNumActiveLengths() == 0) {
            WriteMesg(filename, product, version, mesg, "lap_rest");
            rest_lap_done = true;
          }
          break;
        }
      case FIT_MESG_NUM_LENGTH:
        {
          fit::LengthMesg length(mesg);

          if (!active_length_done && length.GetLengthType() == FIT_LENGTH_TYPE_ACTIVE) {
            WriteMesg(filename, product, version, mesg, "length_active");
            active_length_done = true;
          }
          if (!rest_length_done && length.GetLengthType() == FIT_LENGTH_TYPE_IDLE) {
            WriteMesg(filename, product, version, mesg, "length_rest");
            rest_length_done = true;
          }
          break;
        }
      case FIT_MESG_NUM_RECORD:
        if (!record_done) {
          WriteMesg(filename, product, version, mesg, "record");
          record_done = true;
        }
        break;
    } 
  }   
}


void swt::Tests::WriteMesg(const std::string &filename, 
    FIT_UINT16 product, FIT_FLOAT32 version, 
    const fit::Mesg &mesg, const std::string &name) {

  log << basename(const_cast<char*>(filename.c_str())) << "," << product << "," << version << "," << name;
  for (unsigned short i = 0; i <  mesg.GetNumFields(); i++) {
    const fit::Field *field = mesg.GetFieldByIndex(i);
    if (field->GetName() == "unknown") 
      log << ",unknown" << static_cast<int>(field->GetNum());
    else
      log << "," << field->GetName();

    switch (field->GetType()) {

      case FIT_BASE_TYPE_ENUM:
        log << "," << static_cast<unsigned int>(field->GetENUMValue());
        break;
      case FIT_BASE_TYPE_SINT8:
        log << "," << static_cast<int>(field->GetSINT8Value());
        break;
      case FIT_BASE_TYPE_UINT8:
        log << "," << static_cast<unsigned int>(field->GetUINT8Value());
        break;
      case FIT_BASE_TYPE_SINT16:
        log << "," << field->GetSINT16Value();
        break;
      case FIT_BASE_TYPE_UINT16:
        log << "," << field->GetUINT16Value();
        break;
      case FIT_BASE_TYPE_SINT32:
        log << "," << field->GetSINT32Value();
        break;
      case FIT_BASE_TYPE_UINT32:
        log << "," << field->GetUINT32Value();
        break;
      case FIT_BASE_TYPE_STRING:
        //log << "," << field->GetSTRINGValue();
        break;
      case FIT_BASE_TYPE_FLOAT32:
        log << "," << field->GetFLOAT32Value();
        break;
      case FIT_BASE_TYPE_FLOAT64:
        log << "," << field->GetFLOAT64Value();
        break;
      case FIT_BASE_TYPE_UINT8Z:
        log << "," << static_cast<unsigned int>(field->GetUINT8ZValue());
        break;
      case FIT_BASE_TYPE_UINT16Z:
        log << "," << field->GetUINT16ZValue();
        break;
      case FIT_BASE_TYPE_UINT32Z:
        log << "," << field->GetUINT32ZValue();
        break;
      case FIT_BASE_TYPE_BYTE:
        log << "," << field->GetBYTEValue();
        break;
    } 
  }

  log << endl;
}


void swt::Tests::TestSwimFile() {

  std::string fit_path = "../fit_files/test/";
  std::string garmin_swim_file = fit_path + "3872642880_20131125-170823-1-1499-ANTFS-4-0.FIT";
  std::string garmin_fr910_file = fit_path + "3864554663_20131119-195816-1-1328-ANTFS-4-0.FIT";
  std::string garmin_fr920_file = fit_path + "3892502708_20141014_075500_1765.FIT";
  std::string garmin_fenix_file = fit_path + "3881241990_2014-05-06-20-14-22-Piscine.fit";
  std::string saveAs;

  FileReader file_reader;
  std::unique_ptr<SwimFile> swim_file = file_reader.Read(garmin_swim_file);

  swim_file->Recalculate();
  swim_file->Save(fit_path + "output/gs_no_changes.fit");

  swim_file = file_reader.Read(garmin_swim_file);
  swim_file->Merge(18);
  swim_file->Save(fit_path + "output/gs_merge.fit");

  swim_file = file_reader.Read(garmin_swim_file);
  swim_file->Split(18);
  swim_file->Save(fit_path + "output/gs_split.fit");

  swim_file = file_reader.Read(garmin_swim_file);
  swim_file->ChangeStroke(22, FIT_SWIM_STROKE_FREESTYLE, ChangeStrokeOption::kLengthOnly);
  swim_file->Save(fit_path + "output/gs_change_stroke_length_only.fit");

  swim_file = file_reader.Read(garmin_swim_file);
  swim_file->ChangeStroke(22, FIT_SWIM_STROKE_BUTTERFLY, ChangeStrokeOption::kLap);
  swim_file->Save(fit_path + "output/gs_change_stroke_lap.fit");

  swim_file = file_reader.Read(garmin_swim_file);
  swim_file->ChangeStroke(22, FIT_SWIM_STROKE_FREESTYLE, ChangeStrokeOption::kAll);
  swim_file->Save(fit_path + "output/gs_change_stroke_all.fit");

  swim_file = file_reader.Read(garmin_swim_file);
  swim_file->ChangePoolLength(50, FIT_DISPLAY_MEASURE_METRIC);
  swim_file->Save(fit_path + "output/gs_change_pool_length.fit");

  swim_file = file_reader.Read(garmin_swim_file);
  swim_file->Delete(19);
  swim_file->Save(fit_path + "output/gs_delete_simple.fit");

  swim_file = file_reader.Read(garmin_swim_file);
  swim_file->Delete(18);
  swim_file->Delete(19);
  swim_file->Delete(20);
  swim_file->Delete(21);
  swim_file->Delete(22);
  swim_file->Delete(23);
  swim_file->Delete(24);
  swim_file->Delete(25);
  swim_file->Save(fit_path + "output/gs_delete_convert_lap_to_rest.fit");

  // FR910

  swim_file = file_reader.Read(garmin_fr910_file);
  swim_file->Recalculate();
  swim_file->Save(fit_path + "output/fr910_no_changes.fit");

  swim_file = file_reader.Read(garmin_fr910_file);
  swim_file->Merge(12);
  swim_file->Save(fit_path + "output/fr910_merge.fit");

  swim_file = file_reader.Read(garmin_fr910_file);
  swim_file->Split(12);
  swim_file->Save(fit_path + "output/fr910_split.fit");

  swim_file = file_reader.Read(garmin_fr910_file);
  swim_file->ChangeStroke(12, FIT_SWIM_STROKE_FREESTYLE, ChangeStrokeOption::kLengthOnly);
  swim_file->Save(fit_path + "output/fr910_change_stroke_length_only.fit");

  swim_file = file_reader.Read(garmin_fr910_file);
  swim_file->ChangeStroke(12, FIT_SWIM_STROKE_BUTTERFLY, ChangeStrokeOption::kLap);
  swim_file->Save(fit_path + "output/fr910_change_stroke_lap.fit");

  swim_file = file_reader.Read(garmin_fr910_file);
  swim_file->ChangeStroke(12, FIT_SWIM_STROKE_FREESTYLE, ChangeStrokeOption::kAll);
  swim_file->Save(fit_path + "output/fr910_change_stroke_all.fit");

  swim_file = file_reader.Read(garmin_fr910_file);
  swim_file->ChangePoolLength(25, FIT_DISPLAY_MEASURE_METRIC);
  swim_file->Save(fit_path + "output/fr910_change_pool_length.fit");

  swim_file = file_reader.Read(garmin_fr910_file);
  swim_file->Delete(61);
  swim_file->Save(fit_path + "output/fr910_delete_simple.fit");

  swim_file = file_reader.Read(garmin_fr910_file);
  swim_file->Delete(61);
  swim_file->Delete(62);
  swim_file->Save(fit_path + "output/fr910_delete_convert_lap_to_rest.fit");

  swim_file = file_reader.Read(fit_path + "3842314005_20140112-124550-1-1328-ANTFS-4-0.FIT");
  swim_file->Delete(35);
  swim_file->Save(fit_path + "output/fr910_delete_duplicate.fit");

  // Fénix 2

  swim_file = file_reader.Read(garmin_fenix_file);
  swim_file->Recalculate();
  swim_file->Save(fit_path + "output/fenix_no_changes.fit");

  swim_file = file_reader.Read(garmin_fenix_file);
  swim_file->Merge(5);
  swim_file->Save(fit_path + "output/fenix_merge.fit");

  swim_file = file_reader.Read(garmin_fenix_file);
  swim_file->Split(5);
  swim_file->Save(fit_path + "output/fenix_split.fit");

  swim_file = file_reader.Read(garmin_fenix_file);
  swim_file->ChangeStroke(5, FIT_SWIM_STROKE_BREASTSTROKE, ChangeStrokeOption::kLengthOnly);
  swim_file->Save(fit_path + "output/fenix_change_stroke_length_only.fit");

  swim_file = file_reader.Read(garmin_fenix_file);
  swim_file->ChangeStroke(5, FIT_SWIM_STROKE_BUTTERFLY, ChangeStrokeOption::kLap);
  swim_file->Save(fit_path + "output/fenix_change_stroke_lap.fit");

  swim_file = file_reader.Read(garmin_fenix_file);
  swim_file->ChangeStroke(6, FIT_SWIM_STROKE_BACKSTROKE, ChangeStrokeOption::kAll);
  swim_file->Save(fit_path + "output/fenix_change_stroke_all.fit");

  swim_file = file_reader.Read(garmin_fenix_file);
  swim_file->ChangePoolLength(25, FIT_DISPLAY_MEASURE_METRIC);
  swim_file->Save(fit_path + "output/fenix_change_pool_length.fit");

  swim_file = file_reader.Read(garmin_fenix_file);
  swim_file->Delete(5);
  swim_file->Save(fit_path + "output/fenix_delete_simple.fit");

  swim_file = file_reader.Read(garmin_fenix_file);
  swim_file->Delete(20);
  swim_file->Save(fit_path + "output/fenix_delete_convert_lap_to_rest.fit");

  // FR920 

  swim_file = file_reader.Read(garmin_fr920_file);
  swim_file->Recalculate();
  swim_file->Save(fit_path + "output/fr920_no_changes.fit");

  swim_file = file_reader.Read(garmin_fr920_file);
  swim_file->Merge(5);
  swim_file->Save(fit_path + "output/fr920_merge.fit");

  swim_file = file_reader.Read(garmin_fr920_file);
  swim_file->Split(9);
  swim_file->Save(fit_path + "output/fr920_split.fit");

  swim_file = file_reader.Read(garmin_fr920_file);
  swim_file->ChangeStroke(5, FIT_SWIM_STROKE_BREASTSTROKE, ChangeStrokeOption::kLengthOnly);
  swim_file->Save(fit_path + "output/fr920_change_stroke_length_only.fit");

  swim_file = file_reader.Read(garmin_fr920_file);
  swim_file->ChangeStroke(5, FIT_SWIM_STROKE_BUTTERFLY, ChangeStrokeOption::kLap);
  swim_file->Save(fit_path + "output/fr920_change_stroke_lap.fit");

  swim_file = file_reader.Read(garmin_fr920_file);
  swim_file->ChangeStroke(5, FIT_SWIM_STROKE_BACKSTROKE, ChangeStrokeOption::kAll);
  swim_file->Save(fit_path + "output/fr920_change_stroke_all.fit");

  swim_file = file_reader.Read(garmin_fr920_file);
  swim_file->ChangePoolLength(50, FIT_DISPLAY_MEASURE_METRIC);
  swim_file->Save(fit_path + "output/fr920_change_pool_length.fit");

  swim_file = file_reader.Read(garmin_fr920_file);
  swim_file->Delete(5);
  swim_file->Save(fit_path + "output/fr920_delete_simple.fit");

  swim_file = file_reader.Read(garmin_fr920_file);
  swim_file->Delete(47);
  swim_file->Delete(48);
  swim_file->Delete(49);
  swim_file->Delete(50);
  swim_file->Delete(51);
  swim_file->Delete(52);
  swim_file->Delete(53);
  swim_file->Save(fit_path + "output/fr920_delete_convert_lap_to_rest.fit");
}

