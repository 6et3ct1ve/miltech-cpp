#include "ballistics.hpp"

#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#define _USE_MATH_DEFINES

int main(int argc, char* argv[])  // NOLINT(modernize-use-trailing-return-type) - stylistic only, main() signature is idiomatic as-is
{
  if (argc < 2) {
    std::cerr << "No input file specified\n";
    return 1;
  }

  DroneState drone{};
  std::ifstream input(argv[1]);  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic) - bounds already checked via argc above
  if (!input.is_open()) {
    std::cerr << "Unable to read input\n";
    return 1;
  }
  input >> drone.xd >> drone.yd >> drone.zd >> drone.targetX >> drone.targetY >> drone.attackSpeed >> drone.accelerationPath >>
    drone.ammo_name;
  if (input.fail()) {
    std::cerr << "Input format error\n";
    return 1;
  }
  input.close();

  bool ok = true;

  DropPoint dropPoint = compute_drop_point(drone, ok);

  if (!ok) {
    return 1;
  }

  std::ofstream output("output.txt");
  if (!output.is_open()) {
    std::cerr << "Unable to write output\n";
    return 1;
  }
  output << dropPoint.fireX << " " << dropPoint.fireY;
  output.close();

  std::cout << "Success\nDrop point: " << dropPoint.fireX << ", " << dropPoint.fireY << "\n";

  return 0;
}