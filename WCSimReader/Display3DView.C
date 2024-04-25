#include <algorithm>


const char* filename = "/home/zhw1999/WCSim-build/ROOT5/IWCD/develop/rootfile/wcsim_output.root";

  WCSimRootEvent *wcsimrootevent;
  WCSimRootGeom *wcsimrootgeom;
  WCSimRootTrigger *wcsimroottrigger;



TFile *WCSimFile;

//void EventDisplay();
class HitListManager;
void make_gui();
void load_event();
void geometry();
void histogram();

Bool_t DigitQ;

TEveScene *DevelopmentScene;
TEveViewer *DevelopmentViewer;

TEveElementList* devgeometry;

TEveBrowser *browser;

TGNumberEntry *number;

TGTextEntry *eventidentry;
TGTextEntry *QTText;

Int_t event_id;
Double_t min;
Double_t max;
Bool_t setmin;
Bool_t setmax;
TGNumberEntry *minentry;
TGNumberEntry *maxentry;

Double_t y_limitspace = -10;

TGNumberEntry *evententry;

TH1F *hittime;
TH1F *charge;


TEveRGBAPaletteOverlay *lay;
Bool_t Overlay;


void Display3DView(HitListManager& manager = HitListManager())
{
  /*
  char* wcsimdirenv;
    wcsimdirenv = getenv("WCSIMDIR");
    if(wcsimdirenv !=  NULL){
        gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
    }else{
        gSystem->Load("../libWCSimRoot.so");
    }
  */
  
  WCSimFile = new TFile(filename);
  
  // ***********************************************************************
  
  TTree *wcsimT = WCSimFile->Get("wcsimT");
  wcsimrootevent = new WCSimRootEvent();
  wcsimT->SetBranchAddress("wcsimrootevent",&wcsimrootevent);
  wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE); 
  wcsimT->GetEvent(0);
  Int_t nevent = wcsimT->GetEntries();
  cout << nevent << "events" << endl;
  wcsimroottrigger = new WCSimRootTrigger();
  
  TTree *wcsimGeoT = WCSimFile->Get("wcsimGeoT");
  wcsimrootgeom = new WCSimRootGeom();
  wcsimGeoT->SetBranchAddress("wcsimrootgeom",&wcsimrootgeom);
  wcsimGeoT->GetBranch("wcsimrootgeom")->SetAutoDelete(kTRUE);
  wcsimGeoT->GetEntry(0);
   
  // ***********************************************************************
  
  gSystem->Load("libGeom");

  TEveManager::Create();
  
  geometry();

  DevelopmentScene = gEve->SpawnNewScene("Development Scene");
  DevelopmentViewer = gEve->SpawnNewViewer("Development View");
  DevelopmentScene->AddElement(devgeometry);
  DevelopmentViewer->AddScene(DevelopmentScene);
  DevelopmentViewer->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  DevelopmentViewer->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);

  gEve->GetBrowser()->GetTabRight()->SetTab(1);
  make_gui();

  load_event();
  WCSim_read(manager);

  //histogram();  
  
  browser = gEve->GetBrowser();
  browser->StartEmbedding(TRootBrowser::kRight);

  TCanvas *c1 = new TCanvas("c1","c1");
  c1->Draw();
  //hittime->Draw();
  c1->RedrawAxis();
  gEve->GetBrowser()->StopEmbedding("hittime");

  browser->StartEmbedding(TRootBrowser::kRight);
  TCanvas* c2 = new TCanvas("c2","c2");
  c2->Draw();
  //charge->Draw();
  c2->RedrawAxis();
  gEve->GetBrowser()->StopEmbedding("charge");
  
  gEve->GetBrowser()->SetTabTitle("3D View",TRootBrowser::kRight,0);
  gEve->GetDefaultViewer()->SetName("3D View");
  gEve->GetDefaultViewer()->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);

  gEve->GetBrowser()->GetTabRight()->SetTab(0);
  
  gEve->GetDefaultGLViewer()->UpdateScene();  
  gEve->Redraw3D(kTRUE);
  // Reset camera after the first event has been shown.

}

void load_event()
{

  gEve->GetViewers()->DeleteAnnotations();

  //WCSim_read();

  TEveElement *top = gEve->GetCurrentEvent();

  gEve->Redraw3D(kFALSE, kTRUE);

}

