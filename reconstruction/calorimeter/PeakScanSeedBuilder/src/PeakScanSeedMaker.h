#ifndef PeakScanSeedMaker_h
#define PeakScanSeedMaker_h

#include "GaugiKernel/StatusCode.h"
#include "GaugiKernel/DataHandle.h"
#include "GaugiKernel/Algorithm.h"
#include "CaloCell/CaloCellContainer.h"
#include "EventInfo/SeedContainer.h"
#include <string>
#include <vector>

/**
 * @class PeakScanSeedMaker
 * @brief Builds a SeedContainer directly from the calorimeter cells, with no truth information.
 *
 * Scans the EM2 layers (EMB2/EMEC2) for cells above an energy threshold, sorts them by
 * decreasing energy and keeps, greedily, only the cells that are farther than MinDistanceDR
 * from every seed already accepted. The output can be used as InputSeedsKey of CaloClusterMaker.
 */
class PeakScanSeedMaker : public Gaugi::Algorithm
{
  public:
    PeakScanSeedMaker( std::string );

    virtual ~PeakScanSeedMaker()=default;

    virtual StatusCode initialize() override;

    virtual StatusCode execute( SG::EventContext &ctx, int /*evt*/ ) const override;

    virtual StatusCode post_execute( SG::EventContext &ctx ) const override;

    // Required by IAlgorithm, but this algorithm has no histograms and no per-step work.
    virtual StatusCode bookHistograms( SG::EventContext & ) const override { return StatusCode::SUCCESS; }
    virtual StatusCode fillHistograms( SG::EventContext & ) const override { return StatusCode::SUCCESS; }
    virtual StatusCode pre_execute( SG::EventContext & ) const override { return StatusCode::SUCCESS; }
    virtual StatusCode execute( SG::EventContext &, const G4Step * ) const override { return StatusCode::SUCCESS; }
    virtual StatusCode finalize() override { return StatusCode::SUCCESS; }

  private:

    typedef std::vector<const xAOD::CaloCell*> CellList;

    /*! EM2 cells above the energy threshold, sorted by decreasing energy */
    CellList buildSortedCandidates( const CellList &cells ) const;

    /*! Greedy selection: highest energy first, drops everything within MinDistanceDR of an accepted cell */
    CellList pruneNeighbors( const CellList &sortedCandidates ) const;

    float dR( const xAOD::CaloCell *a, const xAOD::CaloCell *b ) const;

    std::string m_cellsKey;
    std::string m_seedsKey;

    float m_energyThreshold;
    float m_minDistanceDR;
};

#endif
