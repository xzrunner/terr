#include "terr/device/OverlayView.h"
#include "terr/DeviceHelper.h"

namespace terr
{
namespace device
{

void OverlayView::Execute(const Context& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, ID_HEIGHTFIELD);
    if (!prev_hf) {
        return;
    }

    auto prev_bmp = DeviceHelper::GetInputBitmap(*this, ID_BITMAP);
    if (!prev_hf) {
        return;
    }

    m_hf  = prev_hf;
    m_bmp = prev_bmp;
}

}
}