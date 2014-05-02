#ifndef SPINFACTORSSSS_FOURBODY_SCALARS_AND_VECTORS_HH
#define SPINFACTORSSSS_FOURBODY_SCALARS_AND_VECTORS_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT

// These come from:
// D. Coffman et al: "Resonant substructure in Kbar pi pi pi decays of D mesons"
// Physical Review D, Vol 45, Number 7, page 2196
// The spin factors are in Table II, page 2201.

#include "Mint/DalitzEventPattern.h"
#include "Mint/IDalitzEvent.h"

#include "Mint/DecayTree.h"
#include "Mint/SpinFactor.h"
#include "Mint/counted_ptr.h"
#include <iostream>


// ==========================================

class SF_DtoPP0_PtoSP1_StoP3P4 : public SpinFactorTrivial{ 
  static DecayTree* _exampleDecay;
 public:
  SF_DtoPP0_PtoSP1_StoP3P4(const DecayTree& )
    : SpinFactorTrivial(){}

  virtual ~SF_DtoPP0_PtoSP1_StoP3P4(){}
  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoPP0_PtoSP1_StoP3P4-trivialSpinFactor";
  }
};
// ==========================================

class SF_DtoS1S2_S1toP1P2_S2toP3P4 : public SpinFactorTrivial{ 
  static DecayTree* _exampleDecay;
 public:
  SF_DtoS1S2_S1toP1P2_S2toP3P4(const DecayTree& )
    : SpinFactorTrivial(){}

  virtual ~SF_DtoS1S2_S1toP1P2_S2toP3P4(){}
  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoS1S2_S1toP1P2_S2toP3P4-trivialSpinFactor";
  }
};

// ==========================================

class SF_DtoPP1P2_PtoP3P4 : public SpinFactorTrivial{ //Laurens wide Ks Spin
  static DecayTree* _exampleDecay;
 public:
  SF_DtoPP1P2_PtoP3P4(const DecayTree& )
    : SpinFactorTrivial(){}

  virtual ~SF_DtoPP1P2_PtoP3P4(){}
  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoPP1P2_PtoP3P4-trivialSpinFactor";
  }
};

// ==========================================

class SF_DtoPP0_PtoVP1_VtoP2P3 : public SpinFactor{
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> P, V;// *P1, *P2, *P3, *P4;

  virtual bool parseTree(const DalitzEventPattern& pat);
  
 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoPP0_PtoVP1_VtoP2P3(const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4)
    , P(0), V(0)
    {
    }

  virtual ~SF_DtoPP0_PtoVP1_VtoP2P3(){}
  virtual double getVal(IDalitzEvent& evt);

  virtual std::string name() const{
    return "SpinFactor4:SF_DtoPP0_PtoVP1_VtoP2P3(" 
      + theBareDecay().oneLiner() + ")";
  }

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();

};

// ==========================================

class SF_DtoAP0_AtoVP1_VtoP2P3_BASE : public SpinFactor{
 protected:
  // static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> A, V;// *P1, *P2, *P3, *P4;

  virtual bool parseTree(const DalitzEventPattern& pat);

 public:
  virtual void printYourself(std::ostream& os=std::cout) const=0;
  SF_DtoAP0_AtoVP1_VtoP2P3_BASE(const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4)
    , A(0), V(0)
    {
    }

  virtual ~SF_DtoAP0_AtoVP1_VtoP2P3_BASE(){}
  virtual double getVal(IDalitzEvent& evt)=0;

  virtual const DecayTree& exampleDecay()=0;
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoAP0_AtoVP1_VtoP2P3_BASE(" 
      + theBareDecay().oneLiner() + ")";
  }

};
// ==========================================

class SF_DtoAP0_AtoVP1_VtoP2P3 : public SF_DtoAP0_AtoVP1_VtoP2P3_BASE{
 protected:
  static DecayTree* _exampleDecay;
 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoAP0_AtoVP1_VtoP2P3(const DecayTree& theDecay) 
    : SF_DtoAP0_AtoVP1_VtoP2P3_BASE(theDecay)
    {
    }

