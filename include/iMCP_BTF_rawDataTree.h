//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Feb 20 23:56:33 2014 by ROOT version 5.34/12
// from TTree eventRawData/VME event RAW Data
// found on file: run0000021_20140220.root
//////////////////////////////////////////////////////////

#ifndef iMCP_BTF_rawDataTree_h
#define iMCP_BTF_rawDataTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <string>

#define NUM_ADC_CHANNELS 40
#define NUM_TDC_CHANNELS 16
#define NUM_DIGI_SAMPLES 40000

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

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class iMCP_BTF_rawDataTree {
public :
  std::map<TString,TObject*> outObjects;
  std::string          outFile;
  TTree          *fChain;   //!pointer to the analyzed TTree or TChain
  Int_t           fCurrent; //!current Tree number in a TChain
  
  // Declaration of leaf types
  UInt_t          evtNumber;
  UInt_t          evtTimeDist;
  UInt_t          evtTimeStart;
  UInt_t          evtTime;
  UInt_t          boardTriggerBit;
  UInt_t          triggerWord;
  UInt_t          nAdcChannels;
  UInt_t          adcBoard[NUM_ADC_CHANNELS];   //[nAdcChannels]
  UInt_t          adcChannel[NUM_ADC_CHANNELS];   //[nAdcChannels]
  UInt_t          adcData[NUM_ADC_CHANNELS];   //[nAdcChannels]
  UInt_t          nTdcChannels;
  UInt_t          tdcBoard[NUM_TDC_CHANNELS];   //[nTdcChannels]
  UInt_t          tdcChannel[NUM_TDC_CHANNELS];   //[nTdcChannels]
  UInt_t          tdcData[NUM_TDC_CHANNELS];   //[nTdcChannels]
  UInt_t          nDigiSamples;
  UInt_t          digiGroup[NUM_DIGI_SAMPLES];   //[nDigiSamples]
  UInt_t          digiChannel[NUM_DIGI_SAMPLES];   //[nDigiSamples]
  UInt_t          digiSampleIndex[NUM_DIGI_SAMPLES];   //[nDigiSamples]
  Float_t         digiSampleValue[NUM_DIGI_SAMPLES];   //[nDigiSamples]
  
  
  // List of branches
  TBranch        *b_evtNumber;   //!
  TBranch        *b_evtTimeDist;   //!
  TBranch        *b_evtTimeStart;   //!
  TBranch        *b_evtTime;   //!
  TBranch        *b_boardTriggerBit;   //!
  TBranch        *b_triggerWord;   //!
  TBranch        *b_nAdcChannels;   //!
  TBranch        *b_adcBoard;   //!
  TBranch        *b_adcChannel;   //!
  TBranch        *b_adcData;   //!
  TBranch        *b_nTdcChannels;   //!
  TBranch        *b_tdcBoard;   //!
  TBranch        *b_tdcChannel;   //!
  TBranch        *b_tdcData;   //!
  TBranch        *b_nDigiSamples;   //!
  TBranch        *b_digiGroup;   //!
  TBranch        *b_digiChannel;   //!
  TBranch        *b_digiSampleIndex;   //!
  TBranch        *b_digiSampleValue;   //!
   
   
   iMCP_BTF_rawDataTree();
   iMCP_BTF_rawDataTree(TTree *tree);

   virtual ~iMCP_BTF_rawDataTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Book(TTree *tree);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

  ClassDef(iMCP_BTF_rawDataTree ,1);
};

#endif

