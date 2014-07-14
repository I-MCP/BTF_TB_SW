#include "iMCP_BTF_simpleRecoTree.h"
#include "Waveform.h"
#include "WaveformFit.h"

#include <iostream>

#include <TKey.h>
#include <TPRegexp.h>

ClassImp(iMCP_BTF_simpleRecoTree);

iMCP_BTF_simpleRecoTree::iMCP_BTF_simpleRecoTree(TTree *tree) : iMCP_BTF_rawDataTree(tree)
{
} 

iMCP_BTF_simpleRecoTree::~iMCP_BTF_simpleRecoTree()
{
}

void iMCP_BTF_simpleRecoTree::bookOutputTree()
{
  bookEventInfo();
  bookScintData();
  bookMcpData();
  bookHodoData();
  tree->Print();
}

void iMCP_BTF_simpleRecoTree::bookEventInfo()
{
  tree->Branch("evtNumber",&(treeData._evtInfo.evtNumber),"evtNumber/i");
  tree->Branch("evtTimestamp",&(treeData._evtInfo.evtTimestamp),"evtTimestamp/i");
  tree->Branch("evtTimedist",&(treeData._evtInfo.evtTimedist),"evtTimedist/i");
  tree->Branch("evtTimeSinceStart",&(treeData._evtInfo.evtTimeSinceStart),"evtTimeSinceStart/i");
}

void iMCP_BTF_simpleRecoTree::bookScintData()
{
  tree->Branch("scintFront_adc_data",&(treeData._scintData.scint_adc_data[0]),"scintFront_adc_data/F");
  tree->Branch("scintBack_adc_data",&(treeData._scintData.scint_adc_data[1]),"scintBack_adc_data/F");
  bookWaveform(TString("scintFront"),treeData._scintData.scint_digi_data[0]);
  bookWaveform(TString("scintBack"),treeData._scintData.scint_digi_data[1]);
}

void iMCP_BTF_simpleRecoTree::bookMcpData()
{
  for (unsigned int i(0);i<MCP_TDC_CHANNELS;++i)
    {
      TString mcpName=Form("mcp_%d",i);
      bookTdcData(mcpName,treeData._mcpData.mcp_tdc_data[i]);
    }

  for (unsigned int i(0);i<MCP_DIGI_CHANNELS;++i)
    {
      TString mcpName=Form("mcp_%d",i);
      bookWaveform(mcpName,treeData._mcpData.mcp_digi_data[i]);
    }
}

void iMCP_BTF_simpleRecoTree::bookHodoData()
{
  bookHodoscopePlane(TString("hodoX"),treeData._hodoData.hodoXClusters);
  bookHodoscopePlane(TString("hodoY"),treeData._hodoData.hodoYClusters);
}

void iMCP_BTF_simpleRecoTree::bookWaveform(TString name, waveform_data& waveform)
{
  tree->Branch(name+"_pedestal",&waveform.pedestal,name+"_pedestal/F");
  tree->Branch(name+"_pedestal_rms",&waveform.pedestal_rms,name+"_pedestal_rms/F");
  tree->Branch(name+"_fit_max_amplitude",&waveform.fit_max_amplitude,name+"_fit_max_amplitude/F");
  tree->Branch(name+"_fit_max_amplitude_err",&waveform.fit_max_amplitude_err,name+"_fit_max_amplitude_err/F");
  tree->Branch(name+"_fit_time",&waveform.fit_time,name+"_fit_time/F");
  tree->Branch(name+"_fit_time_err",&waveform.fit_time_err,name+"_fit_time_err/F");
  tree->Branch(name+"_fit_compatibility",&waveform.fit_compatibility,name+"_fit_compatibility/F");
  tree->Branch(name+"_fit_ndof",&waveform.fit_ndof,name+"_fit_ndof/F");
  tree->Branch(name+"_fit_edm",&waveform.fit_edm,name+"_fit_edm/F");
  tree->Branch(name+"_max_amplitude",&waveform.max_amplitude,name+"_max_amplitude/F");
  tree->Branch(name+"_time_at_frac30",&waveform.time_at_frac30,name+"_time_at_frac30/F");
  tree->Branch(name+"_time_at_frac50",&waveform.time_at_frac50,name+"_time_at_frac50/F");
  tree->Branch(name+"_time_at_max",&waveform.time_at_max,name+"_time_at_max/F");
  tree->Branch(name+"_samples",waveform.samples,name+Form("_samples[%d]/F",DIGI_SAMPLES_TO_STORE));
  tree->Branch(name+"_sampleTime",waveform.sampleTime,name+Form("_sampleTime[%d]/F",DIGI_SAMPLES_TO_STORE));
  tree->Branch(name+"_baseline_samples",waveform.baseline_samples,name+Form("_baseline_samples[%d]/F",BASELINE_DIGI_SAMPLES_TO_STORE));
  tree->Branch(name+"_baseline_sampleTime",waveform.baseline_sampleTime,name+Form("_baseline_sampleTime[%d]/F",BASELINE_DIGI_SAMPLES_TO_STORE));
}

