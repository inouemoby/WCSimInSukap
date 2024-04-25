//const char* filename = "../rootfile/wcsim_output.root";

//TFile *file;

void rootlogon()
{

  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIMDIR");

  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
  }else{
    gSystem->Load("../libWCSimRoot.so");
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


