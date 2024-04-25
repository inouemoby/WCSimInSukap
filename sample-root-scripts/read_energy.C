/*
#include <iostream>
#include <TH1F.h>
#include <stdio.h>
#include <stdlib.h>
#include "TPad.h"
#include "/home/zhw1999/WCSim-build/ROOT5/nuPRISM/develop/include/WCSimRootEvent.hh"
#include "/home/zhw1999/WCSim-build/ROOT5/nuPRISM/develop/include/WCSimRootGeom.hh"
#include "/home/zhw1999/WCSim-build/ROOT5/nuPRISM/develop/include/WCSimRootOptions.hh"
#include "/home/zhw1999/WCSim-build/ROOT5/nuPRISM/develop/include/WCSimRootTools.hh"
*/

void read_energy(const char *filename = "../rootfile/wcsim_output.root")
{
    char *wcsimdirenv;
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
    TTree *tree = (TTree *)file->Get("wcsimT");
    if (!tree)
    {
        // 处理找不到"wcsimT"的情况
        file->Close();
        return;
    }

    // 声明需要读取的变量
    WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
    tree->SetBranchAddress("wcsimrootevent", &wcsimrootsuperevent);

    // 创建TH1F来保存能量分布和动量分布
    TH1F *energyHist = new TH1F("Energy Distribution", "Energy Distribution", 100, 5, 9.5);       // 假设能量范围在0到10之间
    TH1F *momentumHist = new TH1F("Momentum Distribution", "Momentum Distribution", 100, 0, 10); // 假设动量范围在0到10之间

    // 获取总事件数
    int numEvents = tree->GetEntries();

    // 循环读取每个事件的信息
    for (int i = 0; i < numEvents; ++i)
    {
        tree->GetEntry(i);

        // 获取WCSimRootTrigger对象，该对象包含每个事件的track信息
        WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

        // 定义每个事件的能量和动量
        double eventEnergy = 0;
        double eventMomentum = 0;

        // 循环读取每个track的能量和动量，并累加到事件的能量和动量中
        for (int j = 0; j < wcsimrootevent->GetNtrack(); ++j)
        {
            WCSimRootTrack *wcsimroottrack = (WCSimRootTrack *)wcsimrootevent->GetTracks()->At(j);

            // 只处理最初生成的粒子的track (Id = 1)
            if (wcsimroottrack->GetId() == 1)
            {
                // 获取粒子的能量和动量
                double energy = wcsimroottrack->GetE();
                double momentum = wcsimroottrack->GetP();

                // 累加到事件的能量和动量中
                eventEnergy += energy;
                eventMomentum += momentum;
            }
        }
        std::cout << i << " event total energy is " << eventEnergy << std::endl;

        // 将每个事件的能量和动量累积到对应的直方图中
        energyHist->Fill(eventEnergy);
        momentumHist->Fill(eventMomentum);
    }

    energyHist->Print();
    momentumHist->Print();

    // 画出能量分布和动量分布的直方图

    /*
    // 创建两个画布，并在不同的画布上绘制直方图
    TCanvas *c1 = new TCanvas("c1", "Energy Distribution", 800, 600);
    c1->Draw();
    //c1->cd(1);
    energyHist->Draw();

    TCanvas *c2 = new TCanvas("c2", "Momentum Distribution", 800, 600);
    c2->Draw();
    //c2->cd(1);
    momentumHist->Draw();
    */
// Histogram 2
TH1F *energyHist2 = new TH1F("Energy Distribution 2", "Energy Distribution 2", 100, 5, 9.5);
energyHist2->SetLineColor(kRed);
double scaleFactor2 = 65; // Scaling factor for the second histogram
energyHist2->Fill(8.999, scaleFactor2 * 52.7);
energyHist2->Fill(8.534, scaleFactor2 * 25.6);
energyHist2->Fill(8.122, scaleFactor2 * 4.65);
energyHist2->Fill(7.698, scaleFactor2 * 1.28);
energyHist2->Fill(7.266, scaleFactor2 * 0.31);
energyHist2->Fill(7.245, scaleFactor2 * 0.11);
energyHist2->Fill(7.161, scaleFactor2 * 0.07);
energyHist2->Fill(7.059, scaleFactor2 * 0.14);
energyHist2->Fill(6.585, scaleFactor2 * 2.74);
energyHist2->Fill(6.261, scaleFactor2 * 0.06);
energyHist2->Fill(6.140, scaleFactor2 * 0.09);
energyHist2->Fill(6.106, scaleFactor2 * 2.39);
energyHist2->Fill(5.974, scaleFactor2 * 0.87);
energyHist2->Fill(5.818, scaleFactor2 * 3.7);
energyHist2->Fill(5.621, scaleFactor2 * 0.16);
energyHist2->Fill(5.528, scaleFactor2 * 0.06);
energyHist2->Fill(5.459, scaleFactor2 * 0.09);
energyHist2->Fill(5.437, scaleFactor2 * 0.72);
energyHist2->Fill(5.313, scaleFactor2 * 1.74);

// Histogram 3
TH1F *energyHist3 = new TH1F("Energy Distribution 3", "Energy Distribution 3", 100, 5, 9.5);
energyHist3->SetLineColor(kGreen);
double scaleFactor3 = 100; // Scaling factor for the third histogram
energyHist3->Fill(8.99, scaleFactor3 * 26);
energyHist3->Fill(8.53, scaleFactor3 * 19);
energyHist3->Fill(8.15, scaleFactor3 * 11);
energyHist3->Fill(7.65, scaleFactor3 * 9);
energyHist3->Fill(7.24, scaleFactor3 * 8);
energyHist3->Fill(6.57, scaleFactor3 * 7);
energyHist3->Fill(6.3, scaleFactor3 * 6);
energyHist3->Fill(6.08, scaleFactor3 * 5);
energyHist3->Fill(5.97, scaleFactor3 * 5);
energyHist3->Fill(5.81, scaleFactor3 * 15);
energyHist3->Fill(5.65, scaleFactor3 * 6);

    TCanvas *c1 = new TCanvas("c1", "Energy and Momentum Distribution", 1200, 600);
    c1->Divide(2, 1); // 将画布分成1行2列

    c1->cd(1); // 切换到第一个子画布
    //energyHist->Draw();
    //energyHist2->Draw("same");
    energyHist3->Draw("same");

    c1->cd(2); // 切换到第二个子画布
    momentumHist->Draw();

    c1->Update();
    // gPad->Modified();
    //  gPad->Update();

    // 关闭文件
    // file->Close();

    // Create a legend and add entries for each histogram
    c1->cd(1);
    
    TLegend* legend = new TLegend(0.1,0.7,0.4,0.9);
legend->AddEntry(energyHist,"Simulation","l"); // Add an entry for the first histogram
legend->AddEntry(energyHist2,"doi.org/10.1007/BF01408184","l"); // Add an entry for the second histogram
legend->AddEntry(energyHist3,"doi.org/10.1103/PhysRev.121.1734","l"); // Add an entry for the third histogram
legend->Draw(); // Draw the legend on the canvas
}