void remove_events()
{  
  TEveEventManager *CurrentEvent = gEve->GetCurrentEvent();
  TEveElement *element = CurrentEvent->FindChild("Cherenkov");
  CurrentEvent->RemoveElement(element);
  TEveElement *develement = DevelopmentScene->FindChild("Cherenkov development view");
  DevelopmentScene->RemoveElement(develement);
}
 

void reload_WCSim_read()
{
  if(Overlay){
    gEve->GetDefaultGLViewer()->RemoveOverlayElement(lay);
    DevelopmentViewer->GetGLViewer()->RemoveOverlayElement(lay);
    WCSim_read();
    gEve->GetDefaultGLViewer()->AddOverlayElement(lay);
    DevelopmentViewer->GetGLViewer()->AddOverlayElement(lay);
  }else{
    WCSim_read();
  }
}

void reload_histogram()
{

  TH1F* old = (TH1F*)gROOT->FindObject(Form("hittime %i",event_id));
  if(old) delete old;
  histogram();
  hittime->Draw();
  
  TH1F* old2 = (TH1F*)gROOT->FindObject(Form("charge %i",event_id));
  if(old2) delete old2;
  histogram();
  charge->Draw();

}

class HitListManager {
private:
    std::vector<double> hitList;
    double defaultValue = 0.0;

public:
    HitListManager() {}

    void AddValue(double value) {
        hitList.push_back(value);
    }

    void SetList(const std::vector<double>& newList) {
        hitList = newList;
    }
    
    void SetListSize(size_t newSize)
    {
        hitList.resize(newSize);
    }
    
    void SetValueAt(int position, double value) {
        if (position >= 1) {
            while (position > hitList.size()) {
                hitList.push_back(defaultValue);
            }
            hitList[position - 1] = value;
        } else {
            // throw std::invalid_argument("Invalid position");
        }
    }

    double GetValueAt(int position) const {
        if (position >= 1 && position <= hitList.size()) {
            return hitList[position - 1];
        } else {
            return defaultValue;
            // throw std::out_of_range("Invalid position");
        }
    }

    const std::vector<double>& GetList() const {
        return hitList;
    }
    
  void PrintHitList()
  {
    std::cout << "Hit List: ";
    for (std::size_t i = 0; i < hitList.size(); ++i)
      {
        std::cout << hitList[i] << " ";
      }
    std::cout << std::endl;
  }
  /*
  void SaveToFile(const std::string& filename) const {
        std::ofstream outFile(filename, std::ios::out | std::ios::binary);
        if (outFile.is_open()) {
            size_t size = hitList.size();
            outFile.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
            outFile.write(reinterpret_cast<const char*>(hitList.data()), size * sizeof(double));
            outFile.close();
            std::cout << "HitList saved to file: " << filename << std::endl;
        } else {
            std::cerr << "Unable to open file for writing: " << filename << std::endl;
        }
    }
  */
    void LoadFromFile(const std::string& filename) {
        std::ifstream inFile(filename, std::ios::in | std::ios::binary);
        if (inFile.is_open()) {
            size_t size;
            inFile.read(reinterpret_cast<char*>(&size), sizeof(size_t));
            hitList.resize(size);
            inFile.read(reinterpret_cast<char*>(hitList.data()), size * sizeof(double));
            inFile.close();
            std::cout << "HitList loaded from file: " << filename << std::endl;
        } else {
            std::cerr << "Unable to open file for reading: " << filename << std::endl;
        }
    }

  void SetFromHistogram(const TH1F* histogram) {
    if (histogram) {
        hitList.clear();  // Clear the existing contents
        int numBins = histogram->GetNbinsX();
        for (int bin = 1; bin <= numBins; ++bin) {
            double binContent = histogram->GetBinContent(bin);
            hitList.push_back(binContent);
        }
    }
  }

double GetMaxValue() const {
    if (!hitList.empty()) {
        double maxVal = hitList[0];  // Assume the first element as the initial maximum
        for (size_t i = 1; i < hitList.size(); ++i) {
            if (hitList[i] > maxVal) {
                maxVal = hitList[i];
            }
        }
        return maxVal;
    } else {
        return defaultValue;
    }
}

