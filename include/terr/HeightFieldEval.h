#pragma once

#include <SM_Vector.h>

namespace terr
{

class HeightField;

class HeightFieldEval
{
public:
    static sm::vec2 Gradient(const HeightField& hf,
        size_t x, size_t y);
    static sm::vec3 Normal(const HeightField& hf,
        size_t x, size_t y, const sm::vec3& scale);

    static void Region(const HeightField& hf, float& min, float& max);

}; // HeightFieldEval

}