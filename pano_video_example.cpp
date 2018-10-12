#include <string>
#include "pano_video_mapper.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

namespace
{
const char* about = "This program generates panoramic video based on camera system calibration results\n";
const char* keys =
    "{help h ?||Print help message}"
    "{@output|<none>|Output folder storing the generated video/frames}"
    "{@videos|<none>|Input file storing input video files location}"
    "{@calibration|<none>|Input file storing calibration results}";
}

int main ( int argc, char** argv )
{
    CommandLineParser parser ( argc, argv, keys );
    parser.about ( about );

    if ( parser.has ( "help" ) ||argc<3 )
    {
        parser.printMessage();
        return 0;
    }

    string output_folder = parser.get<string> ( "@output" );
    string input_file_name = parser.get<string> ( "@videos" );
    string calibration_file_name = parser.get<string> ( "@calibration" );

    if ( !parser.check() )
    {
        parser.printErrors();
        return 0;
    }
    
    PanoVideoMapper pano_video_mapper (output_folder, input_file_name, calibration_file_name);
    pano_video_mapper.GeneratePano();
}
