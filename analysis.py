#!/usr/bin/python3

import os, sys
import argparse

import numpy as np
import ROOT as rt
import pandas as pd

from physlibs.root import root_style_ftm


def main():
    ap = argparse.ArgumentParser(add_help=True)
    ap.add_argument('-i', '--input')
    ap.add_argument('-o', '--output')
    ap.add_argument('-v', '--verbose', action='store_true')
    ap.add_argument('-b', action='store_true', help='ROOT batch mode')
    options = ap.parse_args(sys.argv[1:])

    try: os.makedirs(options.output)
    except FileExistsError: pass

    rootFile = rt.TFile(options.input)

    volumeColors = [rt.kBlack, rt.kBlue, rt.kCyan, rt.kRed, rt.kViolet]
    volumes = ['primary', 'window', 'driftKapton', 'driftCopper', 'conversion']
    volumeTitles = ['Source', 'Window', 'Drift kapton', 'Drift copper', 'Gas conversion']
    
    energySpectra = rt.THStack('EnergySpectra', ';Energy (keV);')
    legend = rt.TLegend(0.2, 0.7, 0.4, 0.9)
    for i,volume in enumerate(volumes):
        volumeTree = rootFile.Get(volume)
        volumeTree.Print()

        treeData, treeColumns = volumeTree.AsMatrix(return_labels=True)
        treeDataFrame = pd.DataFrame(data=treeData, columns=treeColumns)
        energySpectrum = rt.TH1F('HitEnergies'+volume, '', 100, 0, 9)
        for energy in treeDataFrame['energy']: energySpectrum.Fill(energy)
        energySpectrum.SetLineColor(volumeColors[i])
        legend.AddEntry(energySpectrum, volumeTitles[i], 'l')
        energySpectra.Add(energySpectrum)

    energySpectrumCanvas = rt.TCanvas('EnergySpectrumCanvas', '', 1000, 800)
    energySpectrumCanvas.SetLogy()
    energySpectra.Draw('nostack')
    legend.Draw()
    energySpectrumCanvas.SaveAs('%s/EnergySpectra.eps'%(options.output))


    ''' Number of primaries '''
    gasConversionData, gasConversionColumns = rootFile.Get('conversion').AsMatrix(return_labels=True)
    gasConversionDataFrame = pd.DataFrame(data=gasConversionData, columns=gasConversionColumns)
    primariesSpectrum = rt.TH1F('GasPrimaries', ';Primary electrons;', 100, 0, 300)
    for energy in gasConversionDataFrame['primaries']: primariesSpectrum.Fill(energy)
    primariesSpectrum.SetLineColor(rt.kViolet+2)
    primariesSpectrumCanvas = rt.TCanvas('PrimariesSpectrumCanvas', '', 1000, 800)
    primariesSpectrum.Draw()
    primariesSpectrumCanvas.SaveAs('%s/PrimariesSpectrum.eps'%(options.output))

    sys.exit(0)

    runTreeMatrix = runTree.AsMatrix()
    hitEventID = runTree.AsMatrix(['hitEventID'])
    hitTimes = runTree.AsMatrix(['hitTime'])
    vertexPositionsZ = runTree.AsMatrix(['vertexPositionZ'])
    vertexPositionsZ = max(vertexPositionsZ) - vertexPositionsZ

    timePositionHisto = rt.TH2F('TimePositionHisto', ';Photon arrival time (ns);Photon production distance from SiPM (mm)', 80, 0, 4, 40, -20, 100)
    for t,z in zip(hitTimes, vertexPositionsZ): timePositionHisto.Fill(t, z)
    timePositionCanvas = rt.TCanvas('TimePositionCanvas', '', 800, 800)
    timePositionHisto.Draw('colz')
    timePositionCanvas.SaveAs('%s/TimePosition.eps'%(options.output))

    signalTimeHisto = rt.TH1F('SignalTimeHisto', ';Signal time (ns)', 60, 0, 2)
    signalTimePositionHisto = rt.TH2F('SignalTimePositionHisto', ';Signal time (ns);Muon distance from SiPM (mm)', 60, 0, 2, 50, 0, 100)
    signalTimeNphotonsHisto = rt.TH2F('SignalTimeNphotonsHisto', ';Signal time (ns);Number of photons collected', 60, 0, 2, 40, -10, 500)
    positionNphotonsHisto = rt.TH2F('PositionNphotonsHisto', ';Muon hit distance from SiPM (mm);Number of photons collected', 50, 0, 100, 40, -10, 500)
    for eventID in range(nmuons):
        try:
            signalTime = sorted(hitTimes[hitEventID==eventID])[:50][-1]
            nphotons = len(hitTimes[hitEventID==eventID])
        except IndexError: continue
        photonVertices = vertexPositionsZ[hitEventID==eventID]
        photonVertex = sum(photonVertices)/len(photonVertices)
        signalTimeHisto.Fill(signalTime)
        signalTimePositionHisto.Fill(signalTime, photonVertex)
        positionNphotonsHisto.Fill(photonVertex, nphotons)
        signalTimeNphotonsHisto.Fill(signalTime, nphotons)
    
    signalTimeCanvas = rt.TCanvas('SignalTimeCanvas', '', 800, 600)
    #gaus2 = rt.TF1('gaus2', 'gaus(0)+gaus(3)', 0, 10)
    #gaus2.SetParameters(500, 2, 0.8, 50, 4, 1.5)
    #signalTimeHisto.Fit(gaus2)
    signalTimeHisto.Draw()
    #signalTimeHisto.Fit('gaus')
    '''gauss1 = rt.TF1('gauss1', 'gaus', 0, 10)
    gauss2 = rt.TF1('gauss2', 'gaus', 0, 10)
    gauss1.SetParameters(gaus2.GetParameter(0), gaus2.GetParameter(1), gaus2.GetParameter(2))
    gauss2.SetParameters(gaus2.GetParameter(3), gaus2.GetParameter(4), gaus2.GetParameter(5))
    gauss1.SetLineStyle(7)
    gauss2.SetLineStyle(7)
    gauss1.Draw('same')
    gauss2.Draw('same')'''
    signalTimeCanvas.SaveAs('%s/SignalTime.eps'%(options.output))

    signalTimePositionCanvas = rt.TCanvas('SignalTimePositionCanvas', '', 1000, 800)
    signalTimePositionHisto.SetLineWidth(1)
    signalTimePositionHisto.SetLineColor(rt.kCyan+2)
    signalTimePositionHisto.Draw('colz')
    signalTimePositionCanvas.SaveAs('%s/SignalTimePosition.eps'%(options.output))
    
    positionNphotonCanvas = rt.TCanvas('PositionNphotonCanvas', '', 850, 600)
    positionNphotonCanvas.SetLeftMargin(-.1)
    positionNphotonCanvas.SetRightMargin(.11)
    positionNphotonsHisto.Draw('colz')
    positionNphotonCanvas.SaveAs('%s/PositionNphotons.eps'%(options.output))
    
    signalTimeNphotonCanvas = rt.TCanvas('SignalTimeNphotonCanvas', '', 850, 600)
    signalTimeNphotonCanvas.SetLeftMargin(-.1)
    signalTimeNphotonCanvas.SetRightMargin(.11)
    signalTimeNphotonsHisto.Draw('colz')
    signalTimeNphotonCanvas.SaveAs('%s/SignalTimeNphotons.eps'%(options.output))

if __name__=='__main__': main()
