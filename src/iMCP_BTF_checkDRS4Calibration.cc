#include "iMCP_BTF_checkDRS4Calibration.h"
#include "Waveform.h"

#include "TH1F.h"

#include <iostream>

ClassImp(iMCP_BTF_checkDRS4Calibration);

iMCP_BTF_checkDRS4Calibration::iMCP_BTF_checkDRS4Calibration(TTree *tree) : iMCP_BTF_rawDataTree(tree)
{
} 

iMCP_BTF_checkDRS4Calibration::~iMCP_BTF_checkDRS4Calibration()
{
}

void iMCP_BTF_checkDRS4Calibration::cannotOpenFile(const char * file)
{
  std::cerr << "File " << file << " cannot be opened" << std::endl;
  exit(-1);
}

void iMCP_BTF_checkDRS4Calibration::Loop()
{
  if (fChain == 0) return;
 
  TFile *out = TFile::Open(outFile,"RECREATE");  
  if (!out->IsOpen())
    cannotOpenFile(outFile.Data());

  std::cout << "===> output file " << outFile << " opened" << std::endl;

  //   tree=new TTree("eventData","Simple Reconstructed Event Data");
  //   std::cout << "===> output tree " << tree->GetName() << " created" << std::endl;
  //   std::cout << "===> booking output tree" << std::endl;
  //   bookOutputTree();
  //   std::cout << "===> tree is booked" << std::endl;

  TString name;

  for (int i=0;i<interestingChannels.size();++i)
    {
      name=Form("wave_%d_allSamples",interestingChannels[i]);
      writableObjects[name]=(TObject*) (new TH1F(name,name,4096,-0.5,4095.5));
      for(int is=0;is<MAX_SAMPLES_SIZE;++is)
	{
	  name=Form("wave_%d_sample_%d",interestingChannels[i],is);
	  writableObjects[name]=(TObject*) (new TH1F(name,name,4096,-0.5,4095.5));
	}

      for(int is=0;is<MAX_THRESHOLDS_CROSSING-1;++is)
	{
	  name=Form("wave_%d_thr_0_timeCross_%d",interestingChannels[i],is);
	  writableObjects[name]=(TObject*) (new TH1F(name,name,10000,0,1024));
	  name=Form("wave_%d_thr_1_timeCross_%d",interestingChannels[i],is);
	  writableObjects[name]=(TObject*) (new TH1F(name,name,10000,0,1024));
	}
      for(int is=0;is<MAX_THRESHOLDS_CROSSING-1;++is)
	for(int is1=is;is1<MAX_THRESHOLDS_CROSSING;++is1)
	  {
	    name=Form("wave_%d_thr_0_timeCross_%d_%d",interestingChannels[i],is,is1);
	    writableObjects[name]=(TObject*) (new TH1F(name,name,50000,0,500));
	    name=Form("wave_%d_thr_1_timeCross_%d_%d",interestingChannels[i],is,is1);
	    writableObjects[name]=(TObject*) (new TH1F(name,name,50000,0,500));
	  }
    }

  for (int i=0;i<interestingChannels.size()-1;++i)
    for (int j=i+1;j<interestingChannels.size();++j)
      {
	name=Form("waveDifference_%d_%d_allSamples",interestingChannels[i],interestingChannels[j]);
	writableObjects[name]=(TObject*) (new TH1F(name,name,10000,-200,200));
	for(int is=0;is<MAX_SAMPLES_SIZE;++is)
	  {
	    name=Form("waveDifference_%d_%d_sample_%d",interestingChannels[i],interestingChannels[j],is);
	    writableObjects[name]=(TObject*) (new TH1F(name,name,10000,-200,200));
	  }
      }
  
  
  Long64_t nentries = fChain->GetEntriesFast();

  if (maxEntries!=-1)
    nentries=maxEntries;

  Long64_t nbytes = 0, nb = 0;
  
  unsigned int startTimeStamp=0;
  

  int chInterest[34];
  
  for (int i=0;i<34;++i)
    {
      std::vector<int>::iterator result=std::find(interestingChannels.begin(),interestingChannels.end(),i);
      if (result != interestingChannels.end() )
	{
	  chInterest[i]=std::distance(interestingChannels.begin(),result);
	  std::cout << "===> Setting Channel " << i << " at position " << chInterest[i] << std::endl;
	}
      else
	chInterest[i]=-1;
    }

  for (Long64_t jentry=0; jentry<nentries;jentry++) {

//     //clear output tree data
//     treeData.clear();

    Waveform waveforms[interestingChannels.size()+1];
//     Waveform scint_waveforms[SCINT_DIGI_CHANNELS];

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

//     //Filling evtInfo
//     treeData._evtInfo.evtNumber=evtNumber;
//     treeData._evtInfo.evtTimestamp=evtTime;
//     treeData._evtInfo.evtTimedist=evtTimeDist;
//     treeData._evtInfo.evtTimeSinceStart=evtTimeStart;

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
 	if ( chInterest[digiChannel[i]]>-1 )
 	  waveforms[chInterest[digiChannel[i]]].addTimeAndSample(digiSampleIndex[i]*0.2e-9,digiSampleValue[i]);
	
       } //end loop over digis

     for ( int i=0; i<interestingChannels.size();++i)
       {

	 for (int j=0;j<waveforms[i]._samples.size();++j)
	   {
	     name=Form("wave_%d_allSamples",interestingChannels[i]);
	     ((TH1F*) writableObjects[name])->Fill(waveforms[i]._samples[j]);
	     name=Form("wave_%d_sample_%d",interestingChannels[i],j);
	     ((TH1F*) writableObjects[name])->Fill(waveforms[i]._samples[j]);
	   }
	 
	 std::vector<float> crossings_thr_0=waveforms[i].time_at_threshold(0,1000,THR_0_VALUE);
	 std::vector<float> crossings_thr_1=waveforms[i].time_at_threshold(0,1000,THR_1_VALUE);
	 
	 for(int is=0;is<crossings_thr_0.size();++is)
	   {
	     name=Form("wave_%d_thr_0_timeCross_%d",interestingChannels[i],is);
	     ((TH1F*) writableObjects[name])->Fill((crossings_thr_0[is])*1.e9);
	   }
	 for(int is=0;is<crossings_thr_1.size();++is)
	   {
	     name=Form("wave_%d_thr_1_timeCross_%d",interestingChannels[i],is);
	     ((TH1F*) writableObjects[name])->Fill(crossings_thr_1[is]*1.e9);
	   }

	 for(int is=0;is<crossings_thr_0.size()-1;++is)
	   for(int is1=is+1;is1<crossings_thr_0.size();++is1)
	     {
	       name=Form("wave_%d_thr_0_timeCross_%d_%d",interestingChannels[i],is,is1);
	       ((TH1F*)writableObjects[name])->Fill((crossings_thr_0[is1]-crossings_thr_0[is])*1.e9);
	     }

	 for(int is=0;is<crossings_thr_1.size()-1;++is)
	   for(int is1=is+1;is1<crossings_thr_1.size();++is1)
	     {
	       name=Form("wave_%d_thr_1_timeCross_%d_%d",interestingChannels[i],is,is1);
	       ((TH1F*)writableObjects[name])->Fill((crossings_thr_1[is1]-crossings_thr_1[is])*1.e9);
	     }
       }
     
     if ( interestingChannels.size()>1 )
       {
	 //Build relative difference waveforms
	 for (int i=0;i<interestingChannels.size()-1;++i)
	   for (int j=i+1;j<interestingChannels.size();++j)
	     {
	       Waveform difference;
	       assert(waveforms[i]._samples.size() == waveforms[j]._samples.size());
	       for (int is=0;is<waveforms[i]._samples.size();++is)
		 {
		   assert(waveforms[i]._times[is] == waveforms[j]._times[is]);
		   difference.addTimeAndSample(waveforms[i]._times[is],waveforms[i]._samples[is]-waveforms[j]._samples[is]);
		   name=Form("waveDifference_%d_%d_allSamples",interestingChannels[i],interestingChannels[j]);
		   ((TH1F*) writableObjects[name])->Fill(difference._samples[is]);
		   name=Form("waveDifference_%d_%d_sample_%d",interestingChannels[i],interestingChannels[j],is);
		   ((TH1F*) writableObjects[name])->Fill(difference._samples[is]);
		 }
	     }
       }
  } //end loop over entries
  
  //   tree->Write();
  //Writing out all objects
  for (std::map<TString,TObject*>::iterator it=writableObjects.begin();it!=writableObjects.end();++it)
    {
      if (((TH1F*) it->second)->GetEntries()>0)
	it->second->Write();
    }
  out->Close();
  std::cout << "===> " << outFile << " closed" << std::endl;
} //end of function

