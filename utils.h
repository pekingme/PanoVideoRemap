#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class Utils
{
public:
    // Returns if a file exists in file system.
    static bool FileExists ( const string& filename );
    
    // Converts extrinsic parameters (rvec, tvec) to transform 4 x 4.
    static void GetTransform44FromExtrinsic ( const vector<double>& extrinsic, Mat* transform_4_4 );
    
    // Returns the 3d point on sphere screen based on the normalized 2d point [0, 1) in mesh.
    static Point3d GetSpherePointFromScreenPoint( const Point2d& screen_point, const Size& canvas_size, const double radius);
    
    // Evaluates polynomial equation.
    static double EvaluatePolyEquation ( const double* coefficients, const int n, const double x );
};

#endif // UTILS_H
