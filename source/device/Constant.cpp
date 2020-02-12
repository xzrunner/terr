#include "wm/device/Constant.h"
#include "wm/HeightField.h"

namespace wm
{
namespace device
{

void Constant::Execute(const Context& ctx)
{
    m_hf = std::make_shared<HeightField>(m_width, m_height);
    std::vector<float> vals(m_width * m_height, m_value);
    m_hf->Fill(vals);
}

}
}