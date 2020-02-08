#include "wm/device/OverlayView.h"
#include "wm/DeviceHelper.h"

namespace wm
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