#include "WaveformFit.h"
#include "Math/Interpolator.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TMath.h"

#include "assert.h"

#define MAX_INTERPOLATOR_POINTS 10000

namespace WaveformFit
{
  TProfile* refWave;
  TProfile* fitWave;

  //   ROOT::Math::Minimizer* min =0;

  ROOT::Math::Interpolator* y_interpolator=0;
  ROOT::Math::Interpolator* y_err_interpolator=0;

  //   int nPointsUsed;
  //   int nSamplesToInterpolate;

  int xMin,xMax;

  double chi2(const double *par )
  {
    double chisq = 0;
    double delta = 0;
    for (int i=1;i<refWave->GetNbinsX();++i)
      {
	//	delta=(refWave->GetBinContent(i)-(y_interpolator->Eval(refWave->GetBinCenter(i)+par[1])+par[0]))/(TMath::Sqrt(refWave->GetBinError(i)*refWave->GetBinError(i)+y_err_interpolator->Eval(refWave->GetBinCenter(i)+par[1])*y_err_interpolator->Eval(refWave->GetBinCenter(i)+par[1])));
	delta= (refWave->GetBinContent(i)-(y_interpolator->Eval(refWave->GetBinCenter(i)+par[1])+par[0])); //using no error for the moment in the Chi2 definition
	chisq += delta*delta;
      }
    return chisq;
  }

  void alignWaveform(TProfile* ref_profile, TProfile* fit_profile, ROOT::Math::Minimizer* &minimizer)
  {    
    xMin=1;
    xMax=990;

    refWave=ref_profile;
    fitWave=fit_profile;

    minimizer = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");

    minimizer->SetMaxFunctionCalls(1000000);
    minimizer->SetMaxIterations(100000);
    minimizer->SetTolerance(1e-6);
    minimizer->SetPrintLevel(2);

    if (!y_interpolator)
      y_interpolator=new ROOT::Math::Interpolator(MAX_INTERPOLATOR_POINTS, ROOT::Math::Interpolation::kCSPLINE);
    if (!y_err_interpolator)
      y_err_interpolator=new ROOT::Math::Interpolator(MAX_INTERPOLATOR_POINTS, ROOT::Math::Interpolation::kCSPLINE);

    std::vector<double> x,y,y_err;

    for (int i=1;i<=fit_profile->GetNbinsX();++i)
      {
	x.push_back(fit_profile->GetBinCenter(i));
	y.push_back(fit_profile->GetBinContent(i));
	y_err.push_back(fit_profile->GetBinError(i));
      }

    y_interpolator->SetData(x,y);
    y_err_interpolator->SetData(x,y_err);

    ROOT::Math::Functor f(&chi2,2);
    minimizer->SetFunction(f);

    minimizer->SetVariable(0,"deltaV",0.,1e-2);
    minimizer->SetVariable(1,"deltaT",0.,1e-1);
    minimizer->Minimize();
   
    const double* par=minimizer->X();
    std::cout << "+++++ FIT RESULT: " << par[0] << "," << par[1] << std::endl;
    //     minimizer=min;
  }


  
};

