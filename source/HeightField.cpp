#include "wm/HeightField.h"
#include "wm/TextureBaker.h"

#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>

#include <assert.h>

namespace wm
{

HeightField::HeightField(size_t width, size_t height)
    : ScalarField2D(width, height)
{
}

ur::TexturePtr HeightField::GetHeightmap()
{
    if (!m_heightmap || m_dirty) {
        auto& rc = ur::Blackboard::Instance()->GetRenderContext();
        m_heightmap = TextureBaker::GenHeightMap(*this, rc);
        m_dirty = false;
    }
    return m_heightmap;
}

}