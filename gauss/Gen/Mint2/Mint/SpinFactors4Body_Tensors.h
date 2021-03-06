#ifndef SPINFACTORSSSS_FOURBODY_TENSORS_HH
#define SPINFACTORSSSS_FOURBODY_TENSORS_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)

#include "Mint/DalitzEventPattern.h"
#include "Mint/IDalitzEvent.h"

#include "Mint/DecayTree.h"
#include "Mint/SpinFactor.h"
#include "Mint/counted_ptr.h"
#include <iostream>


//====================================================

// ---------- D->VT with varius L states ----------
// ------------------------------------------------
class SF_DtoVT_VtoP0P1_TtoP2P3_BASE : public SpinFactor{
 protected:
  // static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> V, T;// *P1, *P2, *P3, *P4;

  virtual bool parseTree(const DalitzEventPattern& pat);

 public:
  virtual void printYourself(std::ostream& os=std::cout) const=0;
 SF_DtoVT_VtoP0P1_TtoP2P3_BASE(const DecayTree& theDecay) 
   : SpinFactor(theDecay, 4)
    , V(0), T(0)
    {
    }

  virtual ~SF_DtoVT_VtoP0P1_TtoP2P3_BASE(){}
  virtual double getVal(IDalitzEvent& evt)=0;

  virtual const DecayTree& exampleDecay()=0;
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoVT_VtoP0P1_TtoP2P3_BASE("
      + theBareDecay().oneLiner() + ")";
  }

};
// ---------------------------------

class SF_DtoVT_VtoP0P1_TtoP2P3_P : public SF_DtoVT_VtoP0P1_TtoP2P3_BASE{
  static DecayTree* _exampleDecay;
 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  
 SF_DtoVT_VtoP0P1_TtoP2P3_P(const DecayTree& theDecay)
   : SF_DtoVT_VtoP0P1_TtoP2P3_BASE(theDecay)
    {
//      printYourself();
    }
  
  virtual ~SF_DtoVT_VtoP0P1_TtoP2P3_P(){}
  virtual double getVal(IDalitzEvent& evt);

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoVT_VtoP0P1_TtoP2P3_P(" 
      + theBareDecay().oneLiner() + ")";
  }
};

// -----------------------------------------------

class SF_DtoVT_VtoP0P1_TtoP2P3_D : public SF_DtoVT_VtoP0P1_TtoP2P3_BASE{
  static DecayTree* _exampleDecayD;
 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  
 SF_DtoVT_VtoP0P1_TtoP2P3_D(const DecayTree& theDecay)
   : SF_DtoVT_VtoP0P1_TtoP2P3_BASE(theDecay)
    {
//      printYourself();
    }
  
  virtual ~SF_DtoVT_VtoP0P1_TtoP2P3_D(){}
  virtual double getVal(IDalitzEvent& evt);

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoVT_VtoP0P1_TtoP2P3_D(" 
      + theBareDecay().oneLiner() + ")";
  }
};


//------------------------------------------------
// D->TT, various L states
//------------------------------------------------
class SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE : public SpinFactor{
 protected:
  MINT::const_counted_ptr<AssociatedDecayTree> T[2];// *P1, *P2, *P3, *P4;
  virtual bool parseTree(const DalitzEventPattern& pat);

 public:
  virtual void printYourself(std::ostream& os=std::cout) const=0;
 SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE(const DecayTree& theDecay) 
   : SpinFactor(theDecay, 4)
    {
      T[0]=T[1]=0;
    }

  virtual ~SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE(){}
  virtual double getVal(IDalitzEvent& evt)=0;
  
  virtual const DecayTree& exampleDecay()=0;
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE("
      + theBareDecay().oneLiner() + ")";
  }

};
//------------------------------------------------

