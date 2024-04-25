#include "TH1F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TLorentzVector.h"

#include "Display3DView.C"

const char* filename = "../rootfile/wcsim_output.root";

void read_pmttiming_below60_kai()
{
  
  TFile *file = TFile::Open(filename);
  if (!file || file->IsZombie())
    {
      cont << "Can not open the rootfile!" << endl;
      return;
    }
  TTree *tree = (TTree*)file->Get("wcsimT");
  
  HitListManager manager;
  TLorentzVector photonPosition = TLorentzVector(0, 0, 0, 0);
  
  WCSimRootGeom *geo = 0;
  TTree *geotree = (TTree*)file->Get("wcsimGeoT");
  geotree->SetBranchAddress("wcsimrootgeom", &geo);
  geotree->GetEntry(0);

  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
  tree->SetBranchAddress("wcsimrootevent", &wcsimrootsuperevent);
	
  int numPMTs = geo->GetWCNumPMT();


  for (int tubeID = 1; tubeID <= numPMTs; ++tubeID)
    {
      WCSimRootPMT pmtInfo = geo->GetPMT(tubeID - 1);

      TLorentzVector pmtPosition(pmtInfo.GetPosition(0), pmtInfo.GetPosition(1), pmtInfo.GetPosition(2), 0);
      TLorentzVector pmtOrientation(pmtInfo.GetOrientation(0), pmtInfo.GetOrientation(1), pmtInfo.GetOrientation(2), 0);

      TLorentzVector positionVector = pmtPosition - photonPosition;

      double cosTheta = positionVector.Dot(pmtOrientation) / (positionVector.Vect().Mag() * pmtOrientation.Vect().Mag());

      double angle = TMath::ACos(cosTheta) * TMath::RadToDeg();
        
      manager.AddValue(angle);
    }
  
  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
    tree->SetBranchAddress("wcsimrootevent", &wcsimrootsuperevent);
    TH1F *timingHist = new TH1F("PMT Timing", "PMT Timing Distribution under 60 degree", 100, 900, 1050);
    timingHist->GetXaxis()->SetTitle("Time (ns)");
    timingHist->GetYaxis()->SetTitle("Counts");
    int numEvents = tree->GetEntries();
    cout << "Events in total " << numEvents << endl;
    
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
	if (!hitArray)
	  {
	    cout << "Warning: hitArray is nullptr for event " << i << endl;
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
    TCanvas *c1 = new TCanvas("c1", "PMT Timing Distribution under 60 degree", 800, 600);
    //gStyle->SetOptStat(0);
    
    cout << "created canvas" << endl;

    timingHist->Draw();
    cout << "printed hist" << endl;

    gStyle->SetStatX(0.9); // 设置统计信息框右边界的位置
    gStyle->SetStatY(0.9); // 设置统计信息框上边界的位置
    gStyle->SetStatW(0.2); // 设置统计信息框的宽度
    gStyle->SetStatH(0.2); // 设置统计信息框的高度

}
