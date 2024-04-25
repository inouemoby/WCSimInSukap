void tbrowser(){
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

    // 打开 WCSim 输出文件
    TFile *file = new TFile("../rootfile/wcsim_output.root");

    // 创建一个 TBrowser 对象
    TBrowser *browser = new TBrowser();

    // 在 TBrowser 中浏览 WCSim 输出文件
    file->Browse(browser);

}
