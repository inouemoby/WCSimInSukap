#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TSystem.h"


void read_pmtposition_hit(const char* filename = "../rootfile/wcsim_output.root")
{
    char* wcsimdirenv;
    wcsimdirenv = getenv("WCSIMDIR");
    if (wcsimdirenv != NULL)
    {
        gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
    }
    else
    {
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

    // 获取WCSimRootGeom对象
    TTree *geotree = (TTree*)file->Get("wcsimGeoT");
    WCSimRootGeom *geo = 0;
    geotree->SetBranchAddress("wcsimrootgeom", &geo);
    geotree->GetEntry(0);

    // 创建TH1F来保存每个轴的PMT hit总数
    TH1F *xHist = new TH1F("PMT Counts (X)", "PMT Counts (X)", 100, -400, 400);
    TH1F *yHist = new TH1F("PMT Counts (Y)", "PMT Counts (Y)", 100, -400, 400);
    TH1F *zHist = new TH1F("PMT Counts (Z)", "PMT Counts (Z)", 100, -400, 400);

    // 获取总事件数
    int numEvents = tree->GetEntries();

    // 循环读取每个事件的信息
    for (int i = 0; i < numEvents; ++i)
    {
        tree->GetEntry(i);

	// 获取WCSimRootTrigger对象，该对象包含每个事件的track信息
	WCSimRootTrigger* wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

	
        // 获取 WCSimRootCherenkovHit 列表
        TClonesArray *hitArray = wcsimrootevent->GetCherenkovHits();

        // 遍历每个 hit，统计不同轴上的 PMT hit 数量
        for (int j = 0; j < hitArray->GetEntries(); ++j)
        {
            WCSimRootCherenkovHit *hit = (WCSimRootCherenkovHit*)hitArray->At(j);
            int tubeID = hit->GetTubeID();
            WCSimRootPMT pmtInfo = geo->GetPMT(tubeID);

            double x = pmtInfo.GetPosition(0);
            double y = pmtInfo.GetPosition(1);
            double z = pmtInfo.GetPosition(2);

            xHist->Fill(x);
            yHist->Fill(y);
            zHist->Fill(z);
        }
    }

    // 画出三幅直方图
    TCanvas *c1 = new TCanvas("c1", "PMT Counts", 1200, 600);
    c1->Divide(3, 1); // 将画布分成1行3列

    c1->cd(1); // 切换到第一个子画布
    xHist->Draw();

    c1->cd(2); // 切换到第二个子画布
    yHist->Draw();

    c1->cd(3); // 切换到第三个子画布
    zHist->Draw();

    c1->Update();

}
