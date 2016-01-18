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
 */

#include <geometry/trajectory.h>
#include <geometry/point_2d.h>
#include <math/random_generator.h>
#include <scene/scene_2d_continuous.h>
#include <scene/obstacle_2d_gaussian.h>
#include <image/image.h>

#include <vector>
#include <cmath>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <iostream>
#include <Eigen/Dense>

DEFINE_bool(visualize_scenes, false, "Visualize scene?");

using Eigen::MatrixXf;

namespace path {

  // Test that we can construct and destroy a scene.
  TEST(SceneModel, TestScene2DContinuous) {
    math::RandomGenerator rng(0);

    // Create a bunch of obstacles.
    std::vector<Obstacle::Ptr> obstacles;
    for (size_t ii = 0; ii < 10; ii++) {
      double x = rng.Double();
      double y = rng.Double();
      double sigma_xx = 0.005 * rng.Double();
      double sigma_yy = 0.005 * rng.Double();
      double sigma_xy = rng.Double() * std::sqrt(sigma_xx * sigma_yy);

      Obstacle::Ptr obstacle =
        Obstacle2DGaussian::Create(x, y, sigma_xx, sigma_yy, sigma_xy, 0.01);
      obstacles.push_back(obstacle);
    }

    // Create a 2D continous scene.
    Scene2DContinuous scene(0.0, 1.0, 0.0, 1.0, obstacles);

    // If visualize flag is set, query a grid and show the cost map.
    if (FLAGS_visualize_scenes) {
      scene.Visualize("Cost map");
    }
  }

  // Test that we can create a Trajectory in the Scene.
  TEST(SceneModel, TestScene2DContinuousTrajectory) {
    math::RandomGenerator rng(0);

    // Create a bunch of obstacles.
    std::vector<Obstacle::Ptr> obstacles;
    for (size_t ii = 0; ii < 10; ii++) {
      double x = rng.Double();
      double y = rng.Double();
      double sigma_xx = 0.005 * rng.Double();
      double sigma_yy = 0.005 * rng.Double();
      double sigma_xy = rng.Double() * std::sqrt(sigma_xx * sigma_yy);

      Obstacle::Ptr obstacle =
        Obstacle2DGaussian::Create(x, y, sigma_xx, sigma_yy, sigma_xy, 0.01);
      obstacles.push_back(obstacle);
    }

    // Create a 2D continous scene.
    Scene2DContinuous scene(0.0, 1.0, 0.0, 1.0, obstacles);

    // Create a Trajectory.
    Trajectory::Ptr path = Trajectory::Create();
    for (size_t ii = 0; ii < 20; ii++) {
      double x = 0.5 + 0.25 * std::cos(2.0 * M_PI * static_cast<double>(ii) / 20.0) +
        rng.DoubleUniform(-0.05, 0.05);
      double y = 0.5 + 0.25 * std::sin(2.0 * M_PI * static_cast<double>(ii) / 20.0) +
        rng.DoubleUniform(-0.05, 0.05);

      Point::Ptr point = Point2D::Create(x, y);
      path->AddPoint(point);
    }

    // If visualize flag is set, query a grid and show the cost map.
    if (FLAGS_visualize_scenes) {
      scene.Visualize("Trajectory", path);
    }
  }

} //\ namespace path
