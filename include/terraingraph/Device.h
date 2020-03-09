#pragma once

#include "terraingraph/DeviceVarType.h"
#include "terraingraph/typedef.h"

#include <dag/Node.h>

namespace hf { class HeightField; }

namespace terraingraph
{

class Mask;

class Device : public dag::Node<DeviceVarType>
{
public:
    Device() {}

    auto GetHeightField() const { return m_hf; }
    auto GetBitmap() const { return m_bmp; }
    auto GetMask() const { return m_mask; }

protected:
    std::shared_ptr<hf::HeightField> m_hf   = nullptr;
    std::shared_ptr<Bitmap>          m_bmp  = nullptr;
    std::shared_ptr<Mask>            m_mask = nullptr;

    RTTR_ENABLE(dag::Node<DeviceVarType>)

}; // Device

}