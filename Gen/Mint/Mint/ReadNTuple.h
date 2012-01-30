#ifndef READNTUPLE_HH
#define READNTUPLE_HH

#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TString.h"
#include "TTree.h"
#include "TBranch.h"
#include "TEntryList.h"

#include <vector.h>

#include <string>

#include "Mint/counted_ptr.h"
#include "Mint/DalitzEvent.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/DiskResidentEventList.h"
#include "Mint/DalitzEventList.h"
#include "Mint/DalitzEventPattern.h"

using namespace std;
using namespace MINT;

class ReadNTuple
{
private:
	  TFile* _file0;
	  TTree* _oldTree;
	  TTree* _tree;
	  std::string _fname;
	  std::string _ntpName;
	  long int _maxEvents;

	  // Variables to read in
	  std::vector<std::vector<float> > m_input_var;
	  std::vector<int> m_pdg;
	  std::vector<int> set_pat;
	  std::vector<float> m_mother_var;
	  double m_mother_pdg;

	  int m_particle;
	  const char* _cuts;

	  TLorentzVector _pMother;

	  DalitzEventPattern* m_pat;

	  bool _applyFiducalCuts;

public:

	  std::string newFilename() const;

	  ReadNTuple( DalitzEventPattern*
			    , std::string fname
	  		    , std::string ntpName = "DalitzEventList"
	  		    , const char* = ""
	  		    , long int maxEvents = -1 // -1 means: read all
	  		    );

	  ~ReadNTuple();

	  bool getTree();

	  bool SetDaughterBranchAddress(const char* Px, const char* Py, const char* Pz, const char* E, const char* pdg );
	  bool SetMotherBranchAddress(const char* Px, const char* Py, const char* Pz, const char* E, const char* pdg );

	  bool SetEventPattern(DalitzEventPattern*);

	  void ApplyFiducalCuts();
	  bool passFiducalCuts();

	  bool AddFriend(std::string fname
	  	      	  , std::string ntpName);

	  bool getUpdatedTree();

	  MINT::counted_ptr<DalitzEvent> readEntry(unsigned int entry);
	  bool readit(DiskResidentEventList* listPtr, int nEvents=10);


};


ReadNTuple::ReadNTuple( DalitzEventPattern* pat
					  ,	std::string fname
				      , std::string ntpName
				      , const char* cuts
				      , long int maxEvents
				      )
  : _file0(0)
  , _oldTree(0)
  , _tree(0)
  , _fname(fname)
  , _ntpName(ntpName)
  , _maxEvents(maxEvents)
  , m_particle(0)
{

  for (int i = 0; i < 5; i++) {
      vector<float> row; // Create an empty row
      for (int j = 0; j < 5; j++) {
          row.push_back(i * j); // Add an element (column) to the row
      }
      m_input_var.push_back(row); // Add the row to the main vector
  }

  for (int i = 0; i < 5; i++) {
	  m_pdg.push_back(0);
	  m_mother_var.push_back(0);

  }

  SetEventPattern(pat);


  _cuts = cuts;
  _applyFiducalCuts = false;
  getTree();
}


std::string ReadNTuple::newFilename() const{
  string newFilename(_fname);
  newFilename.insert(newFilename.find_last_of('.'), "_reweighted");
  return newFilename;
}

bool ReadNTuple::AddFriend(std::string fname
	      	  	  	  	  , std::string ntpName)
{
	TFile* f = TFile::Open(fname.c_str(), "READ");
	if(0 == f) return false;
	TTree* friendTree = dynamic_cast<TTree*>(f->Get(ntpName.c_str()));
	_oldTree->AddFriend(friendTree);
	return (0 != _oldTree);

}

bool ReadNTuple::getTree(){
  TFile* f = TFile::Open(_fname.c_str(), "READ");
  if(0 == f) return false;
  _oldTree = dynamic_cast<TTree*>(f->Get(_ntpName.c_str()));
  return (0 != _oldTree);

}

