#ifndef WaveForm_h
#define WaveForm_h

#include <vector> 

class Waveform
{
 public:
  std::vector<float> _samples;
  
  struct max_amplitude_informations
  {
    float max_amplitude;
    float time_at_max;
  };
  
  struct baseline_informations
  {
    float pedestal;
    float rms;
  };
  
  explicit Waveform(int nSamples, float* samples)
    {
      _samples.reserve(nSamples);
      for (unsigned int i(0);i<nSamples;++i)
	_samples[i]=samples[i];
    };

  ~Waveform()
    {
    };
  
  //Get the max amplitude between x1 and x2 using nSamplesAroundMax samples and a parabolic fit
  max_amplitude_informations max_amplitude(const int& x1, const int& x2, int nSamplesAroundMax=5) const;

  //Get the time at a given fraction of the amplitude for times between x1 and x2 
  float time_at_frac(const int& x1, const int& x2, const float& frac, const max_amplitude_informations& maxInfos, int SampleToInterpolate=5) const;

  //Get the baseline (pedestal and RMS) informations computed between x1 and x2
  baseline_informations baseline(const int& x1, const int& x2) const;

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


};

#endif
