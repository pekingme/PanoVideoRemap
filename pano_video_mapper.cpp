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

    // Gets camera intrinsic and extrinsic parameters.
    cout << "\tLoading camera system calibration." << endl;
    ReadCameraCalibration ( calibration_file_name );

    // Gets video files with camera names and checks all files exist.
    cout << "\tLoading all video files and camera names." << endl;
    vector<string> camera_names, video_file_names;
    ReadVideoFileAndCameraNames ( video_list_file_name, &camera_names, &video_file_names );
    for ( unsigned i=0; i<video_file_names.size(); i++ )
    {
        cout << "\t - [" << camera_names[i] << "] " << video_file_names[i] << endl;
    }

    // Synchronizes all videos.
    cout << "\tSynchronizes videos ..." << endl;
    FileStorage file_storage ( video_list_file_name, FileStorage::READ );
    double shift_window = file_storage["MaxShift"];
    VideoSynchronizer video_synchronizer;
    video_synchronizer.LoadVideosWithFileNames ( video_file_names, camera_names );
    video_synchronizer.SynchronizeVideoWithAudio ( shift_window*2 );
    vector<VideoClip> synchronized_video_clip = video_synchronizer.GetVideoClips();
    cout << "\tAll videos are synchronized." << endl;

    // Creates video feeds with synchronization and calibration.
    for ( unsigned i=0; i<synchronized_video_clip.size(); i++ )
    {
        Camera camera = _cameras_map[camera_names[i]];
        _video_feeds.push_back ( VideoFeed ( synchronized_video_clip[i], camera, 10 ) );
    }
}

void PanoVideoMapper::GeneratePano()
{
    namedWindow ( "Panoramic frame", CV_WINDOW_NORMAL );
    resizeWindow ( "Panoramic frame", 1000, 500 );
    double current_time = 0.0;
    while ( true )
    {
        bool more_frame = false;
        Mat output_frame = Mat::zeros(1000, 2000, CV_8UC3);
	Mat output_frame_weight = Mat::zeros(1000, 2000, CV_32SC1);
        for ( unsigned c=0; c<_video_feeds.size(); c++ )
        {
            VideoFeed* video_feed = &_video_feeds[c];
            more_frame |= video_feed->PaintOnCanvas ( current_time, &output_frame, &output_frame_weight );
        }
        imshow ( "Panoramic frame", output_frame );
	
        current_time += 1.0/3;

        char enter = cvWaitKey ( 1 );
        if ( !more_frame || enter == 'q' )
        {
            break;
        }
    }
}

void PanoVideoMapper::ReadVideoFileAndCameraNames ( const string& video_list_file_name, vector<string>* camera_names, vector<string>* video_file_names )
{
    FileStorage file_storage ( video_list_file_name, FileStorage::READ );
    FileNode video_file_node = file_storage["Videos"];
    FileNodeIterator video_file_iterator = video_file_node.begin();
    FileNodeIterator video_file_iterator_end = video_file_node.end();
    for ( ; video_file_iterator != video_file_iterator_end; ++video_file_iterator )
    {
        string video_file_name = ( string ) ( *video_file_iterator ) ["File"];
        string camera_name = ( string ) ( *video_file_iterator ) ["CameraName"];
        if ( !Utils::FileExists ( video_file_name ) )
        {
            exit ( -1 );
        }
        video_file_names->push_back ( video_file_name );
        camera_names->push_back ( camera_name );
    }
    file_storage.release();
}

void PanoVideoMapper::ReadCameraCalibration ( const string& calibration_file_name )
{
    FileStorage file_storage ( calibration_file_name, FileStorage::READ );
    FileNode camera_node = file_storage["Cameras"];
    FileNodeIterator camera_node_iterator = camera_node.begin();
    FileNodeIterator camera_node_iterator_end = camera_node.end();
    for ( ; camera_node_iterator != camera_node_iterator_end; ++camera_node_iterator )
    {
        string camera_name = ( *camera_node_iterator ) ["Name"];
        double u0 = ( *camera_node_iterator ) ["U0"];
        double v0 = ( *camera_node_iterator ) ["V0"];
        vector<double> affine, poly, inv_poly, extrinsic;
        affine.push_back ( ( double ) ( *camera_node_iterator ) ["C"] );
        affine.push_back ( ( double ) ( *camera_node_iterator ) ["D"] );
        affine.push_back ( ( double ) ( *camera_node_iterator ) ["E"] );
        ( *camera_node_iterator ) ["Poly"] >> poly;
        ( *camera_node_iterator ) ["InversePoly"] >> inv_poly;
        ( *camera_node_iterator ) ["Extrinsic"] >> extrinsic;

        Camera camera ( camera_name, u0, v0, affine, poly, inv_poly, extrinsic );
        _cameras_map[camera_name] = camera;
    }
}
