#pragma once

#include "wm/ScalarField2D.h"

#include <SM_Vector.h>
#include <unirender/Texture.h>

#include <vector>

namespace wm
{

class HeightField : public ScalarField2D
{
public:
    HeightField(size_t width, size_t height);

    ur::TexturePtr GetHeightmap();

private:
    ur::TexturePtr m_heightmap = nullptr;

}; // HeightField

}