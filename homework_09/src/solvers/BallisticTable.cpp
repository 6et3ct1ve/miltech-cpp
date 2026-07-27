#include "solvers/BallisticTable.h"

#include <algorithm>
#include <array>
#include <fstream>

// NOLINTBEGIN(modernize-use-trailing-return-type, cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers,
// cppcoreguidelines-pro-bounds-constant-array-index)

namespace {

struct Interp {
  int lo;
  float frac;
};

Interp findInterp(float val, const std::vector<float>& axis)
{
  if (val <= axis.front()) {
    return {0, 0.0f};
  }
  if (val >= axis.back()) {
    return {static_cast<int>(axis.size()) - 2, 1.0f};
  }

  auto it = std::lower_bound(axis.begin(), axis.end(), val);
  int i = static_cast<int>(it - axis.begin()) - 1;
  if (i < 0) {
    i = 0;
  }

  float frac = (val - axis[i]) / (axis[i + 1] - axis[i]);
  return {i, frac};
}

BallisticTable::Result lerp(const BallisticTable::Result& a, const BallisticTable::Result& b, float t)
{
  return {
    a.t + (b.t - a.t) * t,
    a.hDist + (b.hDist - a.hDist) * t,
  };
}

}  // namespace

size_t BallisticTable::index(int iz, int iv, int im, int id, int il) const
{
  return ((((static_cast<size_t>(iz) * axisV0.size() + static_cast<size_t>(iv)) * axisM.size() + static_cast<size_t>(im)) * axisD.size() +
           static_cast<size_t>(id)) *
          axisL.size()) +
         static_cast<size_t>(il);
}

const BallisticTable::Result& BallisticTable::at(int iz, int iv, int im, int id, int il) const
{
  return data[index(iz, iv, im, id, il)];
}

bool BallisticTable::load(const std::string& path)
{
  std::ifstream f(path);
  if (!f.is_open()) {
    return false;
  }

  int nZ = 0;
  int nV = 0;
  int nM = 0;
  int nD = 0;
  int nL = 0;
  f >> nZ >> nV >> nM >> nD >> nL;

  axisZ0.resize(static_cast<size_t>(nZ));
  for (auto& v : axisZ0) {
    f >> v;
  }
  axisV0.resize(static_cast<size_t>(nV));
  for (auto& v : axisV0) {
    f >> v;
  }
  axisM.resize(static_cast<size_t>(nM));
  for (auto& v : axisM) {
    f >> v;
  }
  axisD.resize(static_cast<size_t>(nD));
  for (auto& v : axisD) {
    f >> v;
  }
  axisL.resize(static_cast<size_t>(nL));
  for (auto& v : axisL) {
    f >> v;
  }

  size_t total =
    static_cast<size_t>(nZ) * static_cast<size_t>(nV) * static_cast<size_t>(nM) * static_cast<size_t>(nD) * static_cast<size_t>(nL);
  data.resize(total);

  for (size_t i = 0; i < total; i++) {
    f >> data[i].t >> data[i].hDist;
  }

  return f.good();
}

BallisticTable::Result BallisticTable::lookup(float z0, float v0, float m, float d, float l) const
{
  Interp iz = findInterp(z0, axisZ0);
  Interp iv = findInterp(v0, axisV0);
  Interp im = findInterp(m, axisM);
  Interp id = findInterp(d, axisD);
  Interp il = findInterp(l, axisL);

  std::array<Result, 16> v{};
  for (int a = 0; a < 2; a++) {
    for (int b = 0; b < 2; b++) {
      for (int c = 0; c < 2; c++) {
        for (int e = 0; e < 2; e++) {
          const auto& lo = at(iz.lo + a, iv.lo + b, im.lo + c, id.lo + e, il.lo);
          const auto& hi = at(iz.lo + a, iv.lo + b, im.lo + c, id.lo + e, il.lo + 1);
          v[static_cast<size_t>((a * 8) + (b * 4) + (c * 2) + e)] = lerp(lo, hi, il.frac);
        }
      }
    }
  }

  std::array<Result, 8> w{};
  for (int a = 0; a < 2; a++) {
    for (int b = 0; b < 2; b++) {
      for (int c = 0; c < 2; c++) {
        auto wi = static_cast<size_t>((a * 4) + (b * 2) + c);
        auto vi0 = static_cast<size_t>((a * 8) + (b * 4) + (c * 2));
        w[wi] = lerp(v[vi0], v[vi0 + 1], id.frac);
      }
    }
  }

  std::array<Result, 4> u{};
  for (int a = 0; a < 2; a++) {
    for (int b = 0; b < 2; b++) {
      auto ui = static_cast<size_t>((a * 2) + b);
      auto wi0 = static_cast<size_t>((a * 4) + (b * 2));
      u[ui] = lerp(w[wi0], w[wi0 + 1], im.frac);
    }
  }

  std::array<Result, 2> s{};
  for (int a = 0; a < 2; a++) {
    auto si = static_cast<size_t>(a);
    auto ui0 = static_cast<size_t>(a * 2);
    s[si] = lerp(u[ui0], u[ui0 + 1], iv.frac);
  }

  return lerp(s[0], s[1], iz.frac);
}
// NOLINTEND(modernize-use-trailing-return-type, cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers,
// cppcoreguidelines-pro-bounds-constant-array-index)