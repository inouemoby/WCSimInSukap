//const char* filename = "../rootfile/wcsim_output.root";

//TFile *file;

void rootlogon()
{

  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIM_BUILD_DIR");

  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIM_BUILD_DIR}/src/libWCSimRoot.so");
    gSystem->Load("${WCSIM_BUILD_DIR}/src/libWCSimCore.so");
  }else{
    gSystem->Load("../src/libWCSimRoot.so");
    gSystem->Load("../src/libWCSimCore.so");
  }

  /*const char* filename = "../rootfile/wcsim_output.root";
  */

  /*
  file = TFile::Open(filename);
  if (!file || file->IsZombie())
    {
      cont << "Can not open the rootfile!" << endl;
      return;
    }
  */

  /*
  TTree *geotree = (TTree*)file->Get("wcsimGeoT"); 
  
  TTree *tree = (TTree*)file->Get("wcsimT");
  if (!tree)
    {
      file->Close();
      return;
    }
  */

  //file->Close();
}


