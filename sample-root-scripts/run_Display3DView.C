void run_Display3DView()
{
  
  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIMDIR");

  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
  }else{
    gSystem->Load("../libWCSimRoot.so");
  }
     
  gROOT->ProcessLine(".x Display3DView.C");
  
}
