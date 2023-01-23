#pragma once

#include "DRect.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

CDPoint Affine2DRotation(CDPoint pt, CDPoint pivot, float theta);

CDPoint directionalVector(CDPoint &from, CDPoint &to, double scalar);

double angleBetweenVectors(Eigen::Vector2d a, Eigen::Vector2d b);
double angleBetweenVectors(C2DVector v1, C2DVector v2);
float angleBetweenVectors(float v1x, float v1y, float v2x, float v2y);

CString DF_GetFilePath(CString &fullpath);

double GetRelativeTargetValue(double tmax, double tmin, double smax, double smin, double sval);

//----------------------------------------------------------------------------------------
// Checking polygonal shape
int DF_CheckPossibilityOfBecomingPolygon(POINTLIST &ptlist, int &sidx, int &eidx, CDRect &rtRegion);
POINTLIST DF_FindIntersection(POINTLIST &ptlist, int &sidx, int &eidx);

//----------------------------------------------------------------------------------------
// Percentile Computations
//vector<double> DF_ComputeQuartiles(const vector<double> &data, C2DMinMax &data_range);
vector<double> DF_ComputePercentiles(const Eigen::VectorXd &data);
vector<double> DF_ComputeWhiskers(const vector<double>percentiles, C2DMinMax &data_range);
double Quartile(const Eigen::MatrixXd &x, int q);

///////////////////////////////////////////////////////////////////////////////////
/// Usage: finding quartiles
/// std::vector<double> in = { 1,2,3,4,5,6,7,8,9,10,11 };
/// auto quartiles = Quantile<double>(in, { 0.25, 0.5, 0.75 });
/// 
template<typename T>
double Lerp(T v0, T v1, T t)
{
	return (1 - t)*v0 + t*v1;
}

template<typename T>
std::vector<T> Quantile(const std::vector<T>& inData, const std::vector<T>& probs)
{
	if (inData.empty())
	{
		return std::vector<T>();
	}

	if (1 == inData.size())
	{
		return std::vector<T>(1, inData[0]);
	}

	std::vector<T> data = inData;
	std::sort(data.begin(), data.end());
	std::vector<T> quantiles;

	for (size_t i = 0; i < probs.size(); ++i)
	{
		T poi = Lerp<T>(-0.5, data.size() - 0.5, probs[i]);

		size_t left = max(int64_t(std::floor(poi)), int64_t(0));
		size_t right = min(int64_t(std::ceil(poi)), int64_t(data.size() - 1));

		T datLeft = data.at(left);
		T datRight = data.at(right);

		T quantile = Lerp<T>(datLeft, datRight, poi - left);

		quantiles.push_back(quantile);
	}

	return quantiles;
}


template< typename A, typename B >
inline
bool close_enough(A const & a, B const & b,
	typename std::common_type< A, B >::type const & epsilon)
{
	using std::isless;
	assert(isless(0, epsilon)); // epsilon is a part of the whole quantity
	assert(isless(epsilon, 1));
	using std::abs;
	auto const delta = abs(a - b);
	auto const x = abs(a);
	auto const y = abs(b);
	// comparable generally and |a - b| < eps * (|a| + |b|) / 2
	return isless(epsilon * y, x) && isless(epsilon * x, y) && isless((delta + delta) / (x + y), epsilon);
}