//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: G4MaterialPropertyVector.cc,v 1.1 2003-04-29 15:04:57 seaso Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
////////////////////////////////////////////////////////////////////////
// G4MaterialPropertyVector Class Implementation
////////////////////////////////////////////////////////////////////////
//
// File:        G4MaterialPropertyVector.cc
// Version:     1.0
// Created:     1996-02-08
// Author:      Juliet Armstrong
// Updated:     1997-03-25 by Peter Gumplinger
//              > cosmetics (only)
// mail:        gum@triumf.ca
//
////////////////////////////////////////////////////////////////////////

#include "G4MaterialPropertyVector.hh"

/////////////////////////
// Class Implementation  
/////////////////////////

        //////////////
        // Operators
        //////////////

// ++ operator
// -----------
//
G4bool G4MaterialPropertyVector::operator ++()
{
	CurrentEntry++;
	if (CurrentEntry < NumEntries) {
		return true;
	}
	else {
		return false; 
	}
}

// = operator
// ----------
//
G4MaterialPropertyVector&
G4MaterialPropertyVector::operator =(const G4MaterialPropertyVector& right)
{
        if (this == &right) return *this;
	
        // clear the vector of current contents

	MPV.clear();

        // create an actual copy (instead of the shallow copy that the
        // assignment operator defaults to for G4RWTPtrSortedVector)

        NumEntries = 0;
        CurrentEntry = -1;

        for (G4int i = 0 ; i < right.NumEntries; i++) {
                G4MPVEntry *newElement = new G4MPVEntry(right.GetEntry(i));
                MPV.push_back(newElement);
                NumEntries++;
        }

        return *this;
}

        /////////////////
        // Constructors
        /////////////////

G4MaterialPropertyVector::G4MaterialPropertyVector(G4double *PhotonMomenta,
 						   G4double *PropertyValues,
						   G4int     NumElements)
{
	NumEntries = 0;
	CurrentEntry = -1;

	// create a vector filling it with the values
	// from PhotonMomenta[] and PropertyValues[] 

	for(G4int i = 0; i < NumElements; i++) {
		AddElement(PhotonMomenta[i], PropertyValues[i]);
        }
}

G4MaterialPropertyVector::G4MaterialPropertyVector
			  (const G4MaterialPropertyVector &right)
{
	// create an actual copy (instead of the shallow copy that the
	// assignment operator defaults to for G4RWTPtrSortedVector)

        NumEntries = 0;
        CurrentEntry = -1;

        for (G4int i = 0 ; i < right.NumEntries; i++) {
        	G4MPVEntry *newElement = new G4MPVEntry(right.GetEntry(i));
                MPV.push_back(newElement);
                NumEntries++;
        }
}

	////////////////
        // Destructors
        ////////////////

G4MaterialPropertyVector::~G4MaterialPropertyVector()
{
	MPV.clear();
}

        ////////////
        // Methods
        ////////////
void G4MaterialPropertyVector::AddElement(G4double aPhotonMomentum,
					  G4double aPropertyValue) 
{
	G4MPVEntry *newElement;
	
	newElement = new G4MPVEntry(aPhotonMomentum, aPropertyValue);
	MPV.push_back(newElement);
	G4std::sort(MPV.begin(), MPV.end(), MPVEntry_less());
	NumEntries++; 
}

void G4MaterialPropertyVector::RemoveElement(G4double aPhotonMomentum)
{
	G4MPVEntry *newElement;
	G4MPVEntry *success=0;

	newElement = new G4MPVEntry(aPhotonMomentum, DBL_MAX);

	G4std::vector<G4MPVEntry*>::iterator i;
	for (i = MPV.begin(); i != MPV.end(); i++)
	  if (**i == *newElement) {success = *i; break;}
	//	success = MPV.remove(newElement);

	if(success == 0)
	{
	G4Exception("G4MaterialPropertyVector::RemoveElement==>"
					       "element not found");
	return;
	}
	else
	{
	  MPV.erase(i); // remove done here.
	}

	NumEntries--;
}

