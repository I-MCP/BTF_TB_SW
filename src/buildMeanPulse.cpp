/*************************************************************

    compile with --> c++ -o buildMeanPulse buildMeanPulse.cpp `root-config --cflags --glibs`
    run with --> ./buildMeanPulse Scan_*.dat Scan_number

*************************************************************/
#include "TROOT.h"
#include "TFile.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TChain.h"
#include "TF1.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TMath.h"
#include "TString.h"
#include "TApplication.h"
#include "TArrayF.h"

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
#include "../include/histo_func.h"


int main (int argc, char** argv)
{  
//    TApplication* app = new TApplication("app",0,0);
    gROOT->ProcessLine("#include <vector>");
    //-------get run number---------------------------------------------------------
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
    if(argc > 3) nameMCP.at(2) = "Roma1";
    nameMCP.push_back("ScB");
    nameMCP.push_back("Planacon");
    nameMCP.push_back("MiB3");
    nameMCP.push_back("Roma2");

    std::vector<std::string> pcMCP;                                     
    for(unsigned int ii=0; ii<nameMCP.size(); ++ii) pcMCP.push_back("");
    pcMCP.at(Ch_ref1) = tokens_name.at(2);
    pcMCP.at(Ch_ref2) = tokens_name.at(4);
    pcMCP.at(Ch_1) = tokens_name.at(6);
    pcMCP.at(Ch_2) = tokens_name.at(8);
    pcMCP.at(Ch_3) = tokens_name.at(10);
    
    //---treshold setup Scan-dependent
    init();
    const int iScan = atoi(argv[2])-1;
    float Ch_th[6]={0,0,0,0,0,0};
    Ch_th[Ch_ref1] = _th[iScan][Ch_ref1];
    Ch_th[Ch_ref2] = _th[iScan][Ch_ref2];
    Ch_th[Ch_1] = _th[iScan][Ch_1];
    if(argc > 3) Ch_th[Ch_1] = _th[iScan][2];
    Ch_th[Ch_2] = _th[iScan][Ch_2];
    Ch_th[Ch_3] = _th[iScan][Ch_3];
    
//--------Definitions-----------------------------------------------------------
    int nFiles=1, iRun=0, goodEvt=0;
    //---Mean Pulses shape (time & value of each sample)
    vector<float> MS_sampleValue[9], MS_sampleTime[9];
    //---Coincidence tree
    TFile* out = TFile::Open("tmp/meanPulses_test_Ch"+TString(argv[2])+".root","recreate");
    out->cd();
    TTree* outTree = new TTree("MeanPulsesTree", "MeanPulsesTree");
    outTree->SetDirectory(0);
    float MS_Ch_samples[6]={0,0,0,0,0,0};
    float MS_Ch_times[6]={0,0,0,0,0,0};
    TBranch* b_Ch_samples[6];
    TBranch* b_Ch_times[6];
    for(int i=0; i<6; i++)
    {
        char b_samples[20], b_times[20];
        sprintf(b_samples, "MS_Ch%d_samples", i);
        sprintf(b_times, "MS_Ch%d_times", i);
        b_Ch_samples[i] = outTree->Branch(b_samples, &MS_Ch_samples[i], (TString)b_samples+"/F");
        b_Ch_times[i] = outTree->Branch(b_times, &MS_Ch_times[i], (TString)b_times+"/F");
    }
    
//-------do runs loop-----------------------------------------------------------
    ifstream log (argv[1], ios::in);
    while(log >> nFiles)
    {
        vector<float> digiCh[9];
        float timeCF[9];
        float intSignal[9];
        float baseline[9];
        float ampMax[9];
        int count[5]={0,0,0,0,0}, spare[5]={0,0,0,0,0}, spare2[5]={0,0,0,0,0};
        int tot_tr1=0, tot_tr0=0, trig=0;
        int HV1=0, HV2=0, HV3=0;
        //---data chain
        TChain* chain = new TChain("eventRawData");
        InitTree(chain);
        for(int iFiles=0; iFiles<nFiles; iFiles++)
        {
            log >> iRun;
            char iRun_str[30];
            sprintf(iRun_str, "raw_data/run_IMCP_%d_*.root", iRun);
            chain->Add(iRun_str);
            cout << "Reading:  raw_data/run_IMCP_" << iRun << endl;
        }
        log >> HV1 >> HV2 >> HV3;
        if(iRun == 262) continue;
        for(int iEntry=0; iEntry<chain->GetEntries(); iEntry++)
        {
            //---always clear the std::vector !!!
            for(int iCh=0; iCh<9; iCh++)
            {
                digiCh[iCh].clear();
            }
            //---Read the entry
            chain->GetEntry(iEntry);
            //---DAQ bug workaround
            if(iRun < 145) goodEvt = 10;
            else goodEvt = 1;
            if(evtNumber % goodEvt == 0)   
            {
                //---Read SciFront ADC value and set the e- multiplicity 
                //---(default = 1)
                trig = 1;
                for(int iCh=0; iCh<nAdcChannels; iCh++)
                {
                    if(adcData[iCh] > 1100 && adcBoard[iCh] == 1 && adcChannel[iCh] == 0) trig=2;
                    if(adcData[iCh] < 400 && adcBoard[iCh] == 1 && adcChannel[iCh] == 0) trig=0;
                }
                if(trig > 0) continue;
                //---Read digitizer samples
                for(int iSample=0; iSample<nDigiSamples; iSample++)
                        digiCh[digiChannel[iSample]].push_back(digiSampleValue[iSample]);
                for(int iCh=0; iCh<6; iCh++)
                {
                    baseline[iCh]=SubtractBaseline(5, 25, &digiCh[iCh]);
                    if(iCh == 3)
                    {
                        for(int iSample=0; iSample<digiCh[iCh].size(); iSample++)
                            digiCh[iCh].at(iSample) = -1.*digiCh[iCh].at(iSample);
                    }
                    timeCF[iCh]=TimeConstFrac(47, 500, &digiCh[iCh], 1.);
                    int t1 = (int)(timeCF[iCh]/0.2) - 3;
                    int t2 = (int)(timeCF[iCh]/0.2) + 17;
                    if(t1 > 50 && t1 < 500 && t2 > 50 && t2 < 500)
                    {
                        intSignal[iCh] = ComputeIntegral(t1, t2, &digiCh[iCh]);
                        ampMax[iCh] = AmpMax(t1, t2, &digiCh[iCh]);
                    }
                    else
                    {
                        intSignal[iCh] = 100;
                        ampMax[iCh] = AmpMax(0, 500, &digiCh[iCh]);
                    }
                }
                //---Multiplicity == 1 --> compute mean pulse
                if(intSignal[Ch_ref1] < Ch_th[Ch_ref1]/**4*/ && intSignal[Ch_ref2] < Ch_th[Ch_ref2]/**4 && trig==1*/) 
                {
                    //---reset
                    for(int iSample=0; iSample<1024; iSample++)
                    {
                        tot_tr1++;
                        if(intSignal[Ch_1] < Ch_th[Ch_1]*4) 
                        {
                            MS_Ch_samples[Ch_1] = digiCh[Ch_1].at(iSample)/ampMax[Ch_1];
                            MS_Ch_times[Ch_1] = iSample*0.2 - timeCF[Ch_1];
                        }
                        else
                        {
                            MS_Ch_samples[Ch_1] = 0;
                            MS_Ch_times[Ch_1] = -1000;
                        }
                        if(intSignal[Ch_2] < Ch_th[Ch_2]*4)
                        {
                            MS_Ch_samples[Ch_2] = digiCh[Ch_2].at(iSample)/ampMax[Ch_2];
                            MS_Ch_times[Ch_2] = iSample*0.2 - timeCF[Ch_2];
                        }
                        else
                        {
                            MS_Ch_samples[Ch_2] = 0;
                            MS_Ch_times[Ch_2] = -1000;
                        }
                        if(intSignal[Ch_3] < Ch_th[Ch_3]*4)
                        {
                            MS_Ch_samples[Ch_3] = digiCh[Ch_3].at(iSample)/ampMax[Ch_3];
                            MS_Ch_times[Ch_3] = iSample*0.2 - timeCF[Ch_3];
                        }
                        else
                        {
                            MS_Ch_samples[Ch_3] = 0;
                            MS_Ch_times[Ch_3] = -1000;
                        }
                        MS_Ch_samples[Ch_ref1] = digiCh[Ch_ref1].at(iSample);///ampMax[Ch_ref1];
                        MS_Ch_times[Ch_ref1] = iSample*0.2 - timeCF[Ch_ref1];
                        MS_Ch_samples[Ch_ref2] = digiCh[Ch_ref2].at(iSample)/ampMax[Ch_ref2];
                        MS_Ch_times[Ch_ref2] = iSample*0.2 - timeCF[Ch_ref1];
			//---Fill output tree
			outTree->Fill();    
		    }
                }
            }
        }  	
        chain->Delete();
    }
    outTree->Write();
    out->Close();
    
    //--------Save Waves------------------------------------------------------------
    int Nbins = (MS_HIGH_TIME - MS_LOW_TIME) / MS_SAMPLING_UNIT;
    //---Get Mean Pulse
    TFile* in = TFile::Open("tmp/meanPulses_Ch"+TString(argv[2])+".root","r");
    TTree* inTree = (TTree*)in->Get("MeanPulsesTree");
    //---function tree
    TString out_name = TString("tmp/"+tokens_name.at(0)+"_MS_func.root");
    TFile* outWave = TFile::Open(out_name,"recreate");
    outWave->cd();
    TH1F* histosMS[6];
    TH1F* MS_cf_times = new TH1F("MS_cf_times","MS_cf_times",6,0,6);
    TH1F* MS_t1_times = new TH1F("MS_t1_times","MS_t1_times",6,0,6);
    TH1F* MS_t2_times = new TH1F("MS_t2_times","MS_t2_times",6,0,6);
    for(int iCh=0; iCh<6; iCh++)
    {
        char histoMS_name[20];
        sprintf(histoMS_name, "MS_fitfunc_Ch%d", iCh);
        TProfile* pr = new TProfile(histoMS_name, histoMS_name, Nbins, MS_LOW_TIME, MS_HIGH_TIME); 
        char draw_string[100], cuts[100];
        sprintf(draw_string, "-MS_Ch%d_samples:MS_Ch%d_times>>MS_fitfunc_Ch%d", iCh, iCh, iCh);
        sprintf(cuts, "MS_Ch%d_times > %d || MS_Ch%d_times < %d", iCh, (int)MS_LOW_TIME, iCh, (int)MS_HIGH_TIME);
        inTree->Draw(draw_string, cuts);
        histosMS[iCh] = (TH1F*)pr;
        histosMS[iCh]->Write();
        histoFunc* tmp = new histoFunc(histosMS[iCh]);
        TF1* f_tmp = new TF1("f"+TString(iCh), tmp, -15, 30, 4, "histoFunc");
        f_tmp->SetParameters(1,1,0,0);
        MS_cf_times->SetBinContent(iCh+1, f_tmp->GetX(-0.5, -15, f_tmp->GetMinimumX()));
        MS_t1_times->SetBinContent(iCh+1, f_tmp->GetX(-0.1, -15, f_tmp->GetMinimumX()));
        MS_t2_times->SetBinContent(iCh+1, f_tmp->GetX(-0.1, f_tmp->GetMinimumX(), f_tmp->GetMinimumX()+3));
    }
    MS_cf_times->Write();
    MS_t1_times->Write();
    MS_t2_times->Write();
    outWave->Close();
}

        
