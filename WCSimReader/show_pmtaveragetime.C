#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include <map>
#include <utility>

#include "Display3DView.C"

void show_pmtaveragetime(const char* filename = "../rootfile/wcsim_output.root")
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
    tree->SetBranchAddress("wcsimrootevent", &wcsimrootevent);

    // 获取总事件数
    int numEvents = tree->GetEntries();

    
    TTree *geotree = (TTree*)file->Get("wcsimGeoT");
    geotree->SetBranchAddress("wcsimrootgeom", &wcsimrootgeom);
    geotree->GetEntry(0);

    int numPMTs = wcsimrootgeom->GetWCNumPMT();

    // Create a map to store total time and hit count for each PMT ID
std::map<int, std::pair<double, int>> timeMap;

// Loop over events
for (int i = 0; i < numEvents; i = i+100)
{
    tree->GetEntry(i);

    wcsimroottrigger = wcsimrootevent->GetTrigger(0);
    TClonesArray *hitArray = wcsimroottrigger->GetCherenkovDigiHits();

    // Loop over hits
    for (int j = 0; j < hitArray->GetEntries(); ++j)
    {
        WCSimRootCherenkovDigiHit *hit = (WCSimRootCherenkovDigiHit*)hitArray->At(j);
        int tubeID = hit->GetTubeId();
        double time = hit->GetT();

        // Update total time and hit count for this PMT ID
        //if (timeMap.count(tubeID) == 0) {
            // This PMT ID is not in the map yet, so add it
	//timeMap.insert(std::make_pair(tubeID, std::make_pair(time, 1)));
        //} else {
            // This PMT ID is already in the map, so update it
            timeMap[tubeID].first += time;
            timeMap[tubeID].second += 1;
	    //}
    }
}

// Create a histogram to store the average time for each PMT ID
// Assume numPMTs is the total number of PMTs
TH1F *averageTimeHist = new TH1F("Average Time", "Average Time per PMT", numPMTs, 0, numPMTs);

// Calculate average time for each PMT ID and fill the histogram

double totalTime;
int count;
double averageTime;

for (int tubeID = 1; tubeID <= numPMTs; ++tubeID) {
    // 检查该 tubeID 是否存在于 timeMap 中
  std::pair<double, int>& data = timeMap[tubeID];
        // 存在，提取数据并进行操作
        totalTime = data.first;
	count = data.second;
	//cout << count << " in ID " << tubeID << " total time " << totalTime << endl;

        if (count != 0) {
            averageTime = totalTime / count;
            averageTimeHist->SetBinContent(tubeID, averageTime);

        }
 }

 HitListManager manager;
 manager.SetFromHistogram(averageTimeHist);
 manager.ClearBelowThreshold(950);

 Display3DView(manager);

// Draw the histogram
TCanvas *c1 = new TCanvas("c1", "Average Time per PMT", 800, 600);
averageTimeHist->Draw();
}
