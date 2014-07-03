#include "Waveform.h"

#include "TGraph.h"
#include "TF1.h"
#include "TMath.h"

#define WARNING_ERROR 0
ClassImp(Waveform);

using namespace std;

Waveform::max_amplitude_informations Waveform::max_amplitude(const int& x1, const int& x2, int nSamplesAroundMax) const
{
  max_amplitude_informations return_value;  

  if (x1<0 || x2>(_samples.size()-1))
    {
      if (WARNING_ERROR)
	std::cout << "WARNING::Waveform::max_amplitude::gate is outside samples range" << std::endl;
      return return_value;
    }

  int imax=-1;
  float max=-999999.;

  for (unsigned int i(x1);i<=x2;++i)
    {
      //      std::cout << "++++ " <<  i << "," << _times[i] << "," << _samples[i] << "," << max << "," << imax << std::endl;
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
	      //	      x[i]=imax-(nSamplesAroundMax-1)/2+i;
	      x[i]=_times[imax-(nSamplesAroundMax-1)/2+i]*1e9;
	      y[i]=_samples[imax-(nSamplesAroundMax-1)/2+i];
	      //	      std::cout <<  imax << "," << imax-(nSamplesAroundMax-1)/2+i << "," << x[i] << "," << y[i] << std::endl;
      
	      ++nSamples;
	    }
	  else
	    {
	      if (WARNING_ERROR)
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
	  return_value.time_at_max=-(par[1]/(2*par[2]))/1.e9;
	  return_value.sample_at_max=imax;

	  delete graph;
	}
      else
	{
	  if (WARNING_ERROR)
	    std::cout << "WARNING::Waveform::max_amplitude::not enough samples to fit fot maximum. Returning unfitted position" << std::endl;
	  return_value.max_amplitude=max;
	  return_value.time_at_max=_times[imax];
	  return_value.sample_at_max=imax;
	}

    }
  return return_value;
};

//Get the time at a given fraction of the amplitude for times between x1 and x2 
float Waveform::time_at_frac(const float& t1, const float& t2, const float& frac, const max_amplitude_informations& maxInfos, int SampleToInterpolate) const
{
  //std::cout << "_____ " << t1 << "," << t2 << std::endl;
  int tmin=0;
  int tmax=0;
  for (unsigned int i(0);i<_times.size();++i)
    {
      //      std::cout << i << "," << _times[i] << std::endl;
      if (_times[i]<t1)
	tmin=i;
      if (_times[i]<t2)
	tmax=i;
    }

  return time_at_frac(tmin,tmax,frac,maxInfos,SampleToInterpolate);
}


