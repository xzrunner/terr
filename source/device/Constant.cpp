#include "terraingraph/device/Constant.h"

#include <heightfield/HeightField.h>

namespace terraingraph
{
namespace device
{

void Constant::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    m_hf = std::make_shared<hf::HeightField>(m_width, m_height);
    std::vector<int32_t> vals(m_width * m_height, m_value);
    m_hf->SetValues(vals);
}

}
}