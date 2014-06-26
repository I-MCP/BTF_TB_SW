#ifndef iMCP_PILAS_simpleRecoTree_format_h
#define iMCP_PILAS_simpleRecoTree_format_h

#define DIGI_SAMPLES_TO_STORE 200

#define WAVES_DIGI_CHANNELS 3

#include <TROOT.h>

namespace iMCP_PILAS_simpleRecoTree_namespace
{
  struct eventInfo
  {
    unsigned int evtNumber;
    unsigned int evtTimestamp;
    unsigned int evtTimedist;
    unsigned int evtTimeSinceStart;
  };


  struct waveform_data
  {
    float pedestal; //from pre-samples;
    float pedestal_rms; //from pre-samples;
    float max_amplitude; //max_value;
    float time_at_max; //emualated constant fraction data
    float time_at_frac30; //emualated constant fraction data
    float time_at_frac50; //emualated constant fraction data
    float samples[DIGI_SAMPLES_TO_STORE]; //most relevant part of the waveform to be stored for later analysis
  float sampleTime[DIGI_SAMPLES_TO_STORE]; //most relevant part of the waveform to be stored for later analysis

  void clear()
  {
    pedestal=-1;
    pedestal_rms=-1;
    max_amplitude=-1;
    time_at_max=-1;
    time_at_frac30=-1;
    time_at_frac50=-1;
    for (unsigned int i(0);i<DIGI_SAMPLES_TO_STORE;++i)
      {
	samples[i]=-999;
	sampleTime[i]=-1;
      }
  };
};

struct channels_data
{
  std::vector<waveform_data> channels_digi_data;

  void clear()
  {
    for (unsigned int i(0);i<channels_digi_data.size();++i)
      channels_digi_data[i].clear();
  };
};

struct iMCP_PILAS_simpleRecoTree_format
{
  eventInfo _evtInfo;

  //MCP data
  channels_data _channelsData;

  void clear()
  {
    _channelsData.clear();
  };

  ClassDef(iMCP_PILAS_simpleRecoTree_format,1);
};
}
#endif
