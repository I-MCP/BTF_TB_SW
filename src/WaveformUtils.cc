#include "WaveformUtils.h"
#include "Math/Interpolator.h"
#include "TMath.h"
#include "TRandom3.h"

#include "assert.h"


namespace WaveformUtils
{
  TRandom3 rGen(0);

  Waveform generateWaveform(TProfile* ref_profile, float histoTimeUnit, float sampleSize, int nSamples, float amplitude, float pedestal, float rms_amplitude_noise,float time_jitter)
  {
    ROOT::Math::Interpolator profile_interpolator(ref_profile->GetNbinsX(),ROOT::Math::Interpolation::kCSPLINE);
    std::vector<double> x,y;
    for (int i=1;i<=ref_profile->GetNbinsX();++i)
      {
	x.push_back((i-1)*ref_profile->GetBinWidth(i)*histoTimeUnit);
	y.push_back(ref_profile->GetBinContent(i));
      }
    profile_interpolator.SetData(x,y);
    
    Waveform return_data;
    float jitter=WaveformUtils::rGen.Gaus(0,1)*time_jitter;
    for (int i=0;i<nSamples;++i)
      {
	//Here we can add: noise (un-correlated + correlated), amplitude mis-calibration, time mis-calibration
	return_data.addTimeAndSample(i*sampleSize,int(profile_interpolator.Eval(i*sampleSize+jitter)*amplitude+WaveformUtils::rGen.Gaus(pedestal,rms_amplitude_noise)));
      }
    return return_data;

  }

} //end workspace

