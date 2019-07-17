#include "pano_video_mapper.h"

PanoVideoMapper::PanoVideoMapper ( const string& output_folder, const string& video_list_file )
    : output_folder_(output_folder), fps_ ( 30 ), output_size_ ( 2000, 1000 )
{
    cout << "Input video list file: " << video_list_file << endl;
    cout << "Output result folder: " << output_folder << endl;

    // Create output folder if not existed
    Utils::CreateFolderIfNotExists(output_folder);

    // Checks the input videos (if they are in pairs)
    cout << endl << "Checking content of input video folders." << endl;
    ReadInVideoListFile(video_list_file);
}

void PanoVideoMapper::ReadInVideoListFile(const string& video_list_file) {
    if(!Utils::FileExists(video_list_file)) {
        cerr<<"Video list file doesn't exist" << endl;
        exit(-1);
    }

    FileStorage file_storage(video_list_file, FileStorage::READ);
    FileNode video_node = file_storage["Videos"];
    FileNodeIterator video_node_iterator = video_node.begin();
    FileNodeIterator video_node_iterator_end = video_node.end();
    unordered_map<string, vector<string>> video_map;
    for(; video_node_iterator!=video_node_iterator_end; ++video_node_iterator) {
        string camera_name = (*video_node_iterator)["CameraName"];
        string video_folder = (*video_node_iterator)["Folder"];
        if(!Utils::FolderExists(video_folder)) {
            cerr << endl << "Vidoe folder doesn't exist" << endl;
            cerr << video_folder<<endl;
            exit(-1);
        }
        vector<string> video_list = Utils::GetFileList(video_folder);
        for(const string video_name : video_list) {
            video_map[video_name].emplace_back(camera_name);
        }
        camera_names_.emplace_back(camera_name);
        video_folders_.emplace_back(video_folder);
    }

    vector<string> videos;
    for(const auto pair : video_map) {
        if(Utils::EndsWith(pair.first, ".MP4") || Utils::EndsWith(pair.first, ".mp4")) {
            for(const auto camera_name : camera_names_) {
                if(find(pair.second.begin(), pair.second.end(), camera_name) == pair.second.end()) {
                    cout << camera_name << " doesn't have video " << pair.first << endl;
                }
            }
            video_names_.emplace_back(pair.first);
        }
    }

    cout << endl << "Videos to continue:" << endl;
    for(const auto video_name : video_names_) {
        cout << video_name << endl;
    }

    // Read time shift tolerance
    if(!file_storage["MaxShift"].empty()) {
        max_shift_ = file_storage["MaxShift"];
    } else {
        max_shift_ = 5;
    }

    // Read first sample index
    if(!file_storage["SampleStartIndex"].empty()) {
        next_image_index_ = (int) file_storage["SampleStartIndex"];
    } else {
        next_image_index_ = 0;
    }
}

