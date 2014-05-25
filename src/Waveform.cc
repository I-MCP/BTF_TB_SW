#include "Waveform.h"

#include "TGraph.h"
#include "TF1.h"
#include "TMath.h"

ClassImp(Waveform);

Waveform::max_amplitude_informations Waveform::max_amplitude(const int& x1, const int& x2, int nSamplesAroundMax) const
{
  max_amplitude_informations return_value;  

  if (x1<0 || x2>(_samples.size()-1))
    {
      std::cout << "WARNING::Waveform::max_amplitude::gate is outside samples range" << std::endl;
      return return_value;
    }

  int imax=-1;
  int max=-999999.;

  for (unsigned int i(x1);i<=x2;++i)
    {
      if (_samples[i]>=max)
	{
	  max=_samples[i];
	  imax=i;
	}
    }


  if (imax>-1)
    {
      float x[nSamplesAroundMax];
      float y[nSamplesAroundMax];
      int nSamples=0;
      for (unsigned int i(0);i<nSamplesAroundMax;++i)
	{
	  if ( (imax-(nSamplesAroundMax-1)/2+i)>=x1 && (imax-(nSamplesAroundMax-1)/2+i)<=x2)
	    {
	      x[i]=imax-(nSamplesAroundMax-1)/2+i;
	      y[i]=_samples[imax-(nSamplesAroundMax-1)/2+i];
	      ++nSamples;
	    }
	  else
	    {
	      std::cout << "WARNING::Waveform::max_amplitude::maximum found too close to gate edges. Increase gate width" << std::endl;
	    }
	}

      if (nSamples>3)
	{
	  //Now fit with parabolic function around maximum value
	  TGraph* graph=new TGraph(nSamples,x,y);
	  graph->Fit("pol2","Q0+");

	  //FIXME Add a check on the FIT status
	  double *par=graph->GetFunction("pol2")->GetParameters();
	  return_value.max_amplitude=par[0]-(par[1]*par[1]/(4*par[2]));
	  return_value.time_at_max=-par[1]/(2*par[2]);

	  delete graph;
	}
      else
	{
	  std::cout << "WARNING::Waveform::max_amplitude::not enough samples to fit fot maximum. Returning unfitted position" << std::endl;
	  return_value.max_amplitude=max;
	  return_value.time_at_max=imax;
	}

    }
  return return_value;
};

//Get the time at a given fraction of the amplitude for times between x1 and x2 
float Waveform::time_at_frac(const int& x1, const int& x2, const float& frac, const max_amplitude_informations& maxInfos, int SampleToInterpolate) const
{
};

//Get the baseline (pedestal and RMS) informations computed between x1 and x2
Waveform::baseline_informations Waveform::baseline(const int& x1, const int& x2) const
{

  baseline_informations return_value;

  if (x1<0 || x2>(_samples.size()-1))
    {
      std::cout << "WARNING::Waveform::baseline::gate is outside samples range" << std::endl;
      return return_value;
    }
  
  if ((x2-x1)<2)
    {
      std::cout << "WARNING::Waveform::baseline::you need >2 samples to get pedestal & rms" << std::endl;
      return return_value;
    }

  double mean=0;
  double rms=0;
  
  for (unsigned int i(x1);i<=x2;++i)
    {
      mean+=_samples[i];
      rms+=_samples[i]*_samples[i];
    }
  
  mean=mean/(double)(x2-x1+1);
  rms=TMath::Sqrt((x2-x1+1)/(x2-x1))*TMath::Sqrt( rms/(x2-x1+1) - mean*mean );
  return_value.pedestal = mean;
  return_value.rms = rms;

  return return_value;

};
