
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


}


