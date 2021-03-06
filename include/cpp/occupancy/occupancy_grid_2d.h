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

#ifndef PATH_PLANNING_OCCUPANCY_GRID_2D_H
#define PATH_PLANNING_OCCUPANCY_GRID_2D_H

#include <util/disallow_copy_and_assign.h>
#include <util/types.h>
#include <geometry/point_2d.h>
#include <scene/scene_2d_continuous.h>
#include <Eigen/Dense>

using Eigen::MatrixXi;

namespace path {

  class OccupancyGrid2D {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    OccupancyGrid2D(float xmin, float xmax, float ymin, float ymax,
                    float block_size);
    ~OccupancyGrid2D() {}

    // Getters.
    Scene2DContinuous& GetScene() { return scene_; }
    float GetBlockSize() const { return block_size_; }
    float GetXMin() const { return xmin_; }
    float GetXMax() const { return xmax_; }
    float GetYMin() const { return ymin_; }
    float GetYMax() const { return ymax_; }
    int GetNRows() const { return nrows_ ; }
    int GetNCols() const { return ncols_ ; }
    int GetTotalCount() const { return count_; }

    // Operations on the grid.
    void Insert(Point2D::Ptr point);
    int GetCountAt(Point2D::Ptr point) const;
    Point2D::Ptr GetBinCenter(Point2D::Ptr point) const;

    // Visualize this occupancy grid.
    void Visualize(const std::string& title = std::string()) const;

  private:
    MatrixXi grid_;
    Scene2DContinuous scene_;
    float block_size_;
    const float xmin_;
    const float xmax_;
    const float ymin_;
    const float ymax_;
    int nrows_;
    int ncols_;
    int count_;

    // Check if a point is valid.
    bool IsValidPoint(Point2D::Ptr point) const;

    DISALLOW_COPY_AND_ASSIGN(OccupancyGrid2D);
  };

} // \namespace path

#endif
