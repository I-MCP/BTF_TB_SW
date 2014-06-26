#include "iMCP_PILAS_simpleRecoTree.h"
#include "Waveform.h"

#include <iostream>

ClassImp(iMCP_PILAS_simpleRecoTree);

iMCP_PILAS_simpleRecoTree::iMCP_PILAS_simpleRecoTree(TTree *tree) : iMCP_BTF_rawDataTree(tree)
{
} 

iMCP_PILAS_simpleRecoTree::~iMCP_PILAS_simpleRecoTree()
{
}

void iMCP_PILAS_simpleRecoTree::bookOutputTree()
{
  bookEventInfo();
  bookChannelsData();
  tree->Print();
}

void iMCP_PILAS_simpleRecoTree::bookEventInfo()
{
  tree->Branch("evtNumber",&(treeData._evtInfo.evtNumber),"evtNumber/i");
  tree->Branch("evtTimestamp",&(treeData._evtInfo.evtTimestamp),"evtTimestamp/i");
  tree->Branch("evtTimedist",&(treeData._evtInfo.evtTimedist),"evtTimedist/i");
  tree->Branch("evtTimeSinceStart",&(treeData._evtInfo.evtTimeSinceStart),"evtTimeSinceStart/i");
}

void iMCP_PILAS_simpleRecoTree::bookChannelsData()
{
  for (unsigned int i(0);i<treeData._channelsData.channels_digi_data.size();++i)
    {
      TString mcpName=Form("ch_%d",i);
      bookWaveform(mcpName,treeData._channelsData.channels_digi_data[i]);
    }
}

void iMCP_PILAS_simpleRecoTree::bookWaveform(TString name, waveform_data& waveform)
{
  tree->Branch(name+"_pedestal",&waveform.pedestal,name+"_pedestal/F");
  tree->Branch(name+"_pedestal_rms",&waveform.pedestal_rms,name+"_pedestal_rms/F");
  tree->Branch(name+"_max_amplitude",&waveform.max_amplitude,name+"_max_amplitude/F");
  tree->Branch(name+"_time_at_frac30",&waveform.time_at_frac30,name+"_time_at_frac30/F");
  tree->Branch(name+"_time_at_frac50",&waveform.time_at_frac50,name+"_time_at_frac50/F");
  tree->Branch(name+"_time_at_max",&waveform.time_at_max,name+"_time_at_max/F");
  tree->Branch(name+"_samples",waveform.samples,name+Form("_samples[%d]/F",DIGI_SAMPLES_TO_STORE));
  tree->Branch(name+"_sampleTime",waveform.sampleTime,name+Form("_sampleTime[%d]/F",DIGI_SAMPLES_TO_STORE));
}

void iMCP_PILAS_simpleRecoTree::cannotOpenFile(const char * file)
{
  std::cerr << "File " << file << " cannot be opened" << std::endl;
  exit(-1);
}

#define CHANNELS_TO_ANALYZE 3

//PiLas configuration: 2 MCP channels + TR0
#define MCP_0_DIGITIZER_CHANNEL 0
#define MCP_1_DIGITIZER_CHANNEL 1
#define TR_0_DIGITIZER_CHANNEL 8

