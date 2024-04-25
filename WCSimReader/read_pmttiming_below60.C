#include "TH1F.h"
#include "TCanvas.h"
#include "TSystem.h"

#include "cal_pmtinjectangle.C"

const char* filename = "../rootfile/wcsim_output.root";

void read_pmttiming_below60()
{
  
  TFile *file = TFile::Open(filename);
  if (!file || file->IsZombie())
    {
      cont << "Can not open the rootfile!" << endl;
      return;
    }
  TTree *tree = (TTree*)file->Get("wcsimT");
  
  HitListManager manager = cal_pmtinjectangle(TLorentzVector(0, 0, 0, 0));

  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
    tree->SetBranchAddress("wcsimrootevent", &wcsimrootsuperevent);
    TH1F *timingHist = new TH1F("PMT Timing", "PMT Timing Distribution", 100, 900, 1040);
    timingHist->GetXaxis()->SetTitle("Time (ns)");
    timingHist->GetYaxis()->SetTitle("Counts");
    int numEvents = tree->GetEntries();
    cout << "start hit hist" << endl;
    for (int i = 0; i < numEvents; i++)
    {
        tree->GetEntry(i);
	WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
	TClonesArray *hitArray = wcsimrootevent->GetCherenkovDigiHits();
	if (!wcsimrootevent)
	  {
	    cout << "Warning: wcsimrootevent is nullptr for event " << i << endl;
	    continue;
	  }
	for (int j = 0; j < hitArray->GetEntries(); j++)
        {
            WCSimRootCherenkovDigiHit *hit = (WCSimRootCherenkovDigiHit*)hitArray->At(j);
            double time = hit->GetT();
	    int tubeID = hit->GetTubeId();
            double angle = manager.GetValueAt(tubeID);
            if (angle <= 60.0)
            {
                timingHist->Fill(time);
	    }
        }
    }

 
    cout << "end hit hist" << endl;  
    TCanvas *c1 = new TCanvas("c1", "PMT Timing Distribution", 800, 600);
    cout << "created canvas" << endl;

    timingHist->Draw();
    cout << "printed hist" << endl;
    file->Close();

}
