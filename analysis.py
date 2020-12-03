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


    ''' Number of primaries '''
    gasConversionData, gasConversionColumns = rootFile.Get('conversion').AsMatrix(return_labels=True)
    gasConversionDataFrame = pd.DataFrame(data=gasConversionData, columns=gasConversionColumns)

    primariesSpectrum = rt.TH1F('GasPrimaries', ';Primary electrons;', 100, 0, 300)
    for energy in gasConversionDataFrame['primaries']: primariesSpectrum.Fill(energy)
    primariesSpectrum.Scale(1/primariesSpectrum.Integral(), 'width')

    gaus3 = rt.TF1('gaus3', 'gaus(0)+gaus(3)+gaus(6)', 0, 300)
    gaus4 = rt.TF1('gaus3', 'gaus(0)+gaus(3)+gaus(6)+gaus(9)', 0, 300)
    #gaus3.SetParameters(0.05, 200, 10, 0.01, 220, 10, 0.01, 90, 10)
    gaus4.SetParameters(0.05, 200, 10, 0.01, 220, 10, 0.01, 90, 10, 0.002, 120)
    gaus4.SetParameter(11, 10)
    #gaus4.SetLineStyle(7)
    gaus4.SetLineColor(rt.kRed)
    primariesSpectrum.Fit(gaus4)

    primariesSpectrum.SetLineColor(rt.kViolet+2)
    primariesSpectrumCanvas = rt.TCanvas('PrimariesSpectrumCanvas', '', 1000, 800)
    primariesSpectrum.Draw('hist')
    fs = list()
    for i in range(4):
        fs.append(rt.TF1('gaus%d'%(i), 'gaus(0)', 0, 300))
        fs[-1].SetParameters(gaus4.GetParameter(3*i), gaus4.GetParameter(3*i+1), gaus4.GetParameter(3*i+2))
        fs[-1].SetLineStyle(7)
        fs[-1].SetLineColor(rt.kRed)
        fs[-1].Draw('same')
    #gaus4.Draw('same')

    primariesSpectrumCanvas.Update()
    primariesSpectrumCanvas.Draw()
    fitBox = primariesSpectrum.FindObject('stats')
    fitBox.SetTextColor(rt.kRed+2)
    fitBox.SetX1NDC(0.2)
    fitBox.SetY1NDC(0.5)
    fitBox.SetX2NDC(0.5)
    fitBox.SetY2NDC(0.90)

    primariesSpectrumCanvas.SaveAs('%s/PrimariesSpectrum.eps'%(options.output))

    averagePrimaries = primariesSpectrum.GetMean()
    errAveragePrimaries = primariesSpectrum.GetRMS()/primariesSpectrum.GetEntries()**0.5
    print('%1.1f +/- %1.1f primary electrons'%(averagePrimaries, errAveragePrimaries))

    if options.fe55:
        primaryPeakEnergy1 = 5.89
        primaryPeakEnergy2 = 6.49
    elif options.xray:
        primaryPeakEnergy1 = 22.16
        primaryPeakEnergy2 = 24.9
    primaryPeakPrimaries1 = gaus4.GetParameter(1)
    primaryPeakPrimaries2 = gaus4.GetParameter(4)
    primariesToEnergyScale = (primaryPeakEnergy2-primaryPeakEnergy1)/(primaryPeakPrimaries2-primaryPeakPrimaries1)
    primariesToEnergyOffset = primaryPeakEnergy2 - primariesToEnergyScale*primaryPeakPrimaries2
    print('%1.2f peak corresponds to %1.1f primaries'%(primaryPeakEnergy1, primaryPeakPrimaries1))
    print('%1.2f peak corresponds to %1.1f primaries'%(primaryPeakEnergy2, primaryPeakPrimaries2))
    print('Conversion factor %1.2f keV/primary, offset %1.2f keV'%(primariesToEnergyScale, primariesToEnergyOffset))


    ''' Energy spectrum after each layer ''' 
    volumeColors = [rt.kBlack, rt.kBlue, rt.kCyan, rt.kRed, rt.kViolet]
    volumes = ['primary', 'window', 'driftKapton', 'driftCopper', 'conversion']
    volumeTitles = ['Source', 'Window', 'Drift kapton', 'Drift copper', 'Gas conversion']
    
    energySpectra = rt.THStack('EnergySpectra', ';Energy (keV);')
    legend = rt.TLegend(0.2, 0.65, 0.45, 0.9)
    legend.SetHeader('#bf{Position in detector}')
    for i,volume in enumerate(volumes):
        volumeTree = rootFile.Get(volume)
        volumeTree.Print()

        treeData, treeColumns = volumeTree.AsMatrix(return_labels=True)
        treeDataFrame = pd.DataFrame(data=treeData, columns=treeColumns)

        energySpectrum = rt.TH1F('HitEnergies'+volume, '', 100, 0, 7)
        if volume == 'conversion': energies = treeDataFrame['primaries']*primariesToEnergyScale + primariesToEnergyOffset
        else: energies = treeDataFrame['energy']
        for energy in energies: energySpectrum.Fill(energy)
        #energySpectrum.Scale(1/energySpectrum.Integral(), 'width') # normalize spectrum
        energySpectrum.SetLineColor(volumeColors[i])
        legend.AddEntry(energySpectrum, volumeTitles[i], 'l')
        energySpectra.Add(energySpectrum, 'hist')

    energySpectrumCanvas = rt.TCanvas('EnergySpectrumCanvas', '', 1000, 800)
    energySpectrumCanvas.SetLogy()
    energySpectra.Draw('nostack')
    legend.Draw()
    energySpectrumCanvas.SaveAs('%s/EnergySpectra.eps'%(options.output))


if __name__=='__main__': main()