  virtual ~SF_DtoAP0_AtoVP1_VtoP2P3(){}
  virtual double getVal(IDalitzEvent& evt);

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoAP0_AtoVP1D_VtoP2P3(" 
      + theBareDecay().oneLiner() + ")";
  }

};
// ==========================================

class SF_DtoAP0_AtoVP1Dwave_VtoP2P3 : public SF_DtoAP0_AtoVP1_VtoP2P3_BASE{
 protected:
  static DecayTree* _exampleDecayD;
 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoAP0_AtoVP1Dwave_VtoP2P3(const DecayTree& theDecay) 
    : SF_DtoAP0_AtoVP1_VtoP2P3_BASE(theDecay)
    {
    }

  virtual ~SF_DtoAP0_AtoVP1Dwave_VtoP2P3(){}
  virtual double getVal(IDalitzEvent& evt);

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoAP0_AtoVP1Dwave_VtoP2P3(" 
      + theBareDecay().oneLiner() + ")";
  }

};


// ==========================================

class SF_DtoAP0_AtoSP1_StoP2P3 : public SpinFactor{
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> A, S;// *P1, *P2, *P3, *P4;

  virtual bool parseTree(const DalitzEventPattern& pat);

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoAP0_AtoSP1_StoP2P3(const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4)
    , A(0), S(0)
    {
    }

  virtual ~SF_DtoAP0_AtoSP1_StoP2P3(){}
  virtual double getVal(IDalitzEvent& evt);

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoAP0_AtoSP1_StoP2P3(" 
      + theBareDecay().oneLiner() + ")";
  }

};

// ==========================================

class SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE : public SpinFactor{
 protected:
  //  static DecayTree* _exampleDecayS;
  MINT::const_counted_ptr<AssociatedDecayTree> V1, V2;// *P1, *P2, *P3, *P4;

  virtual bool parseTree(const DalitzEventPattern& pat);

 public:
  virtual void printYourself(std::ostream& os=std::cout) const=0;
 SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE(const DecayTree& theDecay) 
   : SpinFactor(theDecay, 4)
    , V1(0), V2(0)
    {
    }
  
  virtual ~SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE(){}
  virtual double getVal(IDalitzEvent& evt)=0;

  virtual const DecayTree& exampleDecay()=0;
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE(" 
      + theBareDecay().oneLiner() + ")";
  }

};

// -----------------------------------------------

class SF_DtoV1V2_V1toP0P1_V1toP2P3_S : public SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE{
  static DecayTree* _exampleDecayS;

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
 SF_DtoV1V2_V1toP0P1_V1toP2P3_S(const DecayTree& theDecay)
   : SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE(theDecay)
    {
//      printYourself();
    }

  virtual ~SF_DtoV1V2_V1toP0P1_V1toP2P3_S(){}
  virtual double getVal(IDalitzEvent& evt);

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoV1V2_V1toP0P1_V1toP2P3_S(" 
      + theBareDecay().oneLiner() + ")";
  }
};
// -----------------------------------------------

class SF_DtoV1V2_V1toP0P1_V1toP2P3_P : public SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE{
  static DecayTree* _exampleDecayP;

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoV1V2_V1toP0P1_V1toP2P3_P(const DecayTree& theDecay)
    : SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE(theDecay)
    {
//      printYourself();
    }

  virtual ~SF_DtoV1V2_V1toP0P1_V1toP2P3_P(){}
  virtual double getVal(IDalitzEvent& evt);

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoV1V2_V1toP0P1_V1toP2P3_P(" 
      + theBareDecay().oneLiner() + ")";
  }
};
// -----------------------------------------------

class SF_DtoV1V2_V1toP0P1_V1toP2P3_D : public SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE{
  static DecayTree* _exampleDecayD;
 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoV1V2_V1toP0P1_V1toP2P3_D(const DecayTree& theDecay)
    : SF_DtoV1V2_V1toP0P1_V1toP2P3_BASE(theDecay)
    {
//      printYourself();
    }

