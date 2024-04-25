#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TLorentzVector.h"

void read_firstelectronposition(const char* filename = "../rootfile/wcsim_output.root")
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

	WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
	tree->SetBranchAddress("wcsimrootevent", &wcsimrootsuperevent);

	// 获取总事件数
	int numEvents = tree->GetEntries();

	// 创建一个二维直方图，用于存储每个事件中第一次出现电子的位置
	TH2F *positionHist = new TH2F("positionHist", "y vs r", 100, 0, 350, 100, -400, 400);
	TH2F *avgPositionHist = new TH2F("avgPositionHist", "Average y vs r", 100, 0, 350, 100, -400, 400);

	positionHist->GetXaxis()->SetTitle("r [cm]");
	positionHist->GetYaxis()->SetTitle("y [cm]");

        avgPositionHist->GetXaxis()->SetTitle("r [cm]");
        avgPositionHist->GetYaxis()->SetTitle("y [cm]");

	
	// 循环读取每个事件的信息
	for (int i = 0; i < numEvents; ++i)
	{
		tree->GetEntry(i);

		// 获取WCSimRootTrigger对象，该对象包含每个事件的track信息
		WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

		// 获取 WCSimRootTrack 列表
		TClonesArray *trackArray = wcsimrootevent->GetTracks();

		// 遍历每个 track，找到第一次出现电子的位置
		for (int j = 0; j < trackArray->GetEntries(); ++j)
		{
			WCSimRootTrack *track = (WCSimRootTrack*)trackArray->At(j);

			if (track->GetIpnu() == 11) // 如果这是一个电子
			{
				double x = track->GetStart(0);
				double y = track->GetStart(1);
				double z = track->GetStart(2);

				double r = sqrt(x*x + z*z);

				positionHist->Fill(r, y);

				break; // 只记录第一次出现电子的位置，所以在找到后就跳出循环
			}
		}
	}

	for (int i = 1; i <= positionHist->GetNbinsX(); ++i)
	{
		for (int j = 1; j <= positionHist->GetNbinsY(); ++j)
		{
			double radius = positionHist->GetXaxis()->GetBinCenter(i);
			double y = positionHist->GetYaxis()->GetBinCenter(j);
			double hits = positionHist->GetBinContent(i, j);
			
			if (radius > 0)
			{
				double avgHits = hits / radius;
				avgPositionHist->Fill(radius, y, avgHits);
				//cout << "average hits in " << i << " and " << j << " is " << avgHits << endl; 
			}
		}
	}

	TCanvas *c1 = new TCanvas("c1", "ave y vs r", 800, 600);
	avgPositionHist->Draw("COLZ");
	gPad->SetLogz();
	c1->Update();
}
