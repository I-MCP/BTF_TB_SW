#ifndef iMCP_BTF_checkDRS4Calibration_h
#define iMCP_BTF_checkDRS4Calibration_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>
#include <TObject.h>

#include <string>
#include <map>
#include <vector>
#include <assert.h>

#include "iMCP_BTF_rawDataTree.h"
/* #include "iMCP_BTF_checkDRS4Calibration_format.h" */

//using namespace iMCP_BTF_checkDRS4Calibration_namespace;

#define MAX_SAMPLES_SIZE 1024
#define MAX_THRESHOLDS 10
#define MAX_THRESHOLDS_CROSSING 10
#define MAX_CHANNELS 10
#define NSAMPLES_INTERPOLATION 9

/* #define THR_0_VALUE 2200 */
/* #define THR_1_VALUE 2700 */

//This class is an analyzer of the rawDataTree and produces a new tree with reconstruced and calibrated data
class iMCP_BTF_checkDRS4Calibration : public iMCP_BTF_rawDataTree 
{
 public :

  struct eventData
  {
    unsigned int evtNumber;
    unsigned int nInterestingChannels;
    unsigned int interestingChannels[MAX_CHANNELS];
    unsigned int nCrossings[MAX_THRESHOLDS];
    float crossings[MAX_THRESHOLDS][MAX_CHANNELS][MAX_THRESHOLDS_CROSSING];
    
    void clear()
    {
      for (int it=0;it<MAX_THRESHOLDS;++it)
	for (int i=0;i<MAX_CHANNELS;++i)
	  for (int j=0;j<MAX_THRESHOLDS_CROSSING;++j)
	      crossings[it][i][j]=-999.;
    }
  };

  TString outFile;
  TTree* tree;

  eventData treeData;

  int maxEntries=-1;

  std::vector<int> interestingChannels;
  std::vector<float> thresholds;
  std::map<TString,TObject*> writableObjects;

  /*   iMCP_BTF_checkDRS4Calibration_format treeData; */

  iMCP_BTF_checkDRS4Calibration(TTree *tree=0);
  virtual ~iMCP_BTF_checkDRS4Calibration();

  //main analysis class
  virtual void  Loop();

  //book tree branches
  /*   void bookEventInfo(); */
  void bookOutputTree(); 
  /*   void bookScintData(); */
  /*   void bookMcpData(); */
  /*   void bookHodoData(); */
  /*   void bookWaveform(TString name, waveform_data& waveform); */
  /*   void bookTdcData(TString name, tdc_data& tdc); */
  /*   void bookHodoscopePlane(TString name, hodoscope_plane_data& plane_data); */

  void cannotOpenFile(const char * file);

  ClassDef(iMCP_BTF_checkDRS4Calibration,1);
};

#endif
