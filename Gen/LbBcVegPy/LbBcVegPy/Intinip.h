// $Id: Intinip.h,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $

#ifndef LBPYTHIA_INTINIP_H
#define LBPYTHIA_INTINIP_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall INTINIP_ADDRESS(void) ;
}
#else
extern "C" {
  void* intinip_address_(void) ;
}
#endif

class Intinip {
public:
  Intinip();
  ~Intinip();

  int& iinip();

  inline void init(); // inlined for speed of access (small function)

private:
  struct INTINIP;
  friend struct INTINIP;
  
  struct INTINIP {
    int iinip;
  };
  int m_dummy;
  double m_realdummy;
  static INTINIP* s_intinip;
};

// Inline implementations for Intinip
// initialise pointer
#ifdef WIN32
void Intinip::init(void) {
  if ( 0 == s_intinip ) s_intinip = static_cast<INTINIP*>(INTINIP_ADDRESS());
}
#else
void Intinip::init(void) {
  if ( 0 == s_intinip ) s_intinip = static_cast<INTINIP*>(intinip_address_());
}
#endif
#endif // LBPYTHIA_INTINIP_H
 
