#include "terraingraph/EvalGPU.h"

#include <heightfield/HeightField.h>
#include <unirender/Device.h>
#include <unirender/ShaderProgram.h>
#include <unirender/TextureDescription.h>
#include <unirender/ComputeBuffer.h>
#include <shadertrans/ShaderTrans.h>
//#include <renderpipeline/UniformNames.h>
#include <painting0/ShaderUniforms.h>
#include <painting3/Shader.h>

namespace terraingraph
{

EvalGPU::EvalGPU(const ur::Device& dev, const std::string& vs, const std::string& fs)
{
    std::vector<unsigned int> _vs, _fs;
    shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::VertexShader, vs, _vs);
    shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::PixelShader, fs, _fs);
    m_shader = dev.CreateShaderProgram(_vs, _fs);
    assert(m_shader);

    //std::vector<ur::VertexAttrib> layout;
    //layout.push_back(ur::VertexAttrib(rp::VERT_POSITION_NAME, 3, 4, 20, 0));
    //layout.push_back(ur::VertexAttrib(rp::VERT_TEXCOORD_NAME, 2, 4, 20, 12));
    //rc.CreateVertexLayout(layout);

    //sp.uniform_names.Add(pt0::UniformTypes::ModelMat, rp::MODEL_MAT_NAME);
    //sp.uniform_names.Add(pt0::UniformTypes::ViewMat,  rp::VIEW_MAT_NAME);
    //sp.uniform_names.Add(pt0::UniformTypes::ProjMat,  rp::PROJ_MAT_NAME);

    //m_shader = std::make_shared<pt3::Shader>(&rc, sp);
}

EvalGPU::EvalGPU(const ur::Device& dev, const std::string& cs)
{
    std::vector<unsigned int> _cs;
    shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::ComputeShader, cs, _cs);
    m_shader = dev.CreateShaderProgram(_cs);
    assert(m_shader);
    m_compute_work_group_size = m_shader->GetComputeWorkGroupSize();
}

bool EvalGPU::RunPS(const ur::Device& dev, const pt0::ShaderUniforms& vals, hf::HeightField& hf) const
{
    if (!m_shader) {
        return false;
    }

    auto w = hf.Width();
    auto h = hf.Height();
    auto tex = hf.GetHeightmap(dev);
    if (w == 0 || h == 0 || !tex) {
        return false;
    }

    RunPS(dev, vals, w, h, tex);
    hf.SetCPUDirty();

    return true;
}

bool EvalGPU::RunPS(const ur::Device& dev, const pt0::ShaderUniforms& vals, Bitmap& bmp) const
{
    if (!m_shader) {
        return false;
    }

    auto w = bmp.Width();
    auto h = bmp.Height();
    if (w == 0 || h == 0) {
        return false;
    }

    ur::TextureDescription desc;
    desc.width  = w;
    desc.height = h;
    desc.format = ur::TextureFormat::RGBA8;
    desc.target = ur::TextureTarget::Texture2D;
    auto tex = dev.CreateTexture(desc, nullptr);
    if (!tex) {
        return false;
    }

    auto pixels = bmp.GetPixels();
    RunPS(dev, vals, w, h, tex, pixels);

    return true;
}

bool EvalGPU::RunCS(const ur::Device& dev, const pt0::ShaderUniforms& vals,
                    int thread_group_count, hf::HeightField& hf) const
{
    auto values = hf.GetValues(dev);

    m_shader->Bind();

    // Allocate buffers
    auto buf = dev.CreateComputeBuffer(values, 0);
    //GLuint data_buf;
    //glGenBuffers(1, &data_buf);
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, data_buf);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * values.size(), &values.front(), GL_STREAM_COPY);

    // Uniforms
    vals.Bind(*m_shader);

    // Dispatch
    ctx.Compute(ds, thread_group_count, 1, 1);
    //glDispatchCompute(thread_group_count, 1, 1);
    //glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Update CPU data
    buf->GetComputeBufferData(values);
    //glGetNamedBufferSubData(data_buf, 0, sizeof(float) * values.size(), values.data());

    // Delete buffers
    //rc.ReleaseComputeBuffer(data_buf);
    //glDeleteBuffers(1, &data_buf);

    hf.SetValues(values);

    //rc.BindShader(0);

    return true;
}

