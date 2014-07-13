//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Jul 13 08:30:26 2014 by ROOT version 5.34/07
// from TTree eventData/Simple Reconstructed Event Data
// found on file: /t3/users/cmsdaqtb/data/data/BTF/IMCP/processedDataTree/v1/run_IMCP_145_20140521-131000_beam_RECO.root
//////////////////////////////////////////////////////////

#ifndef iMCP_BTF_recoTreeAnalysis_h
#define iMCP_BTF_recoTreeAnalysis_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

#define N_MCP_CHANNELS 5
#define N_BASELINE_SAMPLES 50
#define N_WAVE_SAMPLES 200
#define TIME_SAMPLE_BIN_SIZE 0.2 

class iMCP_BTF_recoTreeAnalysis {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   TString         outFile;
   std::map<TString,TObject*> writableObjects;
   int             maxEntries=-1;

   struct  wave_info
   {
     Float_t         pedestal;
     Float_t         pedestal_rms;
     Float_t         max_amplitude;
     Float_t         time_at_frac30;
     Float_t         time_at_frac50;
     Float_t         time_at_max;
     Float_t         samples[N_WAVE_SAMPLES];
     Float_t         sampleTime[N_WAVE_SAMPLES];
     Float_t         baseline_samples[N_BASELINE_SAMPLES];
     Float_t         baseline_sampleTime[N_BASELINE_SAMPLES];
   };

   // Declaration of leaf types
   UInt_t          evtNumber;
   UInt_t          evtTimestamp;
   UInt_t          evtTimedist;
   UInt_t          evtTimeSinceStart;
   Float_t         scintFront_adc_data;
   Float_t         scintBack_adc_data;
   wave_info       scintFront_waveInfo;
   wave_info       scintBack_waveInfo;
   wave_info       mcp_waveInfo[N_MCP_CHANNELS];

   UInt_t          mcp_0_nTdcHits;
   Float_t         mcp_0_tdcMeasurement[1];   //[mcp_0_nTdcHits]
   UInt_t          mcp_1_nTdcHits;
   Float_t         mcp_1_tdcMeasurement[1];   //[mcp_1_nTdcHits]
   UInt_t          mcp_2_nTdcHits;
   Float_t         mcp_2_tdcMeasurement[1];   //[mcp_2_nTdcHits]
   UInt_t          mcp_3_nTdcHits;
   Float_t         mcp_3_tdcMeasurement[1];   //[mcp_3_nTdcHits]

   UInt_t          hodoX_nClusters;
   Float_t         hodoX_clusterPosition[1];   //[hodoX_nClusters]
   Float_t         hodoX_clusterEnergy[1];   //[hodoX_nClusters]
   UInt_t          hodoY_nClusters;
   Float_t         hodoY_clusterPosition[1];   //[hodoY_nClusters]
   Float_t         hodoY_clusterEnergy[1];   //[hodoY_nClusters]