void iMCP_BTF_simpleRecoTree::bookTdcData(TString name, tdc_data& tdc)
{
  tree->Branch(name+"_nTdcHits",&tdc.tdc_nCounts,name+"_nTdcHits/i");
  tree->Branch(name+"_tdcMeasurement",&tdc.tdc_measurements,name+"_tdcMeasurement["+name+"_nTdcHits]/F");
}

void iMCP_BTF_simpleRecoTree::bookHodoscopePlane(TString name, hodoscope_plane_data& plane_data)
{
  tree->Branch(name+"_nClusters",&plane_data.nHodoPlaneClusters,name+"_nClusters/i");
  tree->Branch(name+"_clusterPosition",plane_data.hodoPlaneClusterPosition,name+"_clusterPosition["+name+"_nClusters]/F");
  tree->Branch(name+"_clusterEnergy",plane_data.hodoPlaneClusterRawAdcCount,name+"_clusterEnergy["+name+"_nClusters]/F");
}

void iMCP_BTF_simpleRecoTree::cannotOpenFile(const char * file)
{
  std::cerr << "File " << file << " cannot be opened" << std::endl;
  exit(-1);
}

void iMCP_BTF_simpleRecoTree::Loop()
{
  if (fChain == 0) return;


  //Reading mcp fit profiles from file
  TFile *prof = TFile::Open(mcpFitWaveFile);
  if (!prof->IsOpen())
    cannotOpenFile(mcpFitWaveFile.Data());
  TIter nextkey(prof->GetListOfKeys());
  TPRegexp matchName("mcp_(\\d+)_fitProfile");
  TKey *key;
  while (key = (TKey*)nextkey()) {
    TProfile* p=(TProfile*) key->ReadObj();
    if (!TString(p->GetName()).Contains(matchName))
      continue;
    std::cout << "Getting wave profile " << p->GetName() << std::endl;
    mcpFitProfiles.push_back(p);
  }
  
  TFile *out = TFile::Open(outFile,"RECREATE");  
  if (!out->IsOpen())
    cannotOpenFile(outFile.Data());

  std::cout << "===> output file " << outFile << " opened" << std::endl;
  tree=new TTree("eventData","Simple Reconstructed Event Data");
  std::cout << "===> output tree " << tree->GetName() << " created" << std::endl;
  std::cout << "===> booking output tree" << std::endl;
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

    Waveform mcp_waveforms[MCP_DIGI_CHANNELS];
    Waveform scint_waveforms[SCINT_DIGI_CHANNELS];

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

	//This is MiB1
	if ( digiChannel[i]==MCP_0_DIGITIZER_CHANNEL )
	  mcp_waveforms[0].addTimeAndSample(digiSampleIndex[i]*0.2e-9,digiSampleValue[i]);

	//This is MiB2
	if (digiChannel[i]==MCP_1_DIGITIZER_CHANNEL )
	  mcp_waveforms[1].addTimeAndSample(digiSampleIndex[i]*0.2e-9,digiSampleValue[i]);


	//This is PLANACON
	if (digiChannel[i]==MCP_2_DIGITIZER_CHANNEL )
	  mcp_waveforms[4].addTimeAndSample(digiSampleIndex[i]*0.2e-9,digiSampleValue[i]);

	//This is MiB3
	if (digiChannel[i]==MCP_3_DIGITIZER_CHANNEL )
	  mcp_waveforms[2].addTimeAndSample(digiSampleIndex[i]*0.2e-9,digiSampleValue[i]);

	//This is RM
	if (digiChannel[i]==MCP_4_DIGITIZER_CHANNEL )
	  mcp_waveforms[3].addTimeAndSample(digiSampleIndex[i]*0.2e-9,digiSampleValue[i]);


	if (digiChannel[i] == SCINT_FRONT_DIGITIZER_CHANNEL )
	  scint_waveforms[0].addTimeAndSample(digiSampleIndex[i]*0.2e-9,digiSampleValue[i]);

	if (digiChannel[i] == SCINT_BACK_DIGITIZER_CHANNEL)
	  scint_waveforms[1].addTimeAndSample(digiSampleIndex[i]*0.2e-9,digiSampleValue[i]);

      } //end loop over digis

    //Simple shape analysis    
    Waveform::baseline_informations mcp_pedestals[MCP_DIGI_CHANNELS];
    Waveform::max_amplitude_informations mcp_max[MCP_DIGI_CHANNELS];
    for (unsigned int i(0);i<MCP_DIGI_CHANNELS;++i)
      {
	mcp_pedestals[i]=mcp_waveforms[i].baseline(5,44); //use 40 samples between 5-44 to get pedestal and RMS
	mcp_waveforms[i].offset(mcp_pedestals[i].pedestal);

	if (i!=4)
	  mcp_waveforms[i].rescale(-1); //invert Buderk MCPs signal
	mcp_max[i]=mcp_waveforms[i].max_amplitude(150,350,5); //find max amplitude between 50 and 500 samples

	//Fill tree Data
	treeData._mcpData.mcp_digi_data[i].pedestal=mcp_pedestals[i].pedestal;
	treeData._mcpData.mcp_digi_data[i].pedestal_rms=mcp_pedestals[i].rms;
	treeData._mcpData.mcp_digi_data[i].max_amplitude=mcp_max[i].max_amplitude;
	treeData._mcpData.mcp_digi_data[i].time_at_max=mcp_max[i].time_at_max*1.e9;
	treeData._mcpData.mcp_digi_data[i].time_at_frac30=mcp_waveforms[i].time_at_frac(mcp_max[i].time_at_max - 3.e-9, mcp_max[i].time_at_max, 0.3,  mcp_max[i],7)*1.e9; 
	treeData._mcpData.mcp_digi_data[i].time_at_frac50=mcp_waveforms[i].time_at_frac(mcp_max[i].time_at_max - 3.e-9, mcp_max[i].time_at_max, 0.5,  mcp_max[i],7)*1.e9; 

	if (mcp_max[i].max_amplitude>20)
	  {
	    ROOT::Math::Minimizer* minim;
	    WaveformFit::fitWaveform(&mcp_waveforms[i],mcpFitProfiles[i],13,3,mcp_max[i],mcp_pedestals[i],minim);
	    
	    const double *par=minim->X();
	    const double *errors=minim->Errors();
	    
	    treeData._mcpData.mcp_digi_data[i].fit_max_amplitude=par[0];
	    treeData._mcpData.mcp_digi_data[i].fit_max_amplitude_err=errors[0];
	    treeData._mcpData.mcp_digi_data[i].fit_time=par[1];
	    treeData._mcpData.mcp_digi_data[i].fit_time_err=errors[1];
	    treeData._mcpData.mcp_digi_data[i].fit_compatibility=minim->MinValue();
	    treeData._mcpData.mcp_digi_data[i].fit_edm=minim->Edm();
	    treeData._mcpData.mcp_digi_data[i].fit_ndof=16-minim->NFree();
	  }
	else
	  {
	    treeData._mcpData.mcp_digi_data[i].fit_max_amplitude=-999;
	    treeData._mcpData.mcp_digi_data[i].fit_max_amplitude_err=-999;
	    treeData._mcpData.mcp_digi_data[i].fit_time=-999;
	    treeData._mcpData.mcp_digi_data[i].fit_time_err=-999;
	    treeData._mcpData.mcp_digi_data[i].fit_compatibility=9999;
	    treeData._mcpData.mcp_digi_data[i].fit_edm=9999;
	    treeData._mcpData.mcp_digi_data[i].fit_ndof=-999;
	  }

	//Fill baseline samples
	for (unsigned int isample(0);isample<BASELINE_DIGI_SAMPLES_TO_STORE;++isample)
	  {
	    treeData._mcpData.mcp_digi_data[i].baseline_samples[isample]=mcp_waveforms[i]._samples[isample];
	    treeData._mcpData.mcp_digi_data[i].baseline_sampleTime[isample]=mcp_waveforms[i]._times[isample]*1.e9;
	  }

	//Filling interesting samples
	for (unsigned int isample(0);isample<DIGI_SAMPLES_TO_STORE;++isample)
	  {
	    treeData._mcpData.mcp_digi_data[i].samples[isample]=mcp_waveforms[i]._samples[150+isample];
	    treeData._mcpData.mcp_digi_data[i].sampleTime[isample]=mcp_waveforms[i]._times[150+isample]*1.e9;
	  }
      }

    Waveform::baseline_informations scint_pedestals[SCINT_DIGI_CHANNELS];
    Waveform::max_amplitude_informations scint_max[SCINT_DIGI_CHANNELS];
     for (unsigned int i(0);i<SCINT_DIGI_CHANNELS;++i)
      {
	scint_pedestals[i]=scint_waveforms[i].baseline(5,44); //use 40 samples between 5-44 to get pedestal and RMS
	scint_waveforms[i].offset(scint_pedestals[i].pedestal);
	scint_waveforms[i].rescale(-1); //invert 
	scint_max[i]=scint_waveforms[i].max_amplitude(650,850); //find max amplitude 
	//Fill tree Data
	treeData._scintData.scint_digi_data[i].pedestal=scint_pedestals[i].pedestal;
	treeData._scintData.scint_digi_data[i].pedestal_rms=scint_pedestals[i].rms;
	treeData._scintData.scint_digi_data[i].max_amplitude=scint_max[i].max_amplitude;
	treeData._scintData.scint_digi_data[i].time_at_max=scint_max[i].time_at_max*1.e9;
	treeData._scintData.scint_digi_data[i].time_at_frac30=scint_waveforms[i].time_at_frac(scint_max[i].time_at_max - 30.e-9, scint_max[i].time_at_max, 0.3,  scint_max[i],7)*1.e9; 
	treeData._scintData.scint_digi_data[i].time_at_frac50=scint_waveforms[i].time_at_frac(scint_max[i].time_at_max - 30.e-9, scint_max[i].time_at_max, 0.5,  scint_max[i],7)*1.e9; 



	//Fill baseline samples
	for (unsigned int isample(0);isample<BASELINE_DIGI_SAMPLES_TO_STORE;++isample)
	  {
	    treeData._scintData.scint_digi_data[i].baseline_samples[isample]=scint_waveforms[i]._samples[isample];
	    treeData._scintData.scint_digi_data[i].baseline_sampleTime[isample]=scint_waveforms[i]._times[isample]*1.e9;
	  }

	//Filling interesting samples
	for (unsigned int isample(0);isample<DIGI_SAMPLES_TO_STORE;++isample)
	  {
	    if (i==0)
	      {
		treeData._scintData.scint_digi_data[i].samples[isample]=scint_waveforms[i]._samples[620+isample];
		treeData._scintData.scint_digi_data[i].sampleTime[isample]=scint_waveforms[i]._times[620+isample]*1.e9;
	      }
	    else if (i==1)
	      {
		treeData._scintData.scint_digi_data[i].samples[isample]=scint_waveforms[i]._samples[710+isample];
		treeData._scintData.scint_digi_data[i].sampleTime[isample]=scint_waveforms[i]._times[710+isample]*1.e9;
	      }
	  }
      }
     tree->Fill();
  } //end loop over entries

  tree->Write();
  out->Close();
  std::cout << "===> " << outFile << " closed" << std::endl;
} //end of function

