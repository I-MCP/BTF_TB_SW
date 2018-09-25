/*******************************************************************************

    compile with --> c++ -o doAnalysis doAnalysis.cpp `root-config --cflags --glibs`
    run with --> ./doAnalysis Scan_*.dat Scan_number

*******************************************************************************/
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

#include "TROOT.h"
#include "TFile.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TChain.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TMath.h"
#include "TString.h"
#include "TClass.h"
#include "TApplication.h"

#include "Waveform.h"
#include "strtk.hpp"

using namespace std;

#define MAX_DIGI_SAMPLES_TO_STORE 30000
#define MAX_CHANNELS_STORE 5

struct waveform_data
{
  unsigned int n_channels;
  float pedestal[MAX_CHANNELS_STORE]; //from pre-samples;
  float pedestal_rms[MAX_CHANNELS_STORE]; //from pre-samples;
  float max_amplitude[MAX_CHANNELS_STORE]; //max_value;
  float time_at_max[MAX_CHANNELS_STORE]; //emualated constant fraction data
  float time_at_frac30[MAX_CHANNELS_STORE]; //emualated constant fraction data
  float time_at_frac50[MAX_CHANNELS_STORE]; //emualated constant fraction data
  unsigned int   n_samples;
  float samples[MAX_DIGI_SAMPLES_TO_STORE]; //most relevant part of the waveform to be stored for later analysis
  float sampleTime[MAX_DIGI_SAMPLES_TO_STORE]; //most relevant part of the waveform to be stored for later analysis
  int sampleChannel[MAX_DIGI_SAMPLES_TO_STORE]; //most relevant part of the waveform to be stored for later analysis

  void clear()
  {
    n_channels=0;
    for (unsigned int i(0);i<MAX_CHANNELS_STORE;++i)
      {
	pedestal[i]=-1;
	pedestal_rms[i]=-1;
	max_amplitude[i]=-1;
	time_at_max[i]=-1;
	time_at_frac30[i]=-1;
	time_at_frac50[i]=-1;
      }
    n_samples=0;
    for (unsigned int i(0);i<MAX_DIGI_SAMPLES_TO_STORE;++i)
      {
	samples[i]=-999;
	sampleTime[i]=-1;
	sampleChannel[i]=-1;
      }
  };
};

struct eventInfo
{
  unsigned int evtNumber;
  unsigned int evtTimestamp;
  unsigned int evtTimedist;
  unsigned int evtTimeSinceStart;
};

// struct channels_data
// {
//   std::vector<waveform_data> channels_digi_data;

//   void clear()
//   {
//     for (unsigned int i(0);i<channels_digi_data.size();++i)
//       channels_digi_data[i].clear();
//   };
// };

struct LeCroyEvent
{
  eventInfo _evtInfo;

  //MCP data
  waveform_data _channelsData;

  void clear()
  {
    _channelsData.clear();
  };

  //  ClassDef(iMCP_BTF_simpleRecoTree_format,1);
};

LeCroyEvent event;

void bookOutputTree(TTree* tree);
void bookEventInfo(TTree* tree);
void bookChannelsData(TTree* tree);
void bookWaveform(TTree* tree);

void bookOutputTree(TTree* tree)
{
  bookEventInfo(tree);
  bookChannelsData(tree);
  tree->Print();
}

void bookEventInfo(TTree* tree)
{
  tree->Branch("evtNumber",&(event._evtInfo.evtNumber),"evtNumber/i");
  tree->Branch("evtTimestamp",&(event._evtInfo.evtTimestamp),"evtTimestamp/i");
  tree->Branch("evtTimedist",&(event._evtInfo.evtTimedist),"evtTimedist/i");
  tree->Branch("evtTimeSinceStart",&(event._evtInfo.evtTimeSinceStart),"evtTimeSinceStart/i");
}

void bookChannelsData(TTree* tree)
{
  // for (unsigned int i(0);i<event._channelsData.channels_digi_data.size();++i)
  //   {
  //     TString mcpName=Form("ch_%d",i);
  bookWaveform(tree);
   // }
}

void bookWaveform(TTree* tree)
{
  tree->Branch("n_channels",&(event._channelsData.n_channels),"n_channels/i");
  tree->Branch("pedestal",event._channelsData.pedestal,"pedestal[n_channels]/F");
  tree->Branch("pedestal_rms",event._channelsData.pedestal_rms,"pedestal_rms[n_channels]/F");
  tree->Branch("max_amplitude",event._channelsData.max_amplitude,"max_amplitude[n_channels]/F");
  tree->Branch("time_at_frac30",event._channelsData.time_at_frac30,"time_at_frac30[n_channels]/F");
  tree->Branch("time_at_frac50",event._channelsData.time_at_frac50,"time_at_frac50[n_channels]/F");
  tree->Branch("time_at_max",event._channelsData.time_at_max,"time_at_max[n_channels]/F");

  tree->Branch("n_samples",&(event._channelsData.n_samples),"n_samples/i");
  tree->Branch("samples",event._channelsData.samples,"samples[n_samples]/F");
  tree->Branch("sampleTime",event._channelsData.sampleTime,"sampleTime[n_samples]/F");
  tree->Branch("sampleChannel",event._channelsData.sampleChannel,"sampleChannel[n_samples]/I");
}


