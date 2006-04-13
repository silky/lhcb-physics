// $Id: MCTruthManager.cpp,v 1.3 2006-04-13 06:52:32 gcorti Exp $
// Include files 

// local
#include "GaussTools/MCTruthManager.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCTruthManager
//
// 2005-09-06 : Witold POKORSKI
//-----------------------------------------------------------------------------
static MCTruthManager* instance = 0;

//-----------------------------------------------------------------------------
// constructor
//-----------------------------------------------------------------------------
MCTruthManager::MCTruthManager(  ): event(0)
{
  primarybarcodes.clear();
  creators.clear();
  oscillated.clear();
  segmentations.clear();
}

//-----------------------------------------------------------------------------
// destructor
//-----------------------------------------------------------------------------
MCTruthManager::~MCTruthManager() 
{} 

//-----------------------------------------------------------------------------
// GetInstance
//-----------------------------------------------------------------------------
MCTruthManager* MCTruthManager::GetInstance()
{
  if (instance == 0 )
  {
    instance = new MCTruthManager();
  }
  return instance;
}

//-----------------------------------------------------------------------------
// Make New Event
//-----------------------------------------------------------------------------
void MCTruthManager::NewEvent()
{
  // clear the vector of primary barcordes
  primarybarcodes.clear();

  // clear the vector of vertex-creator identifier
  creators.clear();

  // clear the vector of oscillated particles
  oscillated.clear();

  // clear the map of segmentation of tracks
  segmentations.clear();

  // we delete the old event  
  delete event;

  // and then we instanciate a new one
  event = new HepMC::GenEvent();
}

//-----------------------------------------------------------------------------
// Get list of "primary" bar codes
//-----------------------------------------------------------------------------
const std::vector<int>& MCTruthManager::GetPrimaryBarcodes()
{
  return primarybarcodes;
}

//-----------------------------------------------------------------------------
// Get list of "oscillated" bar codes
//-----------------------------------------------------------------------------
const std::vector<int>& MCTruthManager::GetOscillatedBarcodes()
{
  return oscillated;
}

//-----------------------------------------------------------------------------
// Get creatorID for a vertex provided its barcode
//-----------------------------------------------------------------------------
int MCTruthManager::GetCreatorID(int barcode)
{

  int id = 0;
  std::map<int,int>::const_iterator i = creators.find( barcode );
  if ( i != creators.end() ) id = (*i).second;
  return id;
}

