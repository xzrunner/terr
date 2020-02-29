#include "wm/device/NoiseBrush.h"
#include "wm/HeightField.h"
#include "wm/DeviceHelper.h"
#include "wm/EvalGPU.h"

#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <painting0/ShaderUniforms.h>

#define PATH_MAX_NUM 128

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

out vec4 FragColor;

in VS_OUT {
    vec2 texcoord;
} fs_in;

uniform sampler2D heightmap;

uniform vec3[PATH_MAX_NUM] path;

const float HEIGHT = 0.1;

void main(void)
{
    vec2 pos = fs_in.texcoord;
    float dh = 0;
    for (int i = 0; i < PATH_MAX_NUM; ++i)
    {
        float dist = length(path[i].xy - pos);
        if (dist < path[i].z) {
            dh += HEIGHT - pow(dist / path[i].z, 3);
        }
    }

    float h = texture2D(heightmap, pos).r + dh;
    FragColor = vec4(h);
}

)";

}

namespace wm
{
namespace device
{

void NoiseBrush::Execute()
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
    vals.AddVar("path", pt0::RenderVariant(m_path.data(), std::min(static_cast<int>(m_path.size()), PATH_MAX_NUM)));
    //if (!m_path.empty()) {
    //    vals.AddVar("path", pt0::RenderVariant(&m_path.back(), 1));
    //}

    if (m_path.size() < PATH_MAX_NUM) {
        vals.AddVar("path", pt0::RenderVariant(m_path.data(), m_path.size()));
    } else {
        vals.AddVar("path", pt0::RenderVariant(&m_path[m_path.size() - PATH_MAX_NUM], PATH_MAX_NUM));
    }

    if (!m_hf ||
        m_hf->Width() != prev_hf->Width() ||
        m_hf->Height() != prev_hf->Height()) {
        m_hf = std::make_shared<HeightField>(prev_hf->Width(), prev_hf->Height());
    }
    EVAL->RunPS(rc, textures, vals, *m_hf);
}

void NoiseBrush::Init()
{
    if (!EVAL)
    {
        auto& rc = ur::Blackboard::Instance()->GetRenderContext();

        std::vector<std::string> texture_names;
        texture_names.push_back("heightmap");

        std::string _fs = "#version 330 core\n#define PATH_MAX_NUM " + std::to_string(PATH_MAX_NUM) + "\n" + fs;
        EVAL = std::make_shared<EvalGPU>(rc, vs, _fs, texture_names);
    }
}

}
}