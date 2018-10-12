#ifndef VIDEOFEED_H
#define VIDEOFEED_H

#include <string>
#include <vector>
#include "mesh.h"
#include "camera.h"
#include "video_clip.h"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

class VideoFeed
{
public:
  // Initiates VideoFeed with synchronized VideoClip, calibrated Camera, and size of linear projection.
  VideoFeed(const VideoClip& video_clip, const Camera& camera, const int project_size);
  
  // Paints the indicated frame to corresponding mapped part on the canvas. Returns false if no frame to process, otherwise returns true.
  bool PaintOnCanvas(const double global_time, Mat* canvas, Mat* weight_mat);

private:
  VideoClip _video_clip;
  Camera _camera;
  VideoCapture _video_cap;
  Size _output_size;
  vector<Mesh> _mesh_vector;
};

#endif // VIDEOFEED_H
