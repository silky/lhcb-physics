
#include "RichSimpleFrontEndResponse.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichSimpleFrontEndResponse>  s_factory ;
const         IAlgFactory& RichSimpleFrontEndResponseFactory = s_factory ;

// Standard constructor, initializes variables
RichSimpleFrontEndResponse::RichSimpleFrontEndResponse( const std::string& name,
                                                        ISvcLocator* pSvcLocator )
  : RichAlgBase ( name, pSvcLocator ) {

  declareProperty( "MCRichSummedDepositsLocation",
                   m_mcRichSummedDepositsLocation = MCRichSummedDepositLocation::Default );
  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = MCRichDigitLocation::Default );
  declareProperty( "SimpleCalibration", m_Calibration = 4420 );
  declareProperty( "SimpleBaseline", m_Baseline = 50 );
  declareProperty( "SimpleSigma", m_Sigma = 1.0 );

  Rndm::Numbers m_gaussRndm;
}

RichSimpleFrontEndResponse::~RichSimpleFrontEndResponse () { };

StatusCode RichSimpleFrontEndResponse::initialize() {

  debug() << "Initialize" << endreq;

  // Initialize base class
  StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // create a collection of all pixels
  std::vector<RichSmartID> pixels;
  IRichSmartIDTool * smartIDs;
  acquireTool( "RichSmartIDTool" , smartIDs );
  smartIDs->readoutChannelList(pixels);
  releaseTool( smartIDs );
  actual_base = theRegistry.GetNewBase( pixels );

  m_AdcCut = 85;

  // Gauss randomn dist
  m_gaussRndm.initialize( randSvc(), Rndm::Gauss(0.0,0.9) );

  debug() << " Using simple HPD frontend response algorithm" << endreq
          << " Acquired information for " << pixels.size() << " pixels" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichSimpleFrontEndResponse::finalize() 
{
  debug() << "finalize" << endreq;

  // finalize randomn number generator
  m_gaussRndm.finalize();

  // finalize base class
  return RichAlgBase::finalize();
}

StatusCode RichSimpleFrontEndResponse::execute() {

  debug() << "Execute" << endreq;

  SummedDeposits = get<MCRichSummedDeposits>( m_mcRichSummedDepositsLocation );
  debug() << "Successfully located " << SummedDeposits->size()
          << " MCRichSummedDeposits at " << m_mcRichSummedDepositsLocation << endreq;

  // Run the Simple treatment
  Simple();

  return StatusCode::SUCCESS;
}

StatusCode RichSimpleFrontEndResponse::Simple() {

  // make new mcrichdigits
  MCRichDigits* mcRichDigits = new MCRichDigits();
  put ( mcRichDigits, m_mcRichDigitsLocation );

  for ( MCRichSummedDeposits::const_iterator iSumDep = SummedDeposits->begin();
        iSumDep != SummedDeposits->end(); ++iSumDep ) {

    RichPixelProperties* props = 
      actual_base->DecodeUniqueID( (*iSumDep)->key() );
    if ( props ) {

      // Make new MCRichDigit
      MCRichDigit * newDigit = new MCRichDigit();

      double summedEnergy = 0.0;
      for ( SmartRefVector<MCRichDeposit>::const_iterator deposit =
              (*iSumDep)->deposits().begin();
            deposit != (*iSumDep)->deposits().end(); ++deposit ) {
        if ( (*deposit)->time() > 0.0 &&
             (*deposit)->time() < 25.0 ) {
          summedEnergy += (*deposit)->energy();
          newDigit->addToHits( (*deposit)->parentHit() );
        }
      }
      (*iSumDep)->setSummedEnergy( summedEnergy );

      int value = int((summedEnergy+m_Sigma*m_gaussRndm()/1000)*m_Calibration) + m_Baseline;
      if ( !newDigit->hits().empty() && value >= m_AdcCut ) {
        mcRichDigits->insert( newDigit, (*iSumDep)->key() );
      } else {
        delete newDigit;
      }

    }

  }

  debug() << "Created " << mcRichDigits->size() << " MCRichDigits at "
          << m_mcRichDigitsLocation << endreq;

  return StatusCode::SUCCESS;
}