int main (int argc, char** argv)
{ 
  std::string outputFile = std::string(argv[1]);
  std::string templateName = std::string(argv[2]);

  TFile *out = TFile::Open(outputFile.c_str(),"RECREATE");  
  if (!out->IsOpen())
    {
      std::cerr << "Could not open " << outputFile << std::endl;
      return -1;
    }

  std::string runDir = std::string(argv[3]);
  std::vector<string> chToAnalyze;
  for (int i=4;i<argc;++i)
    chToAnalyze.push_back(std::string(argv[i]));

  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  std::cout << "==> Converting run " << runDir << std::endl;
  std::cout << "==> Channels enabled :";
  for (int i=0; i<chToAnalyze.size(); ++i)
    std::cout << " " << chToAnalyze[i] << "->" << i;
  std::cout << std::endl;
  std::cout << "==> outputFile " << outputFile << std::endl;
  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

  TTree* tree=new TTree("eventData","LeCroy WaveForm Data");
  //  event._channelsData.n_channels=chToAnalyze.size();
  bookOutputTree(tree);

  //Loop over all the events
  for (int i=0;i<=99999;++i)
  //  for (int i=0;i<99;++i)
    {
      int iSave=1;
      event.clear();
      event._evtInfo.evtNumber=i+1;
      
      for (int ich=0;ich<chToAnalyze.size();++ich)
	{
	  std::ostringstream filename;
	  filename << runDir << "/C" << chToAnalyze[ich] << templateName << std::setfill('0') << std::setw(5) << i << ".txt"; 
	  std::string str(filename.str());
	  std::cout << "Opening " << str << std::endl;

	  std::ifstream  myfile(str.c_str(), ios::in);
	  
	  // 	  // Extract ints from data string
	  // 	  std::deque<int> chNumber;
	  
	  // 	  strtk::split_regex("C([\\d]+)Trace",
	  // 			     inputName,
	  // 			     strtk::range_to_type_back_inserter(chNumber),
	  // 			     strtk::regex_match_mode::match_1);
	  
	  //       if (chNumber.size()>0)
	  // 	std::cout  << "chNumber " <<  chNumber[0] << std::endl;
	  //       else
	  // 	continue;
	  
	  if (!myfile.is_open())
	    {
	      //	      std::cerr << "Could not open " << str << std::endl;
	      iSave=0;
	      break;
	      // 	      return -1;
	    }

	  if (i%100 == 0 && ich==0)
	    std::cout << "Reading Lecroy event " << i << " of run " << runDir << std::endl;      

	  std::string line;
	  
	  int nLine=0;
	  
	  string modelName;
	  string segmentSize;
	  string eventDate;
	  string eventTime;
	  string time;
	  string amplitude;
	  Waveform eventWaveform;
	  
	  while ( getline (myfile,line) )
	    {
	      if (nLine == 0)
		{
		  string wavetype;
		  strtk::parse_columns(line,
				       ",",
				       strtk::column_list(0,2),
				       modelName,wavetype);
		  //	      std::cout << "model " << modelName << " wavetype " << wavetype << std::endl;
		  
		}
	      else if (nLine == 1)
		{
		  strtk::parse_columns(line,
				       ",",
				       strtk::column_list(3),
				       segmentSize);
		  // std::cout << "nSamples " << segmentSize << std::endl;
		}
	      else if (nLine >4)
		{
		  strtk::parse_columns(line,
				       ",",
				       strtk::column_list(0,1),
				       time,amplitude);
		  // std::cout << time  << "," << amplitude << std::endl;
		  eventWaveform.addTimeAndSample(std::atof(time.c_str()),std::atof(amplitude.c_str()));
		}
	      ++nLine;
	    }

	  if ( eventWaveform._samples.size()<1)
	    {
	      iSave=0;
	      continue;
	    }

	  Waveform::baseline_informations wave_pedestal;
	  Waveform::max_amplitude_informations wave_max;
	  
	  wave_pedestal=eventWaveform.baseline(1,10); 
	  
	  eventWaveform.offset(wave_pedestal.pedestal);
	  eventWaveform.rescale(-1); //invert Buderk MCPs signal
	  wave_max=eventWaveform.max_amplitude(26,eventWaveform._samples.size()-50,9); //find max amplitude between 50 and 500 samples
	  float cft_30 = eventWaveform.time_at_frac(wave_max.time_at_max - 3.e-9, wave_max.time_at_max, 0.3,  wave_max,9);
	  float cft_50 = eventWaveform.time_at_frac(wave_max.time_at_max - 3.e-9, wave_max.time_at_max, 0.5,  wave_max,9);

	  event._channelsData.n_channels=event._channelsData.n_channels+1;
	  event._channelsData.pedestal[ich]=wave_pedestal.pedestal*1.e3; //storing data in mV
	  event._channelsData.pedestal_rms[ich]=wave_pedestal.rms*1.3;
	  event._channelsData.max_amplitude[ich]=wave_max.max_amplitude*1.e3; 

	  event._channelsData.time_at_max[ich]=wave_max.time_at_max * 1.e9; //storing data in ns
	  event._channelsData.time_at_frac30[ich]=cft_30*1.e9;
	  event._channelsData.time_at_frac50[ich]=cft_50*1.e9;
	  
	  //Filling interesting samples
	  // int s1,s2;
	  // eventWaveform.find_interesting_samples(DIGI_SAMPLES_TO_STORE,wave_max,1.25e-9,1.5e-9,s1,s2);
	  int startingSample=event._channelsData.n_samples;
	  event._channelsData.n_samples+=eventWaveform._samples.size();
	  for (unsigned int isample(0);isample<eventWaveform._samples.size();++isample)
	    {
	      event._channelsData.samples[startingSample+isample]=eventWaveform._samples[isample]*1.e3; //storing data in mV
	      event._channelsData.sampleTime[startingSample+isample]=eventWaveform._times[isample]*1.e9; //storing data in ns
	      event._channelsData.sampleChannel[startingSample+isample]=ich;
	    }
	  myfile.close();
	}
      if (iSave)
	  tree->Fill();
    }
  out->Write();
  out->Close();
}

        
