#include "WaveformUtils.h"
#include "Math/Interpolator.h"
#include "TMath.h"

#include "assert.h"

namespace WaveformUtils
{

  Waveform generateWaveform(TProfile* ref_profile, float sampleSize,int nSamples)
  {
    ROOT::Math::Interpolator profile_interpolator(ref_profile->GetNbinsX(),ROOT::Math::Interpolation::kCSPLINE);
    std::vector<double> x,y;
    for (int i=1;i<=ref_profile->GetNbinsX();++i)
      {
	x.push_back(ref_profile->GetBinCenter(i)*sampleSize);
	y.push_back(ref_profile->GetBinContent(i));
      }
    profile_interpolator.SetData(x,y);
    
    Waveform return_data;
    for (int i=0;i<nSamples;++i)
      {
	//Here we can add: noise (un-correlated + correlated), amplitude mis-calibration, time mis-calibration
	return_data.addTimeAndSample(i*sampleSize,profile_interpolator.Eval(i*sampleSize));
      }
    return return_data;

  }

} //end workspace

