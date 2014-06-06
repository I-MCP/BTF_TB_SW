// compile with: g++ -o doAnalysis `root-config --cflags --glibs` doAnalysis.cpp
// or: g++ -o doAnalysis doAnalysis.cpp `root-config --cflags --glibs`
//Launch eg: ./doAnalysis inputDir SetUp/Scan1_0_ON_5_ON_4_OFF_3_OFF_1_ON.dat outputDir

#include "TApplication.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TH1F.h"
#include "TString.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TChain.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TMath.h"

#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sstream> 
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <iomanip>
#include <ctime>
#include <map>
#include <math.h>

#include "iMCP_BTF_simpleEffAnalysis.h"
#include "Waveform.h"

void iMCP_BTF_simpleEffAnalysis::Loop()
{
  //Initialize output tree
  float coinc_Ch1=0, coinc_Ch2=0, coinc_Ch3=0, coinc_ref=0;
  outTree->Branch("coinc_Ch1",&coinc_Ch1,"coinc_Ch1/F");
  outTree->Branch("coinc_Ch2",&coinc_Ch2,"coinc_Ch2/F");
  outTree->Branch("coinc_Ch3",&coinc_Ch3,"coinc_Ch3/F");
  outTree->Branch("coinc_ref",&coinc_ref,"coinc_ref/F");


  for (int i=0;i<5;++i)
    {
      count[i]=0;
      spare[i]=0;
      spare2[i]=0;
    }

  Waveform digiCh[9];


  unsigned long nevents=fChain->GetEntries();

  int goodEvt,trig;


  for(int iEntry=0; iEntry<nevents; iEntry++)
    {
      float timeCF[9];
      float baseline[9];
      float intBase[9], intSignal[9];

      //---always clear the std::vector !!!
      for(int iCh=0; iCh<9; iCh++)
	  digiCh[iCh].clear();

      //---Read the entry
      fChain->GetEntry(iEntry);
      if (iEntry%1000==0)
	std::cout << "Reading event " << iEntry << "/" << nevents << std::endl;

      //---DAQ bug workaround
      if(id < 145) goodEvt = 10;
      else goodEvt = 1;
      if(evtNumber % goodEvt == 0)   
	{
	  //---Read SciFront ADC value and set the e- multiplicity 
	  //---(default = 1)
	  trig = 1;
	  for(int iCh=0; iCh<nAdcChannels; iCh++)
	    {
	      if(adcData[iCh] > 1500 && adcBoard[iCh] == 1 && adcChannel[iCh] == 0) trig=2;
	      if(adcData[iCh] < 500 && adcBoard[iCh] == 1 && adcChannel[iCh] == 0) trig=0;
	    }
	  if(trig > 1) continue; 
	  //---Read digitizer samples
	  for(int iSample=0; iSample<nDigiSamples; iSample++)
	    digiCh[digiChannel[iSample]].addSample(digiSampleValue[iSample]);
	  //---loop over MPC's channels
	  for(int iCh=0; iCh<6; iCh++)
	    {
	      //Simple shape analysis    
	      Waveform::baseline_informations mcp_pedestal;
	      Waveform::max_amplitude_informations mcp_max;
	      mcp_pedestal=digiCh[iCh].baseline(5,25); //use 40 samples between 5-44 to get pedestal and RMS
	      baseline[iCh]=mcp_pedestal.pedestal;
	      digiCh[iCh].offset(baseline[iCh]);
	      
	      if(iCh != 3)
		digiCh[iCh].rescale(-1); //invert Buderk MCPs signal
	      mcp_max=digiCh[iCh].max_amplitude(47,500); //find max amplitude between 50 and 500 samples
	      //	      std::cout << "++++ iCh " << iCh << " MAX " << mcp_max.max_amplitude << " T@MAX" << mcp_max.time_at_max << std::endl;
	      timeCF[iCh]=digiCh[iCh].time_at_frac(47, 500, 0.2, mcp_max);
	      int t1 = (int)(timeCF[iCh]) - 3;
	      int t2 = (int)(timeCF[iCh]) + 17;
	      //std::cout << "++++ iCh " << iCh << " T1 " << t1 << " T2 " << t2 << std::endl;
	      intBase[iCh] = digiCh[iCh].integral(26, 46);
	      if(t1 > 30 && t1 < 1024 && t2 > 30 && t2 < 1024)
		intSignal[iCh] = digiCh[iCh].integral(t1, t2);
	      //std::cout << "++++ iCh " << iCh << " INT_NOISE " << intBase[iCh] << " INT_SIGNAL " <<  intSignal[iCh] << std::endl;
	    }
	  
	  //---Multiplicity == 1 --> compute efficency, fake rate and timing
	  if(intSignal[Ch_ref1] > 150 && intSignal[Ch_ref2] > 150 && trig==1) 
	    {
	      //---reset
	      coinc_Ch1 = -100;
	      coinc_Ch2 = -100;
	      coinc_Ch3 = -100;
	      tot_tr1++;
	      if(intSignal[Ch_1] > 150) 
		{
		  count[1]=count[1]+1;
		  coinc_Ch1 = timeCF[0] - timeCF[Ch_1];
		}
	      if(intBase[Ch_1] > 150) 
		spare[1]=spare[1]+1;
	      if(intSignal[Ch_2] < -70)
		{
		  count[2]=count[2]+1;
		  coinc_Ch2 = timeCF[0] - timeCF[Ch_2];
		}
	      if(intBase[Ch_2] > 70) 
		spare[2]=spare[2]+1;

	      if(intSignal[Ch_3] > 150)
		{
		  count[3]=count[3]+1;
		  coinc_Ch3 = timeCF[0] - timeCF[Ch_3];
		}

	      if(intBase[Ch_3] > 150) 
		spare[3]=spare[3]+1;

	      //---Fill output tree
	      outTree->Fill();    
	    }
	  //---Multiplicity == 0 --> compute fake rate
	  else if(intSignal[Ch_ref1] <= 150 && intSignal[Ch_ref2] <= 150 && trig==0) 
	    {
	      tot_tr0++;
	      if(intSignal[Ch_1] >  150) spare2[1]=spare2[1]+1; 
	      if(intSignal[Ch_1] >  70) spare2[2]=spare2[2]+1; 
	      if(intSignal[Ch_1] >  150) spare2[3]=spare2[3]+1;
	    }
	}
      outTree->Fill();
    }

}

        
