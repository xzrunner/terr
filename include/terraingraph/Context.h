#pragma once

#include "dag/Context.h"

#include <memory>

namespace ur { class Context; class Device; }

namespace terraingraph
{

class Context : public dag::Context
{
public:
    //ur::RenderState ur_rs;
    std::shared_ptr<ur::Context> ur_ctx = nullptr;
    std::shared_ptr<ur::Device>  ur_dev = nullptr;

}; // Context

}