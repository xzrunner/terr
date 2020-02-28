#include "wm/device/TemplateBrush.h"
#include "wm/device/FileInput.h"
#include "wm/device/Resample.h"
#include "wm/DeviceHelper.h"
#include "wm/HeightField.h"
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
    float h = texture2D(heightmap, uv).a;
    FragColor = vec4(h, h, h, 1.0);
}

)";

}

namespace wm
{
namespace device
{

void TemplateBrush::Execute()
{
    if (!m_brush)
    {
        m_brush = FileInput::LoadHeightField(m_filepath);
        if (m_brush) {
            m_brush = Resample::ResampleHeightField(*m_brush, 256, 256);
        }
    }

    if (!m_brush) {
        return;
    }

    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    m_hf = std::make_shared<HeightField>(*prev_hf);

    auto& rc = ur::Blackboard::Instance()->GetRenderContext();

    std::vector<uint32_t> textures;
    textures.push_back(m_brush->GetHeightmap()->TexID());

    for (auto& p : m_positions)
    {
        pt0::ShaderUniforms vals;
        vals.AddVar("translate", pt0::RenderVariant(-m_translate - p / m_scale));
        vals.AddVar("rotate",    pt0::RenderVariant(m_rotate * SM_DEG_TO_RAD));
        vals.AddVar("scale",     pt0::RenderVariant(sm::vec2(1.0f, 1.0f) / m_scale));

        wm::HeightField hf(m_hf->Width(), m_hf->Height());
        EVAL->RunPS(rc, textures, vals, hf);
        auto dst = m_hf->GetValues();
        auto& src = hf.GetValues();
        for (size_t i = 0, n = dst.size(); i < n; ++i) {
            dst[i] = std::max(dst[i], src[i]);
        }
        m_hf->SetValues(dst);
    }
}

void TemplateBrush::Init()
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