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
// This class defines a 2D continuous scene, templated on the type of obstacle.
//
///////////////////////////////////////////////////////////////////////////////

#include "scene_2d_continuous.h"
#include "scene_model.h"
#include <math/random_generator.h>
#include <geometry/point.h>
#include <geometry/point_2d.h>
#include <glog/logging.h>

namespace path {

  Scene2DContinuous::Scene2DContinuous(double xmin, double xmax,
                                       double ymin, double ymax,
                                       std::vector<Obstacle::Ptr>& obstacles)
    : xmin_(xmin), xmax_(xmax),
      ymin_(ymin), ymax_(ymax),
      obstacles_(obstacles) {

    // Check that all obstacle pointers are not null.
    for (const auto& obstacle : obstacles_)
      CHECK_NOTNULL(obstacle.get());
  }

  // Is this point feasible?
  bool Scene2DContinuous::IsFeasible(Point::Ptr point) const {
    CHECK_NOTNULL(point.get());

    // Check each obstacle.
    for (const auto& obstacle : obstacles_) {
      if (!obstacle->IsFeasible(point))
        return false;
    }

    return true;
  }

  // What is the cost of occupying this point?
  double Scene2DContinuous::Cost(Point::Ptr point) const {
    CHECK_NOTNULL(point.get());

    // Iterate over all obstacles and add costs.
    double total_cost = 0.0;
    for (const auto& obstacle : obstacles_)
      total_cost += obstacle->Cost(point);

    return total_cost;
  }

  // Get a random point in the scene.
  Point::Ptr Scene2DContinuous::GetRandomPoint() const {
    math::RandomGenerator rng(math::RandomGenerator::Seed());

    double x = rng.DoubleUniform(xmin_, xmax_);
    double y = rng.DoubleUniform(ymin_, ymax_);
    Point::Ptr point = Point2D::Create(x, y);
    return point;
  }

  // Are there any obstacles between the given two points?
  bool Scene2DContinuous::LineOfSight(Point::Ptr point1,
                                      Point::Ptr point2) const {
    LineSegment line(point1, point2);

    // Check if line segment intersects any obstacle.
    for (const auto& obstacle : obstacles_) {
      if (line.Intersects(obstacle))
        return false;
    }

    return true;
  }


} // \namespace path
