#ifndef PANOVIDEOMAPPER_H
#define PANOVIDEOMAPPER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "video_feed.h"
#include "camera.h"
#include "video_synchronizer.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class PanoVideoMapper
{
public:
  PanoVideoMapper(const string& output_folder, const string& video_list_file_name, const string& calibration_file_name);
  ~PanoVideoMapper() {}
  
  void GeneratePano();
  
private:
  // Reads all input video files based on video list file.
  void ReadVideoFileAndCameraNames(const string& video_list_file_name, vector<string>* camera_names, vector<string>* video_file_names);
  // Reads camera calibration parameters from calibration file.
  void ReadCameraCalibration(const string& calibration_file_name);
  // Folder to store results.
  const string _output_folder;
  // Vector of video feeds.
  vector<VideoFeed> _video_feeds;
  // Map from name to all cameras, holding intrinsic and extrinsic.
  unordered_map<string, Camera> _cameras_map;
};

#endif // PANOVIDEOMAPPER_H