  double GetMinNonZeroValue() const {
    if (!hitList.empty()) {
        double minNonZero = defaultValue;
        for (const auto &value : hitList) {
            if (value != 0 && (minNonZero == defaultValue || value < minNonZero)) {
                minNonZero = value;
            }
        }
        return minNonZero;
    } else {
        return defaultValue;
    }
}


  double GetMinValue() const {
    // 移除 hitList 中的0
    std::vector<double> nonZeroList;
    for (auto it = hitList.begin(); it != hitList.end(); ++it) {
        if (*it != 0.0) {
            nonZeroList.push_back(*it);
        }
    }

    if (!nonZeroList.empty()) {
        return *std::min_element(nonZeroList.begin(), nonZeroList.end());
    } else {
        return defaultValue;
    }
}
  
void ReplaceAboveThreshold(double threshold) {
    for (size_t i = 0; i < hitList.size(); ++i) {
        if (hitList[i] >= threshold) {
            hitList[i] = defaultValue;
        }
    }
}

void ReplaceBelowThreshold(double threshold) {
    for (size_t i = 0; i < hitList.size(); ++i) {
        if (hitList[i] <= threshold) {
            hitList[i] = defaultValue;
        }
    }
}

  
  void ClearList() {
    hitList.clear();
  }
};

class EvNavHandler
{
public:
  void Fwd()
  {
    if (event_id < wcsimT->GetEntries()) {
      remove_events();
      ++event_id;
      
      reload_histogram();
      reload_WCSim_read();
      
      gEve->GetDefaultGLViewer()->UpdateScene();
      DevelopmentViewer->GetGLViewer()->UpdateScene();
    }else{
      eventidentry->SetTextColor(kRed);
      eventidentry->SetText("Already at last event");
      cout << "Already at last event.\n" << endl;
    }
  }
  void Bck()
  {
    if (event_id > 0) {
      remove_events();
      --event_id;

      reload_histogram();      
      reload_WCSim_read();
      
      gEve->GetDefaultGLViewer()->UpdateScene();
      DevelopmentViewer->GetGLViewer()->UpdateScene();
    }else{
      eventidentry->SetTextColor(kRed);
      eventidentry->SetText("Already at first event");
      cout << "Already at first event." << endl;
    }
  }
  void EventNumber()
  {
    event_id = evententry->GetNumberEntry()->GetIntNumber();

    if (0 <= event_id && event_id < wcsimT->GetEntries()-1) {
      remove_events();

      reload_histogram();      
      reload_WCSim_read();
      
      gEve->GetDefaultGLViewer()->UpdateScene();
      DevelopmentViewer->GetGLViewer()->UpdateScene();
    }else{
      eventidentry->SetTextColor(kRed);
      Int_t nevent = wcsimT->GetEntries();
      eventidentry->SetText(Form("number of events is %d",nevent));
    }
  }
  void SwitchQT()
  {
    remove_events();
    if(DigitQ){
      DigitQ=kFALSE;
      reload_WCSim_read();
    }else{
      DigitQ=kTRUE;
      reload_WCSim_read();
    }
    gEve->GetDefaultGLViewer()->UpdateScene();
    DevelopmentViewer->GetGLViewer()->UpdateScene();
  }
  void ChangeTimeMin()
  {
    remove_events();
    min = minentry->GetNumberEntry()->GetIntNumber();
    cout << "min:" << min << ",max:" << max << endl;
    if(setmax){
      setmin = kTRUE;
      WCSim_read(kTRUE,kTRUE,min,max);
    }else{
      setmin = kTRUE;
      WCSim_read(kTRUE,kFALSE,min,0);
    }
    gEve->GetDefaultGLViewer()->UpdateScene();
    DevelopmentViewer->GetGLViewer()->UpdateScene();
  }
  void ChangeTimeMax()
  {
    remove_events();
    max = maxentry->GetNumberEntry()->GetIntNumber();
    cout << "min:" << min << ",max:" << max << endl;
    if(setmin){
      setmax = kTRUE;
      WCSim_read(kTRUE,kTRUE,min,max);
    }else{
      setmax = kTRUE;
      WCSim_read(kFALSE,kTRUE,0,max);
    }
    gEve->GetDefaultGLViewer()->UpdateScene();
    DevelopmentViewer->GetGLViewer()->UpdateScene();
  }
  void ResetTime()
  {
    remove_events();
    minentry->SetIntNumber(0);
    maxentry->SetIntNumber(0);
    reload_WCSim_read();
    gEve->GetDefaultGLViewer()->UpdateScene();
    DevelopmentViewer->GetGLViewer()->UpdateScene();
  }
  void DisplayOverlay()
  {
    if(Overlay){
      Overlay = kFALSE;
      gEve->GetDefaultGLViewer()->RemoveOverlayElement(lay);
      DevelopmentViewer->GetGLViewer()->RemoveOverlayElement(lay);
    }else{
      Overlay = kTRUE;
      gEve->GetDefaultGLViewer()->AddOverlayElement(lay);
      DevelopmentViewer->GetGLViewer()->AddOverlayElement(lay);
    }
    gEve->GetDefaultGLViewer()->UpdateScene();
    DevelopmentViewer->GetGLViewer()->UpdateScene();
  }
};


