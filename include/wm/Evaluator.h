#pragma once

#include "wm/typedef.h"
#include "wm/DeviceVarType.h"

#include <dag/Node.h>

#include <map>

namespace wm
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
    std::map<std::string, DevicePtr> m_devices_map;

    bool m_dirty = false;

    size_t m_next_id = 0;

}; // Evaluator

}