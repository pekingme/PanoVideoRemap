#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <vector>
#include "utils.h"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

class Camera
{
public:
  Camera(){};
  Camera(const string& name, const int width, const int height, const double u0, const double v0, 
	 const vector<double>& affine, const vector<double>& poly, const vector<double>& inv_poly, 
	 const vector<double>& extrinsic);
  
  // Convert 3d points (N x 3) in world to 2d points (N x 2) on frame. 
  Mat ProjectWorldToFrame(const Mat& world_pts, const bool debug) const;
  
  // Returns the camera center as Point2d.
  Point2d GetCameraCenter() const { return Point2d(_u0, _v0); }
  
  // Returns the camera frame size.
  Size GetFrameSize() const { return Size(_width, _height); }
  
  string GetName() const { return _name; }
  
private:
  // Convert 3d point in camera coordinate system to 2d point in frame.
  Mat ProjectCameraToFrame(const Mat& camera_pt) const;
  
  string _name;
  int _width, _height;
  double _u0, _v0;
  double _c, _d, _e;
  vector<double> _poly;
  vector<double> _inverse_poly;
  Mat _transform_4_4;
};

#endif // CAMERA_H
