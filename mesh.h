#ifndef MESH_H
#define MESH_H

#include <vector>
#include "utils.h"
#include "camera.h"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class Mesh
{
public:
  Mesh(const int row_index, const int col_index, const int mesh_size, const Size& canvas_size, const Camera& camera);
  
  void Paint(Mat* canvas, const Mat& frame, Mat* weight_mat);
  
private:
  bool IsOutOfBound(const Point2d& pt, const Mat& frame);
  
  int _x_1, _x_2;
  int _y_1, _y_2;
  Point2d _pt_a, _pt_b, _pt_c, _pt_d;
};

#endif // MESH_H
