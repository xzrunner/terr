#pragma once

#include <string>
#include <vector>

namespace ur { class Shader; class RenderContext; }
namespace pt0 { class ShaderUniforms; }
namespace hf { class HeightField; }

namespace terraingraph
{

class EvalGPU
{
public:
    EvalGPU(ur::RenderContext& rc, const std::string& vs, const std::string& fs,
        const std::vector<std::string>& textures);
    EvalGPU(ur::RenderContext& rc, const std::string& cs);

    bool RunPS(ur::RenderContext& rc, const std::vector<uint32_t>& textures,
        const pt0::ShaderUniforms& vals, hf::HeightField& hf) const;
    bool RunCS(ur::RenderContext& rc, const pt0::ShaderUniforms& vals,
        int thread_group_count, hf::HeightField& hf) const;

    auto GetShader() const { return m_shader; }
    auto GetComputeWorkGroupSize() const { return m_compute_work_group_size; }

private:
    std::shared_ptr<ur::Shader> m_shader = nullptr;

    int m_compute_work_group_size = 0;

}; // EvalGPU

}