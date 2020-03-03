#include "terraingraph/device/Planet.h"
#include "terraingraph/HemanHelper.h"

#include <heman.h>

namespace terraingraph
{
namespace device
{

void Planet::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    heman_image* he_height = heman_generate_planet_heightmap(
        m_width, m_height, m_seed
    );
    m_hf = HemanHelper::Decode(he_height);
    heman_image_destroy(he_height);
}

}
}