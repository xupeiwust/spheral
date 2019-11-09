//------------------------------------------------------------------------------
// Compute the moments necessary for CRKSPH corrections.
//------------------------------------------------------------------------------
#ifndef __Spheral__computeRKVolumes__
#define __Spheral__computeRKVolumes__

#include <vector>
#include "CRKSPHCorrectionParams.hh"
#include "Geometry/CellFaceFlag.hh"

template<typename Dimension> class ConnectivityMap;
template<typename Dimension> class TableKernel;
template<typename Dimension, typename DataType> class FieldList;

namespace Spheral {

template<typename Dimension>
void
computeRKVolumes(const ConnectivityMap<Dimension>& connectivityMap,
                 const TableKernel<Dimension>& W,
                 const FieldList<Dimension, typename Dimension::Vector>& position,
                 const FieldList<Dimension, typename Dimension::Scalar>& mass,
                 const FieldList<Dimension, typename Dimension::Scalar>& massDensity,
                 const FieldList<Dimension, typename Dimension::SymTensor>& H,
                 const FieldList<Dimension, typename Dimension::SymTensor>& damage,
                 const CRKVolumeType volumeType,
                 FieldList<Dimension, int>& surfacePoint,
                 FieldList<Dimension, typename Dimension::Vector>& deltaCentroid,
                 FieldList<Dimension, std::vector<typename Dimension::Vector>>& etaVoidPoints,
                 FieldList<Dimension, typename Dimension::FacetedVolume>& cells,
                 FieldList<Dimension, std::vector<CellFaceFlag>>& cellFaceFlags,
                 FieldList<Dimension, typename Dimension::Scalar>& volume);

}

#endif