//Get the time at a given fraction of the amplitude for times between x1 and x2 
float Waveform::time_at_frac(const int& x1, const int& x2, const float& frac, const max_amplitude_informations& maxInfos, int SampleToInterpolate) const
{
  int cfSample=maxInfos.sample_at_max;

  //  std::cout << "===== " << x1 << "," << x2 << std::endl;

  for(int iSample=(int)maxInfos.sample_at_max; iSample>max(x1,0); iSample--)
    {
      if(_samples[iSample] < maxInfos.max_amplitude*frac) 
	{
	  cfSample = iSample;
 	  break;
	}
    }
  
  //  std::cout <<  "++++ "  << cfSample << std::endl;
  if (cfSample>-1)
    {
      double x[SampleToInterpolate];
      double y[SampleToInterpolate];
      int nSamples=0;
      for (unsigned int i(0);i<SampleToInterpolate;++i)
	{
	  if ( (cfSample-(SampleToInterpolate-1)/2+i)>=max(x1,0) && (cfSample-(SampleToInterpolate-1)/2+i)<=min(x2,(int)_samples.size()))
	    {
	      //	      x[i]=cfSample-(SampleToInterpolate-1)/2+i;
	      x[i]=_times[cfSample-(SampleToInterpolate-1)/2+i]*1e9;
	      y[i]=_samples[cfSample-(SampleToInterpolate-1)/2+i]-maxInfos.max_amplitude*frac;
	      //std::cout <<  cfSample << "," << cfSample-(SampleToInterpolate-1)/2+i << "," << x[i] << "," << y[i] << std::endl;
      
	      ++nSamples;
	    }
	  else
	    {
	      if (WARNING_ERROR)
		std::cout << "WARNING::Waveform::max_amplitude::maximum found too close to gate edges. Increase gate width" << std::endl;
	    }
	}

      if (nSamples>3)
	{
	  //Now fit with parabolic function around maximum value
	  TGraph* graph=new TGraph(nSamples,x,y);
	  graph->Fit("pol1","Q0+");

	  //FIXME Add a check on the FIT status
	  double *par=graph->GetFunction("pol1")->GetParameters();

	  return -(par[0]/par[1])/1.e9;
	  delete graph;
	}
      else
	{
	  if (WARNING_ERROR)
	    std::cout << "WARNING::Waveform::max_amplitude::not enough samples to fit fot maximum. Returning unfitted position" << std::endl;
	  return _times[cfSample];
	}

    }
   
//   float xx= 0.;
//   float xy= 0.;
//   float Sx = 0.;
//   float Sy = 0.;
//   float Sxx = 0.;
//   float Sxy = 0.;
//   float Chi2 = 0.;
//   int minSample=x1;
//   int cfSample=x1; // first sample over AmpMax*CF 
//   float minValue=0;
  


//   for(int n=-(SampleToInterpolate-1)/2; n<=(SampleToInterpolate-1)/2; n++)
//     {
//       if(cfSample+n<0) continue;
//       double x=_times[cfSample+n]*1.e9;
//       xx = (x)*(x);
//       xy = (x)*(_samples[cfSample+n]);
//       Sx = Sx + (x);
//       Sy = Sy + _samples[cfSample+n];
//       Sxx = Sxx + xx;
//       Sxy = Sxy + xy;
//     }

//   float Delta = SampleToInterpolate*Sxx - Sx*Sx;
//   float A = (Sxx*Sy - Sx*Sxy) / Delta;
//   float B = (SampleToInterpolate*Sxy - Sx*Sy) / Delta;
  
//   float sigma2 = pow(1./sqrt(12)*B,2);
  
//   for(int n=-(SampleToInterpolate-1)/2; n<=(SampleToInterpolate-1)/2; n++)
//     {
//       if(cfSample+n<0) continue;
//       Chi2 = Chi2 + pow(_samples[cfSample+n] - A - B*((_times[cfSample+n]*1.e9)),2)/sigma2;
//     } 
//   // A+Bx = frac * amp
//   float interpolation = (_samples[minSample] * frac - A) / B;
//   return interpolation;				


};

//Get the crossing time of the amplitude at a given threshold for times between t1 and t2 
std::vector<float> Waveform::time_at_threshold(const float& t1, const float& t2, const float& threshold, int SampleToInterpolate) const 
{
  //std::cout << "_____ " << t1 << "," << t2 << std::endl;
  int tmin=0;
  int tmax=0;
  for (unsigned int i(0);i<_times.size();++i)
    {
      //      std::cout << i << "," << _times[i] << std::endl;
      if (_times[i]<t1)
	tmin=i;
      if (_times[i]<t2)
	tmax=i;
    }

  return time_at_threshold(tmin,tmax,threshold,SampleToInterpolate);
}