void PanoVideoMapper::GeneratePano(const string& calibration_file)
{
    // Trash all contents in output folder
    Utils::ClearFolder(output_folder_);

    // Loads camera intrinsic and extrinsic parameters.
    cout << "\tLoading camera system calibration." << endl;
    ReadCameraCalibration ( calibration_file );

    // Creates frame mappers for cameras.
    Mat total_weight = Mat::zeros ( output_size_, CV_64FC1 );
    for(const auto& camera_keyvalue_pair : cameras_map_){
        FrameMapper frame_mapper (camera_keyvalue_pair.second, output_size_, 10);
        frame_mapper_map_[camera_keyvalue_pair.first] = frame_mapper;
        total_weight += frame_mapper.GetWeightMat();
    }
    // Normalizes weight mat in all frame mappers.
    for(auto& frame_mapper_pair : frame_mapper_map_){
        frame_mapper_pair.second.NormalizeWeight(total_weight);
    }

    for(const string& video_name : video_names_) {
        // Prepare parameters for synchronization.
        SynchParameters synch_parameter;
        synch_parameter.shift_window = max_shift_;
        synch_parameter.time_offset.emplace_back(0.0);
        synch_parameter.camera_name_vector = camera_names_;
        for(const auto video_folder : video_folders_) {
            synch_parameter.video_file_vector.emplace_back(Utils::EnsureTrailingSlash(video_folder) + video_name);
        }

        // Prepare output folder
        string video_output_folder = Utils::EnsureTrailingSlash(output_folder_)+Utils::EnsureTrailingSlash(video_name.substr(0,video_name.length()-4));
        Utils::CreateFolderIfNotExists(video_output_folder);

        // Synchronizes videos and saves the result.
        cout << "\tSynchronizing input videos." << endl;
        CombinedVideoClip combined_videos = CombinedVideoClip ( synch_parameter );
        combined_videos.LoadVideosWithFileNames ();
        combined_videos.SynchronizeVideoWithAudio ();
        cout << "\tSaving synchronization result to output folder." << endl;
        combined_videos.SaveSynchronizationResult ( video_output_folder+"SynchedVideos.yaml" );

        // Stitching video.
        vector<string> camera_names = combined_videos.GetCameraNames();
        namedWindow ( "Panoramic frame", CV_WINDOW_NORMAL );
        resizeWindow ( "Panoramic frame", 1000, 500 );
        // Note: Don't use MJPG (low compression and not work in high resolution, seems to be a bug in VideoCapture in opencv 3.4.1)
        // Use MP4V instead. Ignore the warning "fallback to use tag 0x20", it seems ffmpeg use its own codec other than fourcc.
        VideoWriter video_writer ( video_output_folder+"pano_video.mp4", CV_FOURCC ( 'M', 'P', '4', 'V' ), fps_, output_size_ );
        double current_time = 0.0;
        while ( true )
        {
            bool more_frame = false;
            Mat output_frame = Mat::zeros ( output_size_, CV_8UC3 );

            vector<Mat> frame_vector = combined_videos.ReadFramesVector ( current_time, false );
            // If all frames are empty, set flag to stop iteration.

            for ( unsigned i=0; i<frame_vector.size(); i++ )
            {
                if ( !frame_vector[i].empty() )
                {
                    more_frame = true;
                    FrameMapper* frame_mapper = &frame_mapper_map_[camera_names[i]];
                    frame_mapper->PaintOnCanvas ( frame_vector[i], &output_frame );
                }
            }
            if ( !haar_cascade_.empty() )
            {
                // Perform face detection.
                Mat gray;
                cvtColor ( output_frame, gray, CV_BGR2GRAY );
                vector<Rect_<int>> faces;
                haar_cascade_.detectMultiScale ( gray, faces, 1.1, 4, 0, Size(10, 10), Size(100, 100));
                for ( unsigned int i = 0; i<faces.size(); i++ )
                {
                    Rect face_i = faces[i];
                    rectangle ( output_frame, face_i, CV_RGB ( 0, 255, 0 ), 3 );
                }
            }
            setWindowTitle ( "Panoramic frame", "Panoramic frame - time "+to_string ( current_time ) );
            imshow ( "Panoramic frame", output_frame );
            video_writer.write ( output_frame );
            current_time += 1.0 / fps_;
            char enter = cvWaitKey ( 1 );
            if ( !more_frame || enter == 'q' )
            {
                break;
            }
        }
        video_writer.release();
    }
}