  virtual ~SF_DtoV1V2_V1toP0P1_V1toP2P3_D(){}
  virtual double getVal(IDalitzEvent& evt);

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoV1V2_V1toP0P1_V1toP2P3_D(" 
      + theBareDecay().oneLiner() + ")";
  }
};

// ==========================================

class SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1 : public SpinFactor{
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> V2;// *P1, *P2, *P3, *P4;

  virtual bool parseTree(const DalitzEventPattern& pat);

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1(const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4)
    , V2(0)
    {
    }

  virtual ~SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1(){}
  virtual double getVal(IDalitzEvent& evt);

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1(" 
      + theBareDecay().oneLiner() + ")";
  }
};

// ==========================================

class SF_DtoVS_VtoP0P1_StoP2P3 : public SpinFactor{
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> V, S;// *P1, *P2, *P3, *P4;

  virtual bool parseTree(const DalitzEventPattern& pat);

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoVS_VtoP0P1_StoP2P3(const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4)
    , V(0), S(0)
    {
    }

  virtual ~SF_DtoVS_VtoP0P1_StoP2P3(){}
  virtual double getVal(IDalitzEvent& evt);

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoVS_VtoP0P1_StoP2P3(" 
      + theBareDecay().oneLiner() + ")";
  }
};

// ==========================================

class SF_DtoVS_VtoP0P1_StoP2P3_nonResV : public SpinFactor{
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> S;// *P1, *P2, *P3, *P4;

  virtual bool parseTree(const DalitzEventPattern& pat);

 public:
  virtual void printYourself(std::ostream& os=std::cout) const;
  SF_DtoVS_VtoP0P1_StoP2P3_nonResV(const DecayTree& theDecay) 
    : SpinFactor(theDecay, 4)
    , S(0)
    {
    }

  virtual ~SF_DtoVS_VtoP0P1_StoP2P3_nonResV(){}
  virtual double getVal(IDalitzEvent& evt);

  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor4:SF_DtoVS_VtoP0P1_StoP2P3_nonResV(" 
      + theBareDecay().oneLiner() + ")";
  }
};

//===========================================

class SF_DtoV1P0_V1toV2P1_V2toP2P3 : public SpinFactor{
  // author: Loic Esteve
protected:
 static DecayTree* _exampleDecay;
 MINT::const_counted_ptr<AssociatedDecayTree> V1, V2;// *P1, *P2, *P3, *P4;

 virtual bool parseTree(const DalitzEventPattern& pat);

public:
 virtual void printYourself(std::ostream& os=std::cout) const;
 SF_DtoV1P0_V1toV2P1_V2toP2P3(const DecayTree& theDecay) 
   : SpinFactor(theDecay, 4)
   , V1(0), V2(0)
   {
   }

 virtual ~SF_DtoV1P0_V1toV2P1_V2toP2P3(){}
 virtual double getVal(IDalitzEvent& evt);

 static const DecayTree& getExampleDecay();
 virtual const DecayTree& exampleDecay();
 virtual std::string name() const{
   return "SpinFactor4:SF_DtoV1P0_V1toV2P1_V2toP2P3(" 
     + theBareDecay().oneLiner() + ")";
 }

};


// ==========================================


// ==========================================

// -----------------------------------------------


/*
class SF_DtoAP1_AtoSP2_StoP3P4 : public SpinFactor{
 public:
  SF_DtoAP1_AtoSP2_StoP3P4(IDalitzEventAccess* events) 
    : SpinFactor(events)
    {}
}
class SF_DtoV1V2_V1toP1P2_V2toP3P4_S : public SpinFactor{
 public:
  SF_DtoV1V2_V1toP1P2_V2toP3P4_S(IDalitzEventAccess* events) 
    : SpinFactor(events)
    {}
}
class SF_DtoVS_VtoP1P2_StoP3P4 : public SpinFactor{
 public:
  SF_DtoVS_VtoP1P2_StoP3P4(IDalitzEventAccess* events) 
    : SpinFactor(events)
    {}
}

*/
#endif
//
