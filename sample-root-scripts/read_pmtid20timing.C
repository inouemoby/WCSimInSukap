#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TSystem.h"

void read_pmtid20timing(const char* filename = "../rootfile/wcsim_output.root")
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
	TCanvas *c1 = new TCanvas("c1", "PMT Timing Distributions", 1600, 900);
	c1->Divide(5, 4);

	// 创建20个直方图
	TH1F *timingHist[20];
	for (int i = 0; i < 20; ++i) {
		c1->cd(i+1);
		char histName[10];
		sprintf(histName, "h%d", i+1);
		char histTitle[50];
		sprintf(histTitle, "PMT %d Timing Distribution", startID+i);
		timingHist[i] = new TH1F(histName, histTitle, 100, 920, 1100);
		timingHist[i]->GetXaxis()->SetTitle("Hit timing [ns]");
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
		
		// 遍历每个 hit，统计指定tubeID 的timing分布
		for (int j = 0; j < hitArray->GetEntries(); ++j)
		{

			WCSimRootCherenkovDigiHit *hit = (WCSimRootCherenkovDigiHit*)hitArray->At(j);
            int tubeID = hit->GetTubeId();

			if (tubeID >= startID && tubeID < startID + 20) {
				double time = hit->GetT();
				timingHist[tubeID - startID]->Fill(time);
			}
		}
	}

	for (int i = 0; i < 20; ++i) {
	    c1->cd(i+1);
	    timingHist[i]->Draw();
	}

	c1->Update();

	WCSimRootGeom *geo = 0;
	TTree *geotree = (TTree*)file->Get("wcsimGeoT");
        geotree->SetBranchAddress("wcsimrootgeom", &geo);
        geotree->GetEntry(0);
	for (int i = 0; i < 20; ++i) {
	  WCSimRootPMT pmtInfo = geo->GetPMT(startID + i - 1);
	    double x = pmtInfo.GetPosition(0);
	    double y = pmtInfo.GetPosition(1);
	    double z = pmtInfo.GetPosition(2);
	    cout << "PMT ID " << startID + i << " located at: (" << x << ", " << y << ", " << z << ")" << endl;
	}
}
