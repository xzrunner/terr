#include "wm/device/ErosionGPU.h"
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

#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 texcoord;
} fs_in;

uniform sampler2D heightmap;
uniform vec2 offsets;
const float pih = 1.0/(3.14159265358979323846264*0.5);
uniform bool invert, shallow, rough, slope;

vec3 get(vec2 uv){
    float height = texture2D(heightmap, vec2(uv)).a;
    return vec3(uv.s, height, uv.t);
}

void main(){
    vec2 uv = fs_in.texcoord;
    float s = offsets.x;
    float t = offsets.y;

    vec3 pos = get(uv);
    vec3 left = get(uv+vec2(-s, 0.0));
    vec3 right = get(uv+vec2(s, 0.0));
    vec3 top = get(uv+vec2(0.0, t));
    vec3 bottom = get(uv+vec2(0.0, -t));
    vec3 left_top = get(uv+vec2(-s, t));
    vec3 right_top = get(uv+vec2(s, t));
    vec3 left_bottom = get(uv+vec2(-s, -t));
    vec3 right_bottom = get(uv+vec2(s, -t));

    vec4 a = vec4(left.y, right.y, top.y, bottom.y);
    vec4 b = vec4(left_top.y, right_top.y, left_bottom.y, right_bottom.y);

    vec4 comparision;
    float count = 1.0;
    float sum = pos.y;
    float result;

    if(invert){
        comparision = vec4(greaterThan(a, vec4(pos.y)));
        count += dot(comparision, comparision);
        sum += dot(comparision, a);

        if(!rough){
            comparision = vec4(greaterThan(b, vec4(pos.y)));
            count += dot(comparision, comparision);
            sum += dot(comparision, b);
        }
    }
    else{
        comparision = vec4(lessThan(a, vec4(pos.y)));
        count += dot(comparision, comparision);
        sum += dot(comparision, a);

        if(!rough){
            comparision = vec4(lessThan(b, vec4(pos.y)));
            count += dot(comparision, comparision);
            sum += dot(comparision, b);
        }
    }

    if(slope){
        vec3 normal = normalize(vec3(
            left.y - right.y,
            s+t,
            bottom.y - top.y
        ));
        float factor = dot(normal, vec3(0.0, 1.0, 0.0));
        if(shallow){
            factor = 1.0-factor;
        }
        else{
            factor = factor-0.05*count;
        }
        result = mix(sum/count, pos.y, factor);
    }
    else{
        result = sum/count;
    }

    FragColor = vec4(result, result, result, 1.0);
}

)";

}

namespace wm
{
namespace device
{

void ErosionGPU::Execute()
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
    vals.AddVar("offsets", pt0::RenderVariant(m_offsets));
    vals.AddVar("invert",  pt0::RenderVariant(m_invert));
    vals.AddVar("shallow", pt0::RenderVariant(m_shallow));
    vals.AddVar("rough",   pt0::RenderVariant(m_rough));
    vals.AddVar("slope",   pt0::RenderVariant(m_slope));

    m_hf = std::make_shared<HeightField>(prev_hf->Width(), prev_hf->Height());
    EVAL->RunPS(rc, textures, vals, *m_hf);
}

void ErosionGPU::Init()
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