#include "include/utils.h"

bool Utils::FileExists ( const string& path )
{
    return boost::filesystem::is_regular_file(path);
}

bool Utils::FolderExists(const string& path)
{
    return boost::filesystem::is_directory(path);
}

void Utils::CreateFolderIfNotExists(const std::string& path) {
    if (!Utils::FolderExists(path)) {
        boost::filesystem::create_directory(path);
    }
}

vector<string> Utils::GetFileList(const string& path)
{
    vector<string> file_list;
    for(auto it = boost::filesystem::directory_iterator(path);
            it != boost::filesystem::directory_iterator(); ++it) {
        if(boost::filesystem::is_regular_file(*it)) {
            const boost::filesystem::path file_path = *it;
            file_list.push_back(file_path.filename().string());
        }
    }
    return file_list;
}

void Utils::ClearFolder(const string& path)
{
    if(Utils::FolderExists(path)) {
        for(auto it = boost::filesystem::directory_iterator(path);
                it != boost::filesystem::directory_iterator(); ++it) {
            boost::filesystem::remove_all(it->path());
        }
    }
}

string Utils::EnsureTrailingSlash(const string& str)
{
    if ( str.length() > 0 ) {
        if ( str.back() != '/' ) {
            return str + "/";
        }
    } else {
        return str + "/";
    }
    return str;
}

bool Utils::EndsWith(const string& str, const string& end)
{
    if (str.length() >= end.length()) {
        return (0 == str.compare (str.length() - end.length(), end.length(), end));
    } else {
        return false;
    }
}

void Utils::GetTransform44FromExtrinsic ( const vector< double >& extrinsic, Mat* transform_4_4 )
{
    CV_Assert ( transform_4_4->rows == 4 && transform_4_4->cols == 4 );
    Mat rotation_vector ( 3, 1, CV_64FC1, ( void* ) &extrinsic[0] );
    Mat translation_vector ( 3, 1, CV_64FC1, ( void* ) &extrinsic[3] );
    Rodrigues ( rotation_vector, transform_4_4->colRange ( 0, 3 ).rowRange ( 0, 3 ) );
    translation_vector.copyTo ( transform_4_4->col ( 3 ).rowRange ( 0, 3 ) );
}

Point3d Utils::GetSpherePointFromScreenPoint ( const Point2d& screen_point, const Size& canvas_size, const double radius )
{
    double elevation = M_PI * ( screen_point.y / canvas_size.height - 0.5 );
    double azimuth = 2 * M_PI * screen_point.x / canvas_size.width;
    double y = sin ( elevation );
    double sub_radius = cos ( elevation );
    double x = sub_radius * sin ( azimuth );
    double z = sub_radius * cos ( azimuth );
    return Point3d ( x, y, z );
}

double Utils::EvaluatePolyEquation ( const double* coefficients, const int n, const double x )
{
    double y = 0.0;
    double x_i = 1.0;
    for ( int power=0; power<n; power++ )
    {
        y += coefficients[power] * x_i;
        x_i *= x;
    }
    return y;
}