class SF_DtoT1T2_T1toP0P1_T2toP2P3_S : public SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
 SF_DtoT1T2_T1toP0P1_T2toP2P3_S(const DecayTree& theDecay) 
   : SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE(theDecay)
    {
      //printYourself();
    }

  virtual ~SF_DtoT1T2_T1toP0P1_T2toP2P3_S(){}
  virtual double getVal(IDalitzEvent& evt);
  
  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoT1T2_T1toP0P1_T2toP2P3_S("
      + theBareDecay().oneLiner() + ")";
  }

};
//------------------------------------------------
class SF_DtoT1T2_T1toP0P1_T2toP2P3_P : public SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
 SF_DtoT1T2_T1toP0P1_T2toP2P3_P(const DecayTree& theDecay) 
   : SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE(theDecay)
    {
      //printYourself();
    }

  virtual ~SF_DtoT1T2_T1toP0P1_T2toP2P3_P(){}
  virtual double getVal(IDalitzEvent& evt);
  
  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoT1T2_T1toP0P1_T2toP2P3_P("
      + theBareDecay().oneLiner() + ")";
  }

};
//------------------------------------------------
class SF_DtoT1T2_T1toP0P1_T2toP2P3_D : public SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
 SF_DtoT1T2_T1toP0P1_T2toP2P3_D(const DecayTree& theDecay) 
   : SF_DtoT1T2_T1toP0P1_T2toP2P3_BASE(theDecay)
    {
      //printYourself();
    }

  virtual ~SF_DtoT1T2_T1toP0P1_T2toP2P3_D(){}
  virtual double getVal(IDalitzEvent& evt);
  
  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoT1T2_T1toP0P1_T2toP2P3_D("
      + theBareDecay().oneLiner() + ")";
  }

};
// ---------------------------------
//------------------------------------------------

// ------------------------------------------
// D->TP0, various subesquent decays of the T
//------------------------------------------------

class SF_DtoTP0_TtoVP1_VtoP2P3 : public SpinFactor{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> T, V;// *P1, *P2, *P3, *P4;
   virtual bool parseTree(const DalitzEventPattern& pat);

 public:
   virtual void printYourself(std::ostream& os=std::cout) const;
   SF_DtoTP0_TtoVP1_VtoP2P3(const DecayTree& theDecay) 
     : SpinFactor(theDecay, 4)
     , T(0), V(0)
     {
     }
   
   virtual ~SF_DtoTP0_TtoVP1_VtoP2P3(){}
   virtual double getVal(IDalitzEvent& evt);
   
   static const DecayTree& getExampleDecay();
   virtual const DecayTree& exampleDecay();
   virtual std::string name() const{
     return "SpinFactor4:SF_DtoTP0_TtoVP1_VtoP2P3("
       + theBareDecay().oneLiner() + ")";
   }
   
};
//------------------------------------------------

class SF_DtoT1P0_T1toT2P1_T2toP2P3 : public SpinFactor{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> T1, T2;// *P1, *P2, *P3, *P4;
   virtual bool parseTree(const DalitzEventPattern& pat);

 public:
   virtual void printYourself(std::ostream& os=std::cout) const;
   SF_DtoT1P0_T1toT2P1_T2toP2P3(const DecayTree& theDecay) 
     : SpinFactor(theDecay, 4)
     , T1(0), T2(0)
     {
     }
   
   virtual ~SF_DtoT1P0_T1toT2P1_T2toP2P3(){}
   virtual double getVal(IDalitzEvent& evt);
   
   static const DecayTree& getExampleDecay();
   virtual const DecayTree& exampleDecay();
   virtual std::string name() const{
     return "SpinFactor4:SF_DtoT1P0_T1toT2P1_T2toP2P3("
       + theBareDecay().oneLiner() + ")";
   }
   
};

// ------------------ V->TS  ------------

class SF_DtoTS_TtoP0P1_StoP2P3 : public SpinFactor{
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> T, S;// *P1, *P2, *P3, *P4;

  virtual bool parseTree(const DalitzEventPattern& pat);

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
 SF_DtoTS_TtoP0P1_StoP2P3(const DecayTree& theDecay) 
   : SpinFactor(theDecay, 4)
    , T(0), S(0)
    {
    }

