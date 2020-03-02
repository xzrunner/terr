#pragma once

#include <heightfield/ScalarField2D.h>

#include <SM_Vector.h>

namespace hf { class HeightField; }

namespace terraingraph
{

class HeightFieldEval
{
public:
    static sm::vec2 Gradient(const hf::HeightField& hf,
        size_t x, size_t y);
    static sm::vec3 Normal(const hf::HeightField& hf,
        size_t x, size_t y, const sm::vec3& scale);

    static void Region(const hf::HeightField& hf, float& min, float& max);

    // from Outerrain
    static hf::ScalarField2D DrainageArea(const hf::HeightField& hf);
    static hf::ScalarField2D Wetness(const hf::HeightField& hf);
    static hf::ScalarField2D StreamPower(const hf::HeightField& hf);
    static hf::ScalarField2D Slope(const hf::HeightField& hf);

}; // HeightFieldEval

}