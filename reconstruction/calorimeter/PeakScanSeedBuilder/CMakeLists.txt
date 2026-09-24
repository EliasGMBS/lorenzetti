
file(GLOB SOURCES src/*.cxx )
file(GLOB_RECURSE HEADERS src/PeakScanSeedMaker.h )

include_directories(${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../events/CaloCell)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../events/EventInfo)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../core/GaugiKernel)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../core/G4Kernel)


ROOT_GENERATE_DICTIONARY(PeakScanSeedBuilderDict ${HEADERS} LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/src/LinkDef.h  MODULE PeakScanSeedBuilder)

add_library(PeakScanSeedBuilder  OBJECT ${SOURCES} PeakScanSeedBuilderDict.cxx)
install(FILES ${HEADERS}  DESTINATION PeakScanSeedBuilder)


gaugi_install_python_modules( ${CMAKE_CURRENT_SOURCE_DIR}/python PeakScanSeedBuilder)
