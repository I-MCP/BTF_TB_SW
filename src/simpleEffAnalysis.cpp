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

using namespace std;

#include "iMCP_BTF_simpleEffAnalysis.h"

int main (int argc, char** argv)
{  
//-------Read data config files-----------------------------------------------
    char* inputDir = argv[1];
    std::string inputName = std::string(argv[2]);
    char* outputDir = argv[3];
     
    std::string outDir = outputDir;
    
    system(("mkdir -p "+outDir).c_str());

    char split_char = '/';
    std::vector<std::string> tokens;
    std::istringstream split(inputName);
    for(std::string each; getline(split, each, split_char); 
        tokens.push_back(each));
    
    split_char = '_';
    std::vector<std::string> tokens_name;
    
    int icut = 0;
    for(unsigned int ii = 0; ii < tokens.size(); ii++)
        if(tokens.at(ii).find("Scan") != std::string::npos) icut = ii;

    std::istringstream split_name(tokens.at(icut));
    for(std::string each; getline(split_name, each, split_char); 
        tokens_name.push_back(each));
  
    for(int ii = 0; ii < tokens_name.size(); ii++)
        std::cout << ii << " - " << tokens_name.at(ii) << std::endl;

    const int Ch_ref1 = atoi((tokens_name.at(1)).c_str());
    const int Ch_ref2 = atoi((tokens_name.at(3)).c_str());
    const int Ch_1 = atoi((tokens_name.at(5)).c_str());
    const int Ch_2 = atoi((tokens_name.at(7)).c_str());
    const int Ch_3 = atoi((tokens_name.at(9)).c_str());

    std::vector<std::string> nameMCP;
    nameMCP.push_back("MiB1");
    nameMCP.push_back("MiB2");
    if(argc > 4) nameMCP.at(2) = "Roma1";
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
    pcMCP.at(Ch_3).erase(pcMCP.at(Ch_3).size()-4, pcMCP.at(Ch_3).size()-1);
 
//--------Definition----------------------------------------------------------
    int nFiles=1;
    //---coincidence tree
    TFile* outROOT = TFile::Open(TString(outDir)+"/outTiming.root","recreate");  
    outROOT->cd();
    TTree* outTree = new TTree("coicidence_tree", "coicidence_tree");
    outTree->SetDirectory(0);
    //---open output files    
    std::ofstream data1((outDir+"/"+tokens_name.at(0)+"_"+nameMCP.at(Ch_1)+"_pc_"+pcMCP.at(Ch_1)+".dat").c_str());
    std::ofstream data2((outDir+"/"+tokens_name.at(0)+"_"+nameMCP.at(Ch_2)+"_pc_"+pcMCP.at(Ch_2)+".dat").c_str());
    std::ofstream data3((outDir+"/"+tokens_name.at(0)+"_"+nameMCP.at(Ch_3)+"_pc_"+pcMCP.at(Ch_3)+".dat").c_str());
    //---do runs loop
    ifstream log (argv[2], ios::in);
    while(log >> nFiles)
    {
        //-----Run dependend definition
        int HV1=0, HV2=0, HV3=0, id=0, goodEvt=1;
        //---Chain
        TChain* chain = new TChain("eventRawData");
        //-----Read raw data tree-----------------------------------------------
        for(int iFiles=0; iFiles<nFiles; iFiles++)
        {
            log >> id;
            char id_str[40];
            sprintf(id_str, (std::string(inputDir)+"/run_IMCP_%d_*.root").c_str(), id);
            chain->Add(id_str);
            cout << "Reading: run_IMCP_" << id << endl;
        }
        log >> HV1 >> HV2 >> HV3;

	iMCP_BTF_simpleEffAnalysis a(chain);

	a.outTree=outTree;
	a.Ch_1=Ch_1;
	a.Ch_2=Ch_2;
	a.Ch_3=Ch_3;
	a.Ch_ref1=Ch_ref1;
	a.Ch_ref2=Ch_ref2;

	a.Loop();

        //-----Print Infos------------------------------------------------------
        std::cout << "--------------------------" << std::endl;
        std::cout << "number of events:  " << chain->GetEntries()/10 << std::endl;
        std::cout << "Double:  " << a.tot_tr1 << std::endl;
        std::cout << "No e- :  " << a.tot_tr0 << std::endl;
        std::cout << "--------------------------" << std::endl;
        std::cout << "Ch_ref1: " << Ch_ref1 << " Ch_ref2: " << Ch_ref2 << std::endl;
        std::cout << "Measuring Eff for => Ch_1: " << Ch_1 << " Ch_2: " << Ch_2 << " Ch_3: " << Ch_3 << std::endl;
        std::cout << "Ch_1:  " << a.count[1] << "  " << a.spare[1] << "  " << a.spare2[1] << std::endl;
        std::cout << "Ch_2:  " << a.count[2] << "  " << a.spare[2] << "  " << a.spare2[2] << std::endl;
        std::cout << "Ch_3:  " << a.count[3] << "  " << a.spare[3] << "  " << a.spare2[3] << std::endl;
        std::cout << "--------------------------" << std::endl;
        std::cout << "HV1 = " << HV1 << " HV2 = " << HV2 << " HV3 = " << HV3 << std::endl;
        std::cout << "--------------------------" << std::endl;
        double eff1 = ((double)a.count[1]-(double)a.spare[1])/(double)a.tot_tr1;
        double eff2 = ((double)a.count[2]-(double)a.spare[2])/(double)a.tot_tr1;
        double eff3 = ((double)a.count[3]-(double)a.spare[3])/(double)a.tot_tr1;
        std::cout << "Ch_1 eff:    " << eff1 << std::endl;
        std::cout << "Ch_1 e_err:    " << TMath::Sqrt((eff1*(1-eff1))/a.tot_tr1) << std::endl;
        std::cout << "Ch_2 eff:    " << eff2 << std::endl;
        std::cout << "Ch_2 e_err:    " << TMath::Sqrt((eff2*(1-eff2))/a.tot_tr1) << std::endl;
        std::cout << "Ch_3 eff:    " << eff3 << std::endl;
        std::cout << "Ch_3 e_err:  " << TMath::Sqrt((eff3*(1-eff3))/a.tot_tr1) << std::endl;
        std::cout << "--------------------------" << std::endl;
        //---Fill output files
	data1 << HV1 << " " <<  eff1 << " " << 0 << " " << TMath::Sqrt((eff1*(1-eff1))/a.tot_tr1) << std::endl;
	data2 << HV2 << " " <<  eff2 << " " << 0 << " " << TMath::Sqrt((eff2*(1-eff2))/a.tot_tr1) << std::endl;
	data3 << HV3 << " " << eff3 << " " << 0 << " " << TMath::Sqrt((eff3*(1-eff3))/a.tot_tr1) << std::endl;
        //---Get ready for next run
        chain->Delete();
    }
    //-----close everything-----------------------------------------------------
    data1.close();
    data2.close();
    data3.close();
    outTree->Write();
    outROOT->Close();
    
//---------Done-----------------------------------------------------------------
}

        
