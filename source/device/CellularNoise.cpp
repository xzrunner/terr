#include "wm/device/CellularNoise.h"
#include "wm/HeightField.h"
#include "wm/DeviceHelper.h"
#include "wm/EvalGPU.h"

#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <painting0/ShaderUniforms.h>

namespace
{

std::shared_ptr<wm::EvalGPU> EVAL = nullptr;

const char* vs = R"(

#version 330 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texcoord;

out VS_OUT {
    vec2 texcoord;
} vs_out;

void main()
{
	vs_out.texcoord = texcoord;
	gl_Position = position;
}

)";

const char* fs = R"(

// https://thebookofshaders.com/12/
// Author: @patriciogv
// Title: CellularNoise

#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 texcoord;
} fs_in;

uniform vec2 u_resolution;
uniform float u_seed;

vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

void main() {
    vec2 st = fs_in.texcoord/u_resolution.xy;
    st.x *= u_resolution.x/u_resolution.y;

    // Scale
    st *= 3.;

    // Tile the space
    vec2 i_st = floor(st);
    vec2 f_st = fract(st);

    float min_dist = 1.;  // minimun distance

    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            // Neighbor place in the grid
            vec2 neighbor = vec2(float(x),float(y));

            // Random position from current + neighbor place in the grid
            vec2 point = random2(i_st + neighbor);

			// Animate the point
            point = 0.5 + 0.5*sin(u_seed + 6.2831*point);

			// Vector between the pixel and the point
            vec2 diff = neighbor + point - f_st;

            // Distance to the point
            float dist = length(diff);

            // Keep the closer distance
            min_dist = min(min_dist, dist);
        }
    }

    FragColor = vec4(min_dist, min_dist, min_dist, 1.0);
}

)";

}

namespace wm
{
namespace device
{

void CellularNoise::Execute()
{
    m_hf = std::make_shared<HeightField>(m_width, m_height);

    std::vector<uint32_t> textures;

    pt0::ShaderUniforms vals;
    vals.AddVar("u_resolution", pt0::RenderVariant(m_resolution));
    vals.AddVar("u_seed",       pt0::RenderVariant(m_seed));

    auto& rc = ur::Blackboard::Instance()->GetRenderContext();
    EVAL->Run(rc, textures, vals, *m_hf);
}

void CellularNoise::Init()
{
    auto& rc = ur::Blackboard::Instance()->GetRenderContext();

    std::vector<std::string> texture_names;
    EVAL = std::make_shared<EvalGPU>(rc, vs, fs, texture_names);
}

}
}