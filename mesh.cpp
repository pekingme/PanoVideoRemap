#include "mesh.h"

Mesh::Mesh ( const int row_index, const int col_index, const int mesh_size, const Size& canvas_size, const Camera& camera )
{
    _y_1= row_index * mesh_size;
    _x_1 = col_index * mesh_size;
    _y_2 = min ( _y_1 + mesh_size, canvas_size.height ) - 1;
    _x_2 = min ( _x_1 + mesh_size, canvas_size.width ) - 1;
    // Calculates corners in canvas.
    vector<Point2d> canvas_corners;
    canvas_corners.push_back ( Point2d ( _x_1, _y_1 ) );
    canvas_corners.push_back ( Point2d ( _x_2, _y_1 ) );
    canvas_corners.push_back ( Point2d ( _x_1, _y_2 ) );
    canvas_corners.push_back ( Point2d ( _x_2, _y_2 ) );
    // Calculates corners in sphere shaped screen.
    vector<Point3d> sphere_corners;
    sphere_corners.push_back ( Utils::GetSpherePointFromScreenPoint ( canvas_corners[0], canvas_size, 1.0 ) );
    sphere_corners.push_back ( Utils::GetSpherePointFromScreenPoint ( canvas_corners[1], canvas_size, 1.0 ) );
    sphere_corners.push_back ( Utils::GetSpherePointFromScreenPoint ( canvas_corners[2], canvas_size, 1.0 ) );
    sphere_corners.push_back ( Utils::GetSpherePointFromScreenPoint ( canvas_corners[3], canvas_size, 1.0 ) );
    // Calculates corners in camera coordinates system.
    Mat sphere_corners_mat_n_3 = Mat ( sphere_corners ).reshape ( 1, sphere_corners.size() );
    Mat frame_corners_mat_n_2 = camera.ProjectWorldToFrame ( sphere_corners_mat_n_3, false );
    _pt_a = Point2d ( frame_corners_mat_n_2.at<double> ( 0, 0 ), frame_corners_mat_n_2.at<double> ( 0, 1 ) );
    _pt_b = Point2d ( frame_corners_mat_n_2.at<double> ( 1, 0 ), frame_corners_mat_n_2.at<double> ( 1, 1 ) );
    _pt_c = Point2d ( frame_corners_mat_n_2.at<double> ( 2, 0 ), frame_corners_mat_n_2.at<double> ( 2, 1 ) );
    _pt_d = Point2d ( frame_corners_mat_n_2.at<double> ( 3, 0 ), frame_corners_mat_n_2.at<double> ( 3, 1 ) );
}

void Mesh::Paint ( Mat* canvas, const Mat& frame, Mat* weight_mat )
{
    // If all corners are out of bound, then skip the painting.
    if ( IsOutOfBound ( _pt_a, frame ) && IsOutOfBound ( _pt_b, frame )
            && IsOutOfBound ( _pt_c, frame ) && IsOutOfBound ( _pt_d, frame ) )
    {
        return;
    }
    for ( int p_x=_x_1; p_x<=_x_2; p_x++ )
    {
        for ( int p_y=_y_1; p_y<=_y_2; p_y++ )
        {
            double a_x = ( double ) ( p_x - _x_1 ) / ( _x_2 - _x_1 );
            double a_y = ( double ) ( p_y - _y_1 ) / ( _y_2 - _y_1 );
            Point2d pt = ( 1 - a_y ) * ( ( 1 - a_x ) * _pt_a + a_x * _pt_b )
                         + a_y * ( ( 1 - a_x ) *_pt_c + a_x * _pt_d );
            if ( IsOutOfBound ( pt, frame ) )
            {
                continue;
            }
            frame.row ( ( int ) pt.y ).col ( ( int ) pt.x ).copyTo ( canvas->row ( p_y ).col ( p_x ) );
        }
    }
}

bool Mesh::IsOutOfBound ( const Point2d& pt, const Mat& frame )
{
    return pt.x < 0 || pt.y < 0 || pt.x >= frame.cols || pt.y >= frame.rows;
}
