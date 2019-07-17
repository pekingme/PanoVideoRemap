#ifndef FRAMEMAPPER_H
#define FRAMEMAPPER_H

// External headers
#include <opencv2/opencv.hpp>
// Owned headers
#include "camera.h"
#include "mesh.h"

using namespace std;
using namespace cv;

class FrameMapper
{
public:
    FrameMapper() {}
    FrameMapper(const Camera& camera, const Size& output_size, const int project_size);

    // Paint mapped pixels to output canvas.
    void PaintOnCanvas(const Mat& frame, Mat* canvas);

    // Normalizes weight mat based on input total weight mat.
    void NormalizeWeight(Mat total_weight);

    // Returns current weight mat.
    Mat GetWeightMat();

private:
    // Camera parameters for current frame source.
    Camera _camera;
    // Size of output frame.
    Size _output_size;
    // Vector of all meshes mapping from current frame to output canvas.
    vector<Mesh> _mesh_vector;
    // Mat of weight of each pixel on current frame to output canvas.
    Mat _weight_mat;
};

#endif // FRAMEMAPPER_H
