#ifndef iMCP_BTF_simpleRecoTree_h
#define iMCP_BTF_simpleRecoTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>

#include <string>

#include "iMCP_BTF_rawDataTree.h"
#include "iMCP_BTF_simpleRecoTree_format.h"

//This class is an analyzer of the rawDataTree and produces a new tree with reconstruced and calibrated data
class iMCP_BTF_simpleRecoTree : public iMCP_BTF_rawDataTree 
{
 public :
  TString outFile;
  TTree* tree;
  int maxEntries=-1;

  iMCP_BTF_simpleRecoTree_format treeData;

  iMCP_BTF_simpleRecoTree(TTree *tree=0);
  virtual ~iMCP_BTF_simpleRecoTree();

  //main analysis class
  virtual void  Loop();

  //book tree branches
  void bookEventInfo();
  void bookOutputTree();
  void bookScintData();
  void bookMcpData();
  void bookHodoData();
  void bookWaveform(TString name, waveform_data& waveform);
  void bookTdcData(TString name, tdc_data& tdc);
  void bookHodoscopePlane(TString name, hodoscope_plane_data& plane_data);

  void cannotOpenFile(const char * file);

  ClassDef(iMCP_BTF_simpleRecoTree,1);
};

#endif
