// External header
#include <string>
#include <chrono>
#include <opencv2/opencv.hpp>
// Owned header
#include "pano_video_mapper.h"

using namespace std;
using namespace cv;

namespace
{
const char* about = "This program generates panoramic video based on camera system calibration results\n";
const char* keys =
    "{help h ?||Print help message}"
    "{@output|<none>|Output folder storing the generated video/frames}"
    "{@videos|<none>|Input file storing input video files location}"
    "{c calib||Input file storing calibration results}"
    "{p pano||Stitch and output panoramic video}"
    "{s sample|0|Sampling rate in fps, 0 for not sampling}"
    "{f face||Enable face detection}";
}

int main ( int argc, char** argv )
{
    CommandLineParser parser ( argc, argv, keys );
    parser.about ( about );

    if ( parser.has ( "help" ) || argc < 2 )
    {
        parser.printMessage();
        return 0;
    }

    string output_folder = parser.get<string> ( "@output" );
    string video_list_file = parser.get<string> ( "@videos" );
    string calibration_file = parser.get<string> ( "calib" );
    bool stitch_pano = parser.has("pano");
    float sample_rate = parser.get<float> ( "sample" );
    bool face_detection_enabled = parser.has ( "face" );

    if(stitch_pano && calibration_file.empty()) {
        cerr << "Need camera calibration file for panoramic video stitching" << endl << endl;
        parser.printMessage();
        return 0;
    }

    if ( !parser.check() )
    {
        parser.printErrors();
        return 0;
    }

    PanoVideoMapper pano_video_mapper ( output_folder, video_list_file );
    if ( face_detection_enabled )
    {
        pano_video_mapper.EnableFaceDetection();
    }

    cout << endl << "Warning: Existed contents in output folder will be removed." << endl;
    cout << "Press any key to continue." << endl;
    waitKey(0);

    auto start = chrono::high_resolution_clock::now();

    if(sample_rate > 0.0)
    {
        pano_video_mapper.SaveSamples(sample_rate);
    }

    if ( stitch_pano )
    {
        pano_video_mapper.GeneratePano(calibration_file);
    }

    auto time = chrono::high_resolution_clock::now();
    double minutes_count = chrono::duration_cast<chrono::minutes>(time-start).count();
    cout << endl << "Time elasped: " << minutes_count  << " minutes." << endl;
}
