// builds scaler tree from acc file
// -- this also allows for empty bunches documented in "pathologies.dat" to be manually omitted
//    from relative luminosity computation ("CLEAN UP PROCEDURE")

void mk_tree(const char * acc_file="datfiles/acc.dat",Int_t board)
{
  // read acc file into tree
  char outfile_n[64];
  sprintf(outfile_n,"counts_bd%d.root",board);
  TFile * outfile = new TFile(outfile_n,"RECREATE");
  TTree * acc = new TTree("acc","counts tree from acc.dat");
  char cols[2048];
  char EW[2][16];

  if(board==1)
  {
    strcpy(EW[0],"w");
    strcpy(EW[1],"e");
  }
  else if(board==5)
  {
    strcpy(EW[0],"e");
    strcpy(EW[1],"w");
  }
  else 
  {
    fprintf(stderr,"ERROR: board != 1 or 5\n");
    return;
  };

  // board 1
  char ZDCcols[2][512];

  // board 5
  char bbc_cols[512];
  char zdc_cols[512];
  char vpd_cols[512];
  char bbc_map[4][16];
  char vpd_map[4][16];
  char zdc_map[2][16];

  strcpy(bbc_cols,"");
  strcpy(vpd_cols,"");
  strcpy(zdc_cols,"");
  
  strcpy(bbc_map[0],"9");
  strcpy(bbc_map[1],"11");
  strcpy(bbc_map[2],"14");
  strcpy(bbc_map[3],"16");

  strcpy(vpd_map[0],"1");
  strcpy(vpd_map[1],"4");
  strcpy(vpd_map[2],"7");
  strcpy(vpd_map[3],"10");

  strcpy(zdc_map[0],"f");
  strcpy(zdc_map[1],"b");

  
  if(board==1)
  {
    for(Int_t j=0; j<2; j++)
    {
      strcpy(ZDCcols[j],"");
      for(Int_t k=0; k<8; k++) sprintf(ZDCcols[j],"%s:%sh%d",ZDCcols[j],EW[j],k);
      for(Int_t k=0; k<8; k++) sprintf(ZDCcols[j],"%s:%sv%d",ZDCcols[j],EW[j],k);
      for(Int_t k=0; k<8; k++) sprintf(ZDCcols[j],"%s:%stsum%d",ZDCcols[j],EW[j],k);
      sprintf(ZDCcols[j],"%s:%sf",ZDCcols[j],EW[j]);
      sprintf(ZDCcols[j],"%s:%sb",ZDCcols[j],EW[j]);
      sprintf(ZDCcols[j],"%s:%stac",ZDCcols[j],EW[j]);
    };

    sprintf(cols,"i/I:runnum/I:fi/I:fill/I:t/D:freq/D:bx/I%s%s:tot_bx/D:blue/I:yell/I:pattern/I:kicked/I",ZDCcols[0],ZDCcols[1]);
  }

  else if(board==5)
  {
    for(Int_t j=0; j<2; j++)
    {
      for(Int_t k=0; k<4; k++)
      {
        sprintf(bbc_cols,"%s:b%s%s/D",bbc_cols,EW[j],bbc_map[k]);
        sprintf(vpd_cols,"%s:v%s%s/D",vpd_cols,EW[j],vpd_map[k]);
        if(k<2) sprintf(zdc_cols,"%s:z%s%s/D",zdc_cols,EW[j],zdc_map[k]);
      };
    };

    sprintf(cols,"i/I:runnum/I:fi/I:fill/I:t/D:freq/D:bx/I%s%s%s:BBCcoin/D:tot_bx/D:blue/I:yell/I:sp/I",bbc_cols,vpd_cols,zdc_cols);
  };


  printf("%s\n",cols);
  acc->ReadFile(acc_file,cols);

  acc->Write("acc");

  return;

  // set branch addresses to read through acc tree
  Int_t index,runnum,fill_index,fill,bx;
  Double_t bbc[8];
  Double_t zdc[8];
  Double_t vpd[8];
  Double_t time,freq;
  Double_t tot_bx;
  Int_t blue,yell;
  acc->SetBranchAddress("i",&index);
  acc->SetBranchAddress("runnum",&runnum);
  acc->SetBranchAddress("fi",&fill_index);
  acc->SetBranchAddress("fill",&fill);
  acc->SetBranchAddress("t",&time);
  acc->SetBranchAddress("freq",&freq);
  acc->SetBranchAddress("bx",&bx);
  char str[16];
  for(Int_t i=0; i<8; i++) { sprintf(str,"bbc_%d",i); acc->SetBranchAddress(str,&bbc[i]); };
  for(Int_t i=0; i<8; i++) { sprintf(str,"zdc_%d",i); acc->SetBranchAddress(str,&zdc[i]); };
  for(Int_t i=0; i<8; i++) { sprintf(str,"vpd_%d",i); acc->SetBranchAddress(str,&vpd[i]); };
  acc->SetBranchAddress("tot_bx",&tot_bx);
  acc->SetBranchAddress("blue",&blue);
  acc->SetBranchAddress("yell",&yell);
  

  // restructure tree into one suitable for analysis
  TTree * sca = new TTree("sca","restructured tree");
  Double_t bbce,bbcw,bbcx; // e=east, w=west, x=coincidence
  Double_t zdce,zdcw,zdcx;
  Double_t vpde,vpdw,vpdx;
  Bool_t kicked;
  sca->Branch("i",&index,"i/I");
  sca->Branch("runnum",&runnum,"runnum/I");
  sca->Branch("fi",&fill_index,"fi/I");
  sca->Branch("fill",&fill,"fill/I");
  sca->Branch("t",&time,"t/D");
  sca->Branch("freq",&freq,"freq/D");
  sca->Branch("bx",&bx,"bx/I");
  sca->Branch("bbce",&bbce,"bbce/D");
  sca->Branch("bbcw",&bbcw,"bbcw/D");
  sca->Branch("bbcx",&bbcx,"bbcx/D");
  sca->Branch("zdce",&zdce,"zdce/D");
  sca->Branch("zdcw",&zdcw,"zdcw/D");
  sca->Branch("zdcx",&zdcx,"zdcx/D");
  sca->Branch("vpde",&vpde,"vpde/D");
  sca->Branch("vpdw",&vpdw,"vpdw/D");
  sca->Branch("vpdx",&vpdx,"vpdx/D");
  sca->Branch("tot_bx",&tot_bx,"tot_bx/D");
  sca->Branch("blue",&blue,"blue/I");
  sca->Branch("yell",&yell,"yell/I");
  sca->Branch("kicked",&kicked,"kicked/O");


  // read kicked bunches tree from "kicked" file
  TTree * kicked_tr = new TTree();
  kicked_tr->ReadFile("kicked","fill/I:bx/I:spinbit/I");
  Int_t kicked_fill,kicked_bx,kicked_spinbit;
  kicked_tr->SetBranchAddress("fill",&kicked_fill);
  kicked_tr->SetBranchAddress("bx",&kicked_bx);
  kicked_tr->SetBranchAddress("spinbit",&kicked_spinbit);
  
  for(Int_t q=0; q<acc->GetEntries(); q++)
  {
    acc->GetEntry(q);

    // -- see bit_doc.txt for bit details
    bbce = bbc[1] + bbc[3] + bbc[5] + bbc[7]; // e + we + xe + xwe
    bbcw = bbc[2] + bbc[3] + bbc[6] + bbc[7]; // w + we + xw + xwe
    bbcx = bbc[3] + bbc[7];                   // we + xwe

    zdce = zdc[1] + zdc[3] + zdc[5] + zdc[7]; // e + we + xe + xwe
    zdcw = zdc[2] + zdc[3] + zdc[6] + zdc[7]; // w + we + xw + xwe
    zdcx = zdc[3] + zdc[7];                   // we + xwe
    
    vpde = vpd[1] + vpd[3] + vpd[5] + vpd[7]; // e + we + xe + xwe
    vpdw = vpd[2] + vpd[3] + vpd[6] + vpd[7]; // w + we + xw + xwe
    vpdx = vpd[3] + vpd[7];                   // we + xwe


    // KICKED BUNCHES
    kicked=false;

    for(Int_t kk=0; kk<kicked_tr->GetEntries(); kk++)
    {
      kicked_tr->GetEntry(kk);
      if(fill==kicked_fill && bx==kicked_bx) kicked=true;
    };
    
    sca->Fill(); // new kick method
  };

  acc->Write("acc");
  sca->Write("sca");
  printf("%s written\n",outfile_n);
};
