#include "pch.h"
#include "DataFunctions.h"
#include <numeric> // accumulate
#include <math.h>

CDPoint directionalVector(CDPoint &from, CDPoint &to, double scalar)
{
	Eigen::Vector2d vecFrom(from.x, from.y);
	Eigen::Vector2d vecTo(to.x, to.y);

	Eigen::Vector2d directional_vector = vecTo - vecFrom;
	directional_vector = directional_vector / directional_vector.norm(); // unit vector

	Eigen::Vector2d output = vecFrom + directional_vector * scalar;

	return CDPoint(output(0), output(1));
}

CDPoint Affine2DRotation(CDPoint pt, CDPoint pivot, float theta)
{
	CDPoint newpt;
	newpt.x = pivot.x + (pt.x - pivot.x) * cos(theta) - (pt.y - pivot.y) * sin(theta);
	newpt.y = pivot.y + (pt.x - pivot.x) * sin(theta) + (pt.y - pivot.y) * cos(theta);

	return newpt;

	////Affine Transformation (Rotation by pivot point)
	//Eigen::Vector3f vec(pt.x, pt.y, 0);
	//Eigen::Vector3f axis(0, 0, 1); // z-axis rotation (clockwise)
	//Eigen::Vector3f pivot3d(pivot.x, pivot.y, 0); // pivot of rotation
	//Eigen::Affine3f A = Eigen::Translation3f(pivot3d)
	//	* Eigen::AngleAxisf(theta, axis) 
	//	* Eigen::Translation3f(-pivot3d);
	//vec = A * vec;
	//return CDPoint(vec(0), vec(1));
}

double angleBetweenVectors(Eigen::Vector2d a, Eigen::Vector2d b)
{
	// convert to 3d point because cross product works on 3d point only
	Eigen::Vector3d a3d = { a(0), a(1), 0 };
	Eigen::Vector3d b3d = { b(0), b(1), 0 };
	
	double radian = 0.0;
	radian = std::atan2(a3d.cross(b3d).norm(), a3d.dot(b3d));

	//double cross_product = a(0)*b(1) - a(1) * b(0);
	//if (cross_product < 0)
	//	radian = -radian; // clockwise side of line

	return radian;
}

double angleBetweenVectors(C2DVector v1, C2DVector v2)
{
	// acos((v1.x * v2.x + v1.y * v2.y) / ((sqrt(v1.x*v1.x + v1.y*v1.y) * sqrt(v2.x*v2.x + v2.y*v2.y)))) / M_PI * 180
	double theta = (v1.x * v2.x + v1.y * v2.y) / ((sqrt(v1.x*v1.x + v1.y*v1.y) * sqrt(v2.x*v2.x + v2.y*v2.y)));
	double degree = acos(theta) / M_PI * 180;
	return degree;
}

float angleBetweenVectors(float v1x, float v1y, float v2x, float v2y)
{
	float theta = (v1x * v2x + v1y * v2y) / ((sqrtf(v1x*v1x + v1y*v1y) * sqrtf(v2x*v2x + v2y*v2y)));
	float radian = acosf(theta);

	double cross_product = v1x*v2y - v1y*v2x;
	if (cross_product < 0)
		radian = -radian; // clockwise side of line
	
	return radian;
}

CString DF_GetFilePath(CString &fullpath)
{
	CString filePath = fullpath.Left(fullpath.ReverseFind('\\') + 1);

	return filePath;
}

vector<double> DF_ComputePercentiles(const Eigen::VectorXd &data)
{
	vector<double> dPercentiles;
	if (data.size() < 5) return dPercentiles; // not able to compute 

	// determine quartiles	
	for (int i = 0; i < data.size(); i++)
		dPercentiles.push_back(data(i));
	dPercentiles = Quantile<double>(dPercentiles, { 0.25, 0.5, 0.75 });

	return dPercentiles;
}

vector<double> DF_ComputeWhiskers(const vector<double>percentiles,C2DMinMax &data_range)
{
	vector<double> dWhiskers;
	if (percentiles.size() < 2) return dWhiskers; // not able to compute 

	// determine interquartile range (IQR): 25th to the 75th percentile
	double dQ1 = percentiles[0];
	double dQ2 = percentiles[1];
	double dQ3 = percentiles[2];
	double dIQR = dQ3 - dQ1;

	//-----------------------------------------------------------------------
	// determine semi interquartile range (IQR)
	double dSemiIQR = dIQR * 1.5;
	double dQ1_SemiIQR = dQ1 - dSemiIQR;
	double dQ3_SemiIQR = dQ3 + dSemiIQR;
	dWhiskers.push_back(dQ1_SemiIQR);
	dWhiskers.push_back(dQ3_SemiIQR);

	// check if the semiIQR range is larger than the original data range.
	if (data_range.GetMin() > dQ1_SemiIQR) data_range.SetMin(dQ1_SemiIQR);
	if (data_range.GetMax() < dQ3_SemiIQR) data_range.SetMax(dQ3_SemiIQR);

	return dWhiskers;
}

