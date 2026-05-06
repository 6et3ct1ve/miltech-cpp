#include <fstream>
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>

const int ticks_per_revolution = 1024;
const double wheel_radius_m = 0.3;
const double wheelbase_m = 1.0;

int main(int argc, char** argv) 
{
    // The program expects exactly one argument: a path to telemetry samples.
    if (argc != 2) 
    {
        std::cerr << "usage: ugv_odometry <input_path>\n";
        return 1;
    }

    std::ifstream fin(argv[1]);
        if (!fin.is_open())
    {
        std::cerr << "File read error\n";
        return 1;
    }

    long prev_timestamp, prev_fl, prev_fr, prev_bl, prev_br;

    fin >> prev_timestamp >> prev_fl >> prev_fr >> prev_bl >> prev_br;

    long timestamp_ms, fl_ticks, fr_ticks, bl_ticks, br_ticks;

    while (fin >> timestamp_ms >> fl_ticks >> fr_ticks >> bl_ticks >> br_ticks) 
    {
        long d_fl = fl_ticks - prev_fl;
        long d_fr = fr_ticks - prev_fr;
        long d_bl = bl_ticks - prev_bl;
        long d_br = br_ticks - prev_br;

        double d_left  = static_cast<double>(d_fl + d_bl)/2.0;
        double d_right = static_cast<double>(d_fr + d_br)/2.0;

        double distance_per_tick = 2.0 * M_PI * wheel_radius_m / static_cast<double>(ticks_per_revolution);
        double dL = d_left  * distance_per_tick;
        double dR = d_right * distance_per_tick;


    }

    if (fin.fail()) 
    { 
        std::cout << "Input format error\n"; 
        return 1;
    }
    fin.close();

    // TODO: implement wheel odometry for a 4-wheel differential-drive UGV.
    //
    // Model parameters:
    //   ticks_per_revolution = 1024
    //   wheel_radius_m       = 0.3
    //   wheelbase_m          = 1.0
    //
    // Input: a text file with 5 whitespace-separated values per line:
    //         timestamp_ms fl_ticks fr_ticks bl_ticks br_ticks
    // Output: a table on stdout, starting from the second sample:
    //         timestamp_ms x y theta

    return 0;
}
