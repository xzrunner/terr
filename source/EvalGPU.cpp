#include "wm/EvalGPU.h"
#include "wm/HeightField.h"

#include <unirender/VertexAttrib.h>
#include <unirender/RenderContext.h>
//#include <renderpipeline/UniformNames.h>
#include <painting0/ShaderUniforms.h>
#include <painting3/Shader.h>

namespace wm
{

EvalGPU::EvalGPU(ur::RenderContext& rc, const std::string& vs, const std::string& fs,
                 const std::vector<std::string>& textures)
{
    std::vector<ur::VertexAttrib> layout;
    //layout.push_back(ur::VertexAttrib(rp::VERT_POSITION_NAME, 3, 4, 20, 0));
    //layout.push_back(ur::VertexAttrib(rp::VERT_TEXCOORD_NAME, 2, 4, 20, 12));
    //rc.CreateVertexLayout(layout);

    pt3::Shader::Params sp(textures, layout);
    sp.vs = vs.c_str();
    sp.fs = fs.c_str();

    //sp.uniform_names.Add(pt0::UniformTypes::ModelMat, rp::MODEL_MAT_NAME);
    //sp.uniform_names.Add(pt0::UniformTypes::ViewMat,  rp::VIEW_MAT_NAME);
    //sp.uniform_names.Add(pt0::UniformTypes::ProjMat,  rp::PROJ_MAT_NAME);

    m_shader = std::make_shared<pt3::Shader>(&rc, sp);
}

bool EvalGPU::Run(ur::RenderContext& rc, const std::vector<uint32_t>& textures,
                  const pt0::ShaderUniforms& vals, HeightField& hf) const
{
    if (!m_shader) {
        return false;
    }

    auto w = hf.Width();
    auto h = hf.Height();
    if (w == 0 || h == 0) {
        return false;
    }

    auto tex = rc.CreateTexture(nullptr, w, h, ur::TEXTURE_RGBA8);
    assert(tex != 0);

    auto fbo = rc.CreateRenderTarget(0);
    assert(fbo != 0);

    int vp_x, vp_y, vp_w, vp_h;
    rc.GetViewport(vp_x, vp_y, vp_w, vp_h);

    rc.BindRenderTarget(fbo);
    rc.BindRenderTargetTex(tex, ur::ATTACHMENT_COLOR0);
    rc.SetViewport(0, 0, w, h);
    assert(rc.CheckRenderTargetStatus());

    rc.SetClearFlag(ur::MASKC | ur::MASKD);
    rc.SetClearColor(0x0000000);
    rc.Clear();

    m_shader->SetUsedTextures(textures);

    m_shader->Use();

    vals.Bind(*m_shader);

    rc.RenderQuad(ur::RenderContext::VertLayout::VL_POS_TEX);

    uint8_t* pixels = new uint8_t[w * h * 3];
    rc.ReadPixels(pixels, 3, 0, 0, w, h);

    std::vector<float> heights(w * h);
    for (size_t i = 0, n = heights.size(); i < n; ++i) {
        heights[i] = pixels[i * 3 + 3] / 255.0f;
    }
    delete[] pixels;

    rc.UnbindRenderTarget();
    rc.SetViewport(vp_x, vp_y, vp_w, vp_h);
    rc.ReleaseRenderTarget(fbo);
    rc.ReleaseTexture(tex);

    hf.Fill(heights);

    return true;
}

}