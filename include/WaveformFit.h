#ifndef WAVEFORM_FIT_H
#define WAVEFORM_FIT_H

#include "TProfile.h"
#include "Waveform.h"
#include "Math/Minimizer.h"

namespace WaveformFit
{
  void alignWaveform(TProfile* ref_profile, TProfile* fit_profile, ROOT::Math::Minimizer* &minimizer);
  void fitWaveform(Waveform* wave, TProfile* amplitudeProfile, int samplesAfterMax, Waveform::max_amplitude_informations max, ROOT::Math::Minimizer* &minimizer);
}
#endif
