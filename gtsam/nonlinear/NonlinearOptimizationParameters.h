/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file NonlinearOptimizationParameters.h
 * @date Oct 14, 2010
 * @author Kai Ni
 * @brief structure to store parameters for nonlinear optimization
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace gtsam {

// a container for all related parameters
struct NonlinearOptimizationParameters {

	typedef boost::shared_ptr<NonlinearOptimizationParameters> shared_ptr ;
	typedef NonlinearOptimizationParameters This;
	typedef boost::shared_ptr<NonlinearOptimizationParameters> sharedThis ;

	typedef enum {
		SILENT,
		ERROR,
		LAMBDA,
		TRYLAMBDA,
		VALUES,
		DELTA,
		TRYCONFIG,
		TRYDELTA,
		LINEAR,
		DAMPED
	} verbosityLevel;

	typedef enum {
		FAST,
		BOUNDED,
		CAUTIOUS
	} LambdaMode;

	// thresholds
	double absDecrease_; /* threshold for the absolute decrease per iteration */
	double relDecrease_; /* threshold for the relative decrease per iteration */
	double sumError_; /* threshold for the sum of error */
	size_t maxIterations_ ;
	double lambda_ ;
	double lambdaFactor_ ;

	// flags and mode switches
	verbosityLevel verbosity_;
	LambdaMode lambdaMode_;
	bool useQR_; 					/// if true, solve whole system with QR, otherwise use LDL when possible

	NonlinearOptimizationParameters(): absDecrease_(1e-6), relDecrease_(1e-6), sumError_(0.0),
			maxIterations_(100), lambda_(1e-5), lambdaFactor_(10.0), verbosity_(SILENT),
			lambdaMode_(BOUNDED),	useQR_(false) {}

	NonlinearOptimizationParameters(double absDecrease, double relDecrease, double sumError,
			int iIters = 100, double lambda = 1e-5, double lambdaFactor = 10,
			verbosityLevel v = SILENT, LambdaMode lambdaMode = BOUNDED, bool useQR = false)
	:absDecrease_(absDecrease), relDecrease_(relDecrease), sumError_(sumError),
	 maxIterations_(iIters), lambda_(lambda), lambdaFactor_(lambdaFactor), verbosity_(v),
	 lambdaMode_(lambdaMode), useQR_(useQR) {}

	NonlinearOptimizationParameters(const NonlinearOptimizationParameters &parameters):
		absDecrease_(parameters.absDecrease_),
		relDecrease_(parameters.relDecrease_),
		sumError_(parameters.sumError_),
		maxIterations_(parameters.maxIterations_),
		lambda_(parameters.lambda_),
		lambdaFactor_(parameters.lambdaFactor_),
		verbosity_(parameters.verbosity_),
		lambdaMode_(parameters.lambdaMode_),
		useQR_(parameters.useQR_) {}

	/* a copy of old instance except some parameters */
	sharedThis newLambda_(double lambda) const {
		sharedThis ptr (boost::make_shared<NonlinearOptimizationParameters>(*this)) ;
		ptr->lambda_ = lambda ;
		return ptr ;
	}

	/* new instance with default parameters except some partially assigned parameters */

	static sharedThis newVerbosity(verbosityLevel verbosity) {
		sharedThis ptr (boost::make_shared<NonlinearOptimizationParameters>()) ;
		ptr->verbosity_ = verbosity ;
		return ptr ;
	}

	static sharedThis newMaxIterations(int maxIterations) {
		sharedThis ptr (boost::make_shared<NonlinearOptimizationParameters>()) ;
		ptr->maxIterations_ = maxIterations ;
		return ptr ;
	}

	static sharedThis newLambda(double lambda) {
		sharedThis ptr (boost::make_shared<NonlinearOptimizationParameters>());
		ptr->lambda_ = lambda ;
		return ptr ;
	}

	static sharedThis newDrecreaseThresholds(double absDecrease, double relDecrease) {
		sharedThis ptr (boost::make_shared<NonlinearOptimizationParameters>());
		ptr->absDecrease_ = absDecrease;
		ptr->relDecrease_ = relDecrease;
		return ptr ;
	}

	static sharedThis newFactorization(bool useQR) {
		sharedThis ptr (boost::make_shared<NonlinearOptimizationParameters>());
		ptr->useQR_= useQR;
		return ptr ;
	}
};
}
