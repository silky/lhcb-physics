

#include "GiGa/GiGaTrajectoryPoint.h"


////////////////////////////////////////////////////////////////////////////////////////
G4Allocator<GiGaTrajectoryPoint> s_GiGaTrajectoryPointAllocator;
////////////////////////////////////////////////////////////////////////////////////////
GiGaTrajectoryPoint::GiGaTrajectoryPoint() 
  : G4TrajectoryPoint (   )
  , m_time            ( 0 )
{};
////////////////////////////////////////////////////////////////////////////////////////
GiGaTrajectoryPoint::GiGaTrajectoryPoint( const Hep3Vector& Pos , const double& Time)
  : G4TrajectoryPoint( Pos  ) 
  , m_time           ( Time )
{};
////////////////////////////////////////////////////////////////////////////////////////
GiGaTrajectoryPoint::GiGaTrajectoryPoint( const HepLorentzVector&  vect4)
  : G4TrajectoryPoint( vect4     )
  , m_time           ( vect4.t() )
{};
////////////////////////////////////////////////////////////////////////////////////////
GiGaTrajectoryPoint::GiGaTrajectoryPoint( const GiGaTrajectoryPoint& right ) 
  : G4TrajectoryPoint( right               )  
  , m_time           ( right.time()        ) 
{};
///////////////////////////////////////////////////////////////////////////////////////
GiGaTrajectoryPoint::~GiGaTrajectoryPoint(){};
///////////////////////////////////////////////////////////////////////////////////////
