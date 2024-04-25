void run_script(const char* fileName = "./read_pmtorien.C")
{
  char* wcsimdirenv;
  wcsimdirenv = getenv("WCSIMDIR");

  if (wcsimdirenv != NULL)
  {
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
  }
  else
  {
    gSystem->Load("../libWCSimRoot.so");
  }

  TString command = TString::Format(".x %s", fileName);
  gROOT->ProcessLine(command);
}
