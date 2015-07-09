/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 *  @file  BearingFactor.H
 *  @author Frank Dellaert
 **/

#pragma once

#include <gtsam/nonlinear/ExpressionFactor.h>
#include <gtsam/geometry/concepts.h>
#include <gtsam/base/Testable.h>

namespace gtsam {

/**
 * Binary factor for a bearing measurement
 * @addtogroup SAM
 */
template <class Pose, class Point, class Measured = typename Pose::Rotation>
class BearingFactor : public ExpressionFactor<Measured> {
 private:
  typedef BearingFactor<Pose, Point> This;
  typedef ExpressionFactor<Measured> Base;

  /** concept check by type */
  GTSAM_CONCEPT_TESTABLE_TYPE(Measured)
  GTSAM_CONCEPT_POSE_TYPE(Pose)

 public:
  /** primary constructor */
  BearingFactor(Key poseKey, Key pointKey, const Measured& measured,
                const SharedNoiseModel& model)
      : Base(model, measured,
             Expression<Measured>(Expression<Pose>(poseKey), &Pose::bearing,
                                  Expression<Point>(pointKey))) {}

  virtual ~BearingFactor() {}

  /** print contents */
  void print(const std::string& s = "",
             const KeyFormatter& keyFormatter = DefaultKeyFormatter) const {
    std::cout << s << "BearingFactor, bearing = ";
    this->measurement_.print();
    Base::print("", keyFormatter);
  }
};  // BearingFactor

}  // namespace gtsam
