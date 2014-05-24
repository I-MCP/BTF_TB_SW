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
parser.add_option("-i","--inputFile")
parser.add_option("-L","--libDir")
parser.add_option("-d","--dir") # DQM_HOME directory
parser.add_option("-o","--outputDir")
#parser.add_option("-p","--plotsDir")

(options,args)=parser.parse_args()

import ROOT as r
r.gROOT.SetBatch(1)
r.gSystem.Load(options.libDir+"/libBTFTBFW.so")

file = r.TFile.Open(options.inputFile)
if (not file.IsOpen()):
    print "Cannot open "+ options.inputFile
    exit(-1)
tree = file.Get("eventRawData")

os.system('mkdir -p %s'%options.outputDir)

a=r.iMCP_BTF_simpleRecoTre(tree)
a.outFile=options.outputDir+"/"+os.path.splitext(path_leaf(options.inputFile))[0]+"_dqmPlots.root"
a.Loop()

