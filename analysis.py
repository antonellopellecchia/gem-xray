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
    ap.add_argument('--energies', nargs='+', type=float)
    ap.add_argument('--primaries', nargs='+', type=float)
    ap.add_argument('--calibration', nargs='+', type=float)
    ap.add_argument('--label', type=str)
    ap.add_argument('--geometry', type=str)
    ap.add_argument('--materials', nargs='+', type=str)
    ap.add_argument('--materialLabels', nargs='+', type=str)
    ap.add_argument('-v', '--verbose', action='store_true')
    ap.add_argument('-b', action='store_true', help='ROOT batch mode')
    options = ap.parse_args(sys.argv[1:])

    if not options.calibration: # assume iron spectrum and try calibrating
        primaryPeakEnergy = [5.89, 6.49]
        primaryPeakPrimaries = [200, 220]

    energyBins = int(options.energies[0])
    energyBot = options.energies[1]
    energyTop = options.energies[2]

    primariesBins = int(options.primaries[0])
    primariesBot = options.primaries[1]
    primariesTop = options.primaries[2]

    try: os.makedirs(options.output)
    except FileExistsError: pass

    rootFile = rt.TFile(options.input)


    ''' Number of primaries '''
    gasConversionData, gasConversionColumns = rootFile.Get('conversion').AsMatrix(return_labels=True)
    gasConversionDataFrame = pd.DataFrame(data=gasConversionData, columns=gasConversionColumns)

    primariesSpectrum = rt.TH1F('GasPrimaries', ';Primary electrons;', primariesBins, primariesBot, primariesTop)
    for energy in gasConversionDataFrame['primaries']: primariesSpectrum.Fill(energy)
    primariesSpectrum.Scale(1/primariesSpectrum.Integral(), 'width')


    primariesSpectrum.SetLineColor(rt.kViolet+2)
    primariesSpectrumCanvas = rt.TCanvas('PrimariesSpectrumCanvas', '', 1000, 800)
    primariesSpectrum.Draw('hist')


    if not options.calibration: # assume iron spectrum and try calibrating
        gaus3 = rt.TF1('gaus3', 'gaus(0)+gaus(3)+gaus(6)', 0, 300)
        gaus4 = rt.TF1('gaus3', 'gaus(0)+gaus(3)+gaus(6)+gaus(9)', 0, 300)
        #gaus3.SetParameters(0.05, 200, 10, 0.01, 220, 10, 0.01, 90, 10)
        gaus4.SetParameters(0.05, primaryPeakPrimaries[0], 10, 0.01, primaryPeakPrimaries[1], 10, 0.01, 90, 10, 0.002, 120)
        gaus4.SetParameter(11, 10)
        #gaus4.SetLineStyle(7)
        gaus4.SetLineColor(rt.kRed)
        primariesSpectrum.Fit(gaus4)
        fs = list()
        for i in range(4):
            fs.append(rt.TF1('gaus%d'%(i), 'gaus(0)', 0, 300))
            fs[-1].SetParameters(gaus4.GetParameter(3*i), gaus4.GetParameter(3*i+1), gaus4.GetParameter(3*i+2))
            fs[-1].SetLineStyle(7)
            fs[-1].SetLineColor(rt.kRed)
            fs[-1].Draw('same')

        primariesSpectrumCanvas.Update()
        primariesSpectrumCanvas.Draw()
        fitBox = primariesSpectrum.FindObject('stats')
        fitBox.SetTextColor(rt.kRed+2)
        fitBox.SetX1NDC(0.2)
        fitBox.SetY1NDC(0.5)
        fitBox.SetX2NDC(0.5)
        fitBox.SetY2NDC(0.90)

        primaryPeakPrimaries[0] = gaus4.GetParameter(1)
        primaryPeakPrimaries[1] = gaus4.GetParameter(4)
        primariesToEnergyScale = (primaryPeakEnergy[1]-primaryPeakEnergy[0])/(primaryPeakPrimaries[1]-primaryPeakPrimaries[0])
        primariesToEnergyOffset = primaryPeakEnergy[1] - primariesToEnergyScale*primaryPeakPrimaries[1]
        print('%1.2f peak corresponds to %1.1f primaries'%(primaryPeakEnergy[0], primaryPeakPrimaries[0]))
        print('%1.2f peak corresponds to %1.1f primaries'%(primaryPeakEnergy[1], primaryPeakPrimaries[1]))
        print('Conversion factor %1.5f keV/primary, offset %1.5f keV'%(primariesToEnergyScale, primariesToEnergyOffset))
    else:
        primariesToEnergyScale, primariesToEnergyOffset = options.calibration[0], options.calibration[1]

    if options.label:
        latex = rt.TLatex()
        latex.SetTextAlign(32)
        latex.SetTextSize(.035)
        latex.DrawLatexNDC(.95, .96, options.label)

    primariesSpectrumCanvas.SaveAs('%s/PrimariesSpectrum.eps'%(options.output))
    primariesSpectrumCanvas.SaveAs('%s/PrimariesSpectrum.root'%(options.output))

    averagePrimaries = primariesSpectrum.GetMean()
    errAveragePrimaries = primariesSpectrum.GetRMS()/primariesSpectrum.GetEntries()**0.5
    print('%1.1f +/- %1.1f primary electrons'%(averagePrimaries, errAveragePrimaries))



    ''' Energy spectrum after each layer ''' 
    volumeColors = [rt.kBlack, rt.kBlue, rt.kCyan, rt.kRed, rt.kViolet, rt.kGreen, 38, 28]
    if options.geometry:
        if options.geometry=='10x10':
            volumes = ['primary', 'window', 'driftKapton', 'driftCopper', 'conversion']
            volumes = ['primary', 'kapton1', 'kapton2', 'copper3', 'conversion']
            volumeTitles = ['X-ray source', 'After 125 #mum window kapton', 'After 50 #mum drift kapton', 'After 5 #mum drift copper', 'Conversion in 3 mm gas gap']
        else:
            print('Unknown geometry')
            sys.exit(1)
    elif options.materials:
        volumes = ['primary']
        volumeTitles = ['Source']
        for i,material in enumerate(options.materials):
            volumes.append(material+str(i+1))
            if options.materialLabels: volumeTitles.append(options.materialLabels[i])
            else: volumeTitles.append(volumes[-1])
        volumes.append('conversion')
        volumeTitles.append('Conversion in 3 mm gas gap')
    else:
        print('Please specify materials or geometry')
        sys.exit(1)
    
    energySpectra = rt.THStack('EnergySpectra', ';Energy (keV);')
    legend = rt.TLegend(0.65, 0.6, 0.92, 0.92)
    legend.SetHeader('#bf{Position in detector}')
    for i,volume in enumerate(volumes):
        volumeTree = rootFile.Get(volume)
        volumeTree.Print()
        if volumeTree.GetEntries()==0: continue

        treeData, treeColumns = volumeTree.AsMatrix(return_labels=True)
        treeDataFrame = pd.DataFrame(data=treeData, columns=treeColumns)

        energySpectrum = rt.TH1F('HitEnergies'+volume, '', energyBins, energyBot, energyTop)
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

    if options.label:
        latex = rt.TLatex()
        latex.SetTextAlign(32)
        latex.SetTextSize(.035)
        latex.DrawLatexNDC(.95, .96, options.label)

    energySpectrumCanvas.SaveAs('%s/EnergySpectra.eps'%(options.output))
    energySpectrumCanvas.SaveAs('%s/EnergySpectra.root'%(options.output))

    print('%1.1f +/- %1.1f primary electrons'%(averagePrimaries, errAveragePrimaries))

if __name__=='__main__': main()
