#!/usr/bin/env python   

import sys
import os
from array import *

import ntpath

def path_leaf(path):
    head, tail = ntpath.split(path)
    return tail or ntpath.basename(head)


from optparse import OptionParser
parser=OptionParser()
parser.add_option("-p","--profileFile")
parser.add_option("-s","--rms_amplitude_noise")
parser.add_option("-L","--libDir")
parser.add_option("-n","--numberOfEvents",default=-1)
parser.add_option("-d","--dir") # DQM_HOME directory
parser.add_option("-o","--outputDir")
#parser.add_option("-p","--plotsDir")

(options,args)=parser.parse_args()

import ROOT as r
r.gROOT.SetBatch(1)
r.gSystem.Load(options.libDir+"/libBTFTBSW.so")


os.system('mkdir -p %s'%options.outputDir)

a=r.SimulatePulses()

a.profileFile=options.profileFile
a.nEvents=int(options.numberOfEvents)
a.outFile=options.outputDir+"/"+os.path.splitext(path_leaf(options.profileFile))[0]+"_simulatedPulses_RAWDATA.root"
a.rms_amplitude_noise=float(options.rms_amplitude_noise)

a.Loop()