//Get the crossing time of the amplitude at a given threshold for times between t1 and t2 
std::vector<float> Waveform::time_at_threshold(const int& x1, const int& x2, const float& threshold, int SampleToInterpolate) const 
{
  int direction= 1 - 2*( _samples[max(x1,0)]<threshold ); //=-1 if first sample is below threshold, =1 if above
  
  std::vector<int> cfSample;
  
  for(int iSample=max(x1,0); iSample< min( (int) _samples.size() , x2); ++iSample)
    {
      if( (direction==-1 && (_samples[iSample] > threshold ) ) 
	  || (direction==1 && (_samples[iSample] < threshold ) ) ) 
	{
	  cfSample.push_back(iSample);
	  direction*=-1;
	}
    }

  std::vector<float> crossingTimes;

  for(int icross=0;icross<cfSample.size();++icross)
    {
      double x[SampleToInterpolate];
      double y[SampleToInterpolate];
      int nSamples=0;
      for (unsigned int i(0);i<SampleToInterpolate;++i)
	{
	  if ( (cfSample[icross]-(SampleToInterpolate-1)/2+i)>=max(x1,0) && (cfSample[icross]-(SampleToInterpolate-1)/2+i)<=min(x2,(int)_samples.size()))
	    {
	      //	      x[i]=cfSample[icross]-(SampleToInterpolate-1)/2+i;
	      x[i]=_times[cfSample[icross]-(SampleToInterpolate-1)/2+i]*1e9;
	      y[i]=_samples[cfSample[icross]-(SampleToInterpolate-1)/2+i]-threshold;
	      //std::cout <<  cfSample[icross] << "," << cfSample[icross]-(SampleToInterpolate-1)/2+i << "," << x[i] << "," << y[i] << std::endl;
      	      ++nSamples;
	    }
	  else
	    {
	      if (WARNING_ERROR)
		std::cout << "WARNING::Waveform::max_amplitude::maximum found too close to gate edges. Increase gate width" << std::endl;
	    }
	}

      if (nSamples>3)
	{
	  //Now fit with parabolic function around maximum value
	  TGraph* graph=new TGraph(nSamples,x,y);
	  graph->Fit("pol1","Q0+");

	  //FIXME Add a check on the FIT status
	  double *par=graph->GetFunction("pol1")->GetParameters();

	  crossingTimes.push_back( -(par[0]/par[1])/1.e9 );
	  delete graph;
	}
      else
	{
	  if (WARNING_ERROR)
	    std::cout << "WARNING::Waveform::max_amplitude::not enough samples to fit fot maximum. Returning unfitted position" << std::endl;
	  crossingTimes.push_back( _times[cfSample[icross]] );
	}
      
    }
  return crossingTimes;
}

void Waveform::find_interesting_samples(int nSamples, const max_amplitude_informations& maxInfos, float riseTime, float fallTime, int& x1,int &x2)
{
  //std::cout << "_____ " << t1 << "," << t2 << std::endl;
  int tmin=0;
  int tmax=0;
  for (unsigned int i(0);i<_times.size();++i)
    {
      //      std::cout << i << "," << _times[i] << std::endl;
      if (_times[i]<maxInfos.time_at_max-2.*riseTime)
	tmin=i;
      if (_times[i]<maxInfos.time_at_max+1.*fallTime)
	tmax=i;
    }

  if ((tmax-tmin+1)==nSamples)
    {
      x1=tmin;
      x2=tmax;
    }
  else if ((tmax-tmin+1)<nSamples)
    {
      if (_samples.size()<nSamples)
	{
	  x1=0;
	  x2=_samples.size()-1;
	}
      else if (tmin+nSamples-1<_samples.size())
	{
	  x1=tmin;
	  x2=(tmin+nSamples-1);
	}
      else
	{
	  x2=_samples.size();
	  x1=x2-nSamples+1;
	}
    }
  else if ((tmax-tmin+1)>nSamples)
    {
      x1=tmin;
      x2=(tmin+nSamples-1);
    }
}
//Get the baseline (pedestal and RMS) informations computed between x1 and x2
Waveform::baseline_informations Waveform::baseline(const int& x1, const int& x2) const
{

  baseline_informations return_value;

  if (x1<0 || x2>(_samples.size()-1))
    {
      if (WARNING_ERROR)
	std::cout << "WARNING::Waveform::baseline::gate is outside samples range" << std::endl;
      return return_value;
    }
  
  if ((x2-x1)<2)
    {
      if (WARNING_ERROR)
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
