#ifndef WaveForm_h
#define WaveForm_h

#include "TROOT.h"
 
#include <vector> 
#include <algorithm> 

class Waveform
{
 public:
  std::vector<float> _samples;
  std::vector<float> _times;
  
  struct max_amplitude_informations
  {
    float max_amplitude;
    float time_at_max;
    int sample_at_max;
  };

  
  struct baseline_informations
  {
    float pedestal;
    float rms;
  };

  Waveform()
    {
    };

  //constructor from float array
  Waveform(int nSamples, float* times, float* samples)
    {
      _samples.resize(nSamples);
      _times.resize(nSamples);
      for (unsigned int i(0);i<nSamples;++i)
	{
	  _samples[i]=samples[i];
	  _times[i]=times[i];
	}
    };

  //constructor from std::vector<float>
  Waveform(const std::vector<float>& times, const std::vector<float>& samples)
    {
      _samples.resize(samples.size());
      _times.resize(samples.size());
      std::copy(samples.begin(), samples.end(), _samples.begin());
      std::copy(times.begin(), times.end(), _times.begin());
    };

  ~Waveform()
    {
    };

  //add samples
  void addSample(const float& sample, float sampleTimeSize=0.2e-9)
  {
    _samples.push_back(sample);
    if (_times.size()>0)
      _times.push_back(_times.back()+sampleTimeSize);
    else
      _times.push_back(0.);
  }
  
  void addTimeAndSample(const float& time, const float& sample)
  {
    _samples.push_back(sample);
    _times.push_back(time);
  }

  //open a window of nSamples centered keeping into account where is the maximum and some characteristics of the shape
  void find_interesting_samples(int nSamples, const max_amplitude_informations& maxInfos, float riseTime, float fallTime, int& x1,int &x2);

  //Get the max amplitude between x1 and x2 using nSamplesAroundMax samples and a parabolic fit
  max_amplitude_informations max_amplitude(const int& x1, const int& x2, int nSamplesAroundMax=5) const;

  //Get the time at a given fraction of the amplitude for times between x1 and x2 
  float time_at_frac(const int& i1, const int& i2, const float& frac, const max_amplitude_informations& maxInfos, int SampleToInterpolate=5) const;

  //Get the time at a given fraction of the amplitude for times between x1 and x2 
  float time_at_frac(const float& t1, const float& t2, const float& frac, const max_amplitude_informations& maxInfos, int SampleToInterpolate=5) const;

  //Get the baseline (pedestal and RMS) informations computed between x1 and x2
  baseline_informations baseline(const int& x1, const int& x2) const;

  float integral(const int& x1, const int& x2) const
  {
    float integral=0;
    for(int bin=x1; bin<x2; bin++)
      integral += _samples[bin];
    return integral;
  };

  //clear
  void clear()
  {
    _samples.clear();
  };

  //substract a fixed value from the samples
  void offset(const float& value)
  {
    for (unsigned int i(0);i<_samples.size();++i)
      _samples[i]-=value;
  };

  //rescale all the samples by a given rescale factor
  void rescale(const float& rescale_factor)
  {
    for (unsigned int i(0);i<_samples.size();++i)
      _samples[i]*=rescale_factor;
  };

  ClassDef(Waveform,1);

};

#endif
