#include "terraingraph/device/Transform.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/HeightField.h"
#include "terraingraph/EvalGPU.h"

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

uniform vec2  translate;
uniform float rotate;
uniform vec2  scale;

uniform sampler2D heightmap;

void main(void)
{
    vec2 uv = fs_in.texcoord;
    // scale
    uv = (uv - vec2(0.5, 0.5)) * scale + vec2(0.5, 0.5);
    // rotate
    float sin_factor = sin(rotate);
    float cos_factor = cos(rotate);
    uv = vec2(uv.x - 0.5, uv.y - 0.5) * mat2(cos_factor, sin_factor, -sin_factor, cos_factor) + vec2(0.5, 0.5);
    // translate
    uv = uv + translate;

    uv = clamp(uv, vec2(0, 0), vec2(1, 1));
    float h = texture2D(heightmap, uv).r;
    FragColor = vec4(h, h, h, 1.0);
}

)";

}

namespace terraingraph
{
namespace device
{

void Transform::Execute()
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
    vals.AddVar("translate", pt0::RenderVariant(-m_translate));
    vals.AddVar("rotate",    pt0::RenderVariant(m_rotate * SM_DEG_TO_RAD));
    vals.AddVar("scale",     pt0::RenderVariant(sm::vec2(1.0f, 1.0f) / m_scale));

    m_hf = std::make_shared<HeightField>(prev_hf->Width(), prev_hf->Height());
    EVAL->RunPS(rc, textures, vals, *m_hf);
}

void Transform::Init()
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