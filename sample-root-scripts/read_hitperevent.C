#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TSystem.h"

void read_hitperevent(const char* filename = "../rootfile/wcsim_output.root")
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

    // 声明需要读取的变量
    WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
    tree->SetBranchAddress("wcsimrootevent", &wcsimrootsuperevent);

	// 创建一个直方图来保存每个事件的hit数
	TH1F *hitHist = new TH1F("Event Hits", "Number of Hits per Event", 80, 0, 1040);

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

		// 统计每个事件的hit数，并填充到直方图中
		hitHist->Fill(hitArray->GetEntries());
	}

	TCanvas *c1 = new TCanvas("c1", "Number of Hits per Event", 800, 600);
	hitHist->Draw();
	c1->Update();
}
