// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT
/*

Compile me with:
g++ `root-config --cflags --libs` -L $ROOTSYS/lib -lRooFit -lHtml -lMinuit c.cpp -o whatever


  NOTE: This macro is kept for back compatibility only.
  Use instead the executable $ROOTSYS/bin/hadd
  
  This macro will add histograms from a list of root files and write them
  to a target root file. The target file is newly created and must not be
  identical to one of the source files.

  Author: Sven A. Schmidt, sven.schmidt@cern.ch
  Date:   13.2.2001

  This code is based on the hadd.C example by Rene Brun and Dirk Geppert,
  which had a problem with directories more than one level deep.
  (see macro hadd_old.C for this previous implementation).
  
  The macro from Sven has been enhanced by 
     Anne-Sylvie Nicollerat <Anne-Sylvie.Nicollerat@cern.ch>
   to automatically add Trees (via a chain of trees).
  
  To use this macro, modify the file names in function hadd.
  
  NB: This macro is provided as a tutorial.
      Use $ROOTSYS/bin/hadd to merge many histogram files

 */


#include <string.h>
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"

#include "RooPlot.h"
#include "RooPlotable.h"
#include "RooDataSet.h"

TList *FileList;
TFile *Target;

void MergeRootfile( TDirectory *target, TList *sourcelist );


int main(int argc, char** argv) {

  if(argc < 3){
    cout << "please provide at least one output and one input file"
	 << endl;
    exit(0);
  }

  Target = TFile::Open( argv[1], "RECREATE" );
  cout << " output file: " << argv[1] << endl;

  FileList = new TList();

  for(int i=2; i<argc; i++){
    cout << " adding " << argv[i] << " to list" << endl;
    FileList->Add( TFile::Open(argv[i]) );
  }

  cout << "made list, now merging. " << endl;

  MergeRootfile( Target, FileList );
  Target->Close();

  return 0;

}   

void MergeRootfile( TDirectory *target, TList *sourcelist ) {

  //  cout << "Target path: " << target->GetPath() << endl;
  TString path( (char*)strstr( target->GetPath(), ":" ) );
  path.Remove( 0, 2 );

  TFile *first_source = (TFile*)sourcelist->First();
  first_source->cd( path );
  TDirectory *current_sourcedir = gDirectory;
  //gain time, do not add the objects in the list in memory
  Bool_t status = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);

  // loop over all keys in this directory
  TChain *globChain = 0;
  TIter nextkey( current_sourcedir->GetListOfKeys() );
  TKey *key, *oldkey=0;
  while ( (key = (TKey*)nextkey())) {

    //keep only the highest cycle number for each key
    if (oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;

    // read object from first source file
    first_source->cd( path );
    TObject *obj = key->ReadObj();

    if ( obj->IsA()->InheritsFrom( "RooPlot" ) ) {
      cout << "found a decendent of a RooPlot "
	   << obj->GetName() << endl;
      // descendant of TH1 -> merge it

      //      cout << "Merging histogram " << obj->GetName() << endl;
      RooPlot *rp1 = (RooPlot*)obj;
      cout << " rp1->GetTitle: " << rp1->GetTitle() << endl;
      std::string title1=rp1->GetTitle();
      // loop over all source files and add the content of the
      // correspondant histogram to the one pointed to by "h1"
      TFile *nextsource = (TFile*)sourcelist->After( first_source );
      while ( nextsource ) {
        
        // make sure we are at the correct directory level by cd'ing to path
        nextsource->cd( path );

	TList* keyList=gDirectory->GetListOfKeys();
	TIter next(keyList);
	TKey * tmpKey;
	while ((tmpKey = (TKey*)next())) {
	  TObject *tmpObj = tmpKey->ReadObj();
	  std::string title2 = tmpObj->GetTitle();
	  if(title2==title1){

	    RooPlot *rp2 = (RooPlot*) tmpObj;

	    
	    for(int i=0; i< rp2->numItems();i++){
	      TObject* plot_obj = rp1->getObject(i);
	      cout << " NAME " << plot_obj->GetName();
	      if (plot_obj->IsA()->InheritsFrom( "TH1" )){
		cout << " INHERITS FROM TH1...";
		TH1 *h1=(TH1*) plot_obj;
		h1->Add((TH1*)rp2->getObject(i));
		cout << " added! ";
	      }
	    }
	    
	    //	    rp1->Add(rp2);
	    cout << "all done for " << rp1->GetName() << endl;
	    delete tmpObj;
	    break;
	  }
	}
        nextsource = (TFile*)sourcelist->After( nextsource );
      }
    } else if ( obj->IsA()->InheritsFrom( "TH1" ) ) {
      // descendant of TH1 -> merge it

      //      cout << "Merging histogram " << obj->GetName() << endl;
      TH1 *h1 = (TH1*)obj;

      // loop over all source files and add the content of the
      // correspondant histogram to the one pointed to by "h1"
      TFile *nextsource = (TFile*)sourcelist->After( first_source );
      while ( nextsource ) {
        
        // make sure we are at the correct directory level by cd'ing to path
        nextsource->cd( path );
        TKey *key2 = (TKey*)gDirectory->GetListOfKeys()->FindObject(h1->GetName());
        if (key2) {
           TH1 *h2 = (TH1*)key2->ReadObj();
           h1->Add( h2 );
           delete h2;
        }

        nextsource = (TFile*)sourcelist->After( nextsource );
      }
    } else if ( obj->IsA()->InheritsFrom( "TTree" ) ) {
      
      // loop over all source files create a chain of Trees "globChain"
      const char* obj_name= obj->GetName();

      globChain = new TChain(obj_name);
      globChain->Add(first_source->GetName());
      TFile *nextsource = (TFile*)sourcelist->After( first_source );
      //      const char* file_name = nextsource->GetName();
      // cout << "file name  " << file_name << endl;
     while ( nextsource ) {
     	  
       globChain->Add(nextsource->GetName());
       nextsource = (TFile*)sourcelist->After( nextsource );
     }

    } else if ( obj->IsA()->InheritsFrom( "TDirectory" ) ) {
      // it's a subdirectory

      cout << "Found subdirectory " << obj->GetName() << endl;

      // create a new subdir of same name and title in the target file
      target->cd();
      TDirectory *newdir = target->mkdir( obj->GetName(), obj->GetTitle() );

      // newdir is now the starting point of another round of merging
      // newdir still knows its depth within the target file via
      // GetPath(), so we can still figure out where we are in the recursion
      MergeRootfile( newdir, sourcelist );

    } else {

      // object is of no type that we know or can handle
      cout << "Unknown object type, name: " 
           << obj->GetName() << " title: " << obj->GetTitle() << endl;
    }

    // now write the merged histogram (which is "in" obj) to the target file
    // note that this will just store obj in the current directory level,
    // which is not persistent until the complete directory itself is stored
    // by "target->Write()" below
    if ( obj ) {
      target->cd();

      //!!if the object is a tree, it is stored in globChain...
	if(obj->IsA()->InheritsFrom( "TTree" ))
          globChain->Merge(target->GetFile(),0,"keep");
	else
	obj->Write( key->GetName() );
    }

  } // while ( ( TKey *key = (TKey*)nextkey() ) )

  // save modifications to target file
  target->SaveSelf(kTRUE);
  TH1::AddDirectory(status);
}
