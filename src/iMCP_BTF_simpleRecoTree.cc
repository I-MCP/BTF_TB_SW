#include "iMCP_BTF_simpleRecoTree.h"
#include "Waveform.h"

#include <iostream>

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
  tree->Branch(name+"_max_amplitude",&waveform.max_amplitude,name+"_max_amplitude/F");
  tree->Branch(name+"_time_at_frac",&waveform.time_at_frac,name+"_time_at_frac/F");
  tree->Branch(name+"_time_at_max",&waveform.time_at_max,name+"_time_at_max/F");
  tree->Branch(name+"_samples",waveform.samples,name+Form("_samples[%d]/F",DIGI_SAMPLES_TO_STORE));
  tree->Branch(name+"_sampleIndex",waveform.sampleIndex,name+Form("_sampleIndex[%d]/I",DIGI_SAMPLES_TO_STORE));
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
	  mcp_waveforms[0].addSample(digiSampleValue[i]);

	//This is MiB2
	if (digiChannel[i]==MCP_1_DIGITIZER_CHANNEL )
	  mcp_waveforms[1].addSample(digiSampleValue[i]);


	//This is PLANACON
	if (digiChannel[i]==MCP_2_DIGITIZER_CHANNEL )
	  mcp_waveforms[4].addSample(digiSampleValue[i]);

	//This is MiB3
	if (digiChannel[i]==MCP_3_DIGITIZER_CHANNEL )
	  mcp_waveforms[2].addSample(digiSampleValue[i]);

	//This is RM
	if (digiChannel[i]==MCP_4_DIGITIZER_CHANNEL )
	  mcp_waveforms[3].addSample(digiSampleValue[i]);


	if (digiChannel[i] == SCINT_FRONT_DIGITIZER_CHANNEL )
	  scint_waveforms[0].addSample(digiSampleValue[i]);

	if (digiChannel[i] == SCINT_BACK_DIGITIZER_CHANNEL)
	  scint_waveforms[1].addSample(digiSampleValue[i]);

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
	mcp_max[i]=mcp_waveforms[i].max_amplitude(150,350); //find max amplitude between 50 and 500 samples

	//Fill tree Data
	treeData._mcpData.mcp_digi_data[i].pedestal=mcp_pedestals[i].pedestal;
	treeData._mcpData.mcp_digi_data[i].pedestal_rms=mcp_pedestals[i].rms;
	treeData._mcpData.mcp_digi_data[i].max_amplitude=mcp_max[i].max_amplitude;
	treeData._mcpData.mcp_digi_data[i].time_at_max=mcp_max[i].time_at_max;
	treeData._mcpData.mcp_digi_data[i].time_at_frac=-1; //not yet filled

	//Filling interesting samples
	for (unsigned int isample(0);isample<DIGI_SAMPLES_TO_STORE;++isample)
	  {
	    treeData._mcpData.mcp_digi_data[i].samples[isample]=mcp_waveforms[i]._samples[150+isample];
	    treeData._mcpData.mcp_digi_data[i].sampleIndex[isample]=150+isample;
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
	treeData._scintData.scint_digi_data[i].time_at_max=scint_max[i].time_at_max;
	treeData._scintData.scint_digi_data[i].time_at_frac=-1; //not yet filled

	//Filling interesting samples
	for (unsigned int isample(0);isample<DIGI_SAMPLES_TO_STORE;++isample)
	  {
	    if (i==0)
	      {
		treeData._scintData.scint_digi_data[i].samples[isample]=scint_waveforms[i]._samples[620+isample];
		treeData._scintData.scint_digi_data[i].sampleIndex[isample]=620+isample;
	      }
	    else if (i==1)
	      {
		treeData._scintData.scint_digi_data[i].samples[isample]=scint_waveforms[i]._samples[710+isample];
		treeData._scintData.scint_digi_data[i].sampleIndex[isample]=710+isample;
	      }
	  }
      }
     tree->Fill();
  } //end loop over entries

  tree->Write();
  out->Close();
  std::cout << "===> " << outFile << " closed" << std::endl;
} //end of function

