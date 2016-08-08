//
//  apv_raw_ped.h
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//

#ifndef apv_raw_ped_h
#define apv_raw_ped_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>

// Fixed size dimensions of array or collections stored in the TTree if any.

class apv_raw_ped {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   UInt_t          evt;
   Int_t           daqSec;
   Int_t           daqMicroSec;
   Int_t           srsTimeStamp;
   std::vector<unsigned int> *srsFec;
   std::vector<unsigned int> *srsChip;
   std::vector<unsigned int> *srsChan;
   std::vector<std::string>  *mmChamber;
   std::vector<int>     *mmLayer;
   std::vector<char>    *mmReadout;
   std::vector<int>     *mmStrip;
   std::vector<double>  *ped_mean;
   std::vector<double>  *ped_stdev;
   std::vector<double>  *ped_sigma;

   // List of branches
   TBranch        *b_evt;   //!
   TBranch        *b_daqSec;   //!
   TBranch        *b_daqMicroSec;   //!
   TBranch        *b_srsTimeStamp;   //!
   TBranch        *b_srsFec;   //!
   TBranch        *b_srsChip;   //!
   TBranch        *b_srsChan;   //!
   TBranch        *b_mmChamber;   //!
   TBranch        *b_mmLayer;   //!
   TBranch        *b_mmReadout;   //!
   TBranch        *b_mmStrip;   //!
   TBranch        *b_ped_mean;   //!
   TBranch        *b_ped_stdev;   //!
   TBranch        *b_ped_sigma;   //!

   apv_raw_ped(TTree *tree=0);
   virtual ~apv_raw_ped();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef apv_raw_ped_cxx
apv_raw_ped::apv_raw_ped(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("__pedestals.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("__pedestals.root");
      }
      f->GetObject("apv_raw_ped",tree);

   }
   Init(tree);
}

apv_raw_ped::~apv_raw_ped()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t apv_raw_ped::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t apv_raw_ped::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void apv_raw_ped::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   srsFec = 0;
   srsChip = 0;
   srsChan = 0;
   mmChamber = 0;
   mmLayer = 0;
   mmReadout = 0;
   mmStrip = 0;
   ped_mean = 0;
   ped_stdev = 0;
   ped_sigma = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("evt", &evt, &b_evt);
   fChain->SetBranchAddress("daqSec", &daqSec, &b_daqSec);
   fChain->SetBranchAddress("daqMicroSec", &daqMicroSec, &b_daqMicroSec);
   fChain->SetBranchAddress("srsTimeStamp", &srsTimeStamp, &b_srsTimeStamp);
   fChain->SetBranchAddress("srsFec", &srsFec, &b_srsFec);
   fChain->SetBranchAddress("srsChip", &srsChip, &b_srsChip);
   fChain->SetBranchAddress("srsChan", &srsChan, &b_srsChan);
   fChain->SetBranchAddress("mmChamber", &mmChamber, &b_mmChamber);
   fChain->SetBranchAddress("mmLayer", &mmLayer, &b_mmLayer);
   fChain->SetBranchAddress("mmReadout", &mmReadout, &b_mmReadout);
   fChain->SetBranchAddress("mmStrip", &mmStrip, &b_mmStrip);
   fChain->SetBranchAddress("ped_mean", &ped_mean, &b_ped_mean);
   fChain->SetBranchAddress("ped_stdev", &ped_stdev, &b_ped_stdev);
   fChain->SetBranchAddress("ped_sigma", &ped_sigma, &b_ped_sigma);
   Notify();
}

Bool_t apv_raw_ped::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void apv_raw_ped::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t apv_raw_ped::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef apv_raw_ped_cxx
