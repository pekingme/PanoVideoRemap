#ifndef COMBINEDVIDEOCLIP_H
#define COMBINEDVIDEOCLIP_H

#include <iostream>
#include <string>
#include <vector>

#include "utils.h"
#include "video_clip.h"
#include "synch_parameters.h"

#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

class CombinedVideoClip
{
public:
    CombinedVideoClip () {}
    CombinedVideoClip ( const SynchParameters& parameters, const bool synchronized = false )
        :parameters_ ( parameters ), synchronized_ ( synchronized ) {}

    // Read synchronization parameters from yaml file.
    static void ReadSynchParametersFromFile ( const string& file_name, SynchParameters* parameters );

    // Saves synchronization result including video locations, recording camera names, and time offset in seconds.
    void SaveSynchronizationResult ( const string& file_name );
    
    // Creates video clips from the filenames with camera names.
    // Video files will be verified. If any file doesn't exist, it will exit exceptionally.
    void LoadVideosWithFileNames ( const bool synchronized = false );

    // Synchronizes video clips based on audio samples in a certain range of time.
    // The maximum time shift between the first and last video clips is no more than the window.
    void SynchronizeVideoWithAudio ();

    // Reads synchronized frames from each video and stores them in a vector.
    // If the video has not started or finished, the frame return will be empty.
    // If videos has not been synchronized, it will exit exceptionally.
    vector<Mat> ReadFramesVector ( const double global_time, const bool to_gray );

    int GetVideoCount()
    {
        return video_count_;
    }

    // Playbacks all videos with synchronizing shifts together.
    void ViewSynchronizedVideos ();

    vector<string> GetCameraNames()
    {
        return parameters_.camera_name_vector;
    }
private:
    // Synchronizes one video clip by comparing its audio samples with referecne samples, and returns the amount of time it leads the reference.
    double SynchronizeToReference ( const Mat& reference_samples, const Mat& audio_samples, VideoClip* video_clip );

    SynchParameters parameters_;
    vector<VideoClip> video_clip_vector_;
    int video_count_;
    bool synchronized_;
};

#endif // COMBINEDVIDEOCLIP_H
