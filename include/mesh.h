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
    Mesh() {}
    Mesh(const int row_index, const int col_index, const int mesh_size,
         const Size& canvas_size, const Camera& camera, Mat* weight_mat);

    bool CheckValidity(const Camera& camera);

    void Paint(Mat* canvas, const Mat& frame, const Mat& weight_mat);

private:
    bool IsOutOfBound(const Point2d& pt, const int width, const int height);

    int _x_1, _x_2;
    int _y_1, _y_2;
    Point2d _pt_a, _pt_b, _pt_c, _pt_d;

    double _blending_weight_th = 0.1;
};

#endif // MESH_H
