#include "SimulatePulses.h"
#include "Waveform.h"
#include "WaveformUtils.h"

#include <TKey.h>

#include <iostream>

ClassImp(SimulatePulses);

SimulatePulses::SimulatePulses() : iMCP_BTF_rawDataTree()
{
}


SimulatePulses::~SimulatePulses() 
{
}

void SimulatePulses::cannotOpenFile(const char * file)
{
  std::cerr << "File " << file << " cannot be opened" << std::endl;
  exit(-1);
}

void SimulatePulses::Loop()
{

  //Read profiles from input file
  TFile *prof = TFile::Open(profileFile);  
  TIter nextkey(prof->GetListOfKeys());
  TKey *key;
  while (key = (TKey*)nextkey()) {
    TProfile* p=(TProfile*) key->ReadObj();
    std::cout << "Getting wave profile " << p->GetName() << std::endl;
    ref_profiles.push_back(p);
  }

  TFile *out = TFile::Open(outFile,"RECREATE");  
  if (!out->IsOpen())
    cannotOpenFile(outFile.Data());

  std::cout << "===> output file " << outFile << " opened" << std::endl;
  tree=new TTree("eventRawData","Simulated RAW DATA");
  std::cout << "===> output tree " << tree->GetName() << " created" << std::endl;
  std::cout << "===> booking output tree" << std::endl;
  Book(tree);
  std::cout << "===> tree is booked" << std::endl;

  //not adding any adc or tdc channel for the moment
  nAdcChannels=0;
  nTdcChannels=0;
  evtTimeDist=0;
  evtTimeStart=0;
  evtTime=0;
  boardTriggerBit=0;
  triggerWord=0;

  for (int ientry=0;ientry<nEvents;++ientry)
    {
      evtNumber = ientry + 1;

      if (evtNumber%1000==0)
	std::cout << "Simulating event " << evtNumber << std::endl;
      
      nDigiSamples=0;

      std::vector<Waveform> simulatedWaves;

      for (int iprofile=0;iprofile<ref_profiles.size();++iprofile)
	simulatedWaves.push_back(WaveformUtils::generateWaveform(ref_profiles[iprofile],SAMPLE_BIN_SIZE,NSAMPLES,rms_amplitude_noise));
      
      
      for (int iwave=0;iwave<simulatedWaves.size();++iwave)
	{
	  for (int isample=0;isample<simulatedWaves[iwave]._samples.size();++isample)
	    {
	      digiGroup[nDigiSamples]=int(iwave/9);
	      digiChannel[nDigiSamples]=iwave%9;
	      digiSampleIndex[nDigiSamples]=isample;
	      digiSampleValue[nDigiSamples]=simulatedWaves[iwave]._samples[isample];
	      ++nDigiSamples;
	    }
	}
      tree->Fill();
    } //end loop over simulated events

  tree->Write();
  out->Close();
  std::cout << "===> " << outFile << " closed" << std::endl;
}
