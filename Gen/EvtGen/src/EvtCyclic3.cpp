/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtCyclic3.cpp,v 1.1 2003-10-02 17:34:48 robbep Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include <assert.h>
#include <string.h>
#include <iostream>
#include "EvtGenBase/EvtCyclic3.hh"

using namespace EvtCyclic3;


Index EvtCyclic3::permute(Index i, Perm p)
{
  if(p == ABC) return i;
  else if(p == BCA) {
    if(i == A) return C;
    else if(i == B) return A;
    else if(i == C) return B;
  }
  else if(p == BCA) {
    if(i == A) return C;
    else if(i == B) return A;
    else if(i == C) return B;
  }
  else if(p == CAB) {
    if(i == A) return B;
    else if(i == B) return C;
    else if(i == C) return A;
  }
  else if(p == CBA) {
    if(i == A) return C;
    else if(i == B) return B;
    else if(i == C) return A;
  }
  else if(p == BAC) {
    if(i == A) return B;
    else if(i == B) return A;
    else if(i == C) return C;
  }
  else if(p == ACB) {
    if(i == A) return A;
    else if(i == B) return C;
    else if(i == C) return B;
  }
  return i ;
}

Perm EvtCyclic3::permutation(Index i1,Index i2,Index i3)
{
  assert(i1 != i2  && i2 != i3 && i3 != i1);
  if(i1 == A) return (i2 == B) ? ABC : ACB;
  if(i1 == B) return (i2 == C) ? BCA : BAC;
  if(i1 == C) return (i2 == A) ? CAB : CBA;
  return ABC ;
}


Perm EvtCyclic3::permute(Perm i,Perm p)
{
  Index i1 = permute(permute(A,i),p);
  Index i2 = permute(permute(B,i),p);
  Index i3 = permute(permute(C,i),p);

  return permutation(i1,i2,i3);
}

Pair EvtCyclic3::permute(Pair i, Perm p)
{
  Index i1 = permute(first(i),p);
  Index i2 = permute(second(i),p);
  return combine(i1,i2);
}


Pair EvtCyclic3::i2pair(int i)
{
  assert(0<=i && i<=2); 
  switch(i) {
  case 0: return BC;
  case 1: return CA;
  case 2: return AB;
  }
  assert(0); return AB; // should never get here
}



Index EvtCyclic3::prev(Index i) 
{
  switch(i) {
  case A: return C;
  case B: return A;
  case C: return B;
  }
  assert(0); return A; // should never get here
}


Index EvtCyclic3::next(Index i) 
{
  switch(i) {
  case A: return B;
  case B: return C;
  case C: return A;
  }
  assert(0); return A; // should never get here
}


Index EvtCyclic3::other(Index i, Index j)
{
  assert(i != j);
  switch(i) {
    
  case A:
    switch(j) {
    case B: return C;
    case C: return B;
    case A: return A; /** should not happen **/
    }
  case B:
    switch(j) {
    case C: return A;
    case A: return C;
    case B: return B; /** should not happen **/
    }
  case C:
    switch(j) {
    case A: return B;
    case B: return A;
    case C: return C; /** should not happen **/
    }
  }
  assert(0); return A; // should never get here
}


// Index-to-pair conversions

Pair EvtCyclic3::other(Index i) 
{
  switch(i) {
  case A: return BC;
  case B: return CA;
  case C: return AB;
  }
  assert(0); return AB; // should never get here
}


Pair EvtCyclic3::combine(Index i, Index j)
{
  return other(other(i,j));
}


// Pair-to-pair conversions

Pair EvtCyclic3::prev(Pair i) 
{
  Pair ret = CA;
  if(i == BC) ret = AB;
  else
    if(i == CA) ret = BC;

  return ret;
} 

Pair EvtCyclic3::next(Pair i) 
{
  Pair ret = BC;
  if(i == BC) ret = CA;
  else
    if(i == CA) ret = AB;

  return ret;
} 

Pair EvtCyclic3::other(Pair i, Pair j)
{
  return combine(other(i),other(j));
}


// Pair-to-index conversions


Index EvtCyclic3::first(Pair i) 
{
  switch(i) {
  case BC: return B;
  case CA: return C;
  case AB: return A;
  }
  assert(0); return A; // should never get here
}  


Index EvtCyclic3::second(Pair i) 
{
  switch(i) {
  case BC: return C;
  case CA: return A;
  case AB: return B;
  }
  assert(0); return A; // should never get here
} 


Index EvtCyclic3::other(Pair i) 
{
  switch(i) {
  case BC: return A;
  case CA: return B;
  case AB: return C;
  }
  assert(0); return A; // should never get here
}


Index EvtCyclic3::common(Pair i, Pair j)
{
 return other(other(i,j));
}


Index EvtCyclic3::strToIndex(const char* str) 
{
  if(strcmp(str,"A")) return A;
  else if(strcmp(str,"B")) return B;
  else if(strcmp(str,"C")) return C;
  else assert(0);
}


Pair EvtCyclic3::strToPair(const char* str)
{
  if(!strcmp(str,"AB") || !strcmp(str,"BA")) return AB;
  else if(!strcmp(str,"BC") || !strcmp(str,"CB")) return BC;
  else if(!strcmp(str,"CA") || !strcmp(str,"AC")) return CA;
  else assert(0);
}


char* EvtCyclic3::c_str(Index i)
{
  switch(i) {
  case A: return "A";
  case B: return "B";
  case C: return "C";
  }
  assert(0); return 0; // sngh
}


char* EvtCyclic3::c_str(Pair i)
{
  switch(i) {
  case BC: return "BC";
  case CA: return "CA";
  case AB: return "AB";
  }
  assert(0); return 0; // sngh
}

char* EvtCyclic3::c_str(Perm p)
{
  if(p == ABC) return "ABC";
  if(p == BCA) return "BCA";
  if(p == CAB) return "CAB";
  if(p == CBA) return "CBA";
  if(p == BAC) return "BAC";
  if(p == ACB) return "ACB";
  return "ABC" ;
}

char* EvtCyclic3::append(const char* str, EvtCyclic3::Index i)
{
  // str + null + 1 character
  char* s = new char[strlen(str)+2];
  strcpy(s,str);
  strcat(s,c_str(i));
  
  return s;
}

char* EvtCyclic3::append(const char* str, EvtCyclic3::Pair i)
{
  // str + null + 2 characters
  char* s = new char[strlen(str)+3];
  strcpy(s,str);
  strcat(s,c_str(i));
  
  return s;
}


std::ostream& operator<<(std::ostream& os, EvtCyclic3::Index i) 
{
  switch(i) {
  case A: { os << "A"; return os; }
  case B: { os << "B"; return os; }
  case C: { os << "C"; return os; }
  }
  assert(0); return os; // should never get here
}


std::ostream& operator<<(std::ostream& os, EvtCyclic3::Pair i)
{
  switch(i) {
  case BC: { os << "BC"; return os; }
  case CA: { os << "CA"; return os; }
  case AB: { os << "AB"; return os; }
  }
  assert(0); return os; // should never get here
}




