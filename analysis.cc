#include "root-style/style.cc"

#include <regex>

void analysis(string path="") {
  TStyle *style = setStyle();
  style->cd();
  gROOT->SetStyle("Garfield");
  gROOT->ForceStyle();

  int nbins = 120;
  
  string volumes[5] = {"primary", "fr4", "cathode", "gas", "conversion"};
  map<string, int> colors;
  for (int i=0; i<5; i++) colors[volumes[i]] = i+5;

  TGraphErrors *gPrimaryElectrons = new TGraphErrors();
  double gasIonizationEnergy = 31.2; // from previous HEED simulation    

  TSystemDirectory *dir = new TSystemDirectory("rootFiles", path.c_str());
  TList *dirFiles = dir->GetListOfFiles();
  TObjLink *fLink;
  regex fileNameRegex("\\w+_(\\d+)");
  smatch fileNameMatch;
  int fileIndex = 0;
  for (fLink=dirFiles->FirstLink(); fLink; fLink=fLink->Next()) {
    map<string, TH1F*> spectrumMap;
    
    if (!(fLink->GetObject()->ClassName()==string("TSystemFile"))) continue;
    string rootFileName = fLink->GetObject()->GetName();
    std::regex_search(rootFileName, fileNameMatch, fileNameRegex);
    string copperThicknessString = string(fileNameMatch[1]);
    double copperThickness = atof(string(fileNameMatch[1]).c_str());
    //if (copperThickness!=60.) continue;
    for (string volume:volumes) spectrumMap[volume] = new TH1F(string(copperThicknessString+volume).c_str(), ";hit energy [keV];counts", nbins, 0., 53.);

    string rootFilePath = string(path+"/"+rootFileName);
    cout << "Processing " << rootFilePath << "... " << endl;
    TFile *rootFileIn = new TFile(rootFilePath.c_str());

    for (string volume:volumes) {
      TTree *tree = (TTree *) rootFileIn->Get(volume.c_str());
      int nHits = tree->GetEntries();
      
      double energy;
      TBranch *hitEnergyBranch = tree->GetBranch("energy");
      hitEnergyBranch->SetAddress(&energy);
      for (int j=0; j<nHits; j++) {
	tree->GetEvent(j);
	spectrumMap[volume]->Fill(energy*gasIonizationEnergy*gasIonizationEnergy/1.e3);
      }
      spectrumMap[volume]->GetYaxis()->SetRangeUser(1., 1.e7);
      spectrumMap[volume]->SetLineColor(colors[volume]);
    }
    gPrimaryElectrons->SetPoint(fileIndex, copperThickness, spectrumMap["conversion"]->GetMean()*1.e3/gasIonizationEnergy);
    gPrimaryElectrons->SetPointError(fileIndex, 0., spectrumMap["conversion"]->GetRMS()*1.e3/gasIonizationEnergy);
    
    TLatex *text = new TLatex();
    text->SetTextSize(0.03);
    
    TCanvas *cHitEnergy = new TCanvas(string("c"+copperThicknessString).c_str(), "", 800, 600);
    cHitEnergy->cd();
    cHitEnergy->SetLogy();
    gStyle->SetOptStat(0);

    TLegend *legend = new TLegend(0.6, 0.15, 0.9, 0.25);
    legend->SetHeader("#bf{Material}", "C");
    legend->SetNColumns(2);
    int i=0;
    for (string volume:volumes) {
      TH1F *h = spectrumMap[volume];
      if (i==0) h->Draw();
      else h->Draw("same");
      legend->AddEntry(h, volume.c_str(), "l");
      i++;
    }
    legend->Draw();
    text->DrawLatexNDC(.15, .966, "#bf{GEANT4+HEED}");
    text->SetTextAlign(31);
    text->DrawLatexNDC(.95, .966, string("Ag-target X-ray tube, "+to_string((int)copperThickness)+" #mum copper tape").c_str());
    cHitEnergy->SaveAs(string(path+"/../eps/spectrum_"+copperThicknessString+".eps").c_str());
    cHitEnergy->SaveAs(string(path+"/../root/spectrum_"+copperThicknessString+".root").c_str());

    fileIndex++;
  }
  
  TCanvas *cPrimaryElectrons = new TCanvas("cPrimaries", "", 800, 600);
  cPrimaryElectrons->cd();
  gPrimaryElectrons->Draw("AP");
  gPrimaryElectrons->SetTitle(";copper thickness (#mum);primary electrons");
  cPrimaryElectrons->SaveAs(string(path+"/../eps/primary_electrons.eps").c_str());
}
