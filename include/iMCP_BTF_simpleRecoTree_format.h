#ifndef iMCP_BTF_simpleRecoTree_format_h
#define iMCP_BTF_simpleRecoTree_format_h

#define MAX_HODO_CLUSTER  8

#define MAX_TDC_MEASUREMENTS  10

#define DIGI_SAMPLES_TO_STORE 200

#define BASELINE_DIGI_SAMPLES_TO_STORE 50

#define SCINT_ADC_CHANNELS 2
#define SCINT_DIGI_CHANNELS 2

#define MCP_TDC_CHANNELS 4
#define MCP_DIGI_CHANNELS 5

#include <TROOT.h>

namespace iMCP_BTF_simpleRecoTree_namespace
{
  struct eventInfo
  {
    unsigned int evtNumber;
    unsigned int evtTimestamp;
    unsigned int evtTimedist;
    unsigned int evtTimeSinceStart;
  };

  struct tdc_data
  {
    int tdc_nCounts;
    float tdc_measurements[MAX_TDC_MEASUREMENTS];

    void clear()
    {
      for (unsigned int i(0);i<MAX_TDC_MEASUREMENTS;++i)
	tdc_measurements[i]=-1;
      tdc_nCounts=0;
    };

  };

  struct waveform_data
  {
    float pedestal; //from pre-samples;
    float pedestal_rms; //from pre-samples;
    float max_amplitude; //max_value;
    float fit_max_amplitude; //max_value from fit
    float fit_max_amplitude_err; //max_value from fit
    float fit_time; //time as fitted
    float fit_time_err; //time as fitted
    float fit_compatibility; //time as fitted
    float fit_edm; //time as fitted
    float fit_ndof; //time as fitted
    float time_at_max; //emualated constant fraction data
    float time_at_frac30; //emualated constant fraction data
    float time_at_frac50; //emualated constant fraction data
    float samples[DIGI_SAMPLES_TO_STORE]; //most relevant part of the waveform to be stored for later analysis
    float sampleTime[DIGI_SAMPLES_TO_STORE]; //most relevant part of the waveform to be stored for later analysis
    float baseline_samples[BASELINE_DIGI_SAMPLES_TO_STORE]; //most relevant part of the waveform to be stored for later analysis
    float baseline_sampleTime[BASELINE_DIGI_SAMPLES_TO_STORE]; //most relevant part of the waveform to be stored for later analysis

    void clear()
    {
      pedestal=-1;
      pedestal_rms=-1;
      max_amplitude=-1;
      fit_max_amplitude=-1;
      fit_time=-1;
      fit_compatibility=-1;
      fit_edm=-1;
      fit_ndof=-1;
      time_at_max=-1;
      time_at_frac30=-1;
      time_at_frac50=-1;
      for (unsigned int i(0);i<DIGI_SAMPLES_TO_STORE;++i)
	{
	  samples[i]=-999;
	  sampleTime[i]=-1;
	}
      for (unsigned int i(0);i<BASELINE_DIGI_SAMPLES_TO_STORE;++i)
	{
	  baseline_samples[i]=-999;
	  baseline_sampleTime[i]=-1;
	}
    };
  };

  struct hodoscope_plane_data
  {
    int nHodoPlaneClusters; //number of clusters (particles) crossing x hodoscope
    float hodoPlaneClusterPosition[MAX_HODO_CLUSTER];
    float hodoPlaneClusterRawAdcCount[MAX_HODO_CLUSTER]; 
    void clear()
    {
      nHodoPlaneClusters=0;
      for (unsigned int i(0);i<MAX_HODO_CLUSTER;++i)
	{
	  hodoPlaneClusterPosition[i]=-1;
	  hodoPlaneClusterRawAdcCount[i]=-1;
	}
    };
  };

  struct hodoscope_data
  {
    hodoscope_plane_data hodoXClusters;
    hodoscope_plane_data hodoYClusters;

    void clear()
    {
      hodoXClusters.clear();
      hodoYClusters.clear();
    };

  };

  struct scint_data
  {
    float scint_adc_data[SCINT_ADC_CHANNELS]; //scint front and back adc count (pedestal substracted)
    waveform_data scint_digi_data[SCINT_DIGI_CHANNELS]; //scint waveforms

    void clear()
    {
      for (unsigned int i(0);i<SCINT_ADC_CHANNELS;++i)
	scint_adc_data[i]=-999;
      for (unsigned int i(0);i<SCINT_DIGI_CHANNELS;++i)
	scint_digi_data[i].clear();
    };
  };

  struct mcp_data
  {
    tdc_data mcp_tdc_data[MCP_TDC_CHANNELS]; 
    waveform_data mcp_digi_data[MCP_DIGI_CHANNELS]; //mcp waveforms

    void clear()
    {
      for (unsigned int i(0);i<MCP_TDC_CHANNELS;++i)
	mcp_tdc_data[i].clear();
      for (unsigned int i(0);i<MCP_DIGI_CHANNELS;++i)
	mcp_digi_data[i].clear();
    };
  };

  struct iMCP_BTF_simpleRecoTree_format
  {
    eventInfo _evtInfo;

    //Scintillator front[0] & back[1] data
    scint_data _scintData;

    //hodoscope reconstructed positions in X&Y planes
    hodoscope_data _hodoData;

    //MCP data
    mcp_data _mcpData;

    void clear()
    {
      _mcpData.clear();
      _hodoData.clear();
      _scintData.clear();
    };

    ClassDef(iMCP_BTF_simpleRecoTree_format,1);
  };
}
#endif