void PanoVideoMapper::SaveSamples(const float sample_rate)
{
    // Trash all contents in output folder
    Utils::ClearFolder(output_folder_);

    for(const string& video_name : video_names_) {
        // Prepare parameters for synchronization.
        SynchParameters synch_parameter;
        synch_parameter.shift_window = max_shift_;
        synch_parameter.time_offset.emplace_back(0.0);
        synch_parameter.camera_name_vector = camera_names_;
        for(const auto video_folder : video_folders_) {
            synch_parameter.video_file_vector.emplace_back(Utils::EnsureTrailingSlash(video_folder) + video_name);
        }

        // Prepare output folder
        string video_output_folder = Utils::EnsureTrailingSlash(output_folder_)+Utils::EnsureTrailingSlash(video_name.substr(0,video_name.length()-4));
        unordered_map<string, string> video_camera_output_folders;
        Utils::CreateFolderIfNotExists(video_output_folder);
        for(const auto camera_name : camera_names_) {
            string video_camera_output_folder = Utils::EnsureTrailingSlash(video_output_folder) + Utils::EnsureTrailingSlash(camera_name);
            Utils::CreateFolderIfNotExists(video_camera_output_folder);
            video_camera_output_folders[camera_name] = video_camera_output_folder;
        }

        // Synchronizes videos and saves the result.
        cout << "\tSynchronizing input videos." << endl;
        CombinedVideoClip combined_videos = CombinedVideoClip ( synch_parameter );
        combined_videos.LoadVideosWithFileNames ();
        combined_videos.SynchronizeVideoWithAudio ();
        cout << "\tSaving synchronization result to output folder." << endl;
        combined_videos.SaveSynchronizationResult ( video_output_folder + "SynchedVideos.yaml" );

        // Go over whole video to collect samples based on sample rate.
        double current_time = 5.0;
        vector<string> camera_names_from_combined_video = combined_videos.GetCameraNames();
        while(true) {
            bool more_frame = false;
            bool all_visible = true;
            vector<Mat> frame_vector = combined_videos.ReadFramesVector(current_time, false);
            for(unsigned i=0; i<frame_vector.size(); i++) {
                if(frame_vector[i].empty()) {
                    all_visible = false;
                } else {
                    more_frame = true;
                }
            }
            // Save frame if all cameras are visible.
            if(all_visible) {
                stringstream image_name_ss;
                image_name_ss << setfill('0') << setw(6) << next_image_index_ << ".jpg";
                string image_name = image_name_ss.str();
                for(unsigned i=0; i<frame_vector.size(); i++) {
                    string image_full_path = video_camera_output_folders[camera_names_from_combined_video[i]] + image_name;
                    imwrite(image_full_path, frame_vector[i]);
                }
                cout << video_name << " --> " << image_name << endl;
                next_image_index_ ++;
            }
            // Break if no more available frames.
            if(more_frame) {
                current_time += 1.0 / sample_rate;
            } else {
                break;
            }
        }
    }
}

void PanoVideoMapper::EnableFaceDetection()
{
    haar_cascade_.load ( "/home/haodong/Workspace/opencv-3.2.0/data/haarcascades/haarcascade_frontalface_alt2.xml" );
    if ( !haar_cascade_.empty() )
    {
        cout << "Face classifier is loaded" << endl;
    }
}

void PanoVideoMapper::ReadCameraCalibration ( const string& calibration_file )
{
    if ( !Utils::FileExists ( calibration_file ) )
    {
        cerr << endl << "Calibration file doesn't exist" << endl;
        exit ( -1 );
    }
    FileStorage file_storage ( calibration_file, FileStorage::READ );
    FileNode camera_node = file_storage["Cameras"];
    FileNodeIterator camera_node_iterator = camera_node.begin();
    FileNodeIterator camera_node_iterator_end = camera_node.end();
    for ( ; camera_node_iterator != camera_node_iterator_end; ++camera_node_iterator )
    {
        string camera_name = ( *camera_node_iterator ) ["Name"];
        int width = ( *camera_node_iterator ) ["Width"];
        int height = ( *camera_node_iterator ) ["Height"];
        double u0 = ( *camera_node_iterator ) ["U0"];
        double v0 = ( *camera_node_iterator ) ["V0"];
        vector<double> affine, poly, inv_poly, extrinsic;
        affine.push_back ( ( double ) ( *camera_node_iterator ) ["C"] );
        affine.push_back ( ( double ) ( *camera_node_iterator ) ["D"] );
        affine.push_back ( ( double ) ( *camera_node_iterator ) ["E"] );
        ( *camera_node_iterator ) ["Poly"] >> poly;
        ( *camera_node_iterator ) ["InversePoly"] >> inv_poly;
        ( *camera_node_iterator ) ["Extrinsic"] >> extrinsic;

        Camera camera ( camera_name, width, height, u0, v0, affine, poly, inv_poly, extrinsic );
        cameras_map_[camera_name] = camera;
    }
}