void make_gui()
{
  browser = gEve->GetBrowser();
  browser->StartEmbedding(TRootBrowser::kLeft);
  
  TGMainFrame *frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
  frmMain->SetWindowName("GUI");
  frmMain->SetCleanup(kDeepCleanup);

  TGVerticalFrame *controls = new TGVerticalFrame(frmMain);
  frmMain->AddFrame(controls, new TGLayoutHints(kLHintsExpandX |kLHintsExpandY));

  EvNavHandler *nv = new EvNavHandler;

  // Event Navigation
  TGGroupFrame *event = new TGGroupFrame(controls,"Event Navigation");
  {
    TGHorizontalFrame *arrows = new TGHorizontalFrame(event);

    TString icondir( Form("%s/icons/", gSystem->Getenv("ROOTSYS")) );

    TGPictureButton *bck = new TGPictureButton(arrows, gClient->GetPicture(icondir+"GoBack.gif"));
    bck->Connect("Clicked()", "EvNavHandler", nv, "Bck()");
    arrows->AddFrame(bck, new TGLayoutHints(kLHintsLeft));

    TGPictureButton *fwd = new TGPictureButton(arrows, gClient->GetPicture(icondir+"GoForward.gif"));
    fwd->Connect("Clicked()", "EvNavHandler", nv, "Fwd()");
    arrows->AddFrame(fwd, new TGLayoutHints(kLHintsLeft));

    eventidentry = new TGTextEntry(arrows);
    eventidentry->SetEnabled(kFALSE);
    arrows->AddFrame(eventidentry, new TGLayoutHints(kLHintsRight));
    
    event->AddFrame(arrows,new TGLayoutHints(kLHintsRight));
    
    TGHorizontalFrame *num = new TGHorizontalFrame(event);
    
    TGLabel *numlabel = new TGLabel(num,"event ID");
    num->AddFrame(numlabel,new TGLayoutHints(kLHintsLeft));

    evententry = new TGNumberEntry(num, 0, 9, 999,
				   TGNumberFormat::kNESReal,
				   TGNumberFormat::kNEANonNegative,
				   TGNumberFormat::kNELLimitMinMax,
				   0, 99999);
    (evententry->GetNumberEntry())->Connect("ReturnPressed()", "EvNavHandler", nv, "EventNumber()");
    num->AddFrame(evententry, new TGLayoutHints(kLHintsLeft));
    
    event->AddFrame(num,new TGLayoutHints(kLHintsLeft));
    
  }
  controls->AddFrame(event,new TGLayoutHints(kLHintsTop));

  // displayed color time/charge
  TGGroupFrame *digit = new TGGroupFrame(controls,"Displayed Color Change,Charge/Time");
  DigitQ=kTRUE;
  {
    TGHorizontalFrame *QT = new TGHorizontalFrame(digit);
    
    if(DigitQ){
      TGTextButton *QTButton = new TGTextButton( QT, "Displayed Color Change");
    }else{
      TGTextButton *QTButton = new TGTextButton( QT, "Displayed Color Change");
    }
    QTButton->Connect( "Pressed()", "EvNavHandler", nv, "SwitchQT()");
    QT->AddFrame(QTButton, new TGLayoutHints(kLHintsLeft));

    QTText = new TGTextEntry(QT);
    QTText->SetEnabled(kFALSE);
    QT->AddFrame(QTText, new TGLayoutHints(kLHintsLeft));
    
    digit->AddFrame(QT,new TGLayoutHints(kLHintsLeft));
  }
  controls->AddFrame(digit);
  
  // Set displayed time 
  TGGroupFrame *time = new TGGroupFrame(controls,"Time");
  {
    TGHorizontalFrame *min = new TGHorizontalFrame(time);
    minentry = new TGNumberEntry(min, 0, 9, 999,
				 TGNumberFormat::kNESReal,
				 TGNumberFormat::kNEANonNegative,
				 TGNumberFormat::kNELLimitMinMax,
				 0, 99999);
    (minentry->GetNumberEntry())->Connect("ReturnPressed()", "EvNavHandler", nv, "ChangeTimeMin()");
    min->AddFrame(minentry, new TGLayoutHints(kLHintsRight));

    TGLabel *minlabel = new TGLabel(min,"min");
    min->AddFrame(minlabel,new TGLayoutHints(kLHintsRight));

    time->AddFrame(min,new TGLayoutHints(kLHintsRight));

    TGHorizontalFrame *max = new TGHorizontalFrame(time);
    maxentry = new TGNumberEntry(max, 0, 9, 999,
				 TGNumberFormat::kNESReal,
				 TGNumberFormat::kNEANonNegative,
				 TGNumberFormat::kNELLimitMinMax,
				 0, 99999);
    (maxentry->GetNumberEntry())->Connect("ReturnPressed()", "EvNavHandler", nv, "ChangeTimeMax()");
    max->AddFrame(maxentry, new TGLayoutHints(kLHintsRight));

    TGLabel *maxlabel = new TGLabel(max,"max");
    max->AddFrame(maxlabel,new TGLayoutHints(kLHintsRight));

    time->AddFrame(max,new TGLayoutHints(kLHintsRight));


    TGTextButton *reset = new TGTextButton( time, "Reset");
    reset->Connect( "Pressed()", "EvNavHandler", nv, "ResetTime()");
    time->AddFrame(reset,new TGLayoutHints(kLHintsCenterX));
  }
  controls->AddFrame(time,new TGLayoutHints(kLHintsTop));

  // Overlay color palette
  TGGroupFrame *overlay = new TGGroupFrame(controls, "Pallete Overlay");
  {
    Overlay = kFALSE;
    TGCheckButton *displayoverlay = new TGCheckButton(overlay, "Display Overlay");
    displayoverlay->Connect("Toggled(Bool_t)", "EvNavHandler", nv, "DisplayOverlay()");
    overlay->AddFrame(displayoverlay, new TGLayoutHints(kLHintsLeft));
  }
  controls->AddFrame(overlay,new TGLayoutHints(kLHintsLeft));
  
  frmMain->MapSubwindows();
  frmMain->Resize();
  frmMain->MapWindow();

  browser->StopEmbedding();
  browser->SetTabTitle("Event Control", 0);    

}

