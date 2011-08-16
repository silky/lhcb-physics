#ifndef MINTDALITZ_CHI2_BINNING_HH
#define MINTDALITZ_CHI2_BINNING_HH

#include "Chi2Box.h"
#include "Chi2BoxSet.h"
#include "IFastAmplitudeIntegrable.h"

#include "IDalitzEventList.h"
#include "IDalitzEvent.h"
#include "IGetRealEvent.h"
#include "IDalitzPdf.h"

#include "DalitzHistoStackSet.h"
#include "TH1D.h"

#include <vector>
#include <iostream>

class Chi2Binning{
  std::vector<Chi2BoxSet> _boxSets;
  int _nData;
  int _nMC;
  double _totalMCWeight;

  static int* __colourPalette;
  static int __Ncol;
  static void makeColourPaletteBlueGrey();
  static void makeColourPaletteBlueWhite();
  static void makeColourPaletteRGB();
  static void makeColourPalette();
  static int* getColourPalette();

  void setHistoColours();

  Chi2BoxSet splitBoxes(IDalitzEventList* events
			 , int maxPerBin
			 ) const;
  int mergeBoxes(Chi2BoxSet& boxes, int minPerBin);

  void resetEventCounts();
  void fillData(IDalitzEventList* data);
  void fillMC(IDalitzEventList* mc, IDalitzPdf* pdf);
  double normFactor() const;
  void setBoxesNormFactors();
  void sortByChi2();
 public:
  Chi2Binning();
  int createBinning(IDalitzEventList* events
		    , int minPerBin = 10
		    , int maxPerBin = 100
		    );

  double setEventsAndPdf(IDalitzEventList* data
			 , IDalitzEventList* mc
			 , IDalitzPdf* pdf
			 , IFastAmplitudeIntegrable* fas=0// usually FitAmpSum*
			 );


  void setFas(IFastAmplitudeIntegrable* fas);

  int numBins() const;

  double getChi2_perBin() const;  
  double chi2_ofBin(unsigned int i) const;
  double getMaxChi2() const;
  void print(std::ostream& os = std::cout) const;

  DalitzHistoStackSet getDataHistoStack();
  DalitzHistoStackSet getMCHistoStack();
  MINT::counted_ptr<TH1D> getChi2Distribution() const;
  void drawChi2Distribution(const std::string& fname = "chi2Distribution.eps")const;

};

class lessByChi2BoxData{
 public:
  bool operator()(const Chi2Box& a, const Chi2Box& b) const;
};

class lessByChi2BoxSetChi2{
 public:
  bool operator()(const Chi2BoxSet& a, const Chi2BoxSet& b) const;
};

std::ostream& operator<<(std::ostream& os, const Chi2Binning& c2b);

#endif
//
