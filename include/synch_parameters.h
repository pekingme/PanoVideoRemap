#ifndef SYNCHPARAMETERS_H
#define SYNCHPARAMETERS_H

#include <vector>
#include <string>
  
using namespace std;
using namespace boost;
  
struct SynchParameters
{
  vector<string> video_file_vector;
  vector<string> camera_name_vector;
  vector<double> time_offset;
  double shift_window;
};

#endif // SYNCHPARAMETERS_H
