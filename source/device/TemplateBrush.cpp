#include "terraingraph/device/TemplateBrush.h"
#include "terraingraph/device/Resample.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/EvalGPU.h"

#include <heightfield/HeightField.h>
#include <heightfield/Loader.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <unirender/Shader.h>
#include <painting0/ShaderUniforms.h>

//#define USE_PINGPONG_TEXTURE

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
layout (location = 0) out vec4 FragColor;

in VS_OUT {
    vec2 texcoord;
} fs_in;

uniform mat4 trans_inv;

uniform sampler2D brush_map;
uniform sampler2D scene_map;

uniform float height_scale;

void main(void)
{
    vec2 src_uv = fs_in.texcoord;
    src_uv = (trans_inv * vec4(src_uv.x, 0.0f, src_uv.y, 1.0)).xz;
    src_uv = clamp(src_uv, vec2(0, 0), vec2(1, 1));

    float h = max(texture2D(scene_map, fs_in.texcoord).r, texture2D(brush_map, src_uv).r * height_scale);
//    float h = texture2D(brush_map, src_uv).r * height_scale;
    FragColor = vec4(h);
}

)";

}

namespace terraingraph
{
namespace device
{

void TemplateBrush::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    if (!m_brush)
    {
        m_brush = hf::Loader::Load(m_filepath);
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

    m_hf = std::make_shared<hf::HeightField>(*prev_hf);

    auto& rc = ur::Blackboard::Instance()->GetRenderContext();

    std::vector<uint32_t> textures;
    textures.push_back(m_brush->GetHeightmap()->TexID());
    textures.push_back(m_hf->GetHeightmap()->TexID());

    auto w = m_hf->Width();
    auto h = m_hf->Height();

#ifdef USE_PINGPONG_TEXTURE
    auto tex = rc.CreateTexture(nullptr, w, h, ur::TEXTURE_RED);
    assert(tex != 0);

    auto read_tex_id = m_hf->GetHeightmap()->TexID();
    auto write_tex_id = tex;
#endif // USE_PINGPONG_TEXTURE

    auto fbo = rc.CreateRenderTarget(0);
    assert(fbo != 0);

    int vp_x, vp_y, vp_w, vp_h;
    rc.GetViewport(vp_x, vp_y, vp_w, vp_h);

    rc.BindRenderTarget(fbo);
#ifdef USE_PINGPONG_TEXTURE
    rc.BindRenderTargetTex(write_tex_id, ur::ATTACHMENT_COLOR0);
#else
    rc.BindRenderTargetTex(m_hf->GetHeightmap()->TexID(), ur::ATTACHMENT_COLOR0);
#endif // USE_PINGPONG_TEXTURE
    rc.SetViewport(0, 0, w, h);
    assert(rc.CheckRenderTargetStatus());

    rc.SetClearFlag(ur::MASKC | ur::MASKD);
    rc.SetClearColor(0x0000000);
    rc.Clear();

    auto shader = EVAL->GetShader();
    shader->SetUsedTextures(textures);

    shader->Use();

    for (auto& mt : m_matrixes)
    {
#ifdef USE_PINGPONG_TEXTURE
        rc.BindRenderTargetTex(write_tex_id, ur::ATTACHMENT_COLOR0);
        rc.BindTexture(read_tex_id, 1);
#endif // USE_PINGPONG_TEXTURE

        pt0::ShaderUniforms vals;
        vals.AddVar("trans_inv", pt0::RenderVariant(mt.Inverted()));

        auto v = mt * sm::vec4(0, 1, 0, 1);
        vals.AddVar("height_scale", pt0::RenderVariant(fabs(v.y)));

        vals.Bind(*shader);

        rc.RenderQuad(ur::RenderContext::VertLayout::VL_POS_TEX);

#ifdef USE_PINGPONG_TEXTURE
        auto tmp = read_tex_id;
        read_tex_id = write_tex_id;
        write_tex_id = tmp;
#endif // USE_PINGPONG_TEXTURE
    }

#ifdef USE_PINGPONG_TEXTURE
    if (write_tex_id == m_hf->GetHeightmap()->TexID()) {
        rc.CopyTexture(0, 0, w, h, ur::TEXTURE_RED, m_hf->GetHeightmap()->TexID());
    }
#endif // USE_PINGPONG_TEXTURE

    rc.UnbindRenderTarget();
    rc.SetViewport(vp_x, vp_y, vp_w, vp_h);
    rc.ReleaseRenderTarget(fbo);
#ifdef USE_PINGPONG_TEXTURE
    rc.ReleaseTexture(tex);
#endif // USE_PINGPONG_TEXTURE

    rc.BindShader(0);

    m_hf->SetCPUDirty();
}

void TemplateBrush::Init()
{
    if (!EVAL)
    {
        auto& rc = ur::Blackboard::Instance()->GetRenderContext();

        std::vector<std::string> texture_names = {
            "brush_map",
            "scene_map"
        };

        EVAL = std::make_shared<EvalGPU>(rc, vs, fs, texture_names);
    }
}

}
}