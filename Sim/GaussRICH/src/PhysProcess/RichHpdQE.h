#ifndef RichHpdQE_h
#define RichHpdQE_h 1
// class with Hpd Quantum efficiencies.
// There would be one instance of this class for
// each hpd in each Hpd in each of the two rich detectors.
// SE 1-3-02.
// stores the photon energy in eV and Qe in absolute values
// in its member variables.
// the standard unit in Gaudi is MeV and hence it needs to be
// multiplied by 1000000 before using with variables of this class.
//
#include <vector>
#include <string>

class RichHpdQE {

public:

  RichHpdQE();
  RichHpdQE(int hpdnum, int richdetnum );
  virtual ~RichHpdQE();

  int curHpdNum() const {return  m_curHpdNum; }
  int curRichDetNum() const {return m_curRichDetNum; }
  int numHpdQEbins() const {return  m_numHpdQEbins; }

  const std::vector<double> & HpdQEff() const { return m_HpdQEff; }
  const std::vector<double> & PhotEnergyHpdQE() const { return m_PhotEnergyHpdQE ; }

  double getHpdQEffBin(const int curqbin) { return m_HpdQEff[curqbin]; }
  double getHpdPhotEnergyFromBin(const int curwbin) { return m_PhotEnergyHpdQE[curwbin];}

  std::vector<double> PhotWavelengthHpdQE();
  double getHpdPhotonWaveLengthFromBin(int curmbin);

  double getHpdPhotonEnergyFromWavelen(double photonwlen);

  double getHpdPhotonWavelenFromEnergy(double photonenergy);

  double getHpdQEffFromPhotEnergy(double photonenergy );

  double getHpdQEffFromPhotWavelen(double photwavelen );

  void setCurrentHpdQEwl(int ihpdnum, int irichdetnum, int inumQEbins,
                         const std::vector<double> & qeffvect, 
                         const std::vector<double> & qeffwavelen );

  void setCurrentHpdQEen(int jhpdnum, int jrichdetnum, int jnumQEbins,
                         const std::vector<double> & jqeffvect,
                         const std::vector<double> & qeffener );

  void setAnHpdQEwl(int inumQEbins,
                    const std::vector<double> & qeffvect, 
                    const std::vector<double> & qeffwavelen );

  void setAnHpdQEen(int jnumQEbins,
                    const std::vector<double> & jqeffvect, 
                    const std::vector<double> & qeffener );

  //  void getHpdQEDataFromDB();

private:

  int m_curHpdNum;
  int m_curRichDetNum;
  int m_numHpdQEbins;
  std::vector<double> m_HpdQEff;
  std::vector<double> m_PhotEnergyHpdQE;
  double m_WaveEnerCnv;
  int HpdQEVerboseLevel;

};

#endif

