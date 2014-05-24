#ifndef iMCP_BTF_simpleRecoTree_h
#define iMCP_BTF_simpleRecoTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <string>

#include "iMCP_BTF_rawDataTree.h"

class iMCP_BTF_simpleRecoTree : public iMCP_BTF_rawDataTree 
{
 public :
  iMCP_BTF_simpleRecoTree(TTree *tree=0);
  virtual ~iMCP_BTF_simpleRecoTree();
  virtual void     Loop();
};

#endif
