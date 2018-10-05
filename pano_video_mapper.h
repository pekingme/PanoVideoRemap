#ifndef PANOVIDEOMAPPER_H
#define PANOVIDEOMAPPER_H

#include <string>
#include <vector>
#include <iostream>
#include "video_feed.h"

using namespace std;

class PanoVideoMapper
{
public:
  PanoVideoMapper(const string& output_folder, const string& video_list_file_name, const string& calibration_file_name);
  ~PanoVideoMapper() {}
private:
  // Reads all input video files based on video list file.
  void ReadVideoFeeds(const string& video_list_file_name);
  // Reads camera calibration parameters from calibration file.
  void ReadCameraCalibration(const string& calibration_file_name);
  // Folder to store results.
  const string _output_folder;
  // Vector of video feeds.
  vector<VideoFeed> _video_feeds;
};

#endif // PANOVIDEOMAPPER_H
