{
  gROOT->ProcessLine(".L macros/fitTimeResVsAmplitude.C+");
  gSystem->MakeDirectory("results");

  gSystem->MakeDirectory("results/pilasTurin2800V");
  TH2F* timeVsAmplitude=new TH2F("timeVsAmplitude","timeVsAmplitude",120,2,302,200,-2,2);
  fitTimeResVsAmplitude(timeVsAmplitude,"/tmp/meridian/mcpRome_2800V.root","eventData","results/pilasTurin2800V","ch_0_time_at_frac30-51.2:ch_0_max_amplitude","(ch_0_time_at_max>50 &&  ch_0_time_at_max<53) && ch_0_max_amplitude>2. && !(ch_0_max_amplitude>14.5 && ch_0_max_amplitude<202)","ch0","MCP BINP Rome#1 -2.8 kV");
  fitTimeResVsAmplitude(timeVsAmplitude,"/tmp/meridian/mcpRome_2800V.root","eventData","results/pilasTurin2800V","ch_1_time_at_frac30-51.2:ch_1_max_amplitude","(ch_1_time_at_max>51 &&  ch_1_time_at_max<53) && ch_1_max_amplitude>2. && !(ch_1_max_amplitude>14.5 && ch_1_max_amplitude<202)","ch1","MCP BINP Rome#2 -2.8 kV");

  gSystem->MakeDirectory("results/pilasTurin2800VZFLPreamp");
  fitTimeResVsAmplitude(timeVsAmplitude,"/tmp/meridian/mcpRomeConv/run13_waves.root","eventData","results/pilasTurin2800VZFLPreamp","ch_0_time_at_frac30-54.5:ch_0_max_amplitude/10","(ch_0_time_at_max>54 &&  ch_0_time_at_max<56) && ch_0_max_amplitude>2.","ch0","MCP BINP Rome#1 -2.8 kV ZFL-1000 LN+");
  fitTimeResVsAmplitude(timeVsAmplitude,"/tmp/meridian/mcpRomeConv/run13_waves.root","eventData","results/pilasTurin2800VZFLPreamp","ch_1_time_at_frac30-54.5:ch_1_max_amplitude/10","(ch_1_time_at_max>54 &&  ch_1_time_at_max<56) && ch_1_max_amplitude>2.","ch1","MCP BINP Rome#2 -2.8 kV ZFL-1000 LN+");

  gSystem->MakeDirectory("results/pilasTurin2300VCividec");
  fitTimeResVsAmplitude(timeVsAmplitude,"/tmp/meridian/mcpRomeConv/run14_waves.root","eventData","results/pilasTurin2300VCividec","ch_0_time_at_frac30-51.2:ch_0_max_amplitude/10","(ch_0_time_at_max>50 &&  ch_0_time_at_max<53) && ch_0_max_amplitude/10>80.","ch0","MCP BINP Rome#1 -2.3 kV Cividec");
  fitTimeResVsAmplitude(timeVsAmplitude,"/tmp/meridian/mcpRomeConv/run14_waves.root","eventData","results/pilasTurin2300VCividec","ch_1_time_at_frac30-51.2:ch_1_max_amplitude/10","(ch_1_time_at_max>50 &&  ch_1_time_at_max<53) && ch_1_max_amplitude/10>30","ch1","MCP BINP Rome#2 -2.3 kV Cividec");

}
