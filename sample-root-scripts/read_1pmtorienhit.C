#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TLorentzVector.h"

void read_1pmtorienhit(const char* filename = "../rootfile/wcsim_output.root")
{
  //define parameter
  //int startID = 1;

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

	TH1F *angleHist = new TH1F("Angle", "PMT ID vs Angle", numPMTs, 1, numPMTs);

	angleHist->GetXaxis()->SetTitle("PMT ID");
	angleHist->GetYaxis()->SetTitle("Angle [degree]");
 
  	TH1F *angleDistriHist = new TH1F("Angle", "PMT Angle Distribution", 90, 0, 90);

	angleDistriHist->GetXaxis()->SetTitle("Angle [degree]");
	angleDistriHist->GetYaxis()->SetTitle("Number of PMTs");

	for (int tubeID = 1; tubeID <= numPMTs; ++tubeID)
	{
        WCSimRootPMT pmtInfo = geo->GetPMT(tubeID - 1);

        TLorentzVector pmtPosition(pmtInfo.GetPosition(0), pmtInfo.GetPosition(1), pmtInfo.GetPosition(2), 0);
        TLorentzVector pmtOrientation(pmtInfo.GetOrientation(0), pmtInfo.GetOrientation(1), pmtInfo.GetOrientation(2), 0);

        TLorentzVector positionVector = pmtPosition - photonPosition;

        double cosTheta = positionVector.Dot(pmtOrientation) / (positionVector.Vect().Mag() * pmtOrientation.Vect().Mag());

        double angle = TMath::ACos(cosTheta) * TMath::RadToDeg();

        angleHist->Fill(tubeID, angle);
        
        angleDistriHist->Fill(angle);
	}
 


	// 获取总事件数
	int numEvents = tree->GetEntries();

	// 创建一个直方图，用于存储每个hit的角度和hit数
	TH1F *angleVsHitsHist = new TH1F("angleVsHits", "Hits vs Angle", 90, 0, 90);
	angleVsHitsHist->GetXaxis()->SetTitle("Angle [degree]");
	angleVsHitsHist->GetYaxis()->SetTitle("Hits");

	// 循环读取每个事件的信息
	for (int i = 0; i < numEvents; ++i)
	{
		tree->GetEntry(i);

		// 获取WCSimRootTrigger对象，该对象包含每个事件的track信息
		WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

		// 获取 WCSimRootCherenkovDigiHit 列表

		TClonesArray *hitArray = wcsimrootevent->GetCherenkovDigiHits();
		
		// 遍历每个 hit，统计指定tubeID 的timing分布
		for (int j = 0; j < hitArray->GetEntries(); ++j)
		{

			WCSimRootCherenkovDigiHit *hit = (WCSimRootCherenkovDigiHit*)hitArray->At(j);
            int tubeID = hit->GetTubeId();

			// 获取该tubeID对应的角度
			double angle = angleHist->GetBinContent(tubeID);

			// 在直方图中填充该hit的角度
			angleVsHitsHist->Fill(angle);
		}
	}

	// 创建一个直方图，用于存储每个角度的平均hit数
	TH1F *angleVsAvgHitsHist = new TH1F("angleVsAvgHits", "Average Hits vs Angle", 90, 0, 90);
	angleVsAvgHitsHist->GetXaxis()->SetTitle("Angle [degree]");
	angleVsAvgHitsHist->GetYaxis()->SetTitle("Average Hits");

	// 计算每个角度的平均hit数，并填充到直方图中
	for (int i = 1; i <= angleVsHitsHist->GetNbinsX(); ++i)
	{
		double angle = angleVsHitsHist->GetBinCenter(i);
		double hits = angleVsHitsHist->GetBinContent(i);
		double pmts = angleDistriHist->GetBinContent(i);

		if (pmts > 0)
		{
			double avgHits = hits / pmts;
			angleVsAvgHitsHist->Fill(angle, avgHits);
		}
	}

	TCanvas *c2 = new TCanvas("c2", "Average Hits vs Angle", 800, 600);
	angleVsAvgHitsHist->Draw();
	c2->Update();
}
