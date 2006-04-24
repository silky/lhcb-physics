// $Id: Intinif.h,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $

#ifndef LBPYTHIA_INTINIF_H
#define LBPYTHIA_INTINIF_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall INTINIF_ADDRESS(void) ;
}
#else
extern "C" {
  void* intinif_address_(void) ;
}
#endif

class Intinif {
public:
  Intinif();
  ~Intinif();

  int& iinif();

  inline void init(); // inlined for speed of access (small function)

private:
  struct INTINIF;
  friend struct INTINIF;
  
  struct INTINIF {
    int iinif;
  };
  int m_dummy;
  double m_realdummy;
  static INTINIF* s_intinif;
};

// Inline implementations for Intinif
// initialise pointer
#ifdef WIN32
void Intinif::init(void) {
  if ( 0 == s_intinif ) s_intinif = static_cast<INTINIF*>(INTINIF_ADDRESS());
}
#else
void Intinif::init(void) {
  if ( 0 == s_intinif ) s_intinif = static_cast<INTINIF*>(intinif_address_());
}
#endif
#endif // LBPYTHIA_INTINIF_H
 
