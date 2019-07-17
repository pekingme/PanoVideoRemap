#include "combined_video_clip.h"

void CombinedVideoClip::ReadSynchParametersFromFile ( const string& file_name, SynchParameters* parameters )
{
    if ( !Utils::FileExists ( file_name ) )
    {
        cerr << "Input file not exists" << endl;
        exit ( -1 );
    }
    FileStorage file_storage ( file_name, FileStorage::READ );
    if ( file_storage["MaxShift"].empty() )
    {
        parameters->shift_window = 5;
    }
    else
    {
        parameters->shift_window = file_storage["MaxShift"];
    }
    FileNode video_file_node = file_storage["Videos"];
    FileNodeIterator video_file_iterator = video_file_node.begin();
    FileNodeIterator video_file_iterator_end = video_file_node.end();
    for ( ; video_file_iterator != video_file_iterator_end; ++video_file_iterator )
    {
        string video_file_name = ( string ) ( *video_file_iterator ) ["File"];
        parameters->video_file_vector.push_back ( video_file_name );
        string camera_name = ( string ) ( *video_file_iterator ) ["CameraName"];
        parameters->camera_name_vector.push_back ( camera_name );
        if ( ( *video_file_iterator ) ["Offset"].empty() )
        {
            parameters->time_offset.push_back ( 0.0 );
        }
        else
        {
            parameters->time_offset.push_back ( ( double ) ( *video_file_iterator ) ["Offset"] );
        }
    }
    file_storage.release();
}

void CombinedVideoClip::SaveSynchronizationResult ( const string& file_name )
{
    FileStorage file_storage ( file_name, FileStorage::WRITE );
    file_storage << "Videos" << "[";
    for ( unsigned i=0; i<video_clip_vector_.size(); i++ )
    {
        VideoClip* clip = &video_clip_vector_[i];
        file_storage << "{";
        file_storage << "CameraName" << clip->GetCameraName();
        file_storage << "File" << clip->GetFileName();
        file_storage << "Offset" << clip->GetShiftInSeconds();
        file_storage << "}";
    }
    file_storage << "]";
    file_storage << "MaxShift" << parameters_.shift_window;
    file_storage.release();
}

void CombinedVideoClip::LoadVideosWithFileNames ( const bool synchronized )
{
    if ( parameters_.camera_name_vector.size() != parameters_.video_file_vector.size() )
    {
        cerr << "The numbers of camera names and video names don't match." << endl;
        exit ( -1 );
    }
    if ( parameters_.video_file_vector.size() == 0 )
    {
        cerr << "No videos are associated for calibration." << endl;
        exit ( -1 );
    }
    video_count_ = parameters_.video_file_vector.size();
    video_clip_vector_.clear();
    video_clip_vector_.resize ( video_count_ );
    for ( int i=0; i<video_count_; i++ )
    {
        if ( !Utils::FileExists ( parameters_.video_file_vector[i] ) )
        {
            cerr << "Video file not found: " << parameters_.video_file_vector[i] << endl;
            exit ( -1 );
        }
        video_clip_vector_[i] = VideoClip ( parameters_.video_file_vector[i], parameters_.camera_name_vector[i] );
        video_clip_vector_[i].SetShiftInSeconds ( parameters_.time_offset[i] );
    }

    synchronized_ = synchronized;
}

