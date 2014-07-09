#ifndef WAVEFORM_FIT_H
#define WAVEFORM_FIT_H

#include "TProfile.h"
#include "Math/Minimizer.h"

namespace WaveformFit
{
  void alignWaveform(TProfile* ref_profile, TProfile* fit_profile, ROOT::Math::Minimizer* &minimizer);
}
#endif
