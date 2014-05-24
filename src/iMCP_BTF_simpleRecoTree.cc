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
}

void iMCP_BTF_simpleRecoTree::Loop()
{
  if (fChain == 0) return;
  
  Long64_t nentries = fChain->GetEntriesFast();
  
  Long64_t nbytes = 0, nb = 0;
  
  unsigned int startTimeStamp=0;
  
  for (Long64_t jentry=0; jentry<nentries;jentry++) {

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
	/*	
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
	*/
      } //end loop over digis
  } //end loop over entries

} //end of function

