#pragma once

#include <heightfield/ScalarField2D.h>

#include <SM_Vector.h>

namespace hf { class HeightField; }
namespace ur2 { class Device; }

namespace terraingraph
{

class HeightFieldEval
{
public:
    static sm::vec2 Gradient(const ur2::Device& dev,
        const hf::HeightField& hf, size_t x, size_t y);
    static sm::vec3 Normal(const ur2::Device& dev,
        const hf::HeightField& hf, size_t x, size_t y, const sm::vec3& scale);

    // from Outerrain
    static hf::ScalarField2D<float>
        DrainageArea(const ur2::Device& dev, const hf::HeightField& hf);
    static hf::ScalarField2D<float>
        Wetness(const ur2::Device& dev, const hf::HeightField& hf);
    static hf::ScalarField2D<float>
        StreamPower(const ur2::Device& dev, const hf::HeightField& hf);
    static hf::ScalarField2D<float>
        Slope(const ur2::Device& dev, const hf::HeightField& hf);

}; // HeightFieldEval

}