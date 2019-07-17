#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>

#include <boost/filesystem.hpp>

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class Utils
{
public:
    // Returns if a file exists in file system.
    static bool FileExists ( const string& path );

    // Returns if a directory exists in file system.
    static bool FolderExists( const string& path );
    
    // Create a folder if not exists.
    static void CreateFolderIfNotExists(const string& path);

    // Returns all files in a directory, not recursively.
    static vector<string> GetFileList(const string& path);
    
    // Clear everything in a directory.
    static void ClearFolder(const string& path);
    
    // Ensure string (as path) ends with forward slash.
    static string EnsureTrailingSlash(const string& str);
    
    // Whether str ends with end.
    static bool EndsWith(const string& str, const string& end);

    // Converts extrinsic parameters (rvec, tvec) to transform 4 x 4.
    static void GetTransform44FromExtrinsic ( const vector<double>& extrinsic, Mat* transform_4_4 );

    // Returns the 3d point on sphere screen based on the normalized 2d point [0, 1) in mesh.
    static Point3d GetSpherePointFromScreenPoint( const Point2d& screen_point, const Size& canvas_size, const double radius);

    // Evaluates polynomial equation.
    static double EvaluatePolyEquation ( const double* coefficients, const int n, const double x );
};

#endif // UTILS_H
