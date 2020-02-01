#pragma once

#include "terr/DeviceVarType.h"

#include <dag/Node.h>

namespace terr
{

class Context;
class HeightField;
class Bitmap;

class Device : public dag::Node<DeviceVarType>
{
public:
    Device() {}

    virtual void Execute(const Context& ctx) = 0;

    auto GetHeightField() const { return m_hf; }
    auto GetBitmap() const { return m_bmp; }

protected:
    std::shared_ptr<HeightField> m_hf  = nullptr;
    std::shared_ptr<Bitmap>      m_bmp = nullptr;

    RTTR_ENABLE(dag::Node<DeviceVarType>)

}; // Device

}