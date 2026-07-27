#pragma once

#include <string>
#include <vector>

struct BallisticTable {
    struct Result {
        float t = 0.0f;
        float hDist = 0.0f;
    };

    std::vector<float> axisZ0;
    std::vector<float> axisV0;
    std::vector<float> axisM;
    std::vector<float> axisD;
    std::vector<float> axisL;

    std::vector<Result> data;

    [[nodiscard]] size_t index(int iz, int iv, int im, int id, int il) const;
    [[nodiscard]] const Result& at(int iz, int iv, int im, int id, int il) const;

    bool load(const std::string& path);

    [[nodiscard]] Result lookup(float z0, float v0, float m, float d, float l) const;
};