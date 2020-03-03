#include "terraingraph/DeviceHelper.h"
#include "terraingraph/Device.h"

#include <dag/Utility.h>

namespace terraingraph
{

DevicePtr DeviceHelper::GetInputDevice(const Device& dev, size_t idx)
{
    auto prev = dag::Utility::GetInputNode(dev, idx);
    return prev ? std::static_pointer_cast<Device>(prev) : nullptr;
}

std::shared_ptr<hf::HeightField>
DeviceHelper::GetInputHeight(const Device& dev, size_t idx)
{
    auto prev_dev = GetInputDevice(dev, idx);
    if (prev_dev) {
        return prev_dev->GetHeightField();
    } else {
        return nullptr;
    }
}

std::shared_ptr<Bitmap>
DeviceHelper::GetInputBitmap(const Device& dev, size_t idx)
{
    auto prev_dev = GetInputDevice(dev, idx);
    if (prev_dev) {
        return prev_dev->GetBitmap();
    } else {
        return nullptr;
    }
}

std::shared_ptr<Mask>
DeviceHelper::GetInputMask(const Device& dev, size_t idx)
{
    auto prev_dev = GetInputDevice(dev, idx);
    if (prev_dev) {
        return prev_dev->GetMask();
    } else {
        return nullptr;
    }
}

}