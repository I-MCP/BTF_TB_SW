#ifndef iMCP_BTF_simpleEffAnalysis_h
#define iMCP_BTF_simpleEffAnalysis_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>

#include <string>

#include "iMCP_BTF_rawDataTree.h"

//This class is an analyzer of the rawDataTree and produces a new tree with reconstruced and calibrated data
class iMCP_BTF_simpleEffAnalysis : public iMCP_BTF_rawDataTree 
{
 public :
  
  /* /\*   char* inputDir; *\/ */
  /* /\*   std::string inputName; *\/ */
  
  /*   char* outputDir; */
  
  iMCP_BTF_simpleEffAnalysis(TTree *tree=0) : iMCP_BTF_rawDataTree(tree) {} ;
    
  virtual ~iMCP_BTF_simpleEffAnalysis() {};
    
  //main analysis class
  virtual void  Loop();
    
  //Tree variables
  TTree* outTree;
  float coinc_Ch1=0, coinc_Ch2=0, coinc_Ch3=0, coinc_ref=0;


  //I/O variables
  int Ch_ref1;
  int Ch_ref2;
  int Ch_1;
  int Ch_2;
  int Ch_3;

  int id=0;
  int count[5];
  int spare[5];
  int spare2[5];

  int tot_tr1=0;
  int tot_tr0=0;

  ClassDef(iMCP_BTF_simpleEffAnalysis,0);
}
;

#endif
