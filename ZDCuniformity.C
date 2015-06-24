{
//////////////////////////////////////////////////////////
//   This file has been automatically generated 
//     (Wed Jun 24 13:43:06 2015 by ROOT version5.34/23)
//   from TTree acc/counts tree from acc.dat
//   found on file: counts_bd1.root
//////////////////////////////////////////////////////////


//Reset ROOT and connect tree file
   gROOT->Reset();
   TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("counts_bd1.root");
   if (!f) {
      f = new TFile("counts_bd1.root");
   }
   TTree * acc = (TTree*) f->Get("acc");

//Declaration of leaves types
   Int_t           i;
   Int_t           runnum;
   Int_t           fi;
   Int_t           fill;
   Double_t        t;
   Double_t        freq;
   Int_t           bx;
   Int_t           wh[8];
   Int_t           wv[8];
   Int_t           wtsum[8];
   Int_t           wf;
   Int_t           wb;
   Int_t           wtac;
   Int_t           eh[8];
   Int_t           ev[8];
   Int_t           etsum[8];
   Int_t           ef;
   Int_t           eb;
   Int_t           etac;
   Double_t        tot_bx;
   Int_t           blue;
   Int_t           yell;
   Int_t           pattern;
   Int_t           kicked;

   // Set branch addresses.
   char wh_n[8][16];
   char wv_n[8][16];
   char wtsum_n[8][16];
   char eh_n[8][16];
   char ev_n[8][16];
   char etsum_n[8][16];
   for(Int_t x=0; x<8; x++)
   {
     sprintf(wh_n[x],"wh%d",x);
     sprintf(wv_n[x],"wv%d",x);
     sprintf(wtsum_n[x],"wtsum%d",x);
     sprintf(eh_n[x],"eh%d",x);
     sprintf(ev_n[x],"ev%d",x);
     sprintf(etsum_n[x],"etsum%d",x);

     acc->SetBranchAddress(wh_n[x],&(wh[x]));
     acc->SetBranchAddress(wv_n[x],&(wv[x]));
     acc->SetBranchAddress(wtsum_n[x],&(wtsum[x]));
     acc->SetBranchAddress(eh_n[x],&(eh[x]));
     acc->SetBranchAddress(ev_n[x],&(ev[x]));
     acc->SetBranchAddress(etsum_n[x],&(etsum[x]));
   };
   acc->SetBranchAddress("i",&i);
   acc->SetBranchAddress("runnum",&runnum);
   acc->SetBranchAddress("fi",&fi);
   acc->SetBranchAddress("fill",&fill);
   acc->SetBranchAddress("t",&t);
   acc->SetBranchAddress("freq",&freq);
   acc->SetBranchAddress("bx",&bx);
   acc->SetBranchAddress("wf",&wf);
   acc->SetBranchAddress("wb",&wb);
   acc->SetBranchAddress("wtac",&wtac);
   acc->SetBranchAddress("ef",&ef);
   acc->SetBranchAddress("eb",&eb);
   acc->SetBranchAddress("etac",&etac);
   acc->SetBranchAddress("tot_bx",&tot_bx);
   acc->SetBranchAddress("blue",&blue);
   acc->SetBranchAddress("yell",&yell);
   acc->SetBranchAddress("pattern",&pattern);
   acc->SetBranchAddress("kicked",&kicked);

   Long64_t nentries = acc->GetEntries();

   Long64_t nbytes = 0;
   TH2D * Wtot2D = new TH2D("Wtot2D","Total West ZDC-SMD hits",7,0,7,8,0,8);
   TH2D * Etot2D = new TH2D("Etot2D","Total East ZDC-SMD hits",7,0,7,8,0,8);
   for(Int_t x=0; x<nentries;x++)
   {
     acc->GetEntry(x);
     if(kicked==0 && blue*yell!=0)
     {
       for(Int_t xx=0; xx<7; xx++)
       {
         for(Int_t yy=0; yy<8; yy++)
         {
           Wtot2D->Fill(xx,yy,wv[xx]+wh[yy]);
           Etot2D->Fill(xx,yy,ev[xx]+eh[yy]);
         };
       };
     };
   };

   
   TFile * outfile = new TFile("unif.root","RECREATE");
   TCanvas * canv = new TCanvas("canv","canv",900,450);
   canv->Divide(2,1);
   gStyle->SetOptStat(0);
   canv->cd(1); Wtot2D->Draw("colz");
   canv->cd(2); Etot2D->Draw("colz");
   canv->Write("canv");
};
