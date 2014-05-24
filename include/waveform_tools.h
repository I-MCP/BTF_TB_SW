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
  
max_amplitude_informations max_amplitude(const int& x1, const int& x2, const std::vector<float>& samples, int nSamplesAroundMax=5);

float time_at_frac(const int& x1, const int& x2, const, const std::vector<float>& samples, const max_amplitude_informations& maxInfos, int SampleToInterpolate=5);

baseline_informations baseline(const int& x1, const int& x2, const std::vector<float>& samples);
