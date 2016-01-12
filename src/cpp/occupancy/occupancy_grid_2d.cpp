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
 * Author: David Fridovich-Keil   ( dfk@eecs.berkeley.edu )
 */

///////////////////////////////////////////////////////////////////////////////
//
// This class defines a 2D occupancy grid.
//
///////////////////////////////////////////////////////////////////////////////

#include "occupancy_grid_2d.h"
#include <geometry/point_2d.h>
#include <scene/obstacle_2d.h>

#include <cmath>
#include <glog/logging.h>

namespace path {

  // Constructor.
  OccupancyGrid2D::OccupancyGrid2D(double xmin, double xmax,
                                   double ymin, double ymax,
                                   double block_size)
    : xmin_(xmin), xmax_(xmax),
      ymin_(ymin), ymax_(ymax) {

    // Set scene.
    scene_.SetBounds(xmin, xmax, ymin, ymax);

    // Determine nrows and ncols.
    nrows_ = static_cast<int>(std::ceil((ymax_ - ymin_) / block_size));
    ncols_ = static_cast<int>(std::ceil((xmax_ - xmin_) / block_size));
    block_size_ = std::max((xmax_ - xmin_) / static_cast<double>(ncols_),
                           (ymax_ - ymin_) / static_cast<double>(nrows_));

    // Create the grid.
    grid_ = MatrixXi::Zero(nrows_, ncols_);
  }

  // Insert a point.
  void OccupancyGrid2D::Insert(Point::Ptr point) {
    if (!IsValidPoint(point)) return;
    Point2D *point2d = std::static_pointer_cast<Point2D>(point).get();

    // Find the nearest bin and insert.
    int ii = static_cast<int>(static_cast<double>(nrows_) *
                              (point2d->GetY() - ymin_) / (ymax_ - ymin_));
    int jj = static_cast<int>(static_cast<double>(ncols_) *
                              (point2d->GetX() - xmin_) / (xmax_ - xmin_));
    ii = nrows_ - ii;
    grid_(ii, jj)++;

    // Add to scene if bin is empty.
    if (GetCountAt(point) == 1) {
      Obstacle::Ptr obstacle = Obstacle2D::Create(GetBinCenter(point),
                                                  0.5 * block_size_);
      scene_.AddObstacle(obstacle);
    }
  }

  // Get number of points in the bin containing the specified point.
  int OccupancyGrid2D::GetCountAt(Point::Ptr point) const {
    if (!IsValidPoint(point)) return -1;
    Point2D *point2d = std::static_pointer_cast<Point2D>(point).get();

    // Get count.
    int ii = static_cast<int>(static_cast<double>(nrows_) *
                              (point2d->GetY() - ymin_) / (ymax_ - ymin_));
    int jj = static_cast<int>(static_cast<double>(ncols_) *
                              (point2d->GetX() - xmin_) / (xmax_ - xmin_));
    ii = nrows_ - ii;
    return grid_(ii, jj);
  }

  // Return the center of the bin which includes the given point.
  Point::Ptr OccupancyGrid2D::GetBinCenter(Point::Ptr point) const {
    if (!IsValidPoint(point)) return Point::Ptr(nullptr);
    Point2D *point2d = std::static_pointer_cast<Point2D>(point).get();

    // Get rounded coordinates.
    int xbin = static_cast<int>(static_cast<double>(ncols_) *
                                (point2d->GetX() - xmin_) / (xmax_ - xmin_));
    int ybin = static_cast<int>(static_cast<double>(nrows_) *
                                (point2d->GetY() - ymin_) / (ymax_ - ymin_));
    double x = static_cast<double>(xbin) * block_size_;
    double y = static_cast<double>(ybin) * block_size_;

    return Point2D::Create(x, y);
  }

  // Check if a point is valid.
  bool OccupancyGrid2D::IsValidPoint(Point::Ptr point) const {
    CHECK_NOTNULL(point.get());

    // Check point type.
    if (!point->IsType(Point::PointType::POINT_2D)) {
      VLOG(1) << "Error. Point is not of type POINT_2D.";
      return false;
    }

    // Check bounds.
    Point2D *point2d = std::static_pointer_cast<Point2D>(point).get();
    if (point2d->GetX() < xmin_ || point2d->GetX() > xmax_ ||
        point2d->GetY() < ymin_ || point2d->GetY() > ymax_) {
      VLOG(1) << "Error. Point is out of bounds.";
      return false;
    }

    return true;
  }


} // \namespace path
