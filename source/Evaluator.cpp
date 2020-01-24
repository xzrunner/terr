#include "terr/Evaluator.h"
#include "terr/Device.h"
#include "terr/Context.h"

#include <dag/Evaluator.h>

#include <stack>

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

    assert(m_devices_map.size() == m_devices_sorted.size());
    m_devices_map.insert({ name, device });
    m_devices_sorted.insert(m_devices_sorted.begin(), device);

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

    assert(m_devices_map.size() == m_devices_sorted.size());
    m_devices_map.erase(itr);
    for (auto itr = m_devices_sorted.begin(); itr != m_devices_sorted.end(); ++itr) {
        if (*itr == device) {
            m_devices_sorted.erase(itr);
            break;
        }
    }

    m_dirty = true;
}

void Evaluator::ClearAllDevices()
{
    if (m_devices_map.empty()) {
        return;
    }

    assert(m_devices_map.size() == m_devices_sorted.size());
    m_devices_map.clear();
    m_devices_sorted.clear();

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
    if (m_devices_sorted.empty()) {
        return;
    }

    TopologicalSorting();

    std::map<DevicePtr, int> map2index;
    for (int i = 0, n = m_devices_sorted.size(); i < n; ++i) {
        map2index.insert({ m_devices_sorted[i], i });
    }

    Context ctx;
    for (int i = 0, n = m_devices_sorted.size(); i < n; ++i)
    {
        auto& device = m_devices_sorted[i];
        if (device->IsDirty()) {
            device->Execute(ctx);
            device->SetDirty(false);
        }
    }
}

void Evaluator::TopologicalSorting() const
{
    std::vector<DevicePtr> devices;
    devices.reserve(m_devices_map.size());
    for (auto itr : m_devices_map) {
        devices.push_back(itr.second);
    }

    // prepare
    std::vector<int> in_deg(m_devices_map.size(), 0);
    std::vector<std::vector<int>> out_devices(devices.size());
    for (int i = 0, n = devices.size(); i < n; ++i)
    {
        auto& device = devices[i];
        for (auto& port : device->GetImports())
        {
            if (port.conns.empty()) {
                continue;
            }

            assert(port.conns.size() == 1);
            auto from = port.conns[0].node.lock();
            assert(from);
            for (int j = 0, m = devices.size(); j < m; ++j) {
                if (from == devices[j]) {
                    in_deg[i]++;
                    out_devices[j].push_back(i);
                    break;
                }
            }
        }
    }

    // sort
    std::stack<int> st;
    m_devices_sorted.clear();
    for (int i = 0, n = in_deg.size(); i < n; ++i) {
        if (in_deg[i] == 0) {
            st.push(i);
        }
    }
    while (!st.empty())
    {
        int v = st.top();
        st.pop();
        m_devices_sorted.push_back(devices[v]);
        for (auto& i : out_devices[v]) {
            assert(in_deg[i] > 0);
            in_deg[i]--;
            if (in_deg[i] == 0) {
                st.push(i);
            }
        }
    }
}

}