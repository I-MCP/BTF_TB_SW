//g++  -o thScan `root-config --cflags --glibs` thScan.cpp
// ./thScan WaveForms_BTF/scan.dat

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

#include "../include/analysis_tools.h"
#include "../include/init_tree_BTF.h"


int main (int argc, char** argv)
{  
    // get run number
    std::string inputName = std::string(argv[1]);
    char split_char = '/';
    std::vector<std::string> tokens;
    std::istringstream split(inputName);
    for(std::string each; getline(split, each, split_char); 
        tokens.push_back(each));
    
    split_char = '_';
    std::vector<std::string> tokens_name;
    std::istringstream split_name(tokens.at(1));
    for(std::string each; getline(split_name, each, split_char); 
        tokens_name.push_back(each));

    const int Ch_ref1 = atoi((tokens_name.at(1)).c_str());
    const int Ch_ref2 = atoi((tokens_name.at(3)).c_str());
    const int Ch_1 = atoi((tokens_name.at(5)).c_str());
    const int Ch_2 = atoi((tokens_name.at(7)).c_str());
    const int Ch_3 = atoi((tokens_name.at(9)).c_str());

    std::vector<std::string> nameMCP;
    nameMCP.push_back("MiB1");
    nameMCP.push_back("MiB2");
    nameMCP.push_back("ScB");
    nameMCP.push_back("Planacon");
    nameMCP.push_back("MiB3");
    nameMCP.push_back("Roma2");
    if(tokens_name.at(0) == "Scan3") nameMCP.at(1) = "Roma1";

    std::vector<std::string> pcMCP;                                     
    for(unsigned int ii=0; ii<nameMCP.size(); ++ii) pcMCP.push_back("");
    pcMCP.at(Ch_ref1) = tokens_name.at(2);
    pcMCP.at(Ch_ref2) = tokens_name.at(4);
    pcMCP.at(Ch_1) = tokens_name.at(6);
    pcMCP.at(Ch_2) = tokens_name.at(8);
    pcMCP.at(Ch_3) = tokens_name.at(10);

    //---treshold setup Scan-dependent 
    init();
    const int iScanTh = atoi(argv[2])-1;
    float Ch_th[6]={0,0,0,0,0,0};
    Ch_th[Ch_ref1] = _th[iScanTh][Ch_ref1];
    Ch_th[Ch_ref2] = _th[iScanTh][Ch_ref2];
    Ch_th[Ch_1] = _th[iScanTh][Ch_1];
    Ch_th[Ch_2] = _th[iScanTh][Ch_2];
    Ch_th[Ch_3] = _th[iScanTh][Ch_3];


    TFile* out = TFile::Open(TString("ntuples/"+tokens_name.at(0)+"_outHistos.root"),"recreate");  
    out->cd();
    
    int nFiles=1, iRun=0, goodEvt=0;
    int iScan = 0;
    
    //---usefull histos
    TH1F* chHistoBase_All[9];
    TH1F* chHistoSignal_All[9];
    TH1F* timeDiffHisto[9];
    TH1F* timeDiffHisto_Double = new TH1F("timeDiffHisto_Double", "timeDiffHisto_Double",5000,-10,10);
    //---histos initialization
    for(int iCh=0; iCh<6; ++iCh)
      {
	char h1[30], h2[30], h3[30];
	sprintf(h1, "histoBase_All_Ch%d", iCh);
	sprintf(h2, "histoSignal_All_Ch%d", iCh);
	sprintf(h3, "histoTime_Ch%d", iCh);
	chHistoBase_All[iCh] = new TH1F(h1,h1,30000,-30000,1000);
	chHistoSignal_All[iCh] = new TH1F(h2, h2,30000,-30000,1000);
	timeDiffHisto[iCh] = new TH1F(h3, h3,5000,-100,100);
      } 

    
    //---do runs loop
    ifstream log (argv[1], ios::in);
    while(log >> nFiles)
    {
      ++iScan;
        vector<float> digiCh[9];
        float timeCF[9];
        float baseline[9];
	float intSignal[9];
        int count[5]={0,0,0,0,0}, spare[5]={0,0,0,0,0}, spare2[5]={0,0,0,0,0};
        int tot_tr1=0, tot_tr0=0, trig=0;
        int HV1=0, HV2=0, HV3=0;

	TH1F* chHistoWF_Ch[9];
	TH1F* chHistoBase_Ch[9];
	TH1F* chHistoSignal_Ch[9];
	TH1F* timeDiffHisto_Triple_Ch1 = new TH1F(Form("timeDiffHisto_Triple_Ch1_Scan%d",iScan), "", 5000,-100,100);
	TH1F* timeDiffHisto_Triple_Ch2 = new TH1F(Form("timeDiffHisto_Triple_Ch2_Scan%d",iScan), "", 5000,-100,100);
	TH1F* timeDiffHisto_Triple_Ch3 = new TH1F(Form("timeDiffHisto_Triple_Ch3_Scan%d",iScan), "", 5000,-100,100);

	char ha[10];
	for (int iiw=0;iiw<9;++iiw){
	  sprintf (ha,"histoWF_Ch%d_Scan_%d",iiw, iScan);
	  chHistoWF_Ch[iiw] = new TH1F( ha, "", 1024,0.,1024);
	  chHistoWF_Ch[iiw]->SetXTitle("Waveform");

	  sprintf (ha,"histoBase_Ch%d_Scan_%d",iiw, iScan);
	  chHistoBase_Ch[iiw] = new TH1F( ha, "", 30000,-30000,1000);
	  chHistoBase_Ch[iiw] -> SetXTitle ("Integral BaseLine Ch(ADC)");

	  sprintf (ha,"histoSignal_Ch%d_Scan_%d",iiw, iScan);
	  chHistoSignal_Ch[iiw] = new TH1F( ha, "", 30000,-30000,1000);
	  chHistoSignal_Ch[iiw] -> SetXTitle ("Integral Signal Ch(ADC)");
	}

        //---data chain
        TChain* chain = new TChain("eventRawData");
        InitTree(chain);
        for(int iFiles=0; iFiles<nFiles; iFiles++){
	  log >> iRun;
	  char iRun_str[30];
	  sprintf(iRun_str, "raw_data/run_IMCP_%d_*.root", iRun);
	  chain->Add(iRun_str);
	  cout << "Reading:  raw_data/run_IMCP_" << iRun << endl;
	}
        log >> HV1 >> HV2 >> HV3;
    
        for(int iEntry=0; iEntry<chain->GetEntries(); iEntry++) {

	  //---always clear the std::vector !!!
	  for(int iCh=0; iCh<9; iCh++){
	    digiCh[iCh].clear();
	  }

            //---Read the entry
            chain->GetEntry(iEntry);

            //---DAQ bug workaround
            if(iRun < 145) goodEvt = 10;
            else goodEvt = 1;

            if(evtNumber % goodEvt == 0){
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
                for(int iSample=0; iSample<nDigiSamples; iSample++){
		  if(digiChannel[iSample] == 3){
		    digiCh[digiChannel[iSample]].push_back(-digiSampleValue[iSample]);
		    chHistoWF_Ch[digiChannel[iSample]]->SetBinContent(digiSampleIndex[iSample]+1, -digiSampleValue[iSample]);
		  }
		  else{
		    digiCh[digiChannel[iSample]].push_back(digiSampleValue[iSample]);
		    chHistoWF_Ch[digiChannel[iSample]]->SetBinContent(digiSampleIndex[iSample]+1, digiSampleValue[iSample]);
		  }

		}
                for(int iCh=0; iCh<6; iCh++){
		  baseline[iCh] = SubtractBaseline(5, 25, &digiCh[iCh]);
		  if(trig == 0) {
 		    chHistoBase_All[iCh]->Fill(ComputeIntegral(26, 46, &digiCh[iCh]));
 		    chHistoBase_Ch[iCh]->Fill(ComputeIntegral(26, 46, &digiCh[iCh]));
// 		    chHistoBase_All[iCh]->Fill(ComputeIntegral(0, 150, &digiCh[iCh]));
// 		    chHistoBase_Ch[iCh]->Fill(ComputeIntegral(0, 150, &digiCh[iCh]));
		  }
		  timeCF[iCh]=TimeConstFrac(30, 500, &digiCh[iCh], 0.5);
		  //		  timeDiffHisto[iCh]->Fill(timeCF[iCh]*0.2-timeCF[0]*0.2); 
		  timeDiffHisto[iCh]->Fill(timeCF[iCh]*0.2); 

		  int t1 = (int)(timeCF[iCh]/0.2) - 3;
		  int t2 = (int)(timeCF[iCh]/0.2) + 17;
		  //---Fill the signal integral histo only if the e- multiplicity is 1
		  if(t1 > 30 && t1 < 1024 && t2 > 30 && t2 < 1024 && trig == 1)
                    {
 		      chHistoSignal_All[iCh]->Fill(ComputeIntegral(t1, t2, &digiCh[iCh]));
 		      chHistoSignal_Ch[iCh]->Fill(ComputeIntegral(t1, t2, &digiCh[iCh]));
		      intSignal[iCh] = ComputeIntegral(t1, t2, &digiCh[iCh]);
                    }
		  else intSignal[iCh] = ComputeIntegral(50, 70, &digiCh[iCh]);
		}// loop over Ch
	    }// good Event
	    //---Multiplicity == 1 --> compute efficency, fake rate and timing                                                                          
	    if(intSignal[Ch_ref1] < Ch_th[Ch_ref1] && intSignal[Ch_ref2] < Ch_th[Ch_ref2] && trig == 1){
	      float tDiff = (timeCF[Ch_ref1] - timeCF[Ch_ref2])*0.2;
	      float tMean = (timeCF[Ch_ref1] + timeCF[Ch_ref2])*0.1;
	      timeDiffHisto_Double->Fill(tDiff);
	      timeDiffHisto_Triple_Ch1->Fill(tMean - (timeCF[Ch_1]*0.2));
	      timeDiffHisto_Triple_Ch2->Fill(tMean - (timeCF[Ch_2]*0.2));
	      timeDiffHisto_Triple_Ch3->Fill(tMean - (timeCF[Ch_3]*0.2));
	    }

	  } // loop over entries

	for(int iw=0; iw<6; ++iw){
	  if(iw == 2) continue;
	  chHistoBase_Ch[iw]->Write();
	  chHistoSignal_Ch[iw]->Write();
	  chHistoWF_Ch[iw]->Write();
	}
	timeDiffHisto_Triple_Ch1->Write();
	timeDiffHisto_Triple_Ch2->Write();
	timeDiffHisto_Triple_Ch3->Write();
        chain->Delete();
    }
 
    for(int iw=0; iw<6; ++iw){
      if(iw == 2) continue;
      chHistoBase_All[iw]->Write();
      chHistoSignal_All[iw]->Write();
      timeDiffHisto[iw]->Write();
   }
    timeDiffHisto_Double->Write();
    out->Close();
    return 0;
}

        