   // List of branches
   TBranch        *b_evtNumber;   //!
   TBranch        *b_evtTimestamp;   //!
   TBranch        *b_evtTimedist;   //!
   TBranch        *b_evtTimeSinceStart;   //!
   TBranch        *b_scintFront_adc_data;   //!
   TBranch        *b_scintBack_adc_data;   //!
   TBranch        *b_scintFront_pedestal;   //!
   TBranch        *b_scintFront_pedestal_rms;   //!
   TBranch        *b_scintFront_max_amplitude;   //!
   TBranch        *b_scintFront_time_at_frac30;   //!
   TBranch        *b_scintFront_time_at_frac50;   //!
   TBranch        *b_scintFront_time_at_max;   //!
   TBranch        *b_scintFront_samples;   //!
   TBranch        *b_scintFront_sampleTime;   //!
   TBranch        *b_scintFront_baseline_samples;   //!
   TBranch        *b_scintFront_baseline_sampleTime;   //!
   TBranch        *b_scintBack_pedestal;   //!
   TBranch        *b_scintBack_pedestal_rms;   //!
   TBranch        *b_scintBack_max_amplitude;   //!
   TBranch        *b_scintBack_time_at_frac30;   //!
   TBranch        *b_scintBack_time_at_frac50;   //!
   TBranch        *b_scintBack_time_at_max;   //!
   TBranch        *b_scintBack_samples;   //!
   TBranch        *b_scintBack_sampleTime;   //!
   TBranch        *b_scintBack_baseline_samples;   //!
   TBranch        *b_scintBack_baseline_sampleTime;   //!
   TBranch        *b_mcp_0_nTdcHits;   //!
   TBranch        *b_mcp_0_tdcMeasurement;   //!
   TBranch        *b_mcp_1_nTdcHits;   //!
   TBranch        *b_mcp_1_tdcMeasurement;   //!
   TBranch        *b_mcp_2_nTdcHits;   //!
   TBranch        *b_mcp_2_tdcMeasurement;   //!
   TBranch        *b_mcp_3_nTdcHits;   //!
   TBranch        *b_mcp_3_tdcMeasurement;   //!
   TBranch        *b_mcp_0_pedestal;   //!
   TBranch        *b_mcp_0_pedestal_rms;   //!
   TBranch        *b_mcp_0_max_amplitude;   //!
   TBranch        *b_mcp_0_time_at_frac30;   //!
   TBranch        *b_mcp_0_time_at_frac50;   //!
   TBranch        *b_mcp_0_time_at_max;   //!
   TBranch        *b_mcp_0_samples;   //!
   TBranch        *b_mcp_0_sampleTime;   //!
   TBranch        *b_mcp_0_baseline_samples;   //!
   TBranch        *b_mcp_0_baseline_sampleTime;   //!
   TBranch        *b_mcp_1_pedestal;   //!
   TBranch        *b_mcp_1_pedestal_rms;   //!
   TBranch        *b_mcp_1_max_amplitude;   //!
   TBranch        *b_mcp_1_time_at_frac30;   //!
   TBranch        *b_mcp_1_time_at_frac50;   //!
   TBranch        *b_mcp_1_time_at_max;   //!
   TBranch        *b_mcp_1_samples;   //!
   TBranch        *b_mcp_1_sampleTime;   //!
   TBranch        *b_mcp_1_baseline_samples;   //!
   TBranch        *b_mcp_1_baseline_sampleTime;   //!
   TBranch        *b_mcp_2_pedestal;   //!
   TBranch        *b_mcp_2_pedestal_rms;   //!
   TBranch        *b_mcp_2_max_amplitude;   //!
   TBranch        *b_mcp_2_time_at_frac30;   //!
   TBranch        *b_mcp_2_time_at_frac50;   //!
   TBranch        *b_mcp_2_time_at_max;   //!
   TBranch        *b_mcp_2_samples;   //!
   TBranch        *b_mcp_2_sampleTime;   //!
   TBranch        *b_mcp_2_baseline_samples;   //!
   TBranch        *b_mcp_2_baseline_sampleTime;   //!
   TBranch        *b_mcp_3_pedestal;   //!
   TBranch        *b_mcp_3_pedestal_rms;   //!
   TBranch        *b_mcp_3_max_amplitude;   //!
   TBranch        *b_mcp_3_time_at_frac30;   //!
   TBranch        *b_mcp_3_time_at_frac50;   //!
   TBranch        *b_mcp_3_time_at_max;   //!
   TBranch        *b_mcp_3_samples;   //!
   TBranch        *b_mcp_3_sampleTime;   //!
   TBranch        *b_mcp_3_baseline_samples;   //!
   TBranch        *b_mcp_3_baseline_sampleTime;   //!
   TBranch        *b_mcp_4_pedestal;   //!
   TBranch        *b_mcp_4_pedestal_rms;   //!
   TBranch        *b_mcp_4_max_amplitude;   //!
   TBranch        *b_mcp_4_time_at_frac30;   //!
   TBranch        *b_mcp_4_time_at_frac50;   //!
   TBranch        *b_mcp_4_time_at_max;   //!
   TBranch        *b_mcp_4_samples;   //!
   TBranch        *b_mcp_4_sampleTime;   //!
   TBranch        *b_mcp_4_baseline_samples;   //!
   TBranch        *b_mcp_4_baseline_sampleTime;   //!
   TBranch        *b_hodoX_nClusters;   //!
   TBranch        *b_hodoX_clusterPosition;   //!
   TBranch        *b_hodoX_clusterEnergy;   //!
   TBranch        *b_hodoY_nClusters;   //!
   TBranch        *b_hodoY_clusterPosition;   //!
   TBranch        *b_hodoY_clusterEnergy;   //!

   iMCP_BTF_recoTreeAnalysis(TTree *tree=0);
   virtual ~iMCP_BTF_recoTreeAnalysis();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

   void cannotOpenFile(const char * file);

   ClassDef(iMCP_BTF_recoTreeAnalysis,1);
};

#endif