//void EvalGPU::RunPS(const std::vector<uint32_t>& textures, const pt0::ShaderUniforms& vals,
//                    size_t dst_w, size_t dst_h, const ur::TexturePtr& tex, unsigned char* out_pixels) const
//{
//    auto fbo = rc.CreateRenderTarget(0);
//    assert(fbo != 0);
//
//    int vp_x, vp_y, vp_w, vp_h;
//    rc.GetViewport(vp_x, vp_y, vp_w, vp_h);
//
//    rc.BindRenderTarget(fbo);
//    rc.BindRenderTargetTex(dst_tex, ur::ATTACHMENT_COLOR0);
//    rc.SetViewport(0, 0, dst_w, dst_h);
//    assert(rc.CheckRenderTargetStatus());
//
//    rc.SetClearFlag(ur::MASKC | ur::MASKD);
//    rc.SetClearColor(0x0000000);
//    rc.Clear();
//
//    rc.SetZTest(ur::DEPTH_DISABLE);
//    rc.SetCullMode(ur::CULL_DISABLE);
//
//    m_shader->SetUsedTextures(textures);
//
//    m_shader->Use();
//
//    vals.Bind(*m_shader);
//
//    rc.RenderQuad(ur::RenderContext::VertLayout::VL_POS_TEX);
//
//    if (out_pixels) {
//        rc.ReadPixels(out_pixels, 3, 0, 0, dst_w, dst_h);
//    }
//
//    rc.UnbindRenderTarget();
//    rc.SetViewport(vp_x, vp_y, vp_w, vp_h);
//    rc.ReleaseRenderTarget(fbo);
//
//    rc.BindShader(0);
//
//    rc.SetZTest(ur::DEPTH_LESS_EQUAL);
//    rc.SetCullMode(ur::CULL_BACK);
//}

void EvalGPU::RunPS(const ur::Device& dev, const pt0::ShaderUniforms& vals, size_t dst_w, size_t dst_h,
                    const ur::TexturePtr& tex, unsigned char* out_pixels) const
{
    //auto fbo = rc.CreateRenderTarget(0);
    //assert(fbo != 0);

    //int vp_x, vp_y, vp_w, vp_h;
    //rc.GetViewport(vp_x, vp_y, vp_w, vp_h);

    //rc.BindRenderTarget(fbo);
    //rc.BindRenderTargetTex(dst_tex, ur::ATTACHMENT_COLOR0);
    //rc.SetViewport(0, 0, dst_w, dst_h);
    //assert(rc.CheckRenderTargetStatus());

    //rc.SetClearFlag(ur::MASKC | ur::MASKD);
    //rc.SetClearColor(0x0000000);
    //rc.Clear();

    //rc.SetZTest(ur::DEPTH_DISABLE);
    //rc.SetCullMode(ur::CULL_DISABLE);

    //m_shader->SetUsedTextures(textures);

    //m_shader->Use();

    //vals.Bind(*m_shader);

    //rc.RenderQuad(ur::RenderContext::VertLayout::VL_POS_TEX);

    //if (out_pixels) {
    //    rc.ReadPixels(out_pixels, 3, 0, 0, dst_w, dst_h);
    //}

    //rc.UnbindRenderTarget();
    //rc.SetViewport(vp_x, vp_y, vp_w, vp_h);
    //rc.ReleaseRenderTarget(fbo);

    //rc.BindShader(0);

    //rc.SetZTest(ur::DEPTH_LESS_EQUAL);
    //rc.SetCullMode(ur::CULL_BACK);
}

}