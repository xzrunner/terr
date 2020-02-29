#include "terraingraph/device/Constant.h"
#include "terraingraph/HeightField.h"

namespace terraingraph
{
namespace device
{

void Constant::Execute()
{
    m_hf = std::make_shared<HeightField>(m_width, m_height);
    std::vector<float> vals(m_width * m_height, m_value);
    m_hf->SetValues(vals);
}

}
}