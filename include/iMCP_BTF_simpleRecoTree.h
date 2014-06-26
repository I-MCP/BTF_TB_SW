#ifndef iMCP_BTF_simpleRecoTree_h
#define iMCP_BTF_simpleRecoTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>

#include <string>

#include "iMCP_BTF_rawDataTree.h"
#include "iMCP_BTF_simpleRecoTree_format.h"

// ADC BOARD 0 -> V265
// ADC BOARD 1 -> V792

//Hodoscope connections
#define HODOX_ADC_BOARD 1
#define HODOX_CHANNELS 8
#define HODOX_ADC_START_CHANNEL 16
#define HODOX_ADC_END_CHANNEL 23

#define HODOY_ADC_BOARD 1
#define HODOY_CHANNELS 8
#define HODOY_ADC_START_CHANNEL 24
#define HODOY_ADC_END_CHANNEL 31

//Scint front & back connections
#define SCINT_FRONT_ADC_BOARD 1
#define SCINT_FRONT_ADC_CHANNEL 0
#define SCINT_FRONT_TDC_CHANNEL 4
#define SCINT_FRONT_DIGITIZER_CHANNEL 6

#define SCINT_BACK_ADC_BOARD 1
#define SCINT_BACK_ADC_CHANNEL 1
#define SCINT_BACK_TDC_CHANNEL 5
#define SCINT_BACK_DIGITIZER_CHANNEL 2


//MCP connections (4 are readout in parallel via CFD+TDC & Digitizer, 1 only digitizer)
//This is MiB1
#define MCP_0_TDC_CHANNEL 0
#define MCP_0_DIGITIZER_CHANNEL 0

//This was MiB2 and sometime a RM MCP when both RM were present
#define MCP_1_TDC_CHANNEL 1
#define MCP_1_DIGITIZER_CHANNEL 1

//This is PLANACON... no TDC
#define MCP_2_DIGITIZER_CHANNEL 3

//This was MiB3
#define MCP_3_TDC_CHANNEL 2
#define MCP_3_DIGITIZER_CHANNEL 4

//This was RM (most of the time RM #2)
#define MCP_4_TDC_CHANNEL 3
#define MCP_4_DIGITIZER_CHANNEL 5

#define TR0_DIGITIZER_CHANNEL 8

using namespace iMCP_BTF_simpleRecoTree_namespace;

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
