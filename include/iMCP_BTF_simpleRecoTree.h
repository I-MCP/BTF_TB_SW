#ifndef iMCP_BTF_simpleRecoTree_h
#define iMCP_BTF_simpleRecoTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <string>

#include "iMCP_BTF_rawDataTree.h"
#include "iMCP_BTF_simpleRecoTree_format.h"

//This class is an analyzer of the rawDataTree and produces a new tree with reconstruced and calibrated data
class iMCP_BTF_simpleRecoTree : public iMCP_BTF_rawDataTree 
{
 public :
  TString outFile;
  TTree* outTree;

  iMCP_BTF_simpleRecoTree_format treeData;

  iMCP_BTF_simpleRecoTree(TTree *tree=0);
  virtual ~iMCP_BTF_simpleRecoTree();

  //main analysis class
  virtual void  Loop();

  void bookOutputTree();
  ClassDef(iMCP_BTF_simpleRecoTree,1);
};

#endif