void iMCP_PILAS_simpleRecoTree::Loop()
{
  if (fChain == 0) return;
 
  TFile *out = TFile::Open(outFile,"RECREATE");  
  if (!out->IsOpen())
    cannotOpenFile(outFile.Data());

  std::cout << "===> output file " << outFile << " opened" << std::endl;
  tree=new TTree("eventData","Simple Reconstructed Event Data");
  std::cout << "===> output tree " << tree->GetName() << " created" << std::endl;
  std::cout << "===> booking output tree" << std::endl;
  treeData._channelsData.channels_digi_data.resize(CHANNELS_TO_ANALYZE); //2 MCP Channel + TR0
  bookOutputTree();
  std::cout << "===> tree is booked" << std::endl;

  Long64_t nentries = fChain->GetEntriesFast();

  if (maxEntries!=-1)
    nentries=maxEntries;

  Long64_t nbytes = 0, nb = 0;
  
  unsigned int startTimeStamp=0;
  
  for (Long64_t jentry=0; jentry<nentries;jentry++) {

    //clear output tree data
    treeData.clear();

    Long64_t ientry = LoadTree(jentry);
    if (jentry%1000==0)
      std::cout << "Analysing event " << jentry << std::endl;
    
    if (ientry < 0) break;
    
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    
    unsigned int itime=0;
    
    if (jentry==0)
      {
	startTimeStamp=evtTime;
	std::cout << "First event taken @ timestamp (days from 01/04/2014) " <<  ((float)startTimeStamp/1000.)/86400. << std::endl;
      }

    //Filling evtInfo
    treeData._evtInfo.evtNumber=evtNumber;
    treeData._evtInfo.evtTimestamp=evtTime;
    treeData._evtInfo.evtTimedist=evtTimeDist;
    treeData._evtInfo.evtTimeSinceStart=evtTimeStart;

    Waveform channels_waveforms[CHANNELS_TO_ANALYZE];
    //Loop over digiSamples 
    for (unsigned int i=0;i<nDigiSamples;++i)
      {
	//Checking that everything makes sense 
	if (digiSampleValue[i]<-50 || digiSampleValue[i]>4200)
	  std::cout << "WARNING DIGI sample value outside ADC 12bit range! " << digiSampleValue[i] << std::endl;
	
	if (digiSampleIndex[i]>1023)
	  std::cout << "WARNING DIGI sample index outside digitizer buffer range" << std::endl;
	
	if (digiGroup[i]>3)
	  std::cout << "WARNING DIGI group is unknown!" << std::endl;
	
	if (digiChannel[i]>8 )
	  std::cout << "WARNING DIGI channel is unknown!" << std::endl;

	if ( digiChannel[i]==MCP_0_DIGITIZER_CHANNEL )
	  channels_waveforms[0].addTimeAndSample(digiSampleValue[i],digiSampleIndex[i]*0.2e-9);

	if (digiChannel[i]==MCP_1_DIGITIZER_CHANNEL )
	  channels_waveforms[1].addTimeAndSample(digiSampleValue[i],digiSampleIndex[i]*0.2e-9);

	if (digiChannel[i]==TR_0_DIGITIZER_CHANNEL )
	  channels_waveforms[2].addTimeAndSample(digiSampleValue[i],digiSampleIndex[i]*0.2e-9);

      } //end loop over digis

    Waveform::baseline_informations channels_pedestals[CHANNELS_TO_ANALYZE];
    Waveform::max_amplitude_informations channels_max[CHANNELS_TO_ANALYZE];
    for (unsigned int i(0);i<CHANNELS_TO_ANALYZE;++i)
      {
	  channels_pedestals[i]=channels_waveforms[i].baseline(1,10); 
	  channels_waveforms[i].offset(channels_pedestals[i].pedestal);
	  channels_waveforms[i].rescale(-1); 
	  channels_max[i]=channels_waveforms[i].max_amplitude(26,channels_waveforms[i]._samples.size()-50,5); //find max amplitude between 50 and 500 samples
	  float cft_30 = channels_waveforms[i].time_at_frac(channels_max[i].time_at_max - 3.e-9, channels_max[i].time_at_max, 0.3,  channels_max[i],5);
	  float cft_50 = channels_waveforms[i].time_at_frac(channels_max[i].time_at_max - 3.e-9, channels_max[i].time_at_max, 0.5,  channels_max[i],5);

	  treeData._channelsData.channels_digi_data[i].pedestal=channels_pedestals[i].pedestal; //storing data in mV
	  treeData._channelsData.channels_digi_data[i].pedestal_rms=channels_pedestals[i].rms;
	  treeData._channelsData.channels_digi_data[i].max_amplitude=channels_max[i].max_amplitude; 
	  treeData._channelsData.channels_digi_data[i].time_at_max=channels_max[i].time_at_max*1.e9; //storing data in ns
	  treeData._channelsData.channels_digi_data[i].time_at_frac30=cft_30*1.e9;
	  treeData._channelsData.channels_digi_data[i].time_at_frac50=cft_50*1.e9;
	  
	  //Filling interesting samples
	  int s1,s2;
	  channels_waveforms[i].find_interesting_samples(DIGI_SAMPLES_TO_STORE,channels_max[i],1.25e-9,1.5e-9,s1,s2);
	  for (unsigned int isample(s1);isample<=s2;++isample)
	    {
	      treeData._channelsData.channels_digi_data[i].samples[isample-s1]=channels_waveforms[i]._samples[isample]; //storing data in mV
	      treeData._channelsData.channels_digi_data[i].sampleTime[isample-s1]=channels_waveforms[i]._times[isample]*1.e9; //storing data in ns
	    }

      }

     tree->Fill();
  } //end loop over entries

  tree->Write();
  out->Close();
  std::cout << "===> " << outFile << " closed" << std::endl;
} //end of function

