// Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToolFactory.h"
// Local
#include "VPRadiationDamageTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VPRadiationDamageTool
//
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY(VPRadiationDamageTool)

//=============================================================================
/// Standard constructor
//=============================================================================
VPRadiationDamageTool::VPRadiationDamageTool(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
    : GaudiTool (type, name, parent),
      m_det(NULL) {
  
  declareInterface<VPRadiationDamageTool>(this);

}

//=============================================================================
/// Destructor
//=============================================================================
VPRadiationDamageTool::~VPRadiationDamageTool() {}

//=============================================================================
/// Initialization
//=============================================================================
StatusCode VPRadiationDamageTool::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  m_det = getDet<DeVP>(DeVPLocation::Default);
 
  // Set fit values for fluence.
  m_a.push_back(3.62911);   
  m_a.push_back(0.000271931);
  m_a.push_back(-3.30213e-06);
  m_a.push_back(-3.42959e-09);
  m_a.push_back(6.35852e-12);
  m_a.push_back(1.93743e-15);
  m_a.push_back(1.62919e-18);

  m_k.push_back(2.29997); 
  m_k.push_back(-0.000121513);
  m_k.push_back(-5.11296e-06);
  m_k.push_back(6.21903e-09);
  m_k.push_back(2.79419e-11);
  m_k.push_back(-6.86717e-14);
  m_k.push_back(4.18305e-17);
  return StatusCode::SUCCESS;

}

//=========================================================================
/// Calculate the fluence at a given position in the global frame.
//=========================================================================
double VPRadiationDamageTool::fluence(const Gaudi::XYZPoint point, 
                                      const double lint) {

  const double x = point.x() / Gaudi::Units::cm;
  const double y = point.y() / Gaudi::Units::cm;
  const double z = point.z() / Gaudi::Units::mm; 
  const double radius = sqrt(x * x + y * y);

  double a = 0.;
  double k = 0.;
  const unsigned int nTermsA = m_a.size();
  const unsigned int nTermsK = m_k.size();
  const unsigned int nTerms = nTermsA > nTermsK ? nTermsA : nTermsK;
  for (unsigned int j = 0; j < nTerms; ++j) {
    const double u = pow(z, j);
    if (j < nTermsA) a += 1.e13 * m_a[j] * u;
    if (j < nTermsK) k += m_k[j] * u;
  }
  const double fluence = a * lint * pow(radius, -k); 
  return fluence;

}

//=========================================================================
/// Calculate the CCE for a given fluence and bias.
//=========================================================================
double VPRadiationDamageTool::chargeCollectionEfficiency(const double f, 
                                                         const double v) {

  // Collected charge before irradiation (assuming 200 um thickness).
  const double mpvNonIrradiated = 15.;
  const double a = 2.2e4 * pow(f, -0.306);
  const double b = -0.1363 * pow(f, 0.2276) + 191.6;
  const double collectedCharge = a * pow(v + b, 0.5);
  if (collectedCharge > mpvNonIrradiated) return 1.;
  return collectedCharge / mpvNonIrradiated;

}