bool ReadNTuple::getUpdatedTree()
{

  TEntryList* elist=0;

  _oldTree->SetBranchStatus("*",1);
  if (_cuts&&strcmp(_cuts,"")!=0) {
	  _oldTree->Draw(">>elist",_cuts,"entryList");
	  elist = (TEntryList*)gDirectory->Get("elist");
	  if (!elist)
	  {
		  std::stringstream msg;
		  msg << "Failed to retrieve TEntryList for cuts " << _cuts;

	  }
	  _oldTree->SetEntryList(elist);
  }
  Long64_t nentries = !elist?_oldTree->GetEntries():elist->GetN();
  if (nentries==0) {
	  std::cout << "No entries after cuts " << _cuts << std::endl;
  }


//  attachit();
  cout << "ReadNTuple::getTree(): Reading from " << _fname
       << ", writing to: " << newFilename() << endl;
  _file0 = TFile::Open(newFilename().c_str(), "RECREATE");
  cout << "opened new file " << _file0 << endl;
  if(0 == _file0) return false;
  _file0->cd();
  cout << "cd'ed to new file " << endl;
//  _tree = _oldTree->CloneTree(0);
  _tree = _oldTree->CopyTree("");

  _tree->SetDirectory(_file0);
  //_tree->CopyEntries(ot, _maxEvents);
  cout << "cloned tree from old file" << endl;
  return (0 != _tree);
}

bool ReadNTuple::SetEventPattern(DalitzEventPattern* pat)
{
	m_pat = pat;
	set_pat = pat->getVectorOfInts();


	return true;
}

void ReadNTuple::ApplyFiducalCuts()
{
	_applyFiducalCuts = true;
}
// Fiducal Cuts
bool ReadNTuple::passFiducalCuts()
{

	bool passCuts = true;

	for (int i = 0; i<4; i++)
	{
		float dx = m_input_var[0][i];
		float dy = m_input_var[1][i];
		float dz = m_input_var[2][i];

		if (fabs(dx) > 0.317*(dz - 2400))
		{
			passCuts = false;
		}
		if (fabs(dy/dz) < 0.02){
			double p1 = 418;
			double p2 = 497;
			double beta1 = 0.01397;
			double beta2 = 0.01605;
			if( p1 - beta1*dz < fabs(dx) || fabs(dx) < p2+ beta2*dz
					|| (fabs(dx) > 600 && fabs(dx) < 700))
				{
					passCuts = false;
				}
		}
		if ( (fabs(dx) > 600 && fabs(dx) < 700))
			{
				passCuts = false;
			}
	}
	return passCuts;
}

bool ReadNTuple::SetDaughterBranchAddress(const char* Px, const char* Py, const char* Pz, const char* E, const char* pdg )
{

	_oldTree->SetBranchAddress(Px,(float*)&m_input_var[0][m_particle]);
	_oldTree->SetBranchAddress(Py,(float*)&m_input_var[1][m_particle]);
	_oldTree->SetBranchAddress(Pz,(float*)&m_input_var[2][m_particle]);
	_oldTree->SetBranchAddress(E,(float*)&m_input_var[3][m_particle]);
	_oldTree->SetBranchAddress(pdg,(int*)&m_pdg[m_particle]);
	m_particle++;

	return true;
}

bool ReadNTuple::SetMotherBranchAddress(const char* Px, const char* Py, const char* Pz, const char* E, const char* pdg )
{

	_oldTree->SetBranchAddress(Px,(float*)&m_mother_var[0]);
	_oldTree->SetBranchAddress(Py,(float*)&m_mother_var[1]);
	_oldTree->SetBranchAddress(Pz,(float*)&m_mother_var[2]);
	_oldTree->SetBranchAddress(E,(float*)&m_mother_var[3]);
	_oldTree->SetBranchAddress(pdg,(int*)&m_mother_pdg);

	return true;
}


