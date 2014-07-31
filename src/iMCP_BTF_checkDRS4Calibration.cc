#include "iMCP_BTF_checkDRS4Calibration.h"
#include "Waveform.h"
#include "WaveformFit.h"

#include "TH1F.h"
#include "TProfile.h"

#include "TStopwatch.h"

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

void iMCP_BTF_checkDRS4Calibration::bookOutputTree()
{
  tree->Branch("evtNumber",&(treeData.evtNumber),"evtNumber/i");
  tree->Branch("nInterestingChannels",&(treeData.nInterestingChannels),"nInterestingChannels/i");
  tree->Branch("interestingChannels",treeData.interestingChannels,"interestingChannels[nInterestingChannels]/i");
  tree->Branch("nThresholds",&(treeData.nThresholds),"nThresholds/i");
  tree->Branch("thresholds",treeData.thresholds,"thresholds[nThresholds]/F");
  for(int it=0;it<thresholds.size();++it)
    {
      tree->Branch(Form("nCrossings_thr_%d",it),&(treeData.nCrossings[it]),Form("nCrossings_thr_%d/i",it));
      tree->Branch(Form("crossings_thr_%d",it),&(treeData.crossings[it][0][0]),Form("crossings_thr_%d[%d][%d]/F",it,MAX_CHANNELS,MAX_THRESHOLDS_CROSSING));
    }
  tree->Print();
}

