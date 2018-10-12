#include "video_feed.h"

VideoFeed::VideoFeed ( const VideoClip& video_clip, const Camera& camera, const int project_size )
    :_video_clip ( video_clip ), _camera ( camera ), _output_size ( Size ( 2000, 1000 ) )
{
    // Loads opencv video capture.
    _video_clip.LoadVideoCapture();
    // Initializes all meshes.
    vector<Point3d> mesh_corners_vector;
    for ( int i=0; i*project_size<_output_size.height; i++ )
    {
        for ( int j=0; j*project_size<_output_size.width; j++ )
        {
	  _mesh_vector.push_back(Mesh(i, j, project_size, _output_size, _camera));
	}
    }
}

bool VideoFeed::PaintOnCanvas ( const double global_time, Mat* canvas, Mat* weight_mat )
{
    double local_time = global_time - _video_clip.GetShiftInSeconds();
    // Skips if not started.
    if ( local_time < 0.0 )
    {
        return false;
    }
    VideoCapture* video_capture = _video_clip.GetVideoCapturePtr();
    video_capture->set ( CV_CAP_PROP_POS_MSEC, local_time * 1000.0 );
    // Skips if already ened.
    Mat frame;
    if ( !video_capture->read ( frame ) )
    {
        return false;
    }
    // Paints each mesh.
    for ( unsigned i=0; i<_mesh_vector.size(); i++ )
    {
	Mesh* mesh = &_mesh_vector[i];
	mesh->Paint(canvas, frame, weight_mat);
    }

    return true;
}