MINT::counted_ptr<DalitzEvent>
ReadNTuple::readEntry(unsigned int entry){

  // we read the MC-truth 4-momenta as we want to re-weight
  // according to the trugh.
  // Things to cut on are read from reconstructed data.

	bool dbThis = true;
  _oldTree->GetEntry(entry);


//  OrderParticles();
  // we'll fill it according to the event pattern
  // 421 -> -321 +211 -211 +211
//  DalitzEventPattern pat(421, -211, +211, +211, -211);

  TLorentzVector mother(m_mother_var[0],m_mother_var[1],m_mother_var[2],m_mother_var[3]);

  TLorentzVector daughter1(m_input_var[0][0],m_input_var[1][0],m_input_var[2][0],m_input_var[3][0]);
  TLorentzVector daughter2(m_input_var[0][1],m_input_var[1][1],m_input_var[2][1],m_input_var[3][1]);
  TLorentzVector daughter3(m_input_var[0][2],m_input_var[1][2],m_input_var[2][2],m_input_var[3][2]);
  TLorentzVector daughter4(m_input_var[0][3],m_input_var[1][3],m_input_var[2][3],m_input_var[3][3]);

  std::vector<TLorentzVector> daughter;
  daughter.push_back(daughter1);
  daughter.push_back(daughter2);
  daughter.push_back(daughter3);
  daughter.push_back(daughter4);

  if (dbThis)
  {
	  if (!(entry%1000))
	  {
		  cout << "PDG: " << (int)m_pdg[0] << "Px: " << m_input_var[0][0] << std::endl;
		  cout << "PDG: " << (int)m_pdg[1] << "Px: " << m_input_var[0][1] << std::endl;
		  cout << "PDG: " << (int)m_pdg[2] << "Px: " << m_input_var[0][2] << std::endl;
		  cout << "PDG: " << m_pdg[3] << "Px: " << m_input_var[0][3] << std::endl;
	  }
  }
   vector<TLorentzVector> PArray(5);

   TLorentzVector* pMother   = &(PArray[0]);
   *pMother = mother;

   int pdgArray[5];
   pdgArray[0] = (int)m_mother_pdg;

   if (dbThis)
   {
	   if (!(entry%1000))
		{
		   std::cout << "PArray Px: " << PArray[0].Px() << std::endl;
		   std::cout << "Mother Px: " << mother.Px() << std::endl;
		}
   }
   std::vector<int> passed_pat = set_pat;
   std::vector<int> this_pat = m_pdg;

   if (set_pat.size() == 0)
   {
	   std::cout << "Could not find EventPattern Did you set it?" << std::endl;
	   counted_ptr<DalitzEvent> zeroPtr(0);
	   return zeroPtr;
   }



   for (int j =1; j<5; j++)
   {
	   for (int i = 0; i < 4; i++)
	   {
		   if (passed_pat[j] == (int)this_pat[i])
		   {

			   pdgArray[j] = (int)this_pat[i];
			   PArray[j] = daughter[i];
			   this_pat[i] = 0;
			   if (dbThis)
			      {
				   if (!(entry%1000))
					{
					  cout << "Pdg: " << passed_pat[j]<< std::endl;
					  cout << "Px: " << PArray[j].Px() << std::endl;

					}
			      }
			   break;
		   }
	   }
   }

//   Randomize indentical final state particles
   for (int i = 1; i < 4; i++)
   {
	   for (int j = i+1; j < 5; j++)
	   {
		   if( pdgArray[i] == pdgArray[j])
		      {
			   if(gRandom->Rndm() >0.5){
			        swap(PArray[i],PArray[j] );
			        swap(pdgArray[i],pdgArray[j] );
			      }
		      }
	   }
   }


   if (dbThis)
      {
	  if(entry < 5){
		for(unsigned int i = 0; i < 5; i++){
		  cout << " mass " << i << ") " << PArray[i].M()
		   << " Px " << i << ") " << PArray[i].Px()
		   << ", pdg " << pdgArray[i] << endl;
		}
	  }

	  if (!(entry%1000))
		{
		  for(unsigned int i = 0; i < 5; i++){
				cout << " mass " << i << ") " << PArray[i].M()
				 << " Px " << i << ") " << PArray[i].Px()
			   << ", pdg " << pdgArray[i] << endl;
			  }
		}
     }
  _pMother.SetXYZT(m_mother_var[0], m_mother_var[1], m_mother_var[2], m_mother_var[3]);

  MINT::counted_ptr<DalitzEvent> evtPtr(new DalitzEvent(*m_pat, PArray));
  return evtPtr;
}


bool ReadNTuple::readit(DiskResidentEventList* listPtr, int maxEvents){
  getUpdatedTree();
  if(0 == listPtr) return false;
  int numEvents = 0;

  std::cout << "Entries " << _oldTree->GetEntries() << " " << _tree->GetEntries() << std::endl;
  for(unsigned int i=0; i < _tree->GetEntries(); i++){

    counted_ptr<DalitzEvent> evtPtr = readEntry(i);

//    if(!passFiducalCuts()) continue;
//    if(! passCuts()) continue;
    numEvents++;
    listPtr->Add(*(evtPtr.get()));
    if(maxEvents > 0 && numEvents > maxEvents) break;
  }
  return true;
}




#endif
