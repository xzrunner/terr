#pragma once

#include <string>
#include <vector>

namespace ur { class Shader; class RenderContext; }
namespace pt0 { class ShaderUniforms; }

namespace wm
{

class HeightField;

class EvalGPU
{
public:
    EvalGPU(ur::RenderContext& rc, const std::string& vs, const std::string& fs,
        const std::vector<std::string>& textures);

    bool Run(ur::RenderContext& rc, const std::vector<uint32_t>& textures,
        const pt0::ShaderUniforms& vals, HeightField& hf) const;

private:
    std::shared_ptr<ur::Shader> m_shader = nullptr;

}; // EvalGPU

}