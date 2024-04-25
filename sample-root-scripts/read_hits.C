void read_hits() {
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
    // Open the WCSim output file
    TFile *file = new TFile("../rootfile/wcsim_output.root", "READ");
    // Get the WCSim tree
    TTree *tree = (TTree*)file->Get("wcsimT");
    // Create a histogram to store the number of hits per event
    TH1F *hist = new TH1F("hist", "Number of Hits per Event", 100, 0, 150);
    // Get the number of entries in the tree
    int nentries = tree->GetEntries();
    
    // Get the WCSimRootEvent object
    WCSimRootEvent *wcsimrootevent = 0;
    tree->SetBranchAddress("wcsimrootevent", &wcsimrootevent);
    
    // Loop over the entries in the tree
    for (int i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        // Get the WCSimRootTrigger object for this event
        WCSimRootTrigger *wcsimroottrigger = wcsimrootevent->GetTrigger(0);
        // Get the number of hits for this event
        int nhits = wcsimroottrigger->GetNcherenkovhits();
        // Fill the histogram with the number of hits
        hist->Fill(nhits);
    }


    
    // Draw the histogram
    hist->Draw();
}
