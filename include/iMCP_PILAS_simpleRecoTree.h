#ifndef iMCP_PILAS_simpleRecoTree_h
#define iMCP_PILAS_simpleRecoTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>

#include <string>

#include "iMCP_BTF_rawDataTree.h"
#include "iMCP_PILAS_simpleRecoTree_format.h"

using namespace iMCP_PILAS_simpleRecoTree_namespace;

//This class is an analyzer of the rawDataTree and produces a new tree with reconstruced and calibrated data
class iMCP_PILAS_simpleRecoTree : public iMCP_BTF_rawDataTree 
{
 public :
  TString outFile;
  TTree* tree;
  int maxEntries=-1;

  iMCP_PILAS_simpleRecoTree_format treeData;

  iMCP_PILAS_simpleRecoTree(TTree *tree=0);
  virtual ~iMCP_PILAS_simpleRecoTree();

  //main analysis class
  virtual void  Loop();

  //book tree branches
  void bookOutputTree();
  void bookEventInfo();
  void bookChannelsData();
  void bookWaveform(TString name, iMCP_PILAS_simpleRecoTree_namespace::waveform_data& waveform);


  void cannotOpenFile(const char * file);

  ClassDef(iMCP_PILAS_simpleRecoTree,1);
};

#endif
