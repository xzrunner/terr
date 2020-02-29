#include "terraingraph/device/FBM.h"
#include "terraingraph/HemanHelper.h"

#include <heman.h>

namespace terraingraph
{
namespace device
{

void FBM::Execute()
{
    heman_image* he_height = heman_generate_simplex_fbm(
        m_width, m_height, m_frequency, m_amplitude, m_octaves, m_lacunarity, m_gain, m_seed
    );
    m_hf = HemanHelper::Decode(he_height);
    heman_image_destroy(he_height);
}

}
}