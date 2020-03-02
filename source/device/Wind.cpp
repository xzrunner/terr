#include "terraingraph/device/Wind.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/EvalGPU.h"

#include <heightfield/HeightField.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <painting0/ShaderUniforms.h>

namespace
{

std::shared_ptr<terraingraph::EvalGPU> EVAL = nullptr;

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

#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 texcoord;
} fs_in;

uniform sampler2D heightmap;
uniform vec2 offsets;
const float pi = 3.14159265358979323846264;
uniform float direction, strengh, strengh2;

vec3 get(vec2 uv){
    float height = texture2D(heightmap, vec2(uv)).r;
    return vec3(uv.s, height, uv.t);
}

vec3 get_normal(vec3 p){
    float x = offsets.x;
    float z = offsets.y;
    return normalize(vec3(
        get(vec2(p.x-x, p.z)).y - get(vec2(p.x+x, p.z)).y,
        x+z,
        get(vec2(p.x, p.z-z)).y - get(vec2(p.x, p.z+z)).y
    ));
}

float sample(vec2 uv, float s, float t){
    return get(vec2(uv.s+s*offsets.s, uv.t+t*offsets.s)).y;
}

void main(){
    float dir = direction * pi * 2.0;
    vec2 uv = fs_in.texcoord;
    vec3 pos = get(uv);
    float low = min(sample(uv, cos(dir), cos(dir+pi/2.0)), pos.y);
    float high = sample(uv, cos(dir-pi), cos(dir-pi/2.0));
    vec3 normal = get_normal(pos);
    float factor = dot(normal, vec3(0, 1, 0));
    float height = (mix(pos.y, low, factor*strengh2) + mix(pos.y, high, acos(factor)*strengh))/2.0;
    FragColor = vec4(height, height, height, 1.0);
}

)";

}

namespace terraingraph
{
namespace device
{

void Wind::Execute()
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    auto& rc = ur::Blackboard::Instance()->GetRenderContext();

    std::vector<uint32_t> textures;
    auto heightmap = prev_hf->GetHeightmap();
    textures.push_back(heightmap->TexID());

    pt0::ShaderUniforms vals;
    vals.AddVar("offsets",   pt0::RenderVariant(m_offsets));
    vals.AddVar("direction", pt0::RenderVariant(m_direction));
    vals.AddVar("strengh",   pt0::RenderVariant(m_strengh));
    vals.AddVar("strengh2",  pt0::RenderVariant(m_strengh2));

    m_hf = std::make_shared<hf::HeightField>(prev_hf->Width(), prev_hf->Height());
    EVAL->RunPS(rc, textures, vals, *m_hf);
}

void Wind::Init()
{
    if (!EVAL)
    {
        auto& rc = ur::Blackboard::Instance()->GetRenderContext();

        std::vector<std::string> texture_names;
        texture_names.push_back("heightmap");

        EVAL = std::make_shared<EvalGPU>(rc, vs, fs, texture_names);
    }
}

}
}