#include "terraingraph/device/Gaussian.h"
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

uniform vec2 offsets;
uniform sampler2D heightmap;

const float a = 1.0/36.0;
const float b = 4.0/36.0;
const float c = 16.0/36.0;

float get(float s, float t){
    return texture2D(heightmap, vec2(s, t)).r;
}

void main(void){
    vec2 uv = fs_in.texcoord;
    float s=uv.s;
    float t=uv.t;
    float x=offsets.x;
    float y=offsets.y;
    float pos = get(uv.s, uv.t);
    float result =
        a*get(uv.s-x, uv.t-y) + b*get(uv.s  , uv.t-y) + a*get(uv.s+x, uv.t-y) +
        b*get(uv.s-x, uv.t  ) + c*pos                 + b*get(uv.s+x, uv.t  ) +
        a*get(uv.s-x, uv.t+y) + b*get(uv.s  , uv.t+y) + a*get(uv.s+x, uv.t+y);
    FragColor = vec4(result, result, result, 1.0);
}

)";

}

namespace terraingraph
{
namespace device
{

void Gaussian::Execute(const std::shared_ptr<dag::Context>& ctx)
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
    vals.AddVar("offsets",   pt0::RenderVariant(m_offsets));

    m_hf = std::make_shared<hf::HeightField>(prev_hf->Width(), prev_hf->Height());

    if (!EVAL)
    {
        //std::vector<std::string> texture_names;
        //texture_names.push_back("heightmap");

        EVAL = std::make_shared<EvalGPU>(dev, vs, fs);
    }
    EVAL->RunPS(dev, vals, *m_hf);
}

}
}