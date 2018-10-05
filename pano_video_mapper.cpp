#include "pano_video_mapper.h"
#include "utils.h"

PanoVideoMapper::PanoVideoMapper ( const string& output_folder, const string& video_list_file_name, const string& calibration_file_name )
    : _output_folder ( output_folder )
{
    cout << "Preparing panoramic video mapping system ..." << endl;

    // Checks if input files exist.
    if ( !Utils::FileExists ( output_folder ) || !Utils::FileExists ( video_list_file_name ) || !Utils::FileExists ( calibration_file_name ) )
    {
        exit ( -1 );
    }
    cout << "\tOutput video/frames folder: " << output_folder << endl;
    cout << "\tInput video list file: " << video_list_file_name << endl;
    cout << "\tCalibration file: " << calibration_file_name << endl;
    
    // Gets video files with camera names and checks all files exist.
    cout << "\tLoading all video files and camera names." << endl;
    vector<string> camera_names, video_file_names;
    ReadVideoFeeds(video_list_file_name);
    for(unsigned i=0; i<video_file_names.size(); i++){
      cout << "\t - [" << camera_names[i] << "] " << video_file_names[i] << endl;
    }
    
    // Gets camera intrinsic and extrinsic parameters.
    cout << "\tLoading camera system calibration." << endl;
    ReadCameraCalibration(calibration_file_name);
}

void PanoVideoMapper::ReadVideoFeeds ( const string& video_list_file_name )
{
  // TODO
}

void PanoVideoMapper::ReadCameraCalibration ( const string& calibration_file_name )
{
  // TODO
}
