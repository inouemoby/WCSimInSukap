#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TSystem.h"

void read_pmtidtiming(const char* filename = "../rootfile/wcsim_output.root")
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

	// 提示用户输入一个指定的tubeID
	int tubeIDInput;
	cout << "Please Input a TubeID: ";
	cin >> tubeIDInput;

	// 创建一个直方图来保存指定tubeID的timing分布
	char histTitle[50];
	sprintf(histTitle, "PMT %d Timing Distribution", tubeIDInput);
	TH1F *timingHist = new TH1F("PMT Timing", histTitle, 100, 900, 1100);
	timingHist->GetXaxis()->SetTitle("Hit timing [ns]");
					 
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

			if (tubeID == tubeIDInput) {
				double time = hit->GetT();
				timingHist->Fill(time);
			}
		}
	}

	char canvasTitle[50];
	sprintf(canvasTitle, "PMT %d Timing Distribution", tubeIDInput);
	TCanvas *c1 = new TCanvas("c1", canvasTitle, 800, 600);
	timingHist->Draw();
	c1->Update();

	WCSimRootGeom *geo = 0;
	TTree *geotree = (TTree*)file->Get("wcsimGeoT");
	geotree->SetBranchAddress("wcsimrootgeom", &geo);
	geotree->GetEntry(0);

	WCSimRootPMT pmtInfo = geo->GetPMT(tubeIDInput - 1);

	double x = pmtInfo.GetPosition(0);
	double y = pmtInfo.GetPosition(1);
	double z = pmtInfo.GetPosition(2);

	cout << "PMT ID " << tubeIDInput << " located at: (" << x << ", " << y << ", " << z << ")" << endl;
}
