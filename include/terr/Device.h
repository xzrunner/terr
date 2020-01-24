#pragma once

#include "terr/DeviceVarType.h"

#include <dag/Node.h>

namespace terr
{

class Context;
class HeightField;

class Device : public dag::Node<DeviceVarType>
{
public:
    Device() {}

    virtual void Execute(const Context& ctx) = 0;

    auto GetHeightField() const { return m_hf; }

protected:
    std::shared_ptr<HeightField> m_hf = nullptr;

    RTTR_ENABLE(dag::Node<DeviceVarType>)

}; // Device

}