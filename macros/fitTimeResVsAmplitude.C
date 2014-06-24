#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TROOT.h"

struct fitSlices
{
  std::vector<TH1F*> _histoSlices;

  std::vector<float> _x;
  std::vector<float> _chi2;
  std::vector<float> _par0;
  std::vector<float> _par0err;
  std::vector<float> _par1;
  std::vector<float> _par1err;
  std::vector<float> _par2;
  std::vector<float> _par2err;
};

fitSlices fitProfile(TH2F* h)
{
  fitSlices returnObject;
  
  for (int i(1);i<=h->GetNbinsX();++i)
    {
      //Fill the slices histogram
      TH1F* sliceHisto=new TH1F(Form("%s_slice%d",h->GetName(),i),Form("%s_slice%d",h->GetName(),i),h->GetNbinsY(),h->GetYaxis()->GetXmin(),h->GetYaxis()->GetXmax());
      for (int j(1);j<=h->GetNbinsY();++j)
	{
	  sliceHisto->SetBinContent(j,h->GetBinContent(i,j));
	  sliceHisto->SetBinError(j,h->GetBinError(i,j));
	}

      std::cout << "+++++ " << sliceHisto->GetName() << std::endl;
      if (sliceHisto->Integral()>10.)
	sliceHisto->Fit("gaus","R","",-1,1);
      

      returnObject._histoSlices.push_back(sliceHisto);
      if (sliceHisto->Integral()>10.)
	{
	  returnObject._x.push_back(h->GetXaxis()->GetBinCenter(i));
	  returnObject._chi2.push_back(sliceHisto->GetFunction("gaus")->GetChisquare());
	  returnObject._par0.push_back(sliceHisto->GetFunction("gaus")->GetParameter(0));
	  returnObject._par0err.push_back(sliceHisto->GetFunction("gaus")->GetParError(0));
	  returnObject._par1.push_back(sliceHisto->GetFunction("gaus")->GetParameter(1));
	  returnObject._par1err.push_back(sliceHisto->GetFunction("gaus")->GetParError(1));
	  returnObject._par2.push_back(sliceHisto->GetFunction("gaus")->GetParameter(2));
	  returnObject._par2err.push_back(sliceHisto->GetFunction("gaus")->GetParError(2));
	}
    }
  return returnObject;
}

void fitTimeResVsAmplitude(const char* filename,const char* outputDir){

  TFile *_file0 = TFile::Open(filename);
  //  TH2F* timeVsAmplitude=new TH2F("timeVsAmplitude","timeVsAmplitude",120,2,322,400,49.,53.);
  TH2F* timeVsAmplitude=new TH2F("timeVsAmplitude","timeVsAmplitude",120,2,302,200,-2,2);
  TTree* eventData=(TTree*)_file0->Get("eventData");

  //  eventData->Project(timeVsAmplitude->GetName(),"ch_1_time_at_frac30-51.2:ch_1_max_amplitude","(ch_1_time_at_max>50 &&  ch_1_time_at_max<53) && ch_1_max_amplitude>2. && !(ch_1_max_amplitude>14.5 && ch_1_max_amplitude<202)","");
  //  eventData->Project(timeVsAmplitude->GetName(),"ch_1_time_at_frac30-ch_0_time_at_frac30:(ch_1_max_amplitude*ch_0_max_amplitude)/TMath::Sqrt(ch_0_max_amplitude*ch_0_max_amplitude+ch_1_max_amplitude*ch_1_max_amplitude)","(ch_1_time_at_max>50 &&  ch_1_time_at_max<53) && ch_1_max_amplitude>2. && (ch_0_time_at_max>50 &&  ch_0_time_at_max<53) && ch_0_max_amplitude>2.","");
  eventData->Project(timeVsAmplitude->GetName(),"ch_0_time_at_frac30-51.2:ch_0_max_amplitude","(ch_0_time_at_max>50 &&  ch_0_time_at_max<53) && ch_0_max_amplitude>2. && !(ch_0_max_amplitude>14.5 && ch_0_max_amplitude<202)","");

  fitSlices timeVsAmplitudeProfile=fitProfile(timeVsAmplitude);

  double x[timeVsAmplitudeProfile._x.size()];
  double xErr[timeVsAmplitudeProfile._x.size()];
  double chi2[timeVsAmplitudeProfile._x.size()];
  double mean[timeVsAmplitudeProfile._x.size()];
  double meanErr[timeVsAmplitudeProfile._x.size()];
  double sigma[timeVsAmplitudeProfile._x.size()];
  double sigmaErr[timeVsAmplitudeProfile._x.size()];

  for (int i(0);i<timeVsAmplitudeProfile._x.size();++i)
    {
      x[i]=timeVsAmplitudeProfile._x[i];
      xErr[i]=0;
      chi2[i]=timeVsAmplitudeProfile._chi2[i];
      mean[i]=timeVsAmplitudeProfile._par1[i];
      meanErr[i]=timeVsAmplitudeProfile._par1err[i];
      sigma[i]=timeVsAmplitudeProfile._par2[i];
      sigmaErr[i]=timeVsAmplitudeProfile._par2err[i];
    }

  TLatex *text=new TLatex();
  text->SetTextAlign(12);
  text->SetTextSize(0.035);

  gROOT->SetStyle("plain");

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(111111);
  TFile* f=TFile::Open(Form("%s/fitProfiles.root",outputDir),"RECREATE");
  
  TGraphErrors* gr=new TGraphErrors(timeVsAmplitudeProfile._x.size(),x,sigma,xErr,sigmaErr);
  TCanvas *c=new TCanvas("c","c",1024,768);
  gr->SetMarkerStyle(20);
  gr->SetMarkerSize(1.1);
  gr->GetXaxis()->SetTitle("Signal amplitude (mV)");
  gr->GetYaxis()->SetTitle("#sigma_{t} (ns)");
  gr->Draw("AP");


  TF1* f1=new TF1("f1","TMath::Sqrt(([0]*[0])/(x*x)+[1]*[1])",1,400);
  f1->SetParameter(0,0.8);
  f1->SetParameter(1,0.01);
  gr->Fit("f1");


  text->DrawTextNDC(0.14,0.93,"MCP BINP Rome#1 -2.8 kV");  
  text->DrawLatex(230.,0.25,"#sigma_{t} = #frac{p0}{A} #oplus p1");  

  c->SaveAs(Form("%s/sigmaProfile.png",outputDir));
  f->cd();
  for (int i(0);i<timeVsAmplitude->GetNbinsX();++i)
    if (timeVsAmplitudeProfile._histoSlices[i]->Integral()>0)
      timeVsAmplitudeProfile._histoSlices[i]->Write();

  f->Write();
  f->Close();

}
