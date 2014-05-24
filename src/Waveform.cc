#include "Waveform.h"

ClassImp(Waveform);

Waveform::max_amplitude_informations Waveform::max_amplitude(const int& x1, const int& x2, int nSamplesAroundMax) const
{
};

//Get the time at a given fraction of the amplitude for times between x1 and x2 
float Waveform::time_at_frac(const int& x1, const int& x2, const float& frac, const max_amplitude_informations& maxInfos, int SampleToInterpolate) const
{
};

//Get the baseline (pedestal and RMS) informations computed between x1 and x2
Waveform::baseline_informations Waveform::baseline(const int& x1, const int& x2) const
{
};
