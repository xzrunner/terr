#include "wm/device/Step.h"
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
uniform float low, high;

void main(void){
    vec2 pos = fs_in.texcoord;
    float height = texture2D(heightmap, pos).a;
    float result = smoothstep(low, high, height);
    FragColor = vec4(result, result, result, 1.0);
}

)";

}

namespace wm
{
namespace device
{

void Step::Execute()
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
    vals.AddVar("low",  pt0::RenderVariant(m_low));
    vals.AddVar("high", pt0::RenderVariant(m_high));

    m_hf = std::make_shared<HeightField>(prev_hf->Width(), prev_hf->Height());
    EVAL->RunPS(rc, textures, vals, *m_hf);
}

void Step::Init()
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