#include "camera.h"

Camera::Camera ( const string& name, const int width, const int height, const double u0, const double v0,
                 const vector< double >& affine, const vector< double >& poly, const vector< double >& inv_poly,
                 const vector< double >& extrinsic )
    : _name ( name ), _width ( width ), _height ( height ), _u0 ( u0 ), _v0 ( v0 ), _c ( affine[0] ), _d ( affine[1] ), _e ( affine[2] ),
    _poly ( poly ), _inverse_poly ( inv_poly )
{
    _transform_4_4 = Mat::eye ( 4, 4, CV_64FC1 );
    Utils::GetTransform44FromExtrinsic ( extrinsic, &_transform_4_4 );
    _transform_4_4.col ( 3 ).rowRange ( 0, 3 ) *= 0.0;
}

Mat Camera::ProjectWorldToFrame ( const Mat& world_pts, const bool debug ) const
{
    CV_Assert ( world_pts.cols == 3 );
    Mat world_pts_n_4 = Mat::ones ( world_pts.rows, 4, world_pts.type() );
    world_pts.copyTo ( world_pts_n_4.colRange ( 0, 3 ) );
    Mat camera_pts_n_4 = world_pts_n_4 * _transform_4_4.inv().t();
    Mat frame_pts_n_2 ( camera_pts_n_4.rows, 2, CV_64FC1 );

    if ( debug )
    {
        cout << world_pts_n_4 << endl;
        cout << camera_pts_n_4 << endl;
        cout << _transform_4_4 << endl;
    }

    for ( int i=0; i<camera_pts_n_4.rows; i++ )
    {
        ProjectCameraToFrame ( camera_pts_n_4.row ( i ) ).copyTo ( frame_pts_n_2.row ( i ) );
    }

    return frame_pts_n_2;
}

Mat Camera::ProjectCameraToFrame ( const Mat& camera_pt ) const
{
    CV_Assert ( camera_pt.cols >= 3 );

    // Gets x y z from matrix.
    double x = camera_pt.at<double> ( 0, 0 );
    double y = camera_pt.at<double> ( 0, 1 );
    double z = camera_pt.at<double> ( 0, 2 );
    // Result matrix.
    Mat frame_pt ( 1, 2, CV_64FC1, numeric_limits<double>().max() );
    // If point is behind camera, return invalid result.
    if ( z < 0.0 )
    {
        return frame_pt;
    }
    // Calculates norm on xy plane.
    double norm_on_xy = sqrt ( x*x + y*y );
    if ( norm_on_xy == 0.0 )
    {
        norm_on_xy = 1e-14;
    }
    // Calculates projected position on the frame.
    double theta = atan2 ( -z, norm_on_xy );
    double rho = Utils::EvaluatePolyEquation ( _inverse_poly.data(), _inverse_poly.size(), theta );
    // u, v of corner on frame without affine.
    double u = x * rho / norm_on_xy;
    double v = y * rho / norm_on_xy;
    // Affines corner on frame.
    frame_pt.at<double> ( 0, 0 ) = _c * u + _d * v + _u0;
    frame_pt.at<double> ( 0, 1 ) = _e * u + v + _v0;
    return frame_pt;
}
