#ifndef WAVEFORM_UTILS_H
#define WAVEFORM_UTILS_H

#include "Waveform.h"
#include "TProfile.h"


namespace WaveformUtils
{
  Waveform generateWaveform(TProfile* ref_profile,float sampleSize,int nSamples, float rms_amplitude_noise);
};

#endif
