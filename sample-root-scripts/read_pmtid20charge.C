#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TSystem.h"

void read_pmtid20charge(const char* filename = "../rootfile/wcsim_output.root")
{
  //define parameter
  int startID = 1;

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

    // 声明需要读取的变量
    WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
    tree->SetBranchAddress("wcsimrootevent", &wcsimrootsuperevent);

	// 创建一个画布，其中包含4*5个子图
	TCanvas *c1 = new TCanvas("c1", "PMT Charge Distributions", 1600, 900);
	c1->Divide(5, 4);

	// 创建20个直方图
	TH1F *chargeHist[20];
	for (int i = 0; i < 20; ++i) {
		c1->cd(i+1);
		char histName[10];
		sprintf(histName, "h%d", i+1);
		char histTitle[50];
		sprintf(histTitle, "PMT %d Charge Distribution", startID+i);
		chargeHist[i] = new TH1F(histName, histTitle, 50, 0, 3.5);
		chargeHist[i]->GetXaxis()->SetTitle("Charge");
	}

	// 获取总事件数
	int numEvents = tree->GetEntries();

	// 循环读取每个事件的信息
	for (int i = 0; i < numEvents; ++i)
	{
		tree->GetEntry(i);

		// 获取WCSimRootTrigger对象，该对象包含每个事件的track信息
		WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

		// 获取 WCSimRootCherenkovDigiHit 列表

		TClonesArray *hitArray = wcsimrootevent->GetCherenkovDigiHits();
		
		// 遍历每个 hit，统计指定tubeID 的charge分布
		for (int j = 0; j < hitArray->GetEntries(); ++j)
		{

			WCSimRootCherenkovDigiHit *hit = (WCSimRootCherenkovDigiHit*)hitArray->At(j);
            int tubeID = hit->GetTubeId();

			if (tubeID >= startID && tubeID < startID + 20) {
				double charge = hit->GetQ();
				chargeHist[tubeID - startID]->Fill(charge);
			}
		}
	}

	for (int i = 0; i < 20; ++i) {
	    c1->cd(i+1);
	    chargeHist[i]->Draw();
	}

	c1->Update();
}
