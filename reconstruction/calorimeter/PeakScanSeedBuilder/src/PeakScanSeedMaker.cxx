
#include "PeakScanSeedMaker.h"
#include "G4Kernel/CaloPhiRange.h"
#include "CaloCell/CaloCell.h"
#include "CaloCell/enumeration.h"
#include "G4SystemOfUnits.hh"
#include <algorithm>
#include <cmath>

using namespace SG;
using namespace Gaugi;


PeakScanSeedMaker::PeakScanSeedMaker( std::string name ) :
  IMsgService(name),
  Algorithm()
{
  declareProperty( "InputCellsKey"   , m_cellsKey="Cells"            );
  declareProperty( "OutputSeedsKey"  , m_seedsKey="PeakScanSeeds"    );
  declareProperty( "EnergyThreshold" , m_energyThreshold=1*GeV       );
  declareProperty( "MinDistanceDR"   , m_minDistanceDR=0.4           );
  declareProperty( "OutputLevel"     , m_outputLevel=1               );
}

//!=====================================================================

StatusCode PeakScanSeedMaker::initialize()
{
  CHECK_INIT();
  setMsgLevel(m_outputLevel);
  return StatusCode::SUCCESS;
}

//!=====================================================================

StatusCode PeakScanSeedMaker::execute( EventContext &ctx, int /*evt*/ ) const
{
  return post_execute(ctx);
}

//!=====================================================================

StatusCode PeakScanSeedMaker::post_execute( EventContext &ctx ) const
{
  SG::WriteHandle<xAOD::SeedContainer> seeds( m_seedsKey, ctx );
  seeds.record( std::unique_ptr<xAOD::SeedContainer>( new xAOD::SeedContainer() ) );

  SG::ReadHandle<xAOD::CaloCellContainer> container( m_cellsKey, ctx );
  if( !container.isValid() )
  {
    MSG_FATAL( "It's not possible to read the xAOD::CaloCellContainer from this Context using this key " << m_cellsKey );
  }

  const CellList candidates = buildSortedCandidates( **container.ptr() );
  const CellList selected   = pruneNeighbors( candidates );

  int seed_id = 0;
  for( const auto cell : selected )
  {
    seeds->push_back( new xAOD::Seed( seed_id++, cell->e(), cell->et(), cell->eta(), cell->phi() ) );
    MSG_INFO( "PeakScan seed " << seed_id-1 << " in eta = " << cell->eta() << ", phi = " << cell->phi() << ", e = " << cell->e() );
  }

  MSG_INFO( candidates.size() << " EM2 candidates above threshold -> " << selected.size() << " seeds after dR pruning" );
  return StatusCode::SUCCESS;
}

//!=====================================================================

PeakScanSeedMaker::CellList PeakScanSeedMaker::buildSortedCandidates( const CellList &cells ) const
{
  CellList candidates;

  for( const auto cell : cells )
  {
    const auto sampling = cell->descriptor()->sampling();
    if( sampling != CaloSampling::EMB2 && sampling != CaloSampling::EMEC2 ) continue;
    if( cell->e() <= m_energyThreshold ) continue;
    candidates.push_back( cell );
  }

  std::stable_sort( candidates.begin(), candidates.end(),
                    []( const xAOD::CaloCell *a, const xAOD::CaloCell *b ){ return a->e() > b->e(); } );
  return candidates;
}

//!=====================================================================

PeakScanSeedMaker::CellList PeakScanSeedMaker::pruneNeighbors( const CellList &sortedCandidates ) const
{
  CellList seeds;
  std::vector<bool> removed( sortedCandidates.size(), false );

  for( size_t i=0; i<sortedCandidates.size(); ++i )
  {
    if( removed[i] ) continue;
    seeds.push_back( sortedCandidates[i] );

    for( size_t j=i+1; j<sortedCandidates.size(); ++j )
    {
      if( !removed[j] && dR( sortedCandidates[i], sortedCandidates[j] ) <= m_minDistanceDR )
        removed[j] = true;
    }
  }
  return seeds;
}

//!=====================================================================

float PeakScanSeedMaker::dR( const xAOD::CaloCell *a, const xAOD::CaloCell *b ) const
{
  const float deta = a->eta() - b->eta();
  const float dphi = CaloPhiRange::diff( a->phi(), b->phi() );
  return std::sqrt( deta*deta + dphi*dphi );
}