/*
void WCSim_read(Bool_t setmin = kFALSE,
		Bool_t setmax = kFALSE,
		Double_t mintime = 0,
		Double_t maxtime = 0,
    HitListManager& manager
		)
   */
void WCSim_read(HitListManager& manager = HitListManager())
{
  
  eventidentry->SetTextColor(0xff0000);
  eventidentry->SetText(Form("Loading event %d...", event_id));
  gSystem->ProcessEvents();
  /*
  const Int_t NRGBs = 3;
const Int_t NCont = 255;

Double_t stops[NRGBs] = { 0.00, 0.50, 1.00 };
Double_t red[NRGBs]   = { 0.00, 1.00, 1.00 };
Double_t green[NRGBs] = { 0.00, 0.00, 1.00 };
Double_t blue[NRGBs]  = { 1.00, 0.00, 0.00 };

TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
gStyle->SetNumberContours(NCont);
  */

  // Blue
  //gStyle->SetPalette(1);

  // Rainbow
  gStyle->SetPalette(55);

  
  Int_t numpmt = wcsimrootgeom->GetWCNumPMT();
  
  //gStyle->SetPalette(1,0);
  //gStyle->SetPalette(1);  

  //TEveRGBAPalette *pal = new TEveRGBAPalette(0,90);
  TEveRGBAPalette *pal = new TEveRGBAPalette(manager.GetMinValue(), manager.GetMaxValue());

   /*
  pal->SetLimits(0,2);
  pal->SetMin(0);
  pal->SetMax(2);
   */
  TEveBoxSet *pmtcone = new TEveBoxSet("Cherenkov");
  pmtcone->SetPalette(pal);
  pmtcone->Reset(TEveBoxSet::kBT_Cone,kFALSE,1024);
  TEveBoxSet *pmtconedev = new TEveBoxSet("Cherenkov development view");
  pmtconedev->SetPalette(pal);
  pmtconedev->Reset(TEveBoxSet::kBT_Cone,kFALSE,1024);

  TEveVector pos,dir;
  TEveVector pos2,dir2;
  //Double_t maxQ = 0;
  //Double_t minT = 1e10;
  //Double_t maxT = -1e10;
  
  wcsimT->GetEntry(event_id);
  //Int_t numevent = wcsimrootevent->GetNumberOfEvents();
  //cout << "Number of triggers:" << numevent << endl;
  cout << "Number of PMTs:" << numpmt << endl;
  
  for (int pmtID = 1; pmtID <= numpmt; ++pmtID)
  {
      WCSimRootPMT pmt = wcsimrootgeom->GetPMT(pmtID - 1);
      
      Double_t pmtx = pmt.GetPosition(0);
      Double_t pmty = pmt.GetPosition(1);
      Double_t pmtz = pmt.GetPosition(2);
      
      Double_t pmtorix = pmt.GetOrientation(0);
      Double_t pmtoriy = pmt.GetOrientation(1);
      Double_t pmtoriz = pmt.GetOrientation(2);
      
      Double_t pmtradius = wcsimrootgeom->GetWCPMTRadius();
      
      Double_t radius = wcsimrootgeom->GetWCCylRadius();
      Double_t length = wcsimrootgeom->GetWCCylLength();
      
      Double_t color = manager.GetValueAt(pmtID);

      if (color <= 0.01){
        continue;
      }

      //   cout << "PMT" << pmtID << " color: " << color << endl;

      if(length/2-y_limitspace <= pmty){
	pos2.Set(pmtx,-pmtz+radius+length/2,0);
	dir2.Set(0,0,-1);
      }else if(pmty <= y_limitspace-length/2){
	pos2.Set(pmtx,pmtz-radius-length/2,0);
	dir2.Set(0,0,-1);
      }else{
	Float_t theta = TMath::ATan2(-pmtz,pmtx)+TMath::Pi()/2.0;
	Float_t pmtx1 = theta*length/2;
	Float_t xshift= (TMath::Pi())*length/2;
	Float_t x = pmtx1;
	if(x>xshift) x=x-(xshift*2);
	pmtx1=x;
	pos2.Set(pmtx1,pmty,0);
	dir2.Set(0,0,-1);
      }
      
      pos.Set(pmtx,pmty,pmtz);
      dir.Set(pmtorix,pmtoriy,pmtoriz);

      pmtcone->AddCone(pos,dir,pmtradius);
      pmtconedev->AddCone(pos2,dir2,pmtradius);

      pmtcone->DigitValue(color);
      pmtconedev->DigitValue(color);

       
  }
   
  pmtcone->RefitPlex();
  gEve->AddElement(pmtcone);
  
  pmtconedev->RefitPlex();
  DevelopmentScene->AddElement(pmtconedev);
    
  // lay = new TEveRGBAPaletteOverlay(TEveRGBAPalette(0,3),0.55,0.05,0.4,0.05);
  //TEveRGBAPalette *pal = new TEveRGBAPalette(0, 100);
  lay = new TEveRGBAPaletteOverlay(pal, 0.55, 0.05, 0.4, 0.05);

  
  gEve->Redraw3D(kFALSE, kTRUE);
  eventidentry->SetTextColor(kBlack);
  eventidentry->SetText(Form("Event %d loaded",event_id));
  
}

