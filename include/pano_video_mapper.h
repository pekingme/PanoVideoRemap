#ifndef PANOVIDEOMAPPER_H
#define PANOVIDEOMAPPER_H

// External headers
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <boost/filesystem.hpp>
// Owned headers
#include "utils.h"
#include "camera.h"
#include "frame_mapper.h"
// Third party headers
#include "combined_video_clip.h"
#include "synch_parameters.h"

using namespace std;
using namespace cv;
using namespace boost;

class PanoVideoMapper
{
public:
    PanoVideoMapper(const string& output_folder, const string& video_list_file);
    ~PanoVideoMapper() {}

    void GeneratePano(const string& calibration_file);
    
    void SaveSamples(const float sample_rate);

    void EnableFaceDetection();

private:
    // Reads video list file content to class parameteres.
    void ReadInVideoListFile(const string& video_list_file);
    
    // Reads camera calibration parameters from calibration file.
    void ReadCameraCalibration(const string& calibration_file);
    
    //================= Basic parameters
    
    // Folder to store results
    const string output_folder_;
    // Vector of all camera names to work with
    vector<string> camera_names_;
    // Vector of video folders
    vector<string> video_folders_;
    // Vector of all video names to work with
    vector<string> video_names_;
    // Time shift tolerance for synchronization
    float max_shift_;

    //================= Generate panoramic video
    
    // Frame rate of output video.
    const int fps_;
    // Frame size of output video or frames.
    const Size output_size_;
    // Map from name to all cameras, holding intrinsic and extrinsic.
    unordered_map<string, Camera> cameras_map_;
    // Unordered map of frame mappers.
    unordered_map<string, FrameMapper> frame_mapper_map_;
    // Face classifier.
    CascadeClassifier haar_cascade_;
    
    //================= Sample frame from video
    
    // Index for the name of next sampled frame.
    long next_image_index_;
};

#endif // PANOVIDEOMAPPER_H
