__all__ = ["PeakScanSeedMaker"]

from GaugiKernel import Cpp
from GaugiKernel.macros import *
from RootStreamBuilder import RootStreamAODFlags as flags
import ROOT

class PeakScanSeedMaker( Cpp ):
  """
  Python wrapper for the C++ PeakScanSeedMaker algorithm.

  Finds seeds directly from the calorimeter cells (no truth): EM2 cells above an energy
  threshold, sorted by energy, with neighbors within MinDistanceDR removed.
  """

  def __init__( self, name,
                InputCellsKey   : str,
                OutputSeedsKey  : str,
                EnergyThreshold : float=flags.PeakScanEnergyThreshold,
                MinDistanceDR   : float=flags.PeakScanMinDR,
                OutputLevel     : int=0,
              ):
    """
    Args:
        name (str): Name of the algorithm instance.
        InputCellsKey (str): StoreGate key for input cells.
        OutputSeedsKey (str): StoreGate key for the output seeds.
        EnergyThreshold (float): Minimum EM2 cell energy to be a seed candidate (MeV).
        MinDistanceDR (float): Minimum dR between two accepted seeds.
        OutputLevel (int): Logging level.
    """

    Cpp.__init__(self, ROOT.PeakScanSeedMaker(name) )

    self.setProperty( "InputCellsKey"   , InputCellsKey   )
    self.setProperty( "OutputSeedsKey"  , OutputSeedsKey  )
    self.setProperty( "EnergyThreshold" , EnergyThreshold )
    self.setProperty( "MinDistanceDR"   , MinDistanceDR   )
    self.setProperty( "OutputLevel"     , OutputLevel     )
