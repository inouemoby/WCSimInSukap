#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TLorentzVector.h"

void read_pmtorienvspositiony(const char* filename = "../rootfile/wcsim_output.root")
{
    char* wcsimdirenv;
    wcsimdirenv = getenv("WCSIMDIR");
    if(wcsimdirenv !=  NULL){
        gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
    }else{
        gSystem->Load("../libWCSimRoot.so");
    }

    // 打开ROOT文件
    TFile *file = TFile::Open(filename);
    if (!file || file->IsZombie())
    {
        // 处理无法打开文件的情况
        return;
    }

    // 获取TTree "wcsimT"
    TTree *tree = (TTree*)file->Get("wcsimT");
    if (!tree)
    {
        // 处理找不到"wcsimT"的情况
        file->Close();
        return;
    }

	TLorentzVector photonPosition(0, 0, 0, 0); 

	WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
	tree->SetBranchAddress("wcsimrootevent", &wcsimrootsuperevent);
	
	WCSimRootGeom *geo = 0;
	TTree *geotree = (TTree*)file->Get("wcsimGeoT");
	geotree->SetBranchAddress("wcsimrootgeom", &geo);
	geotree->GetEntry(0);

	int numPMTs = geo->GetWCNumPMT();

	TH2F *angleHist = new TH2F("Angle", "PMT Angle Distribution vs Y", 90, 0, 90, 100, -350, 350);

	angleHist->GetXaxis()->SetTitle("Angle [degree]");
	angleHist->GetYaxis()->SetTitle("Photon Position Y [cm]");

	for (int tubeID = 1; tubeID <= numPMTs; ++tubeID)
	{
        WCSimRootPMT pmtInfo = geo->GetPMT(tubeID - 1);

        TLorentzVector pmtPosition(pmtInfo.GetPosition(0), pmtInfo.GetPosition(1), pmtInfo.GetPosition(2), 0);
        TLorentzVector pmtOrientation(pmtInfo.GetOrientation(0), pmtInfo.GetOrientation(1), pmtInfo.GetOrientation(2), 0);

		for (int y = -350; y <= 350; y = y+7)
		{
			photonPosition.SetY(y);

			TLorentzVector positionVector = pmtPosition - photonPosition;

			double cosTheta = positionVector.Dot(pmtOrientation) / (positionVector.Vect().Mag() * pmtOrientation.Vect().Mag());

			double angle = TMath::ACos(cosTheta) * TMath::RadToDeg();

			angleHist->Fill(angle, y);
		}
	}

	TCanvas *c2 = new TCanvas("c2", "PMT Angle Distribution", 800, 600);
	angleHist->Draw("COLZ");
	c2->Update();
}