//-----------------------------------------------------------------------------
// Add new particle and related vertex(ices) to event
//-----------------------------------------------------------------------------
void MCTruthManager::AddParticle(HepLorentzVector& momentum, 
                                 HepLorentzVector& prodpos, 
                                 HepLorentzVector& endpos, 
                                 int pdg_id, int partID, int motherID,
                                 bool directParent, int creatorID,
                                 bool hasOscillated)
{
  // we create a new particle with barcode = partID
  HepMC::GenParticle* particle = new HepMC::GenParticle(momentum, pdg_id);
  particle->suggest_barcode(partID);
  // we initialize the 'segmentations' map
  // for the moment particle is not 'segmented' 
  segmentations[partID] = 1;

  // if it oscillated we put it the oscillated list
  if( hasOscillated ) {
    oscillated.push_back(partID);
  }
  
  // we create the GenVertex corresponding to the end point of the track
  HepMC::GenVertex* endvertex = new HepMC::GenVertex(endpos);
  
  // barcode of the endvertex = - barcode of the track
  endvertex->suggest_barcode(-partID);  
  creators[-partID] = creatorID; 
  endvertex->add_particle_in(particle);
  event->add_vertex(endvertex);
  
  if(motherID) // not a primary
  {
    // here we could try to improve speed by searching only through particles which 
    // belong to the given primary tree
    HepMC::GenParticle* mother = event->barcode_to_particle(motherID);
    //
    if(mother)
    {
      // we first check whether the mother's end vertex corresponds to the particle's
      // production vertex
      HepMC::GenVertex* motherendvtx = mother->end_vertex();
      HepLorentzVector motherendpos = motherendvtx->position();

      if( motherendpos.x() == prodpos.x() &&
          motherendpos.y() == prodpos.y() &&
          motherendpos.z() == prodpos.z() ) // if yes, we attach the particle
      {
        motherendvtx->add_particle_out(particle);
      }
      else // if not, we check whether the mother is biological or adopted
      {            
        if(!directParent) // adopted
        {  
          bool found = false;
          // first check if any of the dummy particles has the end vertex at the right place
          for(HepMC::GenVertex::particles_out_const_iterator 
                it=motherendvtx->particles_out_const_begin();
              it!=motherendvtx->particles_out_const_end();it++)
          {
            if((*it)->pdg_id()==-99000000)
            {
              HepLorentzVector dummypos = (*it)->end_vertex()->position();
              
              if( dummypos.x() == prodpos.x() &&
                  dummypos.y() == prodpos.y() &&
                  dummypos.z() == prodpos.z() ) 
              {
                (*it)->end_vertex()->add_particle_out(particle);
                found = true;
                break;
              }
            }
          }
          // and if not, create a dummy particle connecting to the end vertex of the mother
          if(!found)
          {
            HepMC::GenVertex* childvtx = new HepMC::GenVertex(prodpos);
            childvtx->add_particle_out(particle);
            // the dummy vertex gets the barcode -500000 minus the daughter particle barcode
            childvtx->suggest_barcode(-500000-partID);
            creators[-500000-partID] = creatorID;
            event->add_vertex(childvtx);
            
            HepMC::GenParticle* dummypart = new HepMC::GenParticle(HepLorentzVector(),-99000000);
            // the dummy particle gets the barcode 500000 plus the daughter particle barcode
            dummypart->suggest_barcode(500000+partID);
            childvtx->add_particle_in(dummypart);
            motherendvtx->add_particle_out(dummypart);
          }
        }
        else // biological
        {
          // we 'split' the mother into two particles and create a new vertex
          HepMC::GenVertex* childvtx = new HepMC::GenVertex(prodpos);
          childvtx->add_particle_out(particle);
          event->add_vertex(childvtx);
          // we first detach the mother from its original vertex 
          motherendvtx->remove_particle(mother);
          // and attach it to the new vertex
          childvtx->add_particle_in(mother);

          // now we create a new particle representing the mother after interaction
          // the barcode of the new particle is 10000000 + the original barcode 
          HepMC::GenParticle* mothertwo = new HepMC::GenParticle(*mother);
          mothertwo->suggest_barcode(10000000 + mother->barcode());
          // we also reset the barcodes of the vertices
          motherendvtx->suggest_barcode(-100000000 - mother->barcode());
          childvtx->suggest_barcode(-mother->barcode());
          creators[-mother->barcode()] = creatorID;
          // we attach it to the new vertex where interaction took place
          childvtx->add_particle_out(mothertwo);
          // and we attach it to the original endvertex
          motherendvtx->add_particle_in(mothertwo);
        }
      }
    }
    else 
      // mother GenParticle is not there for some reason...
      // if this happens, we need to revise the philosophy... 
      // a solution would be to create HepMC particles at the begining of each track
    {
      std::cerr << "barcode " <<  motherID << " mother not there! "<<  std::endl;
    }
  }
  else // primary
  {
    
    //
    HepMC::GenVertex* primaryvtx = new HepMC::GenVertex(prodpos);
     
    primaryvtx->add_particle_out(particle);
    event->add_vertex(primaryvtx);
    // add id to the list of primaries
    primarybarcodes.push_back(partID);
    
  } 
}

//-----------------------------------------------------------------------------
// Print event for debugging
//-----------------------------------------------------------------------------
void MCTruthManager::PrintEvent()
{
  event->print();

  // looping over primaries and print the decay tree for each of them
  for(std::vector<int>::const_iterator primarybar=primarybarcodes.begin();
      primarybar!=primarybarcodes.end();primarybar++)
  {
    printTree(event->barcode_to_particle(*primarybar), " | ");
  }
}

void MCTruthManager::printTree(HepMC::GenParticle* particle, std::string offset)
{
  std::cout << offset << "---  barcode: " << particle->barcode() << " pdg: " 
            << particle->pdg_id() << " energy: " << particle->momentum().e() 
            << " production vertex: "<< particle->production_vertex()->position() << std::endl;
  
  for(HepMC::GenVertex::particles_out_const_iterator 
        it=particle->end_vertex()->particles_out_const_begin();
        it!=particle->end_vertex()->particles_out_const_end();
      it++)
  {
    std::string deltaoffset = "";

    if( fmod((*it)->barcode(),10000000) != fmod(particle->barcode(),10000000) )
      {
        deltaoffset = " | ";
      }

    printTree((*it), offset + deltaoffset);
  } 
}
