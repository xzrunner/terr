#include "terr/Evaluator.h"
#include "terr/Device.h"
#include "terr/Context.h"
#include "terr/typedef.h"

#include <dag/Evaluator.h>

namespace terr
{

void Evaluator::DeviceChaged(const DevicePtr& device)
{
    dag::Evaluator::SetTreeDirty<DeviceVarType>(device);

    m_dirty = true;
}

void Evaluator::AddDevice(const DevicePtr& device)
{
    auto name = device->GetName();
    while (name.empty() || m_devices_map.find(name) != m_devices_map.end())
    {
        if (device->GetName().empty()) {
            name = "device" + std::to_string(m_next_id++);
        } else {
            name = device->GetName() + std::to_string(m_next_id++);
        }
    }
    device->SetName(name);

    m_devices_map.insert({ name, device });

    m_dirty = true;
}

void Evaluator::RemoveDevice(const DevicePtr& device)
{
    if (m_devices_map.empty()) {
        return;
    }

    auto itr = m_devices_map.find(device->GetName());
    if (itr == m_devices_map.end()) {
        return;
    }

    dag::Evaluator::SetTreeDirty<DeviceVarType>(device);

    m_devices_map.erase(itr);

    m_dirty = true;
}

void Evaluator::ClearAllDevices()
{
    if (m_devices_map.empty()) {
        return;
    }

    m_devices_map.clear();

    m_dirty = true;
}

void Evaluator::PropChanged(const DevicePtr& device)
{
    dag::Evaluator::SetTreeDirty<DeviceVarType>(device);

    m_dirty = true;
}

void Evaluator::Connect(const dag::Node<DeviceVarType>::PortAddr& from, const dag::Node<DeviceVarType>::PortAddr& to)
{
    dag::make_connecting<DeviceVarType>(from, to);

    auto device = to.node.lock();
    assert(device && device->get_type().is_derived_from<Device>());
    dag::Evaluator::SetTreeDirty<DeviceVarType>(std::static_pointer_cast<Device>(device));

    m_dirty = true;
}

void Evaluator::Disconnect(const dag::Node<DeviceVarType>::PortAddr& from, const dag::Node<DeviceVarType>::PortAddr& to)
{
    dag::disconnect<DeviceVarType>(from, to);

    auto device = to.node.lock();
    assert(device && device->get_type().is_derived_from<Device>());
    dag::Evaluator::SetTreeDirty<DeviceVarType>(std::static_pointer_cast<Device>(device));

    m_dirty = true;
}

void Evaluator::RebuildConnections(const std::vector<std::pair<dag::Node<DeviceVarType>::PortAddr, dag::Node<DeviceVarType>::PortAddr>>& conns)
{
    // update dirty
    for (auto itr : m_devices_map) {
        if (dag::Evaluator::HasNodeConns<DeviceVarType>(itr.second)) {
            dag::Evaluator::SetTreeDirty<DeviceVarType>(itr.second);
        }
    }

    // remove conns
    for (auto itr : m_devices_map) {
        itr.second->ClearConnections();
    }

    // add conns
    for (auto& conn : conns)
    {
        auto device = conn.second.node.lock();
        assert(device && device->get_type().is_derived_from<Device>());
        dag::Evaluator::SetTreeDirty<DeviceVarType>(std::static_pointer_cast<Device>(device));
        dag::make_connecting<DeviceVarType>(conn.first, conn.second);
    }

    m_dirty = true;
}

void Evaluator::Update()
{
    if (m_devices_map.empty()) {
        return;
    }

    std::vector<std::shared_ptr<dag::Node<DeviceVarType>>> devices;
    devices.reserve(m_devices_map.size());
    for (auto itr : m_devices_map) {
        devices.push_back(itr.second);
    }
    auto sorted_idx = dag::Evaluator::TopologicalSorting(devices);

    Context ctx;
    for (auto& idx : sorted_idx)
    {
        auto device = devices[idx];
        if (device->IsDirty()) {
            std::static_pointer_cast<Device>(device)->Execute(ctx);
            device->SetDirty(false);
        }
    }
}

}