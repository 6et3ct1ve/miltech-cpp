#include <fstream>
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>

const int ticks_per_revolution = 1024;
const double wheel_radius_m = 0.3;
const double wheelbase_m = 1.0;
const double distance_per_tick = 2.0*M_PI*wheel_radius_m/static_cast<double>(ticks_per_revolution);

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
    
    double x = 0.0, y = 0.0, theta = 0.0;

    while (fin >> timestamp_ms >> fl_ticks >> fr_ticks >> bl_ticks >> br_ticks) 
    {
        long d_fl = fl_ticks - prev_fl;
        long d_fr = fr_ticks - prev_fr;
        long d_bl = bl_ticks - prev_bl;
        long d_br = br_ticks - prev_br;

        double d_left  = static_cast<double>(d_fl + d_bl)/2.0;
        double d_right = static_cast<double>(d_fr + d_br)/2.0;

        double dL = d_left*distance_per_tick;
        double dR = d_right*distance_per_tick;

        double d = (dL + dR)/2.0;
        double dtheta = (dR - dL)/wheelbase_m;

        x += d*cos(theta + dtheta/2);
        y += d*sin(theta + dtheta/2);
        theta += dtheta;

        std::cout << timestamp_ms << " " << x << " " << y << " " << theta << "\n";

        prev_timestamp = timestamp_ms;
        prev_fl = fl_ticks;
        prev_fr = fr_ticks;
        prev_bl = bl_ticks;
        prev_br = br_ticks;

    }

    if (fin.bad()) 
    { 
        std::cout << "Input format error\n"; 
        return 1;
    }
    fin.close();

    return 0;
}