  virtual ~SF_DtoTS_TtoP0P1_StoP2P3(){}
  virtual double getVal(IDalitzEvent& evt);
  
  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoTS_TtoP0P1_StoP2P3("
      + theBareDecay().oneLiner() + ")";
  }

};
// ---------------------------------
//    D->pseudoTensor P0, various modes
// ---------------------------------


class SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3 : public SpinFactor{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> PT, V;// *P1, *P2, *P3, *P4;
   virtual bool parseTree(const DalitzEventPattern& pat);

 public:
   virtual void printYourself(std::ostream& os=std::cout) const;
   SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3(const DecayTree& theDecay) 
     : SpinFactor(theDecay, 4)
     , PT(0), V(0)
     {
     }
   
   virtual ~SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3(){}
   virtual double getVal(IDalitzEvent& evt);
   
   static const DecayTree& getExampleDecay();
   virtual const DecayTree& exampleDecay();
   virtual std::string name() const{
     return "SpinFactor4:SF_DtoPseudoTP0_PseudoTtoVP1_VtoP2P3("
       + theBareDecay().oneLiner() + ")";
   }
   
};
//------------------------------------------------

class SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3 : public SpinFactor{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> PT, S;// *P1, *P2, *P3, *P4;
   virtual bool parseTree(const DalitzEventPattern& pat);

 public:
   virtual void printYourself(std::ostream& os=std::cout) const;
   SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3(const DecayTree& theDecay) 
     : SpinFactor(theDecay, 4)
     , PT(0), S(0)
     {
     }
   
   virtual ~SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3(){}
   virtual double getVal(IDalitzEvent& evt);
   
   static const DecayTree& getExampleDecay();
   virtual const DecayTree& exampleDecay();
   virtual std::string name() const{
     return "SpinFactor4:SF_DtoPseudoTP0_PseudoTtoSP1_StoP2P3("
       + theBareDecay().oneLiner() + ")";
   }
   
};
//------------------------------------------------
class SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3 : public SpinFactor{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> PT, T;// *P1, *P2, *P3, *P4;
   virtual bool parseTree(const DalitzEventPattern& pat);

 public:
   virtual void printYourself(std::ostream& os=std::cout) const;
   SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3(const DecayTree& theDecay) 
     : SpinFactor(theDecay, 4)
     , PT(0), T(0)
     {
     }
   
   virtual ~SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3(){}
   virtual double getVal(IDalitzEvent& evt);
   
   static const DecayTree& getExampleDecay();
   virtual const DecayTree& exampleDecay();
   virtual std::string name() const{
     return "SpinFactor4:SF_DtoPseudoTP0_PseudoTtoTP1_TtoP2P3("
       + theBareDecay().oneLiner() + ")";
   }
   
};

// -----------------------------------
// D->PA, A->TP (ang mom L=1 at each vtx)
// -----------------------------------

class SF_DtoAP0_AtoTP1_TtoP2P3 : public SpinFactor{
  // the simplest D->T1, T2, with L=0
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> A, T;// *P1, *P2, *P3, *P4;
   virtual bool parseTree(const DalitzEventPattern& pat);

 public:
   virtual void printYourself(std::ostream& os=std::cout) const;
   SF_DtoAP0_AtoTP1_TtoP2P3(const DecayTree& theDecay) 
     : SpinFactor(theDecay, 4)
     , A(0), T(0)
     {
     }
   
   virtual ~SF_DtoAP0_AtoTP1_TtoP2P3(){}
   virtual double getVal(IDalitzEvent& evt);
   
   static const DecayTree& getExampleDecay();
   virtual const DecayTree& exampleDecay();
   virtual std::string name() const{
     return "SpinFactor4:SF_DtoAP0_AtoTP1_TtoP2P3("
       + theBareDecay().oneLiner() + ")";
   }
   
};


// -----------------------------------
#endif
//
