#pragma once

#include "terr/typedef.h"
#include "terr/DeviceVarType.h"

#include <dag/Node.h>

#include <map>
#include <vector>

namespace terr
{

class Evaluator
{
public:
    // update device
    void DeviceChaged(const DevicePtr& device);

    // update devices
    void AddDevice(const DevicePtr& device);
    void RemoveDevice(const DevicePtr& device);
    void ClearAllDevices();

    // update device prop
    void PropChanged(const DevicePtr& device);

    // update device conn
    void Connect(const dag::Node<DeviceVarType>::PortAddr& from, const dag::Node<DeviceVarType>::PortAddr& to);
    void Disconnect(const dag::Node<DeviceVarType>::PortAddr& from, const dag::Node<DeviceVarType>::PortAddr& to);
    void RebuildConnections(const std::vector<std::pair<dag::Node<DeviceVarType>::PortAddr, dag::Node<DeviceVarType>::PortAddr>>& conns);

    void Update();

private:
    void TopologicalSorting() const;

private:
    std::map<std::string, DevicePtr> m_devices_map;

    mutable std::vector<DevicePtr> m_devices_sorted;

    bool m_dirty = false;

    size_t m_next_id = 0;

}; // Evaluator

}