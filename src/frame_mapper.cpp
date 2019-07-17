#include "frame_mapper.h"

FrameMapper::FrameMapper ( const Camera& camera, const Size& output_size, const int project_size )
    :_camera ( camera ), _output_size ( output_size )
{
    int width = output_size.width;
    int height = output_size.height;
    _weight_mat = Mat::zeros ( height, width, CV_64FC1 );
    for ( int i=0; i*project_size<width; i++ )
    {
        for ( int j=0; j*project_size<height; j++ )
        {
            Mesh mesh = Mesh ( j, i, project_size, output_size, camera, &_weight_mat );
            if ( mesh.CheckValidity(camera) )
            {
                _mesh_vector.push_back ( mesh );
            }
        }
    }
}

void FrameMapper::PaintOnCanvas ( const Mat& frame, Mat* canvas )
{
    // Paints each mesh.
    for ( unsigned i=0; i<_mesh_vector.size(); i++ )
    {
        Mesh* mesh = &_mesh_vector[i];
        mesh->Paint ( canvas, frame, _weight_mat );
    }
}

void FrameMapper::NormalizeWeight ( Mat total_weight )
{
    Mat normalized_weight_mat = Mat::zeros ( total_weight.rows, total_weight.cols, total_weight.type() );
    divide ( _weight_mat, total_weight, normalized_weight_mat );

    normalized_weight_mat.copyTo ( _weight_mat );
}

Mat FrameMapper::GetWeightMat()
{
    return _weight_mat;
}
