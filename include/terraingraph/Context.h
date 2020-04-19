#pragma once

#include "dag/Context.h"

#include <memory>

namespace ur2 { class Context; class Device; }

namespace terraingraph
{

class Context : public dag::Context
{
public:
    //ur2::RenderState ur_rs;
    std::shared_ptr<ur2::Context> ur_ctx = nullptr;
    std::shared_ptr<ur2::Device>  ur_dev = nullptr;

}; // Context

}