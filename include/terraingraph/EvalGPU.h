#pragma once

#include "terraingraph/typedef.h"

#include <unirender/typedef.h>

#include <string>
#include <vector>

namespace ur { class Device; class ShaderProgram; }
namespace pt0 { class ShaderUniforms; }
namespace hf { class HeightField; }

namespace terraingraph
{

class EvalGPU
{
public:
    EvalGPU(const ur::Device& dev, const std::string& vs, const std::string& fs);
    EvalGPU(const ur::Device& dev, const std::string& cs);

    bool RunPS(const ur::Device& dev, const pt0::ShaderUniforms& vals, hf::HeightField& hf) const;
    bool RunPS(const ur::Device& dev, const pt0::ShaderUniforms& vals, Bitmap& bmp) const;
    bool RunCS(const ur::Device& dev, const pt0::ShaderUniforms& vals,
        int thread_group_count, hf::HeightField& hf) const;

    auto GetShader() const { return m_shader; }
    auto GetComputeWorkGroupSize() const { return m_compute_work_group_size; }

private:
    void RunPS(const ur::Device& dev, const pt0::ShaderUniforms& vals, size_t dst_w, size_t dst_h,
        const ur::TexturePtr& tex, unsigned char* pixels = nullptr) const;

private:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

    int m_compute_work_group_size = 0;

}; // EvalGPU

}