#define MAX_HODO_CLUSTER  8

#define MAX_TDC_MEASUREMENTS  10

#define DIGI_SAMPLES_TO_STORE 200

#define SCINT_ADC_CHANNELS 2
#define SCINT_DIGI_CHANNELS 2

#define MCP_TDC_CHANNELS 4
#define MCP_DIGI_CHANNELS 5

struct tdc_data
{
  int tdc_nCounts;
  float tdc_measurements[MAX_TDC_MEASUREMENTS];
};

struct waveform_data
{
  float pedestal; //from pre-samples;
  float max_amplitued; //max_value;
  float time_at_frac; //emualated constant fraction data
  float samples[DIGI_SAMPLES_TO_STORE]; //most relevant part of the waveform to be stored for later analysis
};

struct hodoscope_plane_data
{
  int nHodoPlaneClusters; //number of clusters (particles) crossing x hodoscope
  float hodoPlaneClusterPosition[MAX_HODO_CLUSTER];
  float hodoPlaneClusterRawAdcCount[MAX_HODO_CLUSTER]; 

};

struct hodoscope_data
{
  hodoscope_plane_data hodoXClusters;
  hodoscope_plane_data hodoYClusters;
};

struct iMCP_BTF_simpleRecoTree_format
{
  float scint_adc_data[SCINT_ADC_CHANNELS]; //scint front and back adc count (pedestal substracted)
  waveform_data scint_digi_data[SCINT_DIGI_CHANNELS]; //scint waveforms

  hodoscope_data hodoClusters;

  tdc_data mcp_tdc_data[MCP_TDC_CHANNELS]; 
  waveform_data mcp_digi_data[MCP_DIGI_CHANNELS]; //mcp waveforms
};
