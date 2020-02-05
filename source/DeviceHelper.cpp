#include "terr/DeviceHelper.h"
#include "terr/Device.h"

namespace terr
{

DevicePtr DeviceHelper::GetInputDevice(const Device& dev, size_t idx)
{
    auto& imports = dev.GetImports();
    if (idx < 0 || idx >= imports.size()) {
        return nullptr;
    }

    auto& conns = imports[idx].conns;
    if (conns.empty()) {
        return nullptr;
    }

    assert(imports[idx].conns.size() == 1);
    auto in_dev = imports[idx].conns[0].node.lock();
    assert(in_dev->get_type().is_derived_from<Device>());
    return std::static_pointer_cast<Device>(in_dev);
}

std::shared_ptr<HeightField>
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