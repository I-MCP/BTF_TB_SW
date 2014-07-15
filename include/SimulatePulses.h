#ifndef SIMULATE_PULSES_H
#define SIMULATE_PULSES_H


#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TProfile.h>
#include <TString.h>

#include <string>
#include <vector>

#include "iMCP_BTF_rawDataTree.h"

#define SAMPLE_BIN_SIZE 0.2E-9
#define NSAMPLES 1024

class SimulatePulses : public iMCP_BTF_rawDataTree 
{
 public :
  TString outFile;
  TString profileFile;

  TTree* tree;

  std::vector<TProfile*> ref_profiles;

  int nEvents=100;

  float rms_amplitude_noise=0;

  SimulatePulses();
  virtual ~SimulatePulses();

  //main analysis class
  virtual void  Loop();

  void cannotOpenFile(const char * file);

  ClassDef(SimulatePulses,1);
};

#endif
