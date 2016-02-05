//---------------------------------Spheral++----------------------------------//
// CRKSPHUtilities
//
// Useful methods for using the CRKSPH formalism.
//
// Created by JMO, Fri Aug  8 16:16:33 PDT 2008
//----------------------------------------------------------------------------//
#include "Kernel/TableKernel.hh"
#include "Geometry/innerDoubleProduct.hh"
#include "Geometry/innerProduct.hh"
#include "CRKSPHCorrectionParams.hh"

namespace Spheral {
namespace CRKSPHSpace {

//------------------------------------------------------------------------------
// Compute the corrected kernel value.
//------------------------------------------------------------------------------
template<typename Dimension>
inline
typename Dimension::Scalar
CRKSPHKernel(const KernelSpace::TableKernel<Dimension>& W,
           const CRKOrder correctionOrder,
           const typename Dimension::Vector& rij,
           const typename Dimension::Vector& etai,
           const typename Dimension::Scalar& Hdeti,
           const typename Dimension::Vector& etaj,
           const typename Dimension::Scalar& Hdetj,
           const typename Dimension::Scalar& Ai,
           const typename Dimension::Vector& Bi,
           const typename Dimension::Tensor& Ci) {
  typedef typename Dimension::Scalar Scalar;
  typedef typename Dimension::Vector Vector;
  typedef typename Dimension::Tensor Tensor;

  // Symmetrize eta first, giving us an effective hij.
  Scalar Wij;
  const Scalar rijmag = rij.magnitude();
  if (rijmag < 1.0e-10) {
    Wij = 0.5*(W(0.0, Hdeti) + W(0.0, Hdetj));
  } else {
    const Vector etaij = 0.5*(etai + etaj);
    const Scalar hi = rijmag*safeInvVar(etai.magnitude());
    const Scalar hj = rijmag*safeInvVar(etaj.magnitude());
    const Scalar hij = rijmag*safeInvVar(etaij.magnitude());
    const Scalar Hdetii = Hdeti*Dimension::pownu(hi/hij);
    const Scalar Hdetjj = Hdetj*Dimension::pownu(hj/hij);
    Wij = 0.5*(W(etaij.magnitude(), Hdetii) + W(etaij.magnitude(), Hdetjj));
  }
  // j
  // const Scalar Wij = W(etaj.magnitude(), Hdetj);
  // i
  // const Scalar Wij = W(etai.magnitude(), Hdeti);
  // ij
  // const Scalar Wij = 0.5*(W(etai.magnitude(), Hdeti) + 
  //                         W(etaj.magnitude(), Hdetj));
  if (correctionOrder == ZerothOrder) {
    return Ai*Wij;
  } else if(correctionOrder == LinearOrder) {
    return Ai*(1.0 + Bi.dot(rij))*Wij;
  } else {   //correctionOrder == QuadraticOrder
    return Ai*(1.0 + Bi.dot(rij) + Geometry::innerDoubleProduct<Dimension>(Ci, rij.selfdyad()))*Wij;
  }
}

//------------------------------------------------------------------------------
// Compute the corrected kernel value and gradient.
//------------------------------------------------------------------------------
template<typename Dimension>
inline
void
CRKSPHKernelAndGradient(const KernelSpace::TableKernel<Dimension>& W,
                        const CRKOrder correctionOrder,
                        const typename Dimension::Vector& rij,
                        const typename Dimension::Vector& etai,
                        const typename Dimension::SymTensor& Hi,
                        const typename Dimension::Scalar& Hdeti,
                        const typename Dimension::Vector& etaj,
                        const typename Dimension::SymTensor& Hj,
                        const typename Dimension::Scalar& Hdetj,
                        const typename Dimension::Scalar& Ai,
                        const typename Dimension::Vector& Bi,
                        const typename Dimension::Tensor& Ci,
                        const typename Dimension::Vector& gradAi,
                        const typename Dimension::Tensor& gradBi,
                        const typename Dimension::ThirdRankTensor& gradCi,
                        typename Dimension::Scalar& WCRKSPH,
                        typename Dimension::Scalar& gradWSPH,
                        typename Dimension::Vector& gradWCRKSPH) {
  typedef typename Dimension::Scalar Scalar;
  typedef typename Dimension::Vector Vector;
  typedef typename Dimension::Tensor Tensor;

  // Symmetrize eta first, giving us an effective hij.
  Scalar Wij;
  Vector gradWij;
  const Scalar rijmag = rij.magnitude();
  if (rijmag < 1.0e-10) {
    const std::pair<Scalar, Scalar> WWi = W.kernelAndGradValue(0.0, Hdeti);
    const std::pair<Scalar, Scalar> WWj = W.kernelAndGradValue(0.0, Hdetj);
    Wij = 0.5*(WWi.first + WWi.second);
    gradWij = Vector::zero;
    gradWSPH = 0.5*(WWi.second + WWj.second);
  } else {
    const Vector etaij = 0.5*(etaj + etai);
    const Scalar hi = rijmag*safeInvVar(etai.magnitude());
    const Scalar hj = rijmag*safeInvVar(etaj.magnitude());
    const Scalar hij = rijmag*safeInvVar(etaij.magnitude());
    const Scalar Hdetii = Hdeti*Dimension::pownu(hi/hij);
    const Scalar Hdetjj = Hdetj*Dimension::pownu(hj/hij);
    const std::pair<Scalar, Scalar> WWi = W.kernelAndGradValue(etaij.magnitude(), Hdetii);
    const std::pair<Scalar, Scalar> WWj = W.kernelAndGradValue(etaij.magnitude(), Hdetjj);
    Wij = 0.5*(WWi.first + WWj.first); 
    gradWij = 0.5*((hi/hij*Hi)*etaij.unitVector() * WWi.second + (hj/hij*Hj)*etaij.unitVector() * WWj.second);
    gradWSPH = 0.5*(WWi.second + WWj.second);
  }

  // j
  // const std::pair<Scalar, Scalar> WWj = W.kernelAndGradValue(etaj.magnitude(), Hdetj);
  // const Scalar Wij = WWj.first; 
  // const Vector gradWij = Hj*etaj.unitVector() * WWj.second;
  // gradWSPH = WWj.second;
  // i
  // const std::pair<Scalar, Scalar> WWi = W.kernelAndGradValue(etai.magnitude(), Hdeti);
  // const Scalar Wij = WWi.first; 
  // const Vector gradWij = Hi*etai.unitVector() * WWi.second;
  // gradWSPH = WWi.second;
  // ij
  // const std::pair<Scalar, Scalar> WWi = W.kernelAndGradValue(etai.magnitude(), Hdeti);
  // const std::pair<Scalar, Scalar> WWj = W.kernelAndGradValue(etaj.magnitude(), Hdetj);
  // const Scalar Wij = 0.5*(WWi.first + WWj.first); 
  // const Vector gradWij = 0.5*(Hi*etai.unitVector() * WWi.second +
  //                             Hj*etaj.unitVector() * WWj.second);
  // gradWSPH = 0.5*(WWi.second + WWj.second);
  if (correctionOrder == ZerothOrder) {
    WCRKSPH = Ai*Wij;
    gradWCRKSPH = Ai*gradWij + gradAi*Wij;
  } else if(correctionOrder == LinearOrder) {
    WCRKSPH = Ai*(1.0 + Bi.dot(rij))*Wij;
    gradWCRKSPH = Ai*(1.0 + Bi.dot(rij))*gradWij + Ai*Bi*Wij + gradAi*(1.0 + Bi.dot(rij))*Wij;
    for (size_t ii = 0; ii != Dimension::nDim; ++ii) {
      for (size_t jj = 0; jj != Dimension::nDim; ++jj) {
        gradWCRKSPH(ii) += Ai*Wij*gradBi(jj,ii)*rij(jj);
      }
    }
  } else {  //correctionOrder == QuadraticOrder
    WCRKSPH = Ai*(1.0 + Bi.dot(rij) + Geometry::innerDoubleProduct<Dimension>(Ci, rij.selfdyad()))*Wij;
    gradWCRKSPH = Ai*(1.0 + Bi.dot(rij) + Geometry::innerDoubleProduct<Dimension>(Ci, rij.selfdyad()))*gradWij + Ai*Bi*Wij;
    gradWCRKSPH += gradAi*(1.0 + Bi.dot(rij) + Geometry::innerDoubleProduct<Dimension>(Ci, rij.selfdyad()))*Wij;
    gradWCRKSPH += Ai*(Geometry::innerProduct<Dimension>(rij,gradBi))*Wij;
    gradWCRKSPH += Ai*(Geometry::innerDoubleProduct<Dimension>(rij.selfdyad(),gradCi))*Wij;
    gradWCRKSPH += 2.0*Ai*(Geometry::innerProduct<Dimension>(rij,Ci))*Wij;
  }
}

}
}