//vector<double> DF_ComputeQuartiles(const vector<double> &data, C2DMinMax &data_range)
//{
//	vector<double> dQuartiles;
//
//	if (data.size() < 5) return dQuartiles; // not able to compute 
//
//	// determine quartiles	
//	dQuartiles = Quantile<double>(data, { 0.25, 0.5, 0.75 });
//
//	// determine interquartile range (IQR): 25th to the 75th percentile
//	double dQ1 = dQuartiles[0];
//	double dQ2 = dQuartiles[1];
//	double dQ3 = dQuartiles[2];
//	double dIQR = dQ3 - dQ1;
//
//	//-----------------------------------------------------------------------
//	// determine semi interquartile range (IQR)
//	double dSemiIQR = dIQR * 1.5;
//	double dQ1_SemiIQR = dQ1 - dSemiIQR;
//	double dQ3_SemiIQR = dQ3 + dSemiIQR;
//	dQuartiles.push_back(dQ1_SemiIQR);
//	dQuartiles.push_back(dQ3_SemiIQR);
//
//	// check if the semiIQR range is larger than the original data range.
//	if (data_range.GetMin() > dQ1_SemiIQR) data_range.SetMin(dQ1_SemiIQR);
//	if (data_range.GetMax() < dQ3_SemiIQR) data_range.SetMax(dQ3_SemiIQR);
//
//	return dQuartiles;
//}



double GetRelativeTargetValue(double tmax, double tmin, double smax, double smin, double sval)
{
	double res = 0;
	// min-max normalization Artificial Neural Networks By Kevin L. Priddy, Paul E. Keller
	res = (tmax - tmin)*((sval - smin) / (smax - smin)) + tmin;
	return res;
}




//	http://www.cartage.org.lb/en/themes/Sciences/Mathematics/Algebra/foci/topics/line/Line.htm
// 	Distance between a point and a line
// 	Take a point P(a,b) and a line l with normal equation lx + my + n = 0 .
// 	Then Q(l,m) is a unity normal vector to l.
// 	Call S(c,d) the intersection of l with the perpendicular dropped on l through P.
// 	Then the distance from P to l is |P,S|.
// 	Now, PS = r.Q and |P,S| = |r| .
// 
// 	PS = r.Q
// 
// 	=>      S - P = r. Q
// 
// 	=>      S.Q - P.Q = r.Q.Q
// 
// 	=>      cl + dm - (al + bm) = r. 1
// 	since S on l holds lc  +  md = -n
// 
// 	=>      -n -al - bm = r
// 
// 	=>      la  + mb  + n = -r
// 
// 	=>      |la  + mb  + n| = |r|
// 
// 	The distance between a point P(a,b)
// 	and a line l with normal equation lx + my + n = 0 is
// 	| la + mb + n | .  




int DF_CheckPossibilityOfBecomingPolygon(POINTLIST &ptlist, int &sidx, int &eidx, CDRect &rtRegion)
{
	double dist = -1;
	for (int i = 0; i<(int)ptlist.size() / 3; i++)
	{
		CDPoint P1;
		P1 = ptlist[i];
		for (int j = (int)ptlist.size() - 1; j>(int)ptlist.size() * 2 / 3; j--)
		{
			CDPoint P2;
			P2 = ptlist[j];

			double tmp = sqrt((P2.y - P1.y)*(P2.y - P1.y) + (P2.x - P1.x)*(P2.x - P1.x));
			if (dist == -1 || dist>tmp)
			{
				dist = tmp;
				sidx = i;
				eidx = j;
			}
		}
	}
	if (dist < rtRegion.Width() / 3.) return 1;
	if (dist < rtRegion.Height() / 3.) return 1;

	return -1;
}


POINTLIST DF_FindIntersection(POINTLIST &ptlist, int &sidx, int &eidx)
{
	POINTLIST intersections;
	for (int i = 0; i<(int)ptlist.size() / 2; i++)
	{
		CDPoint P1, P2;
		P1 = ptlist[i];
		P2 = ptlist[i + 1];
		for (int j = (int)ptlist.size() - 2; j>(int)ptlist.size() / 2; j--)
		{
			CDPoint P3, P4;
			P3 = ptlist[j];
			P4 = ptlist[j + 1];

			double denom = (P4.y - P3.y)*(P2.x - P1.x) - (P4.x - P3.x)*(P2.y - P1.y);
			double nume_a = (P4.x - P3.x)*(P1.y - P3.y) - (P4.y - P3.y)*(P1.x - P3.x);
			double nume_b = (P2.x - P1.x)*(P1.y - P3.y) - (P2.y - P1.y)*(P1.x - P3.x);

			if (denom == 0.0f) // no intersection exist
			{
				if (nume_a == 0.0f && nume_b == 0.0f)
					continue; // COINCIDENT
				continue; // PARALLEL
			}

			double ua = nume_a / denom;
			double ub = nume_b / denom;

			if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
			{
				// Get the intersection point.
				double x = P1.x + ua * (P2.x - P1.x);
				double y = P1.y + ua * (P2.y - P1.y);

				sidx = i;
				eidx = j;
				intersections.push_back(CDPoint(x, y));
				continue; // INTERESECTING
			}
		}
	}
	return intersections;
}
