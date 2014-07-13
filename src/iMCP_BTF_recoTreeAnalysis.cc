#include "iMCP_BTF_recoTreeAnalysis.h"

#include <TH2.h>
#include <TProfile.h>
#include <TStyle.h>
#include <TCanvas.h>




#include "Waveform.h"
#include "WaveformFit.h"

#include <iostream>

ClassImp(iMCP_BTF_recoTreeAnalysis);

iMCP_BTF_recoTreeAnalysis::iMCP_BTF_recoTreeAnalysis(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/t3/users/cmsdaqtb/data/data/BTF/IMCP/processedDataTree/v1/run_IMCP_145_20140521-131000_beam_RECO.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/t3/users/cmsdaqtb/data/data/BTF/IMCP/processedDataTree/v1/run_IMCP_145_20140521-131000_beam_RECO.root");
      }
      f->GetObject("eventData",tree);

   }
   Init(tree);
}

iMCP_BTF_recoTreeAnalysis::~iMCP_BTF_recoTreeAnalysis()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t iMCP_BTF_recoTreeAnalysis::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t iMCP_BTF_recoTreeAnalysis::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void iMCP_BTF_recoTreeAnalysis::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("evtNumber", &evtNumber, &b_evtNumber);
   fChain->SetBranchAddress("evtTimestamp", &evtTimestamp, &b_evtTimestamp);
   fChain->SetBranchAddress("evtTimedist", &evtTimedist, &b_evtTimedist);
   fChain->SetBranchAddress("evtTimeSinceStart", &evtTimeSinceStart, &b_evtTimeSinceStart);
   fChain->SetBranchAddress("scintFront_adc_data", &scintFront_adc_data, &b_scintFront_adc_data);
   fChain->SetBranchAddress("scintBack_adc_data", &scintBack_adc_data, &b_scintBack_adc_data);
   fChain->SetBranchAddress("scintFront_pedestal", &scintFront_waveInfo.pedestal, &b_scintFront_pedestal);
   fChain->SetBranchAddress("scintFront_pedestal_rms", &scintFront_waveInfo.pedestal_rms, &b_scintFront_pedestal_rms);
   fChain->SetBranchAddress("scintFront_max_amplitude", &scintFront_waveInfo.max_amplitude, &b_scintFront_max_amplitude);
   fChain->SetBranchAddress("scintFront_time_at_frac30", &scintFront_waveInfo.time_at_frac30, &b_scintFront_time_at_frac30);
   fChain->SetBranchAddress("scintFront_time_at_frac50", &scintFront_waveInfo.time_at_frac50, &b_scintFront_time_at_frac50);
   fChain->SetBranchAddress("scintFront_time_at_max", &scintFront_waveInfo.time_at_max, &b_scintFront_time_at_max);
   fChain->SetBranchAddress("scintFront_samples", scintFront_waveInfo.samples, &b_scintFront_samples);
   fChain->SetBranchAddress("scintFront_sampleTime", scintFront_waveInfo.sampleTime, &b_scintFront_sampleTime);
   fChain->SetBranchAddress("scintFront_baseline_samples", scintFront_waveInfo.baseline_samples, &b_scintFront_baseline_samples);
   fChain->SetBranchAddress("scintFront_baseline_sampleTime", scintFront_waveInfo.baseline_sampleTime, &b_scintFront_baseline_sampleTime);
   fChain->SetBranchAddress("scintBack_pedestal", &scintBack_waveInfo.pedestal, &b_scintBack_pedestal);
   fChain->SetBranchAddress("scintBack_pedestal_rms", &scintBack_waveInfo.pedestal_rms, &b_scintBack_pedestal_rms);
   fChain->SetBranchAddress("scintBack_max_amplitude", &scintBack_waveInfo.max_amplitude, &b_scintBack_max_amplitude);
   fChain->SetBranchAddress("scintBack_time_at_frac30", &scintBack_waveInfo.time_at_frac30, &b_scintBack_time_at_frac30);
   fChain->SetBranchAddress("scintBack_time_at_frac50", &scintBack_waveInfo.time_at_frac50, &b_scintBack_time_at_frac50);
   fChain->SetBranchAddress("scintBack_time_at_max", &scintBack_waveInfo.time_at_max, &b_scintBack_time_at_max);
   fChain->SetBranchAddress("scintBack_samples", scintBack_waveInfo.samples, &b_scintBack_samples);
   fChain->SetBranchAddress("scintBack_sampleTime", scintBack_waveInfo.sampleTime, &b_scintBack_sampleTime);
   fChain->SetBranchAddress("scintBack_baseline_samples", scintBack_waveInfo.baseline_samples, &b_scintBack_baseline_samples);
   fChain->SetBranchAddress("scintBack_baseline_sampleTime", scintBack_waveInfo.baseline_sampleTime, &b_scintBack_baseline_sampleTime);
   fChain->SetBranchAddress("mcp_0_nTdcHits", &mcp_0_nTdcHits, &b_mcp_0_nTdcHits);
   fChain->SetBranchAddress("mcp_0_tdcMeasurement", &mcp_0_tdcMeasurement, &b_mcp_0_tdcMeasurement);
   fChain->SetBranchAddress("mcp_1_nTdcHits", &mcp_1_nTdcHits, &b_mcp_1_nTdcHits);
   fChain->SetBranchAddress("mcp_1_tdcMeasurement", &mcp_1_tdcMeasurement, &b_mcp_1_tdcMeasurement);
   fChain->SetBranchAddress("mcp_2_nTdcHits", &mcp_2_nTdcHits, &b_mcp_2_nTdcHits);
   fChain->SetBranchAddress("mcp_2_tdcMeasurement", &mcp_2_tdcMeasurement, &b_mcp_2_tdcMeasurement);
   fChain->SetBranchAddress("mcp_3_nTdcHits", &mcp_3_nTdcHits, &b_mcp_3_nTdcHits);
   fChain->SetBranchAddress("mcp_3_tdcMeasurement", &mcp_3_tdcMeasurement, &b_mcp_3_tdcMeasurement);
   fChain->SetBranchAddress("mcp_0_pedestal", &mcp_waveInfo[0].pedestal, &b_mcp_0_pedestal);
   fChain->SetBranchAddress("mcp_0_pedestal_rms", &mcp_waveInfo[0].pedestal_rms, &b_mcp_0_pedestal_rms);
   fChain->SetBranchAddress("mcp_0_max_amplitude", &mcp_waveInfo[0].max_amplitude, &b_mcp_0_max_amplitude);
   fChain->SetBranchAddress("mcp_0_time_at_frac30", &mcp_waveInfo[0].time_at_frac30, &b_mcp_0_time_at_frac30);
   fChain->SetBranchAddress("mcp_0_time_at_frac50", &mcp_waveInfo[0].time_at_frac50, &b_mcp_0_time_at_frac50);
   fChain->SetBranchAddress("mcp_0_time_at_max", &mcp_waveInfo[0].time_at_max, &b_mcp_0_time_at_max);
   fChain->SetBranchAddress("mcp_0_samples", mcp_waveInfo[0].samples, &b_mcp_0_samples);
   fChain->SetBranchAddress("mcp_0_sampleTime", mcp_waveInfo[0].sampleTime, &b_mcp_0_sampleTime);
   fChain->SetBranchAddress("mcp_0_baseline_samples", mcp_waveInfo[0].baseline_samples, &b_mcp_0_baseline_samples);
   fChain->SetBranchAddress("mcp_0_baseline_sampleTime", mcp_waveInfo[0].baseline_sampleTime, &b_mcp_0_baseline_sampleTime);
   fChain->SetBranchAddress("mcp_1_pedestal", &mcp_waveInfo[1].pedestal, &b_mcp_1_pedestal);
   fChain->SetBranchAddress("mcp_1_pedestal_rms", &mcp_waveInfo[1].pedestal_rms, &b_mcp_1_pedestal_rms);
   fChain->SetBranchAddress("mcp_1_max_amplitude", &mcp_waveInfo[1].max_amplitude, &b_mcp_1_max_amplitude);
   fChain->SetBranchAddress("mcp_1_time_at_frac30", &mcp_waveInfo[1].time_at_frac30, &b_mcp_1_time_at_frac30);
   fChain->SetBranchAddress("mcp_1_time_at_frac50", &mcp_waveInfo[1].time_at_frac50, &b_mcp_1_time_at_frac50);
   fChain->SetBranchAddress("mcp_1_time_at_max", &mcp_waveInfo[1].time_at_max, &b_mcp_1_time_at_max);
   fChain->SetBranchAddress("mcp_1_samples", mcp_waveInfo[1].samples, &b_mcp_1_samples);
   fChain->SetBranchAddress("mcp_1_sampleTime", mcp_waveInfo[1].sampleTime, &b_mcp_1_sampleTime);
   fChain->SetBranchAddress("mcp_1_baseline_samples", mcp_waveInfo[1].baseline_samples, &b_mcp_1_baseline_samples);
   fChain->SetBranchAddress("mcp_1_baseline_sampleTime", mcp_waveInfo[1].baseline_sampleTime, &b_mcp_1_baseline_sampleTime);
   fChain->SetBranchAddress("mcp_2_pedestal", &mcp_waveInfo[2].pedestal, &b_mcp_2_pedestal);
   fChain->SetBranchAddress("mcp_2_pedestal_rms", &mcp_waveInfo[2].pedestal_rms, &b_mcp_2_pedestal_rms);
   fChain->SetBranchAddress("mcp_2_max_amplitude", &mcp_waveInfo[2].max_amplitude, &b_mcp_2_max_amplitude);
   fChain->SetBranchAddress("mcp_2_time_at_frac30", &mcp_waveInfo[2].time_at_frac30, &b_mcp_2_time_at_frac30);
   fChain->SetBranchAddress("mcp_2_time_at_frac50", &mcp_waveInfo[2].time_at_frac50, &b_mcp_2_time_at_frac50);
   fChain->SetBranchAddress("mcp_2_time_at_max", &mcp_waveInfo[2].time_at_max, &b_mcp_2_time_at_max);
   fChain->SetBranchAddress("mcp_2_samples", mcp_waveInfo[2].samples, &b_mcp_2_samples);
   fChain->SetBranchAddress("mcp_2_sampleTime", mcp_waveInfo[2].sampleTime, &b_mcp_2_sampleTime);
   fChain->SetBranchAddress("mcp_2_baseline_samples", mcp_waveInfo[2].baseline_samples, &b_mcp_2_baseline_samples);
   fChain->SetBranchAddress("mcp_2_baseline_sampleTime", mcp_waveInfo[2].baseline_sampleTime, &b_mcp_2_baseline_sampleTime);
   fChain->SetBranchAddress("mcp_3_pedestal", &mcp_waveInfo[3].pedestal, &b_mcp_3_pedestal);
   fChain->SetBranchAddress("mcp_3_pedestal_rms", &mcp_waveInfo[3].pedestal_rms, &b_mcp_3_pedestal_rms);
   fChain->SetBranchAddress("mcp_3_max_amplitude", &mcp_waveInfo[3].max_amplitude, &b_mcp_3_max_amplitude);
   fChain->SetBranchAddress("mcp_3_time_at_frac30", &mcp_waveInfo[3].time_at_frac30, &b_mcp_3_time_at_frac30);
   fChain->SetBranchAddress("mcp_3_time_at_frac50", &mcp_waveInfo[3].time_at_frac50, &b_mcp_3_time_at_frac50);
   fChain->SetBranchAddress("mcp_3_time_at_max", &mcp_waveInfo[3].time_at_max, &b_mcp_3_time_at_max);
   fChain->SetBranchAddress("mcp_3_samples", mcp_waveInfo[3].samples, &b_mcp_3_samples);
   fChain->SetBranchAddress("mcp_3_sampleTime", mcp_waveInfo[3].sampleTime, &b_mcp_3_sampleTime);
   fChain->SetBranchAddress("mcp_3_baseline_samples", mcp_waveInfo[3].baseline_samples, &b_mcp_3_baseline_samples);
   fChain->SetBranchAddress("mcp_3_baseline_sampleTime", mcp_waveInfo[3].baseline_sampleTime, &b_mcp_3_baseline_sampleTime);
   fChain->SetBranchAddress("mcp_4_pedestal", &mcp_waveInfo[4].pedestal, &b_mcp_4_pedestal);
   fChain->SetBranchAddress("mcp_4_pedestal_rms", &mcp_waveInfo[4].pedestal_rms, &b_mcp_4_pedestal_rms);
   fChain->SetBranchAddress("mcp_4_max_amplitude", &mcp_waveInfo[4].max_amplitude, &b_mcp_4_max_amplitude);
   fChain->SetBranchAddress("mcp_4_time_at_frac30", &mcp_waveInfo[4].time_at_frac30, &b_mcp_4_time_at_frac30);
   fChain->SetBranchAddress("mcp_4_time_at_frac50", &mcp_waveInfo[4].time_at_frac50, &b_mcp_4_time_at_frac50);
   fChain->SetBranchAddress("mcp_4_time_at_max", &mcp_waveInfo[4].time_at_max, &b_mcp_4_time_at_max);
   fChain->SetBranchAddress("mcp_4_samples", mcp_waveInfo[4].samples, &b_mcp_4_samples);
   fChain->SetBranchAddress("mcp_4_sampleTime", mcp_waveInfo[4].sampleTime, &b_mcp_4_sampleTime);
   fChain->SetBranchAddress("mcp_4_baseline_samples", mcp_waveInfo[4].baseline_samples, &b_mcp_4_baseline_samples);
   fChain->SetBranchAddress("mcp_4_baseline_sampleTime", mcp_waveInfo[4].baseline_sampleTime, &b_mcp_4_baseline_sampleTime);
   fChain->SetBranchAddress("hodoX_nClusters", &hodoX_nClusters, &b_hodoX_nClusters);
   fChain->SetBranchAddress("hodoX_clusterPosition", &hodoX_clusterPosition, &b_hodoX_clusterPosition);
   fChain->SetBranchAddress("hodoX_clusterEnergy", &hodoX_clusterEnergy, &b_hodoX_clusterEnergy);
   fChain->SetBranchAddress("hodoY_nClusters", &hodoY_nClusters, &b_hodoY_nClusters);
   fChain->SetBranchAddress("hodoY_clusterPosition", &hodoY_clusterPosition, &b_hodoY_clusterPosition);
   fChain->SetBranchAddress("hodoY_clusterEnergy", &hodoY_clusterEnergy, &b_hodoY_clusterEnergy);
   Notify();
}