G4double 
G4MaterialPropertyVector::GetProperty(G4double aPhotonMomentum) const
{
	G4MPVEntry *target, *temp; 
	G4int left, right;
	G4double ratio1, ratio2, pmright, pmleft, InterpolatedValue;
 
	/////////////////////////
	// Establish table range 
	/////////////////////////

	G4double PMmin   = MPV.front()->GetPhotonMomentum(); 
	G4double minProp = MPV.front()->GetProperty(); 
	G4double PMmax   = MPV.back() ->GetPhotonMomentum();
	G4double maxProp = MPV.back() ->GetProperty();

	///////////////////////////////////////////
	// Does value fall outside range of table?
	///////////////////////////////////////////

	if (aPhotonMomentum < PMmin) 
	{
		G4cout << "\nWarning: G4MaterialPropertyVector::GetProperty";
		G4cout << "\n==> attempt to Retrieve Property below range" 
		<< G4endl;
    G4cout <<" MomMin MomMax PropMin PropMax  "<<PMmin
           <<"   "<< PMmax<<"   "<<minProp<<"   "
           << maxProp<< G4endl;
		return minProp; 
	} 

        if (aPhotonMomentum > PMmax)
	{
		G4cout << "\nWarning: G4MaterialPropertyVector::GetProperty";
		G4cout << "\n==> attempt to Retrieve Property above range" 
		<< G4endl;
    G4cout <<" MomMin MomMax PropMin PropMax  "<<PMmin
           <<"   "<< PMmax<<"   "<<minProp<<"   "
           << maxProp<< G4endl;

		return maxProp;
	} 
	
	target = new G4MPVEntry(aPhotonMomentum, 0.0);

	temp = 0;
	//temp = MPV.find(target);
	G4std::vector<G4MPVEntry*>::const_iterator i;
	for (i = MPV.begin(); i != MPV.end(); i++)
	  if (**i == *target) {temp = *i; break;}
	if (temp != 0) {

		////////////////////////
		// Return actual value
		////////////////////////

                G4double retval = temp->GetProperty();
                delete target;
                return retval;
	}
	else {
		//////////////////////////////
		// Return interpolated value 
		//////////////////////////////

		GetAdjacentBins(aPhotonMomentum, &left, &right);

                pmleft = MPV[left]->GetPhotonMomentum();
                pmright = MPV[right]->GetPhotonMomentum();
                ratio1 = (aPhotonMomentum-pmleft)/(pmright-pmleft);
                ratio2 = 1 - ratio1;
		InterpolatedValue = MPV[left]->GetProperty()*ratio2 + 
				    MPV[right]->GetProperty()*ratio1;
		
		delete target;
		return InterpolatedValue;
	}	
}

G4double G4MaterialPropertyVector::GetProperty() const
{
// 	For use with G4MaterialPropertyVector iterator

	if(CurrentEntry == -1 || CurrentEntry >= NumEntries) {
		G4Exception("G4MaterialPropertyVector::GetProperty ==>"
		"Iterator attempted to Retrieve Property out of range");
                return DBL_MAX;
	}
	else { 
		return MPV[CurrentEntry]->GetProperty();
	}
}

G4double G4MaterialPropertyVector::GetPhotonMomentum() const
{
// 	For use with G4MaterialPropertyVector iterator

	if(CurrentEntry == -1 || CurrentEntry >= NumEntries) {
		G4Exception("G4MaterialPropertyVector::GetPhotonMomentum ==>"
		"Iterator attempted to Retrieve Photon Momentum out of range");
                return DBL_MAX;
	}
	else { 
		return MPV[CurrentEntry]->GetPhotonMomentum();
	}
}

