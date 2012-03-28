//---------------------------------Spheral++----------------------------------//
// ConnectivityMap
//
// Stores the full set of significant neighbors for a set of NodeLists.
//
// Created by J. Michael Owen, Sun Oct 30 15:36:33 PST 2005
//----------------------------------------------------------------------------//
#include <algorithm>

#include "ConnectivityMap.hh"
#include "NodeList/NodeList.hh"
#include "Neighbor/Neighbor.hh"
#include "DataBase/DataBase.hh"
#include "Field/FieldList.hh"
#include "Boundary/Boundary.hh"
#include "Utilities/globalNodeIDs.hh"
#include "Utilities/timingUtilities.hh"
#include "Utilities/mortonOrderIndicies.hh"
#include "Utilities/PairComparisons.hh"

#include "TAU.h"

namespace Spheral {
namespace NeighborSpace {

using namespace std;

using NodeSpace::NodeList;
using NodeSpace::FluidNodeList;
using DataBaseSpace::DataBase;
using FieldSpace::FieldList;
using FieldSpace::Field;
using BoundarySpace::Boundary;

//------------------------------------------------------------------------------
// Destructor.
//------------------------------------------------------------------------------
template<typename Dimension>
ConnectivityMap<Dimension>::
~ConnectivityMap() {
}

//------------------------------------------------------------------------------
// Internal method to build the connectivity for the requested set of NodeLists.
//------------------------------------------------------------------------------
template<typename Dimension>
void
ConnectivityMap<Dimension>::
patchConnectivity(const FieldList<Dimension, int>& flags,
                  const FieldList<Dimension, int>& old2new) {

  // TAU timers.
  TAU_PROFILE("ConnectivityMap::", "patchConnectivity", TAU_USER);

  // We have to recompute the keys to sort nodes by excluding the 
  // nodes that are being removed.
  const size_t numNodeLists = mConnectivity.size();
  if (mDomainDecompIndependent) {
    for (size_t iNodeList = 0; iNodeList != numNodeLists; ++iNodeList) {
      for (size_t i = 0; i != mNodeLists[iNodeList]->numNodes(); ++i) {
        if (flags(iNodeList, i) == 0) mKeys(iNodeList, i) = KeyTraits::maxKey;
      }
    }

    // DataBase<Dimension> dataBase;
    // for (typename vector<const NodeList<Dimension>*>::const_iterator itr = mNodeLists.begin();
    //      itr != mNodeLists.end();
    //      ++itr) {
    //   dataBase.appendNodeList(const_cast<NodeList<Dimension>&>(**itr));
    // }
    // mKeys = mortonOrderIndicies(dataBase, flags);
  }

  // Iterate over the Connectivity (NodeList).
  vector<size_t> iNodesToKill, jNodesToKill;
  vector<pair<int, Key> > keys, nkeys;
  for (size_t iNodeList = 0; iNodeList != numNodeLists; ++iNodeList) {
    iNodesToKill = vector<size_t>();
    keys = vector<pair<int, Key> >();

    // Walk the nodes of the NodeList.
    const size_t numNodes = (mDomainDecompIndependent ? 
                             mNodeLists[iNodeList]->numNodes() :
                             mNodeLists[iNodeList]->numInternalNodes());
    CHECK((*mConnectivity[iNodeList]).size() == numNodes);

    // Patch the traversal ordering and connectivity for this NodeList.
    for (size_t i = 0; i != numNodes; ++i) {

      // Should we patch this set of neighbors?
      if (flags(iNodeList, i) == 0) {
        iNodesToKill.push_back(i);
      } else {
        if (mDomainDecompIndependent) keys.push_back(make_pair(old2new(iNodeList, i), mKeys(iNodeList, i)));
        mNodeTraversalIndicies[iNodeList][i] = old2new(iNodeList, i);
        vector< vector<int> >& neighbors = *(*mConnectivity[iNodeList])[i];
        CHECK(neighbors.size() == numNodeLists);
        for (size_t jNodeList = 0; jNodeList != numNodeLists; ++jNodeList) {
          nkeys = vector<pair<int, Key> >();
          jNodesToKill = vector<size_t>();
          for (size_t k = 0; k != neighbors[jNodeList].size(); ++k) {
            const int j = neighbors[jNodeList][k];
            if (flags(jNodeList, j) == 0) {
              jNodesToKill.push_back(k);
            } else {
              if (mDomainDecompIndependent) nkeys.push_back(make_pair(old2new(jNodeList, j), mKeys(jNodeList, j)));
              neighbors[jNodeList][k] = old2new(jNodeList, j);
            }
          }
          removeElements(neighbors[jNodeList], jNodesToKill);

          // Recompute the ordering of the neighbors.
          if (mDomainDecompIndependent) {
            sort(nkeys.begin(), nkeys.end(), ComparePairsBySecondElement<pair<int, Key> >());
            for (size_t k = 0; k != neighbors[jNodeList].size(); ++k) {
              CHECK2(k == 0 or nkeys[k].second > nkeys[k-1].second,
                     "Incorrect neighbor ordering:  "
                     << i << " "
                     << k << " "
                     << nkeys[k-1].second << " "
                     << nkeys[k].second);
              neighbors[jNodeList][k] = nkeys[k].first;
            }
          } else {
            sort(neighbors[jNodeList].begin(), neighbors[jNodeList].end());
          }

        }
      }
    }
    removeElements(mNodeTraversalIndicies[iNodeList], iNodesToKill);
    removeElements(*mConnectivity[iNodeList], iNodesToKill);

    // Recompute the ordering for traversing the nodes.
    {
      const size_t numNodes = mNodeTraversalIndicies[iNodeList].size();
      if (mDomainDecompIndependent) {
        // keys = vector<pair<int, Key> >();
        // for (size_t k = 0; k != numNodes; ++k) {
        //   const int i = mNodeTraversalIndicies[iNodeList][k];
        //   keys.push_back(make_pair(i, mKeys(iNodeList, i)));
        // }
        sort(keys.begin(), keys.end(), ComparePairsBySecondElement<pair<int, Key> >());
        for (size_t k = 0; k != numNodes; ++k) {
          mNodeTraversalIndicies[iNodeList][k] = keys[k].first;
        }
      } else {
        for (int i = 0; i != numNodes; ++i) {
          mNodeTraversalIndicies[iNodeList][i] = i;
        }
      }
    }
  }
  
  // You can't check valid yet 'cause the NodeLists have not been resized
  // when we call patch!  The valid method should be checked by whoever called
  // this method after that point.
  //ENSURE(valid());
}

//------------------------------------------------------------------------------
// Return the connectivity in terms of global node IDs.
//------------------------------------------------------------------------------
template<typename Dimension>
map<int, vector<int> > 
ConnectivityMap<Dimension>::
globalConnectivity(vector<Boundary<Dimension>*>& boundaries) const {

  // Get the set of global node IDs.
  FieldList<Dimension, int> globalIDs = NodeSpace::globalNodeIDs<Dimension, typename vector<const NodeList<Dimension>*>::const_iterator>
    (mNodeLists.begin(), mNodeLists.end());

  // Make sure all ghost nodes have the appropriate global IDs.
  for (typename vector<Boundary<Dimension>*>::iterator boundItr = boundaries.begin();
       boundItr != boundaries.end();
       ++boundItr) (*boundItr)->applyFieldListGhostBoundary(globalIDs);
  for (typename vector<Boundary<Dimension>*>::iterator boundItr = boundaries.begin();
       boundItr != boundaries.end();
       ++boundItr) (*boundItr)->finalizeGhostBoundary();

  // Now convert our connectivity to global IDs.
  map<int, vector<int> > result;
  const size_t numNodeLists = mNodeLists.size();
  for (size_t nodeListi = 0; nodeListi != numNodeLists; ++nodeListi) {

    const NodeList<Dimension>* nodeListPtr = mNodeLists[nodeListi];
    for (int i = 0; i != nodeListPtr->numInternalNodes(); ++i) {
      const int gid = globalIDs(nodeListi, i);
      result[gid] = vector<int>();

      const vector< vector<int> >& fullConnectivity = connectivityForNode(nodeListPtr, i);
      CHECK(fullConnectivity.size() == numNodeLists);
      for (size_t nodeListj = 0; nodeListj != numNodeLists; ++nodeListj) {
        const vector<int>& connectivity = fullConnectivity[nodeListj];

        for (typename vector<int>::const_iterator jItr = connectivity.begin();
             jItr != connectivity.end();
             ++jItr) result[gid].push_back(globalIDs(nodeListj, *jItr));

      }
      ENSURE(result[gid].size() == numNeighborsForNode(nodeListPtr, i));
    }
  }

  // That's it.
  return result;
}

//------------------------------------------------------------------------------
// Compute the index for the given NodeList in our known set.
//------------------------------------------------------------------------------
template<typename Dimension>
unsigned
ConnectivityMap<Dimension>::
nodeListIndex(const NodeList<Dimension>* nodeListPtr) const {
  return distance(mNodeLists.begin(), 
                  find(mNodeLists.begin(), mNodeLists.end(), nodeListPtr));
}

//------------------------------------------------------------------------------
// Valid test.
//------------------------------------------------------------------------------
template<typename Dimension>
bool
ConnectivityMap<Dimension>::
valid() const {

  // TAU timers.
  TAU_PROFILE("ConnectivityMap", "::valid", TAU_USER);

  // Are the number of NodeLists consistent?
  const int numNodeLists = mNodeLists.size();
  if (mConnectivity.size() != numNodeLists) {
    cerr << "ConnectivityMap::valid: Failed mConnectivity.size() == numNodeLists" << endl;
    return false;
  }

  // Make sure that the NodeLists are listed in the correct sequence, and are
  // FluidNodeLists.
  {
    const NodeListRegistrar<Dimension>& registrar = NodeListRegistrar<Dimension>::instance();
    const vector<string> names = registrar.registeredNames();
    int lastPosition = -1;
    for (typename vector<const NodeList<Dimension>*>::const_iterator itr = mNodeLists.begin();
         itr != mNodeLists.end();
         ++itr) {
      const int newPosition = distance(names.begin(),
                                       find(names.begin(), names.end(), (*itr)->name()));
      if (newPosition <= lastPosition) {
        cerr << "ConnectivityMap::valid: Failed ordering of NodeLists" << endl;
        return false;
      }
      lastPosition = newPosition;
    }
  }

  // Iterate over each NodeList entered.
  for (typename ConnectivityStorageType::const_iterator connectivityItr = mConnectivity.begin();
       connectivityItr != mConnectivity.end();
       ++connectivityItr) {
    const vector< boost::shared_ptr< vector< vector<int> > > >& allNeighbors = **connectivityItr;

    // Are all internal nodes represented?
    const int nodeListIDi = distance(mConnectivity.begin(), connectivityItr);
    CHECK(nodeListIDi >= 0 and nodeListIDi < mNodeLists.size());
    const NodeList<Dimension>* nodeListPtri = mNodeLists[nodeListIDi];
    const int numNodes = mDomainDecompIndependent ? nodeListPtri->numNodes() : nodeListPtri->numInternalNodes();
    const int firstGhostNodei = nodeListPtri->firstGhostNode();
    if (allNeighbors.size() != numNodes) {
      cerr << "ConnectivityMap::valid: Failed test that all nodes set for NodeList "
           << mNodeLists[nodeListIDi]->name()
           << endl;
      return false;
    }

    // Iterate over the nodes for this NodeList.
    for (int i = 0; i != allNeighbors.size(); ++i) {

      // The set of neighbors for this node.  This has to be sized as the number of
      // NodeLists.
      const vector< vector<int> >& allNeighborsForNode = *(allNeighbors[i]);
      if (allNeighborsForNode.size() != numNodeLists) {
        cerr << "ConnectivityMap::valid: Failed allNeighborsForNode.size() == numNodeLists" << endl;
        return false;
      }

      // Iterate over the sets of NodeList neighbors for this node.
      for (int nodeListIDj = 0; nodeListIDj != numNodeLists; ++nodeListIDj) {
        const NodeList<Dimension>* nodeListPtrj = mNodeLists[nodeListIDj];
        const int firstGhostNodej = nodeListPtrj->firstGhostNode();
        const vector<int>& neighbors = allNeighborsForNode[nodeListIDj];

        // We require that the node IDs be sorted, unique, and of course in a valid range.
        if (neighbors.size() > 0) {
          const int minNeighbor = *min_element(neighbors.begin(), neighbors.end());
          const int maxNeighbor = *max_element(neighbors.begin(), neighbors.end());

          if (minNeighbor < 0 or maxNeighbor >= nodeListPtrj->numNodes()) {
            cerr << "ConnectivityMap::valid: Failed test that neighbors must be valid IDs" << endl;
            return false;
          }

          // When enforcing domain independence the ith node may be a ghost, but all of it's neighbors should
          // be internal.
          if (mDomainDecompIndependent and (i >= firstGhostNodei) and (maxNeighbor > firstGhostNodej)) {
            cerr << "ConnectivityMap::valid: Failed test that all neighbors of a ghost node should be internal." << endl;
            return false;
          }

          for (int k = 1; k < neighbors.size(); ++k) {
            if (mDomainDecompIndependent) {
              // In the case of domain decomposition reproducibility, neighbors are sorted
              // by hashed IDs.
              if (mKeys(nodeListIDj, neighbors[k]) < mKeys(nodeListIDj, neighbors[k - 1])) {
                cerr << "ConnectivityMap::valid: Failed test that neighbors must be sorted for node "
                     << i << endl;
                for (vector<int>::const_iterator itr = neighbors.begin();
                     itr != neighbors.end();
                     ++itr) cerr << *itr << " " 
                                 << mKeys(nodeListIDj, *itr) << " ";
                cerr << endl;
                return false;
              }

            } else {
              // Otherwise they should be sorted by local ID.
              if (neighbors[k] <= neighbors[k - 1]) {
                cerr << "ConnectivityMap::valid: Failed test that neighbors must be sorted" << endl;
                for (vector<int>::const_iterator itr = neighbors.begin();
                     itr != neighbors.end();
                     ++itr) cerr << " " << *itr;
                cerr << endl;
                return false;
              }
            }
          }
        }

        // Check that the connectivity is symmetric.
        for (vector<int>::const_iterator jItr = neighbors.begin();
             jItr != neighbors.end();
             ++jItr) {
          if (mDomainDecompIndependent or (*jItr < nodeListPtrj->numInternalNodes())) {
            const vector< vector<int> >& otherNeighbors = connectivityForNode(nodeListPtrj, *jItr);
            if (find(otherNeighbors[nodeListIDi].begin(),
                     otherNeighbors[nodeListIDi].end(),
                     i) == otherNeighbors[nodeListIDi].end()) {
              cerr << "ConnectivityMap::valid: Failed test that neighbors must be symmetric." << endl;
              return false;
            }
          }
        }

      }
    }
  }

  // Check that if we are using domain decompostion independence then the keys 
  // have been calculated.
  if (mDomainDecompIndependent) {
    for (typename vector<const NodeList<Dimension>*>::const_iterator itr = mNodeLists.begin();
         itr != mNodeLists.end();
         ++itr) {
      if (not mKeys.haveNodeList(**itr)) {
        cerr << "ConnectivityMap::valid: missing information from Keys." << endl;
        return false;
      }
    }
  }

  // Make sure all nodes are listed in the node index traversal stuff.
  if (mNodeTraversalIndicies.size() != mNodeLists.size()) {
    cerr << "ConnectivityMap::valid: mNodeTraversalIndicies wrong size!" << endl;
    return false;
  }
  for (int nodeList = 0; nodeList != numNodeLists; ++nodeList) {
    const int numExpected = mDomainDecompIndependent ? mNodeLists[nodeList]->numNodes() : mNodeLists[nodeList]->numInternalNodes();
    bool ok = mNodeTraversalIndicies[nodeList].size() == numExpected;
    for (int i = 0; i != numExpected; ++i) {
      ok = ok and (count(mNodeTraversalIndicies[nodeList].begin(),
                         mNodeTraversalIndicies[nodeList].end(),
                         i) == 1);
    }
    if (not ok) {
      cerr << "ConnectivityMap::valid: mNodeTraversalIndicies elements messed up!" << endl;
      return false;
    }
  }

  // Check that the node traversal is ordered correctly.
  for (int nodeList = 0; nodeList != numNodeLists; ++nodeList) {
    if ((mDomainDecompIndependent and mNodeLists[nodeList]->numNodes() > 0) or
        (not mDomainDecompIndependent and mNodeLists[nodeList]->numInternalNodes() > 0)) {
      const int firstGhostNode = mNodeLists[nodeList]->firstGhostNode();
      for (const_iterator itr = begin(nodeList); itr < end(nodeList) - 1; ++itr) {
        if (not calculatePairInteraction(nodeList, *itr,
                                         nodeList, *(itr + 1), 
                                         firstGhostNode)) {
          cerr << "ConnectivityMap::valid: mNodeTraversalIndicies ordered incorrectly." << endl;
          cerr << *itr << " "
               << *(itr + 1) << " "
               << mKeys(nodeList, *itr) << " "
               << mKeys(nodeList, *(itr + 1)) << " "
               << endl;
          for (int i = 0; i != 100; ++i) cerr << mKeys(nodeList, i) << " " << mNodeLists[nodeList]->positions()(i) << " ";
          cerr << endl;
          return false;
        }
      }
    }
  }

  // Everything must be OK.
  return true;
}

//------------------------------------------------------------------------------
// Internal method to build the connectivity for the requested set of NodeLists.
//------------------------------------------------------------------------------
template<typename Dimension>
void
ConnectivityMap<Dimension>::
computeConnectivity() {

  // TAU timers.
  TAU_PROFILE("ConnectivityMap::", "computeConnectivity", TAU_USER);
  TAU_PROFILE_TIMER(TimeCMBuildDB, "ConnectivityMap::", "computeConnectivity - build DataBase", TAU_USER);
  TAU_PROFILE_TIMER(TimeCMFillOrder, "ConnectivityMap::", "computeConnectivity - fill ordering", TAU_USER);
  TAU_PROFILE_TIMER(TimeCMAllocate, "ConnectivityMap::", "computeConnectivity - allocate memory", TAU_USER);
  TAU_PROFILE_TIMER(TimeCMMain, "ConnectivityMap::", "computeConnectivity - main loop", TAU_USER);
  TAU_PROFILE_TIMER(TimeCMGhost, "ConnectivityMap::", "computeConnectivity - ghost neighbors", TAU_USER);

  typedef typename Dimension::Scalar Scalar;
  typedef typename Dimension::Vector Vector;
  typedef typename Dimension::Tensor Tensor;
  typedef typename Dimension::SymTensor SymTensor;
  typedef Timing::Time Time;

  // Erase any prior information.
  TAU_PROFILE_START(TimeCMBuildDB);
  mConnectivity = ConnectivityStorageType();
  mNodeTraversalIndicies = vector< vector<int> >();

  // Build ourselves a temporary DataBase with the set of NodeLists.
  // Simultaneously find the maximum kernel extent.
  DataBase<Dimension> dataBase;
  double kernelExtent = 0.0;
  for (typename vector<const NodeList<Dimension>*>::const_iterator itr = mNodeLists.begin();
       itr != mNodeLists.end();
       ++itr) {
    dataBase.appendNodeList(const_cast<NodeList<Dimension>&>(**itr));
    kernelExtent = max(kernelExtent, (**itr).neighbor().kernelExtent());
  }
  const double kernelExtent2 = kernelExtent*kernelExtent;
  TAU_PROFILE_STOP(TimeCMBuildDB);

  // If we're trying to be domain decomposition independent, we need a key to sort
  // by that will give us a unique ordering regardless of position.  The Morton ordered
  // hash fills the bill.
  TAU_PROFILE_START(TimeCMFillOrder);
  typedef typename KeyTraits::Key Key;
  const NodeListRegistrar<Dimension>& registrar = NodeListRegistrar<Dimension>::instance();
  if (mDomainDecompIndependent) mKeys = mortonOrderIndicies(dataBase);

  // Fill in the ordering for walking the nodes.
  const unsigned numNodeLists = dataBase.numNodeLists();
  if (mDomainDecompIndependent) {
    for (int iNodeList = 0; iNodeList != numNodeLists; ++iNodeList) {
      const NodeList<Dimension>& nodeList = *mNodeLists[iNodeList];
      mNodeTraversalIndicies.push_back(vector<int>(nodeList.numNodes()));
      CHECK(mNodeTraversalIndicies.size() == iNodeList + 1);
      vector<pair<int, Key> > keys;
      keys.reserve(nodeList.numNodes());
      for (int i = 0; i != nodeList.numNodes(); ++i) keys.push_back(pair<int, Key>(i, mKeys(iNodeList, i)));
      sort(keys.begin(), keys.end(), ComparePairsBySecondElement<pair<int, Key> >());
      for (int i = 0; i != nodeList.numNodes(); ++i) mNodeTraversalIndicies[iNodeList][i] = keys[i].first;
      CHECK(mNodeTraversalIndicies[iNodeList].size() == nodeList.numNodes());
    }
  } else {
    for (int iNodeList = 0; iNodeList != numNodeLists; ++iNodeList) {
      const NodeList<Dimension>& nodeList = *mNodeLists[iNodeList];
      mNodeTraversalIndicies.push_back(vector<int>(nodeList.numInternalNodes()));
      CHECK(mNodeTraversalIndicies.size() == iNodeList + 1);
      for (int i = 0; i != nodeList.numInternalNodes(); ++i) mNodeTraversalIndicies[iNodeList][i] = i;
    }
  }
  CHECK(mNodeTraversalIndicies.size() == numNodeLists);
  TAU_PROFILE_STOP(TimeCMFillOrder);

  // Allocate the memory for storing the connectivity.
  TAU_PROFILE_START(TimeCMAllocate);
  mConnectivity.reserve(numNodeLists);
  for (typename DataBase<Dimension>::NodeListIterator itr = dataBase.nodeListBegin();
       itr != dataBase.nodeListEnd();
       ++itr) {
    typedef vector< boost::shared_ptr< vector< vector<int> > > > Thingy;
    mConnectivity.push_back(boost::shared_ptr<Thingy>(new Thingy()));
    Thingy& connectivity = *(mConnectivity.back());
    const int numNodes = (mDomainDecompIndependent ? 
                          (*itr)->numNodes() : 
                          (*itr)->numInternalNodes());
    connectivity.reserve(numNodes);
    for (int i = 0; i != numNodes; ++i) {
      connectivity.push_back(boost::shared_ptr< vector< vector<int> > >(new vector< vector<int> >(numNodeLists)));
    }
  }

  // Create a list of flags to keep track of which nodes have been completed thus far.
  FieldList<Dimension, int> flagNodeDone = dataBase.newGlobalFieldList(int());
  flagNodeDone = 0;

  // Get the position and H fields.
  const FieldList<Dimension, Vector> position = dataBase.globalPosition();
  const FieldList<Dimension, SymTensor> H = dataBase.globalHfield();
  TAU_PROFILE_STOP(TimeCMAllocate);

  // Predeclare stuff we're going to use in the loop.
  unsigned iiNodeList, ii, iNodeList, jNodeList;
  int i, firstGhostNodej, j, k;
  typename Neighbor<Dimension>::const_iterator masterItr, neighborItr;
  Time start;
  vector<vector<pair<int, double> > > distances;
  vector<vector<pair<int, Key> > > keys;
  Vector rij;
  Scalar eta2i, eta2j;

  // Iterate over the NodeLists.
  TAU_PROFILE_START(TimeCMMain);
  for (iiNodeList = 0; iiNodeList != numNodeLists; ++iiNodeList) {

    // Iterate over the internal nodes in this NodeList, and look
    // for any that are not done yet.
    for (ii = 0; ii != mNodeLists[iiNodeList]->numInternalNodes(); ++ii) {
      if (flagNodeDone(iiNodeList, ii) == 0) {
      
        // Set the master nodes.
        Neighbor<Dimension>::setMasterNeighborGroup(position(iiNodeList, ii),
                                                    H(iiNodeList, ii),
                                                    mNodeLists.begin(),
                                                    mNodeLists.end(),
                                                    mNodeLists[iiNodeList]->neighbor().kernelExtent());

        // Iterate over the full of NodeLists again to work on the master nodes.
        for (iNodeList = 0; iNodeList != numNodeLists; ++iNodeList) {
          const Neighbor<Dimension>& neighbori = mNodeLists[iNodeList]->neighbor();
          CHECK(neighbori.valid());
          Field<Dimension, Scalar>& worki = mNodeLists[iNodeList]->work();

          // Iterate over the master nodes in this NodeList.
          for (masterItr = neighbori.masterBegin();
               masterItr != neighbori.masterEnd();
               ++masterItr) {
            i = *masterItr;
            start = Timing::currentTime();
            CHECK(flagNodeDone(iNodeList, i) == 0);

            // Get the neighbor set we're building for this node.
            vector< vector<int> >& neighbors = *(*mConnectivity[iNodeList])[i];
            CHECK(neighbors.size() == numNodeLists);

            // We keep track of the Morton indicies.
            keys = vector<vector<pair<int, Key> > >(numNodeLists);

            // Get the state for this node.
            const Vector& ri = position(iNodeList, i);
            const SymTensor& Hi = H(iNodeList, i);

            // Iterate over the neighbor NodeLists.
            for (jNodeList = 0; jNodeList != numNodeLists; ++jNodeList) {
              Neighbor<Dimension>& neighborj = mNodeLists[jNodeList]->neighbor();
              firstGhostNodej = mNodeLists[jNodeList]->firstGhostNode();

              // Set the refine neighbors.
              neighborj.setRefineNeighborList(ri, Hi);

              // Iterate over the neighbors in this NodeList.
              for (neighborItr = neighborj.refineNeighborBegin();
                   neighborItr != neighborj.refineNeighborEnd();
                   ++neighborItr) {
                j = *neighborItr;

                // Get the neighbor state.
                const Vector& rj = position(jNodeList, j);
                const SymTensor& Hj = H(jNodeList, j);

                // Compute the normalized distance between this pair.
                rij = ri - rj;
                eta2i = (Hi*rij).magnitude2();
                eta2j = (Hj*rij).magnitude2();

                // If this pair is significant, add it to the list.
                if (eta2i <= kernelExtent2 or eta2j <= kernelExtent2) {

                  // We don't include self-interactions.
                  if ((iNodeList != jNodeList) or (i != j)) {
                    neighbors[jNodeList].push_back(j);

                    if (mDomainDecompIndependent) {
                      keys[jNodeList].push_back(pair<int, Key>(j, mKeys(jNodeList, j)));
                      // In this case we also need to have ghost nodes aware of any internal neighbors.
                      if (j >= firstGhostNodej) {
                        vector< vector<int> >& otherNeighbors = *(*mConnectivity[jNodeList])[j];
                        CHECK(otherNeighbors.size() == numNodeLists);
                        otherNeighbors[iNodeList].push_back(i);
                      }
                    }

                  }
                }
              }
            }
            CHECK(neighbors.size() == numNodeLists);
            CHECK(keys.size() == numNodeLists);
        
            // We have a few options for how to order the neighbors for this node.
            for (k = 0; k != numNodeLists; ++k) {

              if (mDomainDecompIndependent) {
                // Sort in a domain independent manner.
                CHECK(keys[k].size() == neighbors[k].size());
                sort(keys[k].begin(), keys[k].end(), ComparePairsBySecondElement<pair<int, Key> >());
                for (j = 0; j != neighbors[k].size(); ++j) neighbors[k][j] = keys[k][j].first;

              } else {
                // Sort in an attempt to be cache friendly.
                sort(neighbors[k].begin(), neighbors[k].end());

              }
            }

            // Flag this master node as done.
            flagNodeDone(iNodeList, i) = 1;
            worki(i) += Timing::difference(start, Timing::currentTime());
          }
        }
      }
    }
  }
  TAU_PROFILE_STOP(TimeCMMain);

  // In the domain decompostion independent case, we need to sort the neighbors for ghost
  // nodes as well.
  TAU_PROFILE_START(TimeCMGhost);
  if (mDomainDecompIndependent) {
    for (int iNodeList = 0; iNodeList != numNodeLists; ++iNodeList) {
      const NodeList<Dimension>* nodeListPtr = mNodeLists[iNodeList];
      for (int i = nodeListPtr->firstGhostNode();
           i != nodeListPtr->numNodes();
           ++i) {
        vector< vector<int> >& neighbors = *(*mConnectivity[iNodeList])[i];
        CHECK(neighbors.size() == numNodeLists);
        for (int jNodeList = 0; jNodeList != numNodeLists; ++jNodeList) {
          vector<pair<int, Key> > keys;
          keys.reserve(neighbors[jNodeList].size());
          for (vector<int>::const_iterator itr = neighbors[jNodeList].begin();
               itr != neighbors[jNodeList].end();
               ++itr) keys.push_back(pair<int, Key>(*itr, mKeys(jNodeList, *itr)));
          CHECK(keys.size() == neighbors[jNodeList].size());
          sort(keys.begin(), keys.end(), ComparePairsBySecondElement<pair<int, Key> >());
          for (int k = 0; k != keys.size(); ++k) neighbors[jNodeList][k] = keys[k].first;
        }
      }
    }
  }
  TAU_PROFILE_STOP(TimeCMGhost);

  // Post conditions.
  BEGIN_CONTRACT_SCOPE;
  // Make sure that all internal nodes have been completed.
  for (InternalNodeIterator<Dimension> nodeItr = dataBase.internalNodeBegin();
       nodeItr != dataBase.internalNodeEnd();
       ++nodeItr) ENSURE(flagNodeDone(nodeItr) == 1);
  // Make sure we're ready to be used.
  ENSURE(valid());
  END_CONTRACT_SCOPE;

}

}
}

//------------------------------------------------------------------------------
// Explicit instantiation.
//------------------------------------------------------------------------------
#include "Geometry/Dimension.hh"
template class Spheral::NeighborSpace::ConnectivityMap<Spheral::Dim<1> >;
template class Spheral::NeighborSpace::ConnectivityMap<Spheral::Dim<2> >;
template class Spheral::NeighborSpace::ConnectivityMap<Spheral::Dim<3> >;