void histogram()
{
 
  gStyle->SetOptStat(0);
  
  hittime = new TH1F(Form("hittime %i",event_id)," hittime; hittime [ns];Number of hits",200,800,2000);  
  charge = new TH1F(Form("charge %i",event_id)," charge; Q ;Number of hits",150,0,15);
  
  Int_t numpmt = wcsimrootgeom->GetWCNumPMT();
    
  Int_t nevent = wcsimT->GetEntries();

  wcsimT->GetEntry(event_id);
  Int_t numevent = wcsimrootevent->GetNumberOfEvents();
  
  // loop over all events
  for (Int_t numtrigger=0; numtrigger<numevent; numtrigger++){
   
    wcsimroottrigger = wcsimrootevent->GetTrigger(numtrigger);
    Int_t numdigihit = wcsimroottrigger->GetNcherenkovdigihits();

    // loop in one event
    for (Int_t i=0; i<numdigihit; i++){

      WCSimRootCherenkovDigiHit *hit = (WCSimRootCherenkovDigiHit*)
	(wcsimroottrigger->GetCherenkovDigiHits()->At(i));

      Int_t tubeid = hit->GetTubeId();
      WCSimRootPMT pmt = wcsimrootgeom->GetPMT(tubeid);

      Double_t T = hit->GetT();
      Double_t Q = hit->GetQ();
    
      hittime->Fill(T);
      charge->Fill(Q);
    }
  }
    // end loop in one event
    


    wcsimrootevent->ReInitialize();

    hittime->SetLineColor(kRed);
    hittime->SetLineWidth(4);
     
    hittime->GetYaxis()->SetTitleOffset(1.5);

    charge->SetLineColor(kRed);
    charge->SetLineWidth(4);

    charge->GetYaxis()->SetTitleOffset(1.5);
    
    
}


