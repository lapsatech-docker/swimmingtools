#if !defined(SWT_SWIM_FILE)
#define SWT_SWIM_FILE
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "fit_activity_mesg.hpp"
#include "fit_lap_mesg.hpp"
#include "fit_length_mesg.hpp"
#include "fit_mesg.hpp"
#include "fit_session_mesg.hpp"

namespace swt
{
  enum Product {
    kGarminSwim = 1,
    kGarminFr910 = 2,
    kGarminFenix2 = 3,
    kGarminFr920 = 4,
    kTomtom = 5,
    kGarminVivoActive = 6,
    kGarminFenix3 = 7,
    kGarminEpix = 8,
    kGarminFr735 = 9,
    kGarminFenix5 = 10,
    kGarminFr935 = 11,
    kGarminVivoActive3 = 12,
    kGarminD2Charlie = 13,
    kGarminDescentMK1 = 14,
    kGarminApproachS60 = 15,
    kGarminFr645 = 16,
    kGarminFenix5Plus = 17
  }; 

  enum ChangeStrokeOption {
    kLengthOnly = 0,
    kLap = 1,
    kAll = 2
  };

  class FileNotValidException : public std::runtime_error {

    public:
      FileNotValidException(const std::string &msg) 
        :runtime_error(msg)
      { }
  };

  class SwimFile {

    public:
      SwimFile();
      virtual void AddMesg(const void *mesg);
      virtual bool CanMerge(FIT_MESSAGE_INDEX length_index, std::string *error) const;
      bool CanSplitChangeStrokeDelete(FIT_MESSAGE_INDEX length_index,std::string *error) const;
      void ChangeStroke(FIT_MESSAGE_INDEX length_index, FIT_SWIM_STROKE stroke, ChangeStrokeOption option);
      void ChangePoolSize(FIT_FLOAT32 new_size_metric, FIT_DISPLAY_MEASURE display_measure); 
      virtual void Delete(FIT_MESSAGE_INDEX length_index) = 0;
      virtual Product GetProduct() const = 0;
      float GetSoftwareVersion() const {return software_version_;}
      unsigned int GetSerialNumber() const {return serial_number_;}
      const std::vector<fit::LapMesg*> &GetLaps() const {return laps_;};
      const std::vector<fit::LengthMesg*> &GetLengths() const {return lengths_;};
      const std::list<std::unique_ptr<fit::Mesg>> &GetMesgs() const {return mesgs_;};
      double GetRestTime(FIT_MESSAGE_INDEX length_index) const;
      const fit::SessionMesg* GetSession() const {return session_;};
      virtual void Initialize() {};
      virtual bool IsDuplicate(FIT_MESSAGE_INDEX length_index) const {return false;};
      virtual bool IsValid(std::string *error) const;
      void LoadHrData(std::istream& istream);
      virtual void Merge(FIT_MESSAGE_INDEX length_index);
      void Recalculate();
      virtual void Save(const std::string &filename, bool convert=false) const = 0;
      void SetFitProtocolVersion (fit::ProtocolVersion protocol_version);
      virtual void Split(FIT_MESSAGE_INDEX length_index);

    protected:  
      fit::LapMesg* GetLap(FIT_MESSAGE_INDEX length_index) const;  
      virtual void LengthSetTimestamp(fit::LengthMesg *length, FIT_DATE_TIME timestamp);
    
      fit::ActivityMesg * activity_;
      fit::SessionMesg *session_;
      std::vector<fit::LapMesg*> laps_;
      std::vector<fit::LengthMesg*> lengths_;
      std::list<std::unique_ptr<fit::Mesg>> mesgs_;
      std::vector<FIT_DATE_TIME> timer_stop_timestamps_;
      std::vector<char> hr_data_;

      FIT_UINT32Z serial_number_;
      FIT_UINT16 software_version_;
      fit::ProtocolVersion fit_protocol_version_;
      FIT_UINT8 record_local_num_ = FIT_UINT8_INVALID;

    private:
      static const FIT_UINT16 kSoftwareVersionFieldNum = 5;
      const std::string GetSportAsString(FIT_ENUM sport, FIT_ENUM sub_sport) const;
      virtual void UpdateSession() = 0;
      virtual void UpdateLap(fit::LapMesg *lap) = 0;

  };
}
#endif
