#include "terraingraph/device/Step.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/EvalGPU.h"
#include "terraingraph/Context.h"

#include <heightfield/HeightField.h>
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

uniform sampler2D heightmap;
uniform float low, high;

void main(void){
    vec2 pos = fs_in.texcoord;
    float height = texture2D(heightmap, pos).r;
    float result = smoothstep(low, high, height);
    FragColor = vec4(result, result, result, 1.0);
}

)";

}

namespace terraingraph
{
namespace device
{

void Step::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    auto& dev = *std::static_pointer_cast<Context>(ctx)->ur_dev;

    //std::vector<uint32_t> textures;
    auto heightmap = prev_hf->GetHeightmap(dev);
    //textures.push_back(heightmap->TexID());

    pt0::ShaderUniforms vals;
    vals.AddVar("low",  pt0::RenderVariant(m_low));
    vals.AddVar("high", pt0::RenderVariant(m_high));

    m_hf = std::make_shared<hf::HeightField>(prev_hf->Width(), prev_hf->Height());

    if (!EVAL) {
        EVAL = std::make_shared<EvalGPU>(dev, vs, fs);
    }
    EVAL->RunPS(dev, vals, *m_hf);
}

}
}