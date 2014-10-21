/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------1------------------------------------------- */

/**
 * @file testExpression.cpp
 * @date September 18, 2014
 * @author Frank Dellaert
 * @author Paul Furgale
 * @brief unit tests for Block Automatic Differentiation
 */

#include <gtsam/geometry/PinholeCamera.h>
#include <gtsam/geometry/Pose3.h>
#include <gtsam/geometry/Cal3_S2.h>
#include <gtsam/geometry/Cal3Bundler.h>
#include <gtsam_unstable/nonlinear/Expression.h>
#include <gtsam/base/Testable.h>
#include <gtsam/base/LieScalar.h>

#undef CHECK
#include <CppUnitLite/TestHarness.h>

#include <boost/assign/list_of.hpp>
using boost::assign::list_of;
using boost::assign::map_list_of;

using namespace std;
using namespace gtsam;

// The DefaultChart of Camera below is laid out like Snavely's 9-dim vector
typedef PinholeCamera<Cal3Bundler> Camera;

/* ************************************************************************* */
// is_manifold
TEST(Manifold, _is_manifold) {
  using namespace traits;
  EXPECT(!is_manifold<int>::value);
  EXPECT(is_manifold<Point2>::value);
  EXPECT(is_manifold<Matrix24>::value);
  EXPECT(is_manifold<double>::value);
  EXPECT(is_manifold<Vector>::value);
  EXPECT(is_manifold<Matrix>::value);
}

/* ************************************************************************* */
// dimension
TEST(Manifold, _dimension) {
  using namespace traits;
  EXPECT_LONGS_EQUAL(2, dimension<Point2>::value);
  EXPECT_LONGS_EQUAL(8, dimension<Matrix24>::value);
  EXPECT_LONGS_EQUAL(1, dimension<double>::value);
  EXPECT_LONGS_EQUAL(Eigen::Dynamic, dimension<Vector>::value);
  EXPECT_LONGS_EQUAL(Eigen::Dynamic, dimension<Matrix>::value);
}

/* ************************************************************************* */
// charts
TEST(Manifold, DefaultChart) {

  DefaultChart<Point2> chart1(Point2(0, 0));
  EXPECT(chart1.apply(Point2(1,0))==Vector2(1,0));
  EXPECT(chart1.retract(Vector2(1,0))==Point2(1,0));

  Vector v2(2);
  v2 << 1, 0;
  DefaultChart<Vector2> chart2(Vector2(0, 0));
  EXPECT(assert_equal(v2,chart2.apply(Vector2(1,0))));
  EXPECT(chart2.retract(v2)==Vector2(1,0));

  DefaultChart<double> chart3(0);
  Vector v1(1);
  v1 << 1;
  EXPECT(assert_equal(v1,chart3.apply(1)));
  EXPECT_DOUBLES_EQUAL(chart3.retract(v1), 1, 1e-9);

  // Dynamic does not work yet !
  Vector z = zero(2), v(2);
  v << 1, 0;
  DefaultChart<Vector> chart4(z);
  EXPECT(assert_equal(chart4.apply(v),v));
  EXPECT(assert_equal(chart4.retract(v),v));

  Vector v3(3);
  v3 << 1, 1, 1;
  Rot3 I = Rot3::identity();
  Rot3 R = I.retractCayley(v3);
  DefaultChart<Rot3> chart5(I);
  EXPECT(assert_equal(v3,chart5.apply(R)));
  EXPECT(assert_equal(chart5.retract(v3),R));
  // Check zero vector
  DefaultChart<Rot3> chart6(R);
  EXPECT(assert_equal(zero(3),chart6.apply(R)));
}

/* ************************************************************************* */
// zero
TEST(Manifold, _zero) {
  EXPECT(assert_equal(Pose3(),traits::zero<Pose3>::value()));
  Cal3Bundler cal(0, 0, 0);
  EXPECT(assert_equal(cal,traits::zero<Cal3Bundler>::value()));
  EXPECT(assert_equal(Camera(Pose3(),cal),traits::zero<Camera>::value()));
}

/* ************************************************************************* */
// charts
TEST(Manifold, Canonical) {

  Canonical<Point2> chart1;
  EXPECT(chart1.apply(Point2(1,0))==Vector2(1,0));
  EXPECT(chart1.retract(Vector2(1,0))==Point2(1,0));

  Vector v2(2);
  v2 << 1, 0;
  Canonical<Vector2> chart2;
  EXPECT(assert_equal(v2,chart2.apply(Vector2(1,0))));
  EXPECT(chart2.retract(v2)==Vector2(1,0));

  Canonical<double> chart3;
  Eigen::Matrix<double, 1, 1> v1;
  v1 << 1;
  EXPECT(chart3.apply(1)==v1);
  EXPECT_DOUBLES_EQUAL(chart3.retract(v1), 1, 1e-9);

  Canonical<Point3> chart4;
  Point3 point(1, 2, 3);
  Vector v3(3);
  v3 << 1, 2, 3;
  EXPECT(assert_equal(v3,chart4.apply(point)));
  EXPECT(assert_equal(chart4.retract(v3),point));

  Canonical<Pose3> chart5;
  Pose3 pose(Rot3::identity(), point);
  Vector v6(6);
  v6 << 0, 0, 0, 1, 2, 3;
  EXPECT(assert_equal(v6,chart5.apply(pose)));
  EXPECT(assert_equal(chart5.retract(v6),pose));

  Canonical<Camera> chart6;
  Cal3Bundler cal0(0, 0, 0);
  Camera camera(Pose3(), cal0);
  Vector z9 = Vector9::Zero();
  EXPECT(assert_equal(z9,chart6.apply(camera)));
  EXPECT(assert_equal(chart6.retract(z9),camera));

  Cal3Bundler cal; // Note !! Cal3Bundler() != zero<Cal3Bundler>::value()
  Camera camera2(pose, cal);
  Vector v9(9);
  v9 << 0, 0, 0, 1, 2, 3, 1, 0, 0;
  EXPECT(assert_equal(v9,chart6.apply(camera2)));
  EXPECT(assert_equal(chart6.retract(v9),camera2));
}

/* ************************************************************************* */
int main() {
  TestResult tr;
  return TestRegistry::runAllTests(tr);
}
/* ************************************************************************* */