Bool_t iMCP_BTF_recoTreeAnalysis::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void iMCP_BTF_recoTreeAnalysis::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}

Int_t iMCP_BTF_recoTreeAnalysis::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}

void iMCP_BTF_recoTreeAnalysis::cannotOpenFile(const char * file)
{
  std::cerr << "File " << file << " cannot be opened" << std::endl;
  exit(-1);
}


void iMCP_BTF_recoTreeAnalysis::Loop()
{
   if (fChain == 0) return;

   
   Long64_t nentries = fChain->GetEntriesFast();
   
   TFile *out = TFile::Open(outFile,"RECREATE");  
   if (!out->IsOpen())
     cannotOpenFile(outFile.Data());
   
   std::cout << "===> output file " << outFile << " opened" << std::endl;
   
   TProfile* mcp_fitProfile[N_MCP_CHANNELS];
   TProfile* mcp_simProfile[N_MCP_CHANNELS];
   TH2F*     mcp_wave[N_MCP_CHANNELS];

   TString name;

   for (int i=0;i<N_MCP_CHANNELS;++i)
     {
       name=Form("mcp_%d_fitProfile",i);
       mcp_fitProfile[i]=new TProfile(name,name,200,-3.,1.);
       writableObjects[name]=(TObject*) mcp_fitProfile[i];

       name=Form("mcp_%d_fullProfile",i);
       mcp_simProfile[i]=new TProfile(name,name,500,-0.001,99.999);
       writableObjects[name]=(TObject*) mcp_simProfile[i];

       name=Form("mcp_%d_wave",i);
       mcp_wave[i]=new TH2F(name,name,500,-10.,10.,340,-0.5,1.2);
       writableObjects[name]=(TObject*) mcp_wave[i];
     }

   Long64_t nbytes = 0, nb = 0;
   if (maxEntries!=-1)
     nentries=maxEntries;


   
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      ////////////////////////////////////////////////////////////////////
      //BEAM EVENT SELECTION => START
      //Selecting only single electron events
      if (scintFront_waveInfo.max_amplitude<300 || scintFront_waveInfo.max_amplitude >1000)
	continue;

      //electrons passed in the first MCP
      if (mcp_waveInfo[0].max_amplitude<300) 
	continue;
      //BEAM EVENT SELECTION => END
      ////////////////////////////////////////////////////////////////////

      // check profiles of MCP shapes
      for (int i=0;i<N_MCP_CHANNELS;++i)
	{
	  if (mcp_waveInfo[i].max_amplitude<50) //require sufficient amplitude to build reference shape
	    continue;

	  if (i==0 && mcp_waveInfo[i].max_amplitude>1050) //avoid saturation in first amplifier...
	    continue;
	  
	  if (  (mcp_waveInfo[i].time_at_max < 30) || (mcp_waveInfo[i].time_at_max > 55 ) ) //select only the "standard shapes" without issues
	    continue;

	  if ( ((mcp_waveInfo[i].time_at_frac50 - mcp_waveInfo[i].time_at_max) > -1. ) || ((mcp_waveInfo[i].time_at_frac50 - mcp_waveInfo[i].time_at_max) < -1.3 ) ) //select only the "standard shapes" without issues
	    continue;
	  
	  
	  for (int j=0;j<N_WAVE_SAMPLES;++j)
	    {
	      mcp_fitProfile[i]->Fill(mcp_waveInfo[i].sampleTime[j]-mcp_waveInfo[i].time_at_max,mcp_waveInfo[i].samples[j]/mcp_waveInfo[i].max_amplitude);
	      mcp_simProfile[i]->Fill((mcp_waveInfo[i].sampleTime[j]-mcp_waveInfo[i].time_at_max)/TIME_SAMPLE_BIN_SIZE+50.,mcp_waveInfo[i].samples[j]/mcp_waveInfo[i].max_amplitude);
	      mcp_wave[i]->Fill(mcp_waveInfo[i].sampleTime[j]-mcp_waveInfo[i].time_at_max,mcp_waveInfo[i].samples[j]/mcp_waveInfo[i].max_amplitude);
	    }
	  

	}

   } //end of loop

   //   //Writing out all objects
   for (std::map<TString,TObject*>::iterator it=writableObjects.begin();it!=writableObjects.end();++it)
     {
       if (((TH1F*) it->second)->GetEntries()>0)
	 {
	   std::cout << "Writing " << it->first << std::endl;
	   it->second->Write();
	 }
     }
   out->Close();
   std::cout << "===> " << outFile << " closed" << std::endl;
}
