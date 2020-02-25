#include "wm/device/Rock.h"
#include "wm/HemanHelper.h"

#include <heman.h>

namespace wm
{
namespace device
{

void Rock::Execute()
{
    heman_image* he_height = heman_generate_rock_heightmap(
        m_width, m_height, m_seed
    );
    m_hf = HemanHelper::Decode(he_height);
    heman_image_destroy(he_height);
}

}
}