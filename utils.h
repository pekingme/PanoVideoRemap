#ifndef UTILS_H
#define UTILS_H

#include <iostream>

using namespace std;

class Utils
{
public:
    // Returns if a file exists in file system.
    static bool FileExists ( const string& filename );
};

#endif // UTILS_H