void CombinedVideoClip::SynchronizeVideoWithAudio ()
{
    if ( video_count_ < 2 )
    {
        cerr << "Synchronization can only be performed between 2 or more videos." << endl;
        exit ( -1 );
    }

    // Load audio samples from all videos
    vector<Mat> audio_samples;
    double sample_window = parameters_.shift_window * 2;
    for ( int i=0; i<video_count_; i++ )
    {
        Mat samples;
        VideoClip* video_clip = &video_clip_vector_[i];
        if ( !video_clip->ExtractAudioSamples ( &samples, sample_window ) )
        {
            cerr << "Cannot read audio samples from video " << i << endl;
            exit ( -1 );
        }
        audio_samples.push_back ( samples );
    }

    // Calculates time shifts relative to the first video.
    video_clip_vector_[0].SetShiftInSeconds ( 0.0 );
    double max_leading = 0.0;
    for ( int i=1; i<video_count_; i++ )
    {
        double leading = SynchronizeToReference ( audio_samples[0], audio_samples[i], &video_clip_vector_[i] );
        max_leading = min ( max_leading, leading );
    }

    // Offsets all time shift relative to the first started video.
    for ( int i=0; i<video_count_; i++ )
    {
        VideoClip* video_clip = &video_clip_vector_[i];
        video_clip->SetShiftInSeconds ( video_clip->GetShiftInSeconds() - max_leading );
        cout << "\t" << video_clip->GetShiftInSeconds() << " seconds: " << video_clip->GetFileName() << endl;
    }

    synchronized_ = true;
}

vector< Mat > CombinedVideoClip::ReadFramesVector ( const double global_time, const bool to_gray )
{
    if ( !synchronized_ )
    {
        cerr << "Videos has not synchronized." << endl;
        exit ( -1 );
    }
    vector<Mat> frames ( video_count_ );
    for ( int i=0; i<video_count_; i++ )
    {
        VideoClip* video_clip = &video_clip_vector_[i];
        Mat frame = video_clip->ReadSynchedFrame ( global_time );
        if ( !to_gray || frame.empty() || frame.channels() == 1 )
        {
            frames[i] = frame;
        }
        else
        {
            Mat grayscale;
            cvtColor ( frame, grayscale, CV_BGR2GRAY );
            frames[i] = grayscale;
        }
    }
    return frames;
}

void CombinedVideoClip::ViewSynchronizedVideos()
{
    if ( video_count_ < 1 )
    {
        cerr << "No video to playback." << endl;
        exit ( -1 );
    }
    // Creates windows to show each video.
    for ( int i=0; i<video_count_; i++ )
    {
        VideoClip* video_clip = &video_clip_vector_[i];
        namedWindow ( video_clip->GetCameraName(), CV_WINDOW_NORMAL );
    }

    // Playback.
    cout << "Press any key to start playback, then 'q' to quit." << endl;
    cvWaitKey ( 0 );
    double current_time = 0.0;
    while ( true )
    {
        bool no_more_frame = true;
        for ( int i=0; i<video_count_; i++ )
        {
            VideoClip* video_clip = &video_clip_vector_[i];
            Mat frame = video_clip->ReadSynchedFrame ( current_time );
            if ( frame.empty() )
            {
                continue;
            }
            imshow ( video_clip->GetCameraName(), frame );
            no_more_frame = false;
        }
        char enter = cvWaitKey ( 1 );
        if ( enter == 'q' || no_more_frame )
        {
            break;
        }
        else
        {
            current_time += 1.0/30;
        }
    }
}

double CombinedVideoClip::SynchronizeToReference ( const Mat& reference_samples, const Mat& audio_samples, VideoClip* video_clip )
{
    int padding_size = audio_samples.cols;
    Mat reference_audio_sample_with_padding, correlation;
    copyMakeBorder ( reference_samples, reference_audio_sample_with_padding, 0, 0, padding_size, padding_size, cv::BORDER_CONSTANT, Scalar ( 0 ) );
    matchTemplate ( reference_audio_sample_with_padding, audio_samples, correlation, cv::TM_CCORR );

    double min_val, max_val;
    Point min_pos, max_pos;
    minMaxLoc ( correlation, &min_val, &max_val, &min_pos, &max_pos, Mat() );
    video_clip->SetShiftInSeconds ( ( max_pos.x-padding_size ) / video_clip->GetAudioSampleRate() );

    return video_clip->GetShiftInSeconds();
}
