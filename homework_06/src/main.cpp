#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#define _USE_MATH_DEFINES

const double g = 9.81;
struct Ammo { float m, d, l; };

double cardano(Ammo ammo, float v0, float z0)
{
    double a = ammo.d*g*ammo.m - 2*pow(ammo.d, 2)*ammo.l*v0;
    if (fabs(a) < 1e-9) return -1;
    double b = -3*g*pow(ammo.m,2) + 3*ammo.d*ammo.l*ammo.m*v0;
    double c = 6*pow(ammo.m,2)*z0;

    double p = -pow(b, 2)/(3*pow(a, 2));
    double q = 2*pow(b, 3)/(27*pow(a, 3)) + c/a;

    double arg = 3*q/(2*p) * sqrt(static_cast<double>(-3)/p);
    if (arg < -1.0 || arg > 1.0) return -1;

    double phi = acos(arg);
    
    return 2*sqrt(-p/3)*cos((phi+4*M_PI)/3) - b/(3*a);
}

double horizon(double t, float v0, Ammo ammo)
{
    return v0*t\
        - pow(t, 2)*ammo.d*v0/(2*ammo.m) \
        + pow(t, 3)*(6*ammo.d*g*ammo.l*ammo.m - 6*pow(ammo.d, 2)*(pow(ammo.l,2) - 1)*v0)/(36*pow(ammo.m, 2))\
        + pow(t, 4)*(-6*pow(ammo.d,2)*g*ammo.l*(1 + pow(ammo.l, 2)+ pow(ammo.l, 4))*ammo.m \
            + 3*pow(ammo.d, 3)*pow(ammo.l, 2)*(1 + pow(ammo.l, 2))*v0 \
            + 6*pow(ammo.d, 3)*pow(ammo.l, 4)*(1 + pow(ammo.l, 2))*v0)\
            /(36*pow((1 + pow(ammo.l, 2)), 2)*pow(ammo.m, 3))\
        + pow(t, 5)*(3*pow(ammo.d, 3)*g*pow(ammo.l, 3)*ammo.m \
            - 3*pow(ammo.d, 4)*pow(ammo.l, 2)*(1 + pow(ammo.l, 2))*v0)\
            /(36*(1 + pow(ammo.l, 2))*pow(ammo.m, 4));
}

int main()
{ 
    float xd = 0, yd = 0, zd = 0,\
        targetX = 0, targetY = 0,\
        attackSpeed = 0,\
        accrlerationPath = 0;
    char ammo_name[32];
    std::ifstream input("input.txt");
    if (!input.is_open())
    {
        std::cout << "Unable to read input\n";
        return 1;
    }
    input >> xd >> yd >> zd >> targetX >> targetY \
            >> attackSpeed >> accrlerationPath;     
    input.width(31);  
    input >> ammo_name;
    if (input.fail()) 
    { 
        std::cout << "Input format error\n"; 
        return 1; 
    }
    input.close();

    Ammo ammo;
    if (strcmp(ammo_name, "VOG-17") == 0)
        ammo.m = 0.35f, ammo.d = 0.07f, ammo.l = 0.0f;
    else if (strcmp(ammo_name, "M67") == 0)
        ammo.m = 0.6f, ammo.d = 0.10f, ammo.l = 0.0f;
    else if (strcmp(ammo_name, "RKG-3") == 0)
        ammo.m = 1.2f, ammo.d = 0.10f, ammo.l = 0.0f;
    else if (strcmp(ammo_name, "GLIDING-VOG") == 0)
        ammo.m = 0.45f, ammo.d = 0.10f, ammo.l = 1.0f;
    else if (strcmp(ammo_name, "GLIDING-RKG") == 0)
        ammo.m = 1.4f, ammo.d = 0.10f, ammo.l = 1.0f;
    else
    { 
        std::cout << "Unknown ammo\n"; 
        return 1;
    }

    double t = cardano(ammo, attackSpeed, zd);
    if (t < 0) 
    {
        std::cout << "Time error\n";
        return 1;
    }

    double h = horizon(t, attackSpeed, ammo);
    if (h < 0) 
    {
        std::cout << "Hotizon error\n"; 
        return 1;
    }
    
    double D = sqrt(pow(targetX - xd, 2) + pow(targetY - yd, 2));
    if (fabs(D) < 1e-9)
    {
        std::cout << "Calculation error\n";
        return 1;
    }

    if (h + accrlerationPath > D)
    {
        xd = targetX - (targetX - xd)*(h + accrlerationPath)/D;
        yd = targetY - (targetY - yd)*(h + accrlerationPath)/D;
        D = sqrt(pow(targetX - xd, 2) + pow(targetY - yd, 2));
    }

    double ratio = (D - h)/D;
    
    double fireX = xd + (targetX - xd)*ratio;
    double fireY = yd + (targetY - yd)*ratio;

    std::ofstream output("output.txt");
    if (!output.is_open())
    {
        std::cout << "Unable to write output";
        return 1;
    }
    output << fireX << " " << fireY;
    output.close();

    std::cout << "Success\n";

    return 0;
}