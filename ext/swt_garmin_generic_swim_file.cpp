#include "swt_garmin_generic_swim_file.h"

swt::GarminGenericSwimFile::GarminGenericSwimFile()
{}

void swt::GarminGenericSwimFile::UpdateLap(fit::LapMesg *lap) {
  Fr920SwimFile::UpdateLap(lap);


  // Calculation of calories are a bit different than FR920
  // Everything else is the same
  FIT_UINT16 total_calories = 0;
  FIT_UINT16 first_length_index = lap->GetFirstLengthIndex();
  FIT_UINT16 last_length_index = static_cast<FIT_UINT16>(lap->GetFirstLengthIndex() + 
      lap->GetNumLengths() - 1);
  FIT_UINT16 length_total_calories = FIT_UINT16_INVALID;
  for (int index = first_length_index; index <= last_length_index; index++) {
    fit::LengthMesg *length = lengths_.at(index);
    length_total_calories = length->GetTotalCalories() ;
    if (length_total_calories != FIT_UINT16_INVALID)
      total_calories += length_total_calories;
  }  

  lap->SetTotalCalories(total_calories);
}
 
void swt::GarminGenericSwimFile::UpdateSession() {

  Fr920SwimFile::UpdateSession();
}

