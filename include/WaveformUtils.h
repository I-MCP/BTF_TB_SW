#ifndef WAVEFORM_UTILS_H
#define WAVEFORM_UTILS_H

#include "Waveform.h"
#include "TProfile.h"


namespace WaveformUtils
{
  Waveform generateWaveform(TProfile* ref_profile,float time_unit,float sampleSize,int nSamples, float ampl,float pedestal, float rms_amplitude_noise, float time_jitter);
};

#endif
