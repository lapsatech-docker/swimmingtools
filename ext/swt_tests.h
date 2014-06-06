#if !defined(SWT_TESTS)
#define SWT_TESTS
#include <fstream>
#include <string>
#include <vector>
#include "fit_mesg_definition_listener.hpp"
#include "fit_mesg_listener.hpp"

namespace swt
{
  class Tests : fit::MesgListener, fit::MesgDefinitionListener {
    public:
      Tests();
      ~Tests();
      void CopyFenixFile(const std::string &filename);
      void CheckUpdateLapAndSession(std::string filename);
      void ReadDefs(const std::string &filename);
      void ReadSessionLapLength(const std::string &filename);
      void OnMesg(fit::Mesg& mesg);
      void OnMesgDefinition(fit::MesgDefinition& mesgDef);
      void TestSwimFile();

    private:
      std::ofstream log;
      std::vector<fit::Mesg> mesgs_;
      std::vector<fit::MesgDefinition> mesg_defs_;

      void ReadFile(const std::string &filename, FIT_UINT16 *product, 
          FIT_FLOAT32 *version );
      void WriteMesg(const std::string &filename, 
          FIT_UINT16 product, FIT_FLOAT32 version, 
          const fit::Mesg &mesg, const std::string &name);
  };
}
#endif