void geometry()
{

  Double_t radius = wcsimrootgeom->GetWCCylRadius();
  Double_t length = wcsimrootgeom->GetWCCylLength();
  Int_t numpmt = wcsimrootgeom->GetWCNumPMT();
  Double_t pmtradius = wcsimrootgeom->GetWCPMTRadius();
  cout << "cylinder radius:" << length << endl;
  cout << "cylinder length:" << radius << endl;
  cout << "the number of PMT:" << numpmt << endl;
  cout << "PMT radius:" << pmtradius << endl;  
  
  // open geometry manager and define geometry  
  
  new TGeoManager( "cylinder", "cylinder");
  
  // material and medium
  TGeoMaterial *mat = new TGeoMaterial("Vacuum",0,0,0);
  mat->SetTransparency(80);
  TGeoMedium   *med = new TGeoMedium("Vacuum",1,mat);
  
  // set top volume
  TGeoVolume *top = gGeoManager->MakeBox("TOP",med,5000,5000,5000); 
  gGeoManager->SetTopVolume(top);
  
  // Detector geometry
  TGeoShape *shape = new TGeoTube( "tube", 0, length/2, radius);
  TGeoVolume *cyl = new TGeoVolume("cylinder", shape ,med);  
  cyl->SetLineColor(kBlue);
  cyl->SetVisibility(kTRUE);
  TGeoRotation *r1 = new TGeoRotation("r1");
  r1->SetAngles(0,90,0);
  top->AddNode(cyl,1,r1);

  // single PMT shape
  TGeoShape *pmtsh = new TGeoSphere("PMTshape",0,pmtradius,0,180,0,360);
  TGeoVolume *pmtvol = new TGeoVolume("PMTvolume", pmtsh, med);  
  pmtvol->SetVisibility(kTRUE);
  pmtvol->SetLineColor(kYellow-10);
  
  ////////////////// development view geometry //////////////////
  
  TGeoShape *devsphere = new TGeoTube("devsphere", 0, length/2, 0);
  TEveGeoShape *eveshapetop = new TEveGeoShape();
  TEveGeoShape *eveshapebottom = new TEveGeoShape();
  eveshapetop->SetShape(devsphere);
  eveshapetop->SetMainTransparency(90);

  eveshapebottom->SetShape(devsphere);
  eveshapebottom->SetMainTransparency(90);
  
  TGeoShape *devpmt = new TGeoTube("devpmt", 0 , pmtradius, 0);
  
  Double_t cf =(TMath::Pi())*length/2;
  TGeoShape *devbox = new TGeoBBox("devbox", cf, radius, 0);
  TEveGeoShape *eveshapebox = new TEveGeoShape();
  eveshapebox->SetShape(devbox);
  eveshapebox->SetMainTransparency(90);

  TGeoTranslation transtop(0,radius+length/2,0);
  transtop.SetTranslation(transtop);
  eveshapetop->SetTransMatrix(transtop);

  TGeoTranslation transbottom(0,-radius-length/2,0);
  transbottom.SetTranslation(transbottom);
  eveshapebottom->SetTransMatrix(transbottom);

  eveshapetop->SetMainColor(kBlack);
  eveshapebottom->SetMainColor(kBlack);
  eveshapebox->SetMainColor(kBlack);
  
  devgeometry = new TEveElementList("flat geometry");

  devgeometry->AddElement(eveshapetop);
  devgeometry->AddElement(eveshapebottom);
  devgeometry->AddElement(eveshapebox);
  
  TGeoShape *devsphere = new TGeoTube("devsphere", 0, length/2, 0);

  
  // set all PMT
  for (Int_t i=0;i<numpmt;i++){

    //  get PMT position & rotation
    WCSimRootPMT pmt = wcsimrootgeom->GetPMT(i);
    Double_t pmtx = pmt.GetPosition(0);
    Double_t pmty = pmt.GetPosition(1);
    Double_t pmtz = pmt.GetPosition(2);

    Double_t pmtorix = pmt.GetOrientation(0);
    Double_t pmtoriy = pmt.GetOrientation(1);
    Double_t pmtoriz = pmt.GetOrientation(2);


    
    if(length/2-y_limitspace <= pmty){
      TGeoTranslation transtoppmt("top pmt translation",pmtx,pmtz+radius+length/2,0);
      TEveGeoShape *eveshapepmt = new TEveGeoShape();
      eveshapepmt->SetShape(devpmt);
      eveshapepmt->SetTransMatrix(transtoppmt);
      eveshapepmt->SetMainTransparency(90);
      eveshapepmt->SetMainColor(kBlack);
      devgeometry->AddElement(eveshapepmt);

    }else if(pmty <= -length/2+y_limitspace){
      TGeoTranslation transbottompmt("bottom pmt translation",pmtx,-pmtz-radius-length/2,0);
      TEveGeoShape *eveshapepmt = new TEveGeoShape();
      eveshapepmt->SetShape(devpmt);
      eveshapepmt->SetTransMatrix(transbottompmt);
      eveshapepmt->SetMainTransparency(90);
      eveshapepmt->SetMainColor(kBlack);
      devgeometry->AddElement(eveshapepmt);
      
    }else{
      
      Float_t theta = atan2(pmtz,pmtx)+(TMath::Pi()/2.0);
      Float_t pmtx1 = theta*length/2;
      Float_t xshift = (TMath::Pi())*length/2;
      Float_t x = pmtx1;
      if(x>xshift) x=x-(xshift*2);
      pmtx1=x;

      TGeoTranslation transbarrelpmt("barrel pmt translation",pmtx1,pmty,0);
      TEveGeoShape *eveshapepmt = new TEveGeoShape();
      eveshapepmt->SetShape(devpmt);     
      eveshapepmt->SetTransMatrix(transbarrelpmt);
      eveshapepmt->SetMainTransparency(90);
      eveshapepmt->SetMainColor(kBlack);
      devgeometry->AddElement(eveshapepmt);
      
    }

    top->AddNode(pmtvol,i+1,new TGeoTranslation(pmtx,pmty,pmtz));
    
  }

 
  
  // 3D EventDisplay
  TEveGeoTopNode *evegeom = new TEveGeoTopNode(gGeoManager,gGeoManager->GetTopNode());  
  evegeom->SetVisOption(1);
  evegeom->GetNode()->GetVolume()->SetVisibility(kTRUE);
  gEve->AddGlobalElement(evegeom);  
  
  gEve->FullRedraw3D(kTRUE);  

  gGeoManager->SetVisLevel(3);
  gGeoManager->CloseGeometry();

  
}




