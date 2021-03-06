/*
 * Copyright (c) 2015, The Regents of the University of California (Regents).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *    3. Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Please contact the author(s) of this library if you have any questions.
 * Authors: David Fridovich-Keil   ( dfk@eecs.berkeley.edu )
 *          Erik Nelson            ( eanelson@eecs.berkeley.edu )
 */

///////////////////////////////////////////////////////////////////////////////
//
// Wrapper around the FLANN library for approximate nearest neighbor searches.
// This is useful for finding the nearest Point in a collection -- e.g. for
// insertion into a RRT.
//
///////////////////////////////////////////////////////////////////////////////

#include <flann/flann_point_2dtree.h>
#include <glog/logging.h>
#include <iostream>
#include <cmath>

namespace path {

  FlannPoint2DTree::~FlannPoint2DTree() {
    // Free memory from points in the kd tree.
    if (index_ != nullptr) {
      for (size_t ii = 0; ii < index_->size(); ++ii) {
        double* point = index_->getPoint(ii);
        delete[] point;
      }
    }
  }

  // Number of points in the tree.
  int FlannPoint2DTree::Size() const {
    return index_->size();
  }

  // Add points to the index.
  void FlannPoint2DTree::AddPoint(Point2D::Ptr point) {
    CHECK_NOTNULL(point.get());

    // Add point to registry.
    registry_.push_back(point);

    // Copy the input point into FLANN's Matrix type.
    const int kNumColumns = 2;
    flann::Matrix<double> flann_point(new double[kNumColumns], 1, kNumColumns);
    flann_point[0][0] = point->x;
    flann_point[0][1] = point->y;

    // If this is the first point in the index, create the index and exit.
    if (index_ == nullptr) {
      // Single kd-tree. No approximation.
      const int kNumRandomizedKDTrees = 1;
      index_.reset(new flann::Index< flann::L2<double> >(
         flann_point, flann::KDTreeIndexParams(kNumRandomizedKDTrees)));
      index_->buildIndex();
      return;
    }

    // If the index is already created, add the data point to the index. Rebuild
    // every time the index doubles in size to occasionally rebalance the kd tree.
    const int kRebuildThreshold = 2;
    index_->addPoints(flann_point, kRebuildThreshold);
  }

  // Add points to the index.
  void FlannPoint2DTree::AddPoints(std::vector<Point2D::Ptr>& points) {
    for (auto& point : points) {
      AddPoint(point);
    }
  }

  // Queries the kd tree for the nearest neighbor of 'query'.
  bool FlannPoint2DTree::NearestNeighbor(Point2D::Ptr query, Point2D::Ptr& nearest,
                                         float& nn_distance) const {
    CHECK_NOTNULL(query.get());

    if (index_ == nullptr) {
      VLOG(1) << "Index has not been built. Points must be added before "
              <<  "querying the kd tree";
      return false;
    }

    // Convert the input point to the FLANN format.
    const int kNumColumns = 2;
    flann::Matrix<double> flann_query(new double[kNumColumns], 1, kNumColumns);
    flann_query[0][0] = query->x;
    flann_query[0][1] = query->y;

    // Search the kd tree for the nearest neighbor to the query.
    std::vector< std::vector<int> > query_match_indices;
    std::vector< std::vector<double> > query_distances;

    const int kOneNearestNeighbor = 1;
    int num_neighbors_found = index_->knnSearch(
       flann_query, query_match_indices, query_distances, kOneNearestNeighbor,
       flann::SearchParams(flann::FLANN_CHECKS_UNLIMITED) /* no approx */);

    // If we found a nearest neighbor, assign output.
    if (num_neighbors_found > 0) {
      nearest = registry_[ query_match_indices[0][0] ];
      nn_distance = std::sqrt(query_distances[0][0]);
      return true;
    }

    return false;
  }

  // Queries the kd tree for all neighbors of 'query' within the specified radius.
  // Returns whether or not the search exited successfully.
  bool FlannPoint2DTree::RadiusSearch(Point2D::Ptr query,
                                      std::vector<Point2D::Ptr>& neighbors,
                                      float radius) const {
    CHECK_NOTNULL(query.get());

    if (index_ == nullptr) {
      VLOG(1) << "Index has not been built. Points must be added before "
              << "querying the kd tree";
      return false;
    }

    // Convert the input point to the FLANN format.
    const int kNumColumns = 2;
    flann::Matrix<double> flann_query(new double[kNumColumns], 1, kNumColumns);
    flann_query[0][0] = query->x;
    flann_query[0][1] = query->y;

    // Search the kd tree for the nearest neighbor to the query.
    std::vector< std::vector<int> > query_match_indices;
    std::vector< std::vector<double> > query_distances;

    int num_neighbors_found =
      index_->radiusSearch(flann_query, query_match_indices,
                           query_distances, static_cast<float>(radius),
                           flann::SearchParams(flann::FLANN_CHECKS_UNLIMITED));

    // If we found a nearest neighbor, assign output.
    neighbors.clear();
    for (size_t ii = 0; ii < num_neighbors_found; ii++)
      neighbors.push_back(registry_[ query_match_indices[0][ii] ]);

    return true;
  }

}  //\namespace path
