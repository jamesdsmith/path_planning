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
// This is a helper class to do various operations on Point2D objects.
//
///////////////////////////////////////////////////////////////////////////////

#include "point2d_helpers.h"
#include <cmath>

namespace path {

  // Create a Point2D.
  Point2D& Point2DHelpers::Create(float x, float y) const {
    Point2D point;
    point.x = x;
    point.y = y;
    return point;
  }

  // Midpoint.
  Point2D& Point2DHelpers::MidPoint(Point2D& point1, Point2D& point2) const {
    float mid_x = 0.5 * (point1.x + point2.x);
    float mid_y = 0.5 * (point1.y + point2.y);

    Point2D midpoint = Create(mid_x, mid_y);
    return midpoint;
  }

  // Distance between a point and this line segment.
  double Point2DHelpers::DistanceLineToPoint(Point2D& point1,
                                             Point2D& point2,
                                             Point2D& point3) const {
    // Get vectors from point1 to point2 and from point1 to point3.
    float direction_x = point2.x - point1.x;
    float direction_y = point2.y - point1.y;
    float direction_length = std::sqrt(direction_x * direction_x +
                                       direction_y * direction_y);
    direction_x /= direction_length;

    float query_x = point3.x - point1.x;
    float query_y = point3.y - point1.y;
    float query_length = std::sqrt(query_x * query_x +
                                   query_y * query_y);

    // Test if point projects onto line segment.
    float dot_product = direction_x * query_x + direction_y * query_y;
    if (dot_product <= direction_length && dot_product >= 0.0) {
      // Projects onto line segment.
      return std::sqrt(query_length * query_length -
                       dot_product * dot_product);
    } else {
      // Doesn't project onto the line segment.
      float dist1 = DistancePointToPoint(point1, point3);
      float dist2 = DistancePointToPoint(point2, point3);
      return std::min(dist1, dist2);
    }
  }

  // Distance between two points.
  float Point2DHelpers::DistancePointToPoint(Point2D& point1,
                                             Point2D& point2) const {
    float dx = point2.x - point1.x;
    float dy = point2.y - point1.y;
    return std::sqrt(dx * dx + dy * dy);
  }

  // Take a step toward point2 from point1.
  Point2D& Point2DHelpers::StepToward(Point2D& point1, Point2D& point2,
                                      float step_size) const {
    float length = DistancePointToPoint(point1, point2);
    if (step_size > length)
      step_size = length;

    float step_x = point1.x + step_size * (point2.x - point1.x);
    float step_y = point1.y + step_size * (point2.y - point1.y);
    return Create(step_x, step_y);
  }

  // Add two points with a scale factor.
  Point2D& Point2DHelpers::Add(Point2D& point1, Point2D& point2,
                               float scale) const {
    float sum_x = point1.x + scale * point2.x;
    float sum_y = point2.y + scale * point2.y;
    return Create(sum_x, sum_y);
  }

} //\ namespace path