void iMCP_BTF_checkDRS4Calibration::Loop()
{
  if (fChain == 0) return;
 
  TFile *out = TFile::Open(outFile,"RECREATE");  
  if (!out->IsOpen())
    cannotOpenFile(outFile.Data());

  std::cout << "===> output file " << outFile << " opened" << std::endl;

  tree=new TTree("eventData","Crossing Times");
  std::cout << "===> output tree " << tree->GetName() << " created" << std::endl;
  std::cout << "===> booking output tree" << std::endl;
  bookOutputTree();
  std::cout << "===> tree is booked" << std::endl;

  TString name;

  TH1F* wave_allSamples[interestingChannels.size()];
  TProfile* wave_profile[interestingChannels.size()];
  TProfile* wave_deriv_profile[interestingChannels.size()];
  TProfile* wave_residual[interestingChannels.size()];
  TH1F* wave_sample[interestingChannels.size()][1024];
  TH1F* wave_sample_residual[interestingChannels.size()][1024];
  TH1F* wave_timeCross[interestingChannels.size()][thresholds.size()][MAX_THRESHOLDS_CROSSING];
  TH1F* wave_timeCross_difference[interestingChannels.size()][thresholds.size()][MAX_THRESHOLDS_CROSSING][MAX_THRESHOLDS_CROSSING];

  for (int i=0;i<interestingChannels.size();++i)
    {
      name=Form("wave_%d_allSamples",interestingChannels[i]);
      wave_allSamples[i]=new TH1F(name,name,4096,-0.5,4095.5);
      writableObjects[name]=(TObject*) wave_allSamples[i];

      for(int is=0;is<MAX_SAMPLES_SIZE;++is)
	{
	  name=Form("wave_%d_sample_%d",interestingChannels[i],is);
	  wave_sample[i][is]=new TH1F(name,name,4096,-0.5,4095.5);
	  writableObjects[name]=(TObject*) ( wave_sample[i][is] );

	  name=Form("wave_%d_sample_residual_%d",interestingChannels[i],is);
	  wave_sample_residual[i][is]=new TH1F(name,name,500,-50.,50.);
	  writableObjects[name]=(TObject*) ( wave_sample_residual[i][is] );
	}

      name=Form("wave_%d_profile",interestingChannels[i]);
      wave_profile[i]=new TProfile(name,name,1024,-0.5,1023.5,"s"); //error is RMS
      writableObjects[name]=(TObject*) (wave_profile[i]);

      name=Form("wave_%d_deriv_profile",interestingChannels[i]);
      wave_deriv_profile[i]=new TProfile(name,name,1024,-0.5,1023.5,"s"); //error is RMS
      writableObjects[name]=(TObject*) (wave_deriv_profile[i]);

      name=Form("wave_%d_residual",interestingChannels[i]);
      wave_residual[i]=new TProfile(name,name,1024,-0.5,1023.5,"s"); //error is RMS
      writableObjects[name]=(TObject*) (wave_residual[i]);

      //writableObjects[name]=(TObject*) (new TProfile(name,name,1024,-0.5,1023.5)); //error is error on mean 

      for(int it=0;it<thresholds.size();++it)
	{
	  for(int is=0;is<MAX_THRESHOLDS_CROSSING;++is)
	    {
	      name=Form("wave_%d_thr_%d_timeCross_%d",interestingChannels[i],it,is);
	      wave_timeCross[i][it][is]=new TH1F(name,name,10000,0,1024);
	      writableObjects[name]=(TObject*) ( wave_timeCross[i][it][is]);
	    }
	  
	  for(int is=0;is<MAX_THRESHOLDS_CROSSING-1;++is)
	    for(int is1=is+1;is1<MAX_THRESHOLDS_CROSSING;++is1)
	      {
		name=Form("wave_%d_thr_%d_timeCross_%d_%d",interestingChannels[i],it,is,is1);
		wave_timeCross_difference[i][it][is][is1]=new TH1F(name,name,50000,0,500);
		writableObjects[name]=(TObject*) (  wave_timeCross_difference[i][it][is][is1] );
	      }
	}
    }

      
  TH1F* waveDifference_allSamples[interestingChannels.size()][interestingChannels.size()];
  TProfile* waveDifference_profile[interestingChannels.size()][interestingChannels.size()];
  TH1F* waveDifference_sample[interestingChannels.size()][interestingChannels.size()][1024];

  TH1F* waveAlignDifference_allSamples[interestingChannels.size()][interestingChannels.size()];
  TProfile* waveAlignDifference_profile[interestingChannels.size()][interestingChannels.size()];
  TH1F* waveAlignDifference_sample[interestingChannels.size()][interestingChannels.size()][1024];
  TH1F* waveAlignDifference_timeCrossDiffference[interestingChannels.size()][interestingChannels.size()][thresholds.size()][MAX_THRESHOLDS_CROSSING];


  for (int i=0;i<interestingChannels.size()-1;++i)
    for (int j=i+1;j<interestingChannels.size();++j)
      {
	name=Form("waveDifference_%d_%d_allSamples",interestingChannels[i],interestingChannels[j]);
	waveDifference_allSamples[i][j]=new TH1F(name,name,10000,-200,200);
	writableObjects[name]=(TObject*) (waveDifference_allSamples[i][j]);

	name=Form("waveDifference_%d_%d_profile",interestingChannels[i],interestingChannels[j]);
	waveDifference_profile[i][j]=new TProfile(name,name,1024,-0.5,1023.5,"s");
	writableObjects[name]=(TObject*) (waveDifference_profile[i][j]);

	for(int is=0;is<MAX_SAMPLES_SIZE;++is)
	  {
	    name=Form("waveDifference_%d_%d_sample_%d",interestingChannels[i],interestingChannels[j],is);
	    waveDifference_sample[i][j][is]=new TH1F(name,name,10000,-200,200);
	    writableObjects[name]=(TObject*) (waveDifference_sample[i][j][is]);
	  }

	name=Form("waveAlignDifference_%d_%d_allSamples",interestingChannels[i],interestingChannels[j]);
	waveAlignDifference_allSamples[i][j]=new TH1F(name,name,10000,-200,200);
	writableObjects[name]=(TObject*) (waveAlignDifference_allSamples[i][j]);

	name=Form("waveAlignDifference_%d_%d_profile",interestingChannels[i],interestingChannels[j]);
	waveAlignDifference_profile[i][j]=new TProfile(name,name,1024,-0.5,1023.5,"s");
	writableObjects[name]=(TObject*) (waveAlignDifference_profile[i][j]);
	
	for(int is=0;is<MAX_SAMPLES_SIZE;++is)
	  {
	    name=Form("waveAlignDifference_%d_%d_sample_%d",interestingChannels[i],interestingChannels[j],is);
	    waveAlignDifference_sample[i][j][is]=new TH1F(name,name,10000,-200,200);
	    writableObjects[name]=(TObject*) (waveAlignDifference_sample[i][j][is]);
	  }

	for(int it=0;it<thresholds.size();++it)
	  for(int is=0;is<MAX_THRESHOLDS_CROSSING;++is)
	    {
	      name=Form("waveAlignDifference_%d_%d_thr_%d_timeCrossDifference_%d",interestingChannels[i],interestingChannels[j],it,is);
	      waveAlignDifference_timeCrossDiffference[i][j][it][is]=new TH1F(name,name,4000,-1,1);
	      writableObjects[name]=(TObject*) (waveAlignDifference_timeCrossDiffference[i][j][it][is]);
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


  TStopwatch watch;



  watch.Start(true);

  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    
    //     //clear output tree data
    //    treeData.clear();
    Waveform waveforms[interestingChannels.size()];
//     for (int i=0;i<interestingChannels.size();++i)
//       waveforms[i].clear();

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
 	if ( chInterest[digiChannel[i]]>-1 &&  chInterest[digiChannel[i]]<interestingChannels.size() )
 	  waveforms[chInterest[digiChannel[i]]].addTimeAndSample(digiSampleIndex[i]*0.2e-9,digiSampleValue[i]);
	
       } //end loop over digis



     for ( int i=0; i<interestingChannels.size();++i)
       {
	 waveforms[i].interpolate();
	 
	 for (int j=0;j<waveforms[i]._samples.size();++j)
	   {
	     wave_allSamples[i]->Fill(waveforms[i]._samples[j]);
	     wave_profile[i]->Fill(j,waveforms[i]._samples[j]);
	     wave_sample[i][j]->Fill(waveforms[i]._samples[j]);
	     if (j>4 && j<waveforms[i]._samples.size()-4)
	       {
		 float interpolatedValue=waveforms[i].interpolatedValue(j,7); //linear interpolatedValue from [-4,+4] samples
		 wave_deriv_profile[i]->Fill(j,waveforms[i]._interpolator->Deriv(waveforms[i]._times[j])/1.e9);
		 wave_residual[i]->Fill(j,waveforms[i]._samples[j]-interpolatedValue);
		 wave_sample_residual[i][j]->Fill(waveforms[i]._samples[j]-interpolatedValue);
	       }
	   }
	 
	 std::vector< std::vector<float> > crossings;
	 crossings.resize(thresholds.size());
	 
	 for (int it=0;it<thresholds.size();++it)
	   {
	     crossings[it]=waveforms[i].time_at_threshold(0,1000,thresholds[it],NSAMPLES_INTERPOLATION);
	     
	     for(int is=0;is<std::min(MAX_THRESHOLDS_CROSSING, (int) crossings[it].size());++is)
	       wave_timeCross[i][it][is]->Fill((crossings[it][is])*1.e9);
	     
	     for(int is=0;is< std::min(MAX_THRESHOLDS_CROSSING, (int) crossings[it].size())-1;++is)
	       for(int is1=is+1;is1< std::min(MAX_THRESHOLDS_CROSSING, (int) crossings[it].size());++is1)
		 {
		   wave_timeCross_difference[i][it][is][is1]->Fill((crossings[it][is1]-crossings[it][is])*1.e9);
		 }
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
		   
		   waveDifference_profile[i][j]->Fill(is,difference._samples[is]);
					     
		   if (is<990) //last samples from DRS4 are unusable...
		     waveDifference_allSamples[i][j]->Fill(difference._samples[is]);
		   
		   waveDifference_sample[i][j][is]->Fill(difference._samples[is]);
		   
		 }
	     }
       }

  } //end loop over entries
      
     
  std::cout << "------ TIME ELAPSED IN ANALYSIS LOOP ----- " << watch.RealTime() << " ------- PER EVENT: " << (float) watch.RealTime() / (float) nentries << " ------ " << std::endl;


  // now let's fit the difference use the first object as reference
  double alignPar_deltaV[interestingChannels.size()];
  double alignPar_deltaT[interestingChannels.size()];

  alignPar_deltaT[0]=0.;
  alignPar_deltaV[0]=0.;

  for ( int i=1; i<interestingChannels.size();++i)
    {
      ROOT::Math::Minimizer* minim;
      WaveformFit::alignWaveform(((TProfile*) writableObjects[Form("wave_%d_profile",interestingChannels[0])]),((TProfile*) writableObjects[Form("wave_%d_profile",interestingChannels[i])]),minim);
      const double *par=minim->X();
      alignPar_deltaV[i]=par[0];
      alignPar_deltaT[i]=par[1];
    }
  
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    
    //     //clear output tree data
    treeData.clear();

    Waveform waveforms[interestingChannels.size()];

    //     for (int i=0;i<interestingChannels.size();++i)
    //       waveforms[i].clear();    
    
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

    //     Filling evtInfo
    treeData.evtNumber=evtNumber;
    treeData.nInterestingChannels=interestingChannels.size();
    for (int i=0;i<interestingChannels.size();++i)
      treeData.interestingChannels[i]=interestingChannels[i];
    treeData.nThresholds=thresholds.size();
    for (int i=0;i<thresholds.size();++i)
      treeData.thresholds[i]=thresholds[i];
    
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
	 if ( chInterest[digiChannel[i]]>-1 &&  chInterest[digiChannel[i]]<interestingChannels.size() )
	   waveforms[chInterest[digiChannel[i]]].addTimeAndSample( ( digiSampleIndex[i] - alignPar_deltaT[chInterest[digiChannel[i]]]) * 0.2e-9, digiSampleValue[i]+alignPar_deltaV[chInterest[digiChannel[i]]]);
       }
    //end loop over digis
    std::vector< std::vector<float> > crossings_ref;
    crossings_ref.resize(thresholds.size());

    for (int it=0;it<thresholds.size();++it)
      {
	crossings_ref[it]=waveforms[0].time_at_threshold(0,1000,thresholds[it],NSAMPLES_INTERPOLATION);

	treeData.nCrossings[it]=crossings_ref[it].size();
	for(int i=0;i<crossings_ref[it].size();++i)
	  treeData.crossings[it][0][i]=crossings_ref[it][i] * 1.e9;
      }

    for (int j=1;j<interestingChannels.size();++j)
      {
	waveforms[j].interpolate();

	std::vector< std::vector<float> > crossings;
	crossings.resize(thresholds.size());
	for (int it=0;it<thresholds.size();++it)
	  crossings[it]=waveforms[j].time_at_threshold(0,1000,thresholds[it],NSAMPLES_INTERPOLATION);


	Waveform difference;
	assert(waveforms[0]._samples.size() == waveforms[j]._samples.size());
	for (int is=0;is<waveforms[0]._samples.size();++is)
	  {
	    difference.addTimeAndSample( waveforms[0]._times[is], ( waveforms[0]._samples[is] - waveforms[j]._interpolator->Eval(waveforms[0]._times[is]) ) );
	    if (is > 10 && is <990)
	      {
		waveAlignDifference_profile[0][j]->Fill(is,difference._samples[is]);
		waveAlignDifference_allSamples[0][j]->Fill(difference._samples[is]);
		waveAlignDifference_sample[0][j][is]->Fill(difference._samples[is]);
	      }
	    
 	  }

	
	for (int it=0;it<thresholds.size();++it)
	  for (int is=0;is<std::min(crossings[it].size(),crossings_ref[it].size());++is)
	    {
	      waveAlignDifference_timeCrossDiffference[0][j][it][is]->Fill( (crossings_ref[it][is]- crossings[it][is]) * 1.e9 );
	      treeData.crossings[it][j][is]=crossings[it][is] * 1.e9;
	    }
      }
  
    tree->Fill();
     
  }

    

  tree->Write();
//   //Writing out all objects
  for (std::map<TString,TObject*>::iterator it=writableObjects.begin();it!=writableObjects.end();++it)
    {
      if (((TH1F*) it->second)->GetEntries()>0)
	{
	  std::cout << "Writing " << it->first << std::endl;
	  it->second->Write();
	}
    }
  out->Close();
  std::cout << "===> " << outFile << " closed" << std::endl;
} //end of function

