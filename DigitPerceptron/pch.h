// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

#include "hpc_helpers.hpp"

#include <Eigen3.3.7/Dense>
typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> MatrixXd;
typedef Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> MatrixXi;
typedef Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> MatrixXb;

// debugging - copy Eigen.natvis in C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\Packages\Debugger\Visualizers
// https://github.com/cdcseacave/Visual-Studio-Visualizers/blob/master/Eigen.natvis

#endif //PCH_H
