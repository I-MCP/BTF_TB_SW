#include "iMCP_BTF_rawDataTree.h"

ClassImp(iMCP_BTF_rawDataTree);

iMCP_BTF_rawDataTree::iMCP_BTF_rawDataTree() : fChain(0) 
{
}

iMCP_BTF_rawDataTree::iMCP_BTF_rawDataTree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("run0000021_20140220.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("run0000021_20140220.root");
      }
      f->GetObject("eventRawData",tree);

   }
   Init(tree);
}

iMCP_BTF_rawDataTree::~iMCP_BTF_rawDataTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t iMCP_BTF_rawDataTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t iMCP_BTF_rawDataTree::LoadTree(Long64_t entry)
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

void iMCP_BTF_rawDataTree::Init(TTree *tree)
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
   fChain->SetBranchAddress("evtTimeDist", &evtTimeDist, &b_evtTimeDist);
   fChain->SetBranchAddress("evtTimeStart", &evtTimeStart, &b_evtTimeStart);
   fChain->SetBranchAddress("evtTime", &evtTime, &b_evtTime);
   fChain->SetBranchAddress("boardTriggerBit", &boardTriggerBit, &b_boardTriggerBit);
   fChain->SetBranchAddress("triggerWord", &triggerWord, &b_triggerWord);
   fChain->SetBranchAddress("nAdcChannels", &nAdcChannels, &b_nAdcChannels);
   fChain->SetBranchAddress("adcBoard", adcBoard, &b_adcBoard);
   fChain->SetBranchAddress("adcChannel", adcChannel, &b_adcChannel);
   fChain->SetBranchAddress("adcData", adcData, &b_adcData);
   fChain->SetBranchAddress("nTdcChannels", &nTdcChannels, &b_nTdcChannels);
   fChain->SetBranchAddress("tdcBoard", &tdcBoard, &b_tdcBoard);
   fChain->SetBranchAddress("tdcChannel", &tdcChannel, &b_tdcChannel);
   fChain->SetBranchAddress("tdcData", &tdcData, &b_tdcData);
   fChain->SetBranchAddress("nDigiSamples", &nDigiSamples, &b_nDigiSamples);
   fChain->SetBranchAddress("digiGroup", digiGroup, &b_digiGroup);
   fChain->SetBranchAddress("digiChannel", digiChannel, &b_digiChannel);
   fChain->SetBranchAddress("digiSampleIndex", digiSampleIndex, &b_digiSampleIndex);
   fChain->SetBranchAddress("digiSampleValue", digiSampleValue, &b_digiSampleValue);
   Notify();
}

void iMCP_BTF_rawDataTree::Book(TTree *tree)
{
  
  // Set branch addresses and branch pointers
  if (!tree) return;
  fChain = tree;
  fCurrent = -1;

  fChain->Branch("evtNumber",&evtNumber,"evtNumber/i");
  fChain->Branch("evtTimeDist",&evtTimeDist,"evtTimeDist/i");
  fChain->Branch("evtTimeStart",&evtTimeStart,"evtTimeStart/i");
  fChain->Branch("evtTime",&evtTime,"evtTime/i");
  fChain->Branch("boardTriggerBit",&boardTriggerBit,"boardTriggerBit/i");
  fChain->Branch("triggerWord",&triggerWord,"triggerWord/i");
  fChain->Branch("nAdcChannels",&nAdcChannels,"nAdcChannels/i");
  fChain->Branch("adcBoard",adcBoard,"adcBoard[nAdcChannels]/i");
  fChain->Branch("adcChannel",adcChannel,"adcChannel[nAdcChannels]/i");
  fChain->Branch("adcData",adcData,"adcData[nAdcChannels]/i");
  fChain->Branch("nTdcChannels",&nTdcChannels,"nTdcChannels/i");
  fChain->Branch("tdcBoard",tdcBoard,"tdcBoard[nTdcChannels]/i");
  fChain->Branch("tdcChannel",tdcChannel,"tdcChannel[nTdcChannels]/i");
  fChain->Branch("tdcData",tdcData,"tdcData[nTdcChannels]/i");
  fChain->Branch("nDigiSamples",&nDigiSamples,"nDigiSamples/i");
  fChain->Branch("digiGroup",digiGroup,"digiGroup[nDigiSamples]/i");
  fChain->Branch("digiChannel",digiChannel,"digiChannel[nDigiSamples]/i");
  fChain->Branch("digiSampleIndex",digiSampleIndex,"digiSampleIndex[nDigiSamples]/i");
  fChain->Branch("digiSampleValue",digiSampleValue,"digiSample[nDigiSamples]/F");

}

Bool_t iMCP_BTF_rawDataTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void iMCP_BTF_rawDataTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t iMCP_BTF_rawDataTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}

void iMCP_BTF_rawDataTree::Loop()
{
  //Main loop over events
  return;
}