G4double 
G4MaterialPropertyVector::GetPhotonMomentum(G4double aProperty) const
{
// 				***NB*** 
// Assumes that the property is an increasing function of photon momentum (e.g.
// refraction index)
// 				***NB*** 
//
// Returns the photon momentum corresponding to the property value passed in.
// If several photon momentum values correspond to the value passed in, the
// function returns the first photon momentum in the vector that corresponds
// to that value. 

	G4int left, right, mid;
	G4double ratio1, ratio2, pright, pleft, InterpolatedValue;

        //////////////////////////
	// Establish Table range
	//////////////////////////

	G4double PropMin = MPV.front()->GetProperty();
	G4double PMmin   = MPV.front()->GetPhotonMomentum();
	G4double PropMax = MPV.back() ->GetProperty();
	G4double PMmax   = MPV.back() ->GetPhotonMomentum();

	///////////////////////////////////////////
	// Does value fall outside range of table?
	///////////////////////////////////////////

	if (aProperty < PropMin) 
	{
	  G4cout << "\nWarning: G4MaterialPropertyVector::GetPhotonMomentum";
	  G4cout << "\n==> attempt to Retrieve Photon Momentum out of range" 
	       << G4endl; 
	  return PMmin;
	}

	if (aProperty > PropMax) {
          G4cout << "\nWarning: G4MaterialPropertyVector::GetPhotonMomentum";
	  G4cout << "\n==> attempt to Retrieve Photon Momentum out of range" 
	       << G4endl;
	  return PMmax;
	}

	//////////////////////////////
	// Return interpolated value
	//////////////////////////////

	left = 0;
        right = MPV.size(); // was .entries()

        // find values in bins on either side of aProperty 
	
        do {
		mid = (left + right)/2;
		if (MPV[mid]->GetProperty() == aProperty) { 

			// Get first photon momentum value in vector that 
			// corresponds to property value  

			while (mid-1 >= 0 && 
			       MPV[mid-1]->GetProperty() == aProperty) {
				  mid--;
			}

			InterpolatedValue = MPV[mid]->GetPhotonMomentum();
			goto end_GetPhotonMomentum;	
		}
		if (MPV[mid]->GetProperty() < aProperty)
			left = mid;
		else
			right = mid;

	} while ((right - left) > 1);

	pleft = MPV[left]->GetProperty();
	pright = MPV[right]->GetProperty();
	ratio1 = (aProperty - pleft) / (pright - pleft);
	ratio2 = 1 - ratio1;
	InterpolatedValue = MPV[left]->GetPhotonMomentum()*ratio2 +
			    MPV[right]->GetPhotonMomentum()*ratio1;

end_GetPhotonMomentum:

	return InterpolatedValue;
}

void G4MaterialPropertyVector::ResetIterator()
{
	CurrentEntry = -1;
} 

void G4MaterialPropertyVector::DumpVector()
{
 	if (MPV.empty())  {
 		G4cerr << "nothing to dump" << G4endl;
		G4Exception("G4MaterialPropertyVector::DumpVector ==>"
			    "Nothing to dump!  Vector is empty");
 	}

	for (G4int i = 0; i < NumEntries; i++) {
		G4cout << "MPV["<< i << "]: ";
		MPV[i]->DumpEntry();
	}
        G4cout << " Done DumpVector of " << NumEntries << " entries " << G4endl;

} 

G4MPVEntry G4MaterialPropertyVector::GetEntry(G4int i) const
{
        return *MPV[i];
}

void 
G4MaterialPropertyVector::GetAdjacentBins(G4double aPhotonMomentum,
                                          G4int *left,
					  G4int *right) const
{
        G4int mid;

        *left = 0;
        *right = (MPV.size() - 1); // was .entries()

        // find values in bins on either side of aPhotonMomentum

	do {
        	mid = (*left + *right)/2;

                if (MPV[mid]->GetPhotonMomentum() < aPhotonMomentum) 
		{
                	*left = mid;
		}
                else
		{
                	*right = mid;
		}
        } while ((*right - *left) > 1);
}
