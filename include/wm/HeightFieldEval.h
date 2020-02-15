#pragma once

#include "wm/ScalarField2D.h"

#include <SM_Vector.h>

namespace wm
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

    // from Outerrain
    static ScalarField2D DrainageArea(const HeightField& hf);
    static ScalarField2D Wetness(const HeightField& hf);
    static ScalarField2D StreamPower(const HeightField& hf);
    static ScalarField2D Slope(const HeightField& hf);

}; // HeightFieldEval

}