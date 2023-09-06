/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <Atom/RHI/Device.h>
#include <Atom/RHI/Factory.h>
#include <Atom/RHI/MultiDeviceRayTracingBufferPools.h>
#include <Atom/RHI/RHISystemInterface.h>

namespace AZ::RHI
{
    const RHI::Ptr<RHI::MultiDeviceBufferPool>& MultiDeviceRayTracingBufferPools::GetShaderTableBufferPool() const
    {
        AZ_Assert(m_initialized, "MultiDeviceRayTracingBufferPools was not initialized");
        return m_mdShaderTableBufferPool;
    }

    const RHI::Ptr<RHI::MultiDeviceBufferPool>& MultiDeviceRayTracingBufferPools::GetScratchBufferPool() const
    {
        AZ_Assert(m_initialized, "MultiDeviceRayTracingBufferPools was not initialized");
        return m_mdScratchBufferPool;
    }

    const RHI::Ptr<RHI::MultiDeviceBufferPool>& MultiDeviceRayTracingBufferPools::GetBlasBufferPool() const
    {
        AZ_Assert(m_initialized, "MultiDeviceRayTracingBufferPools was not initialized");
        return m_mdBlasBufferPool;
    }

    const RHI::Ptr<RHI::MultiDeviceBufferPool>& MultiDeviceRayTracingBufferPools::GetTlasInstancesBufferPool() const
    {
        AZ_Assert(m_initialized, "MultiDeviceRayTracingBufferPools was not initialized");
        return m_mdTlasInstancesBufferPool;
    }

    const RHI::Ptr<RHI::MultiDeviceBufferPool>& MultiDeviceRayTracingBufferPools::GetTlasBufferPool() const
    {
        AZ_Assert(m_initialized, "MultiDeviceRayTracingBufferPools was not initialized");
        return m_mdTlasBufferPool;
    }

    void MultiDeviceRayTracingBufferPools::Init(MultiDevice::DeviceMask deviceMask)
    {
        if (m_initialized)
        {
            return;
        }

        MultiDeviceObject::Init(deviceMask);

        // Initialize device ray tracing buffer pools
        IterateDevices(
            [this](int deviceIndex)
            {
                RHI::Ptr<RHI::Device> device = RHISystemInterface::Get()->GetDevice(deviceIndex);
                m_deviceObjects[deviceIndex] = Factory::Get().CreateRayTracingBufferPools();
                GetDeviceRayTracingBufferPools(deviceIndex)->Init(device);
                return true;
            });

        m_mdShaderTableBufferPool = aznew RHI::MultiDeviceBufferPool();
        m_mdShaderTableBufferPool->Init(
            deviceMask,
            [this]()
            {
                IterateObjects<SingleDeviceRayTracingBufferPools>([this](auto deviceIndex, auto devicePools)
                {
                    m_mdShaderTableBufferPool->m_deviceObjects[deviceIndex] = devicePools->GetShaderTableBufferPool().get();
                    m_mdShaderTableBufferPool->m_descriptor = devicePools->GetShaderTableBufferPool()->GetDescriptor();
                });
                return ResultCode::Success;
            });

        m_mdScratchBufferPool = aznew RHI::MultiDeviceBufferPool();
        m_mdScratchBufferPool->Init(
            deviceMask,
            [this]()
            {
                IterateObjects<SingleDeviceRayTracingBufferPools>([this](auto deviceIndex, auto devicePools)
                {
                    m_mdScratchBufferPool->m_deviceObjects[deviceIndex] = devicePools->GetScratchBufferPool().get();
                    m_mdScratchBufferPool->m_descriptor = devicePools->GetScratchBufferPool()->GetDescriptor();
                });
                return ResultCode::Success;
            });

        m_mdBlasBufferPool = aznew RHI::MultiDeviceBufferPool();
        m_mdBlasBufferPool->Init(
            deviceMask,
            [this]()
            {
                IterateObjects<SingleDeviceRayTracingBufferPools>([this](auto deviceIndex, auto devicePools)
                {
                    m_mdBlasBufferPool->m_deviceObjects[deviceIndex] = devicePools->GetBlasBufferPool().get();
                    m_mdBlasBufferPool->m_descriptor = devicePools->GetBlasBufferPool()->GetDescriptor();
                });
                return ResultCode::Success;
            });

        m_mdTlasInstancesBufferPool = aznew RHI::MultiDeviceBufferPool();
        m_mdTlasInstancesBufferPool->Init(
            deviceMask,
            [this]()
            {
                IterateObjects<SingleDeviceRayTracingBufferPools>([this](auto deviceIndex, auto devicePools)
                {
                    m_mdTlasInstancesBufferPool->m_deviceObjects[deviceIndex] = devicePools->GetTlasInstancesBufferPool().get();
                    m_mdTlasInstancesBufferPool->m_descriptor = devicePools->GetTlasInstancesBufferPool()->GetDescriptor();
                });
                return ResultCode::Success;
            });

        m_mdTlasBufferPool = aznew RHI::MultiDeviceBufferPool();
        m_mdTlasBufferPool->Init(
            deviceMask,
            [this]()
            {
                IterateObjects<SingleDeviceRayTracingBufferPools>([this](auto deviceIndex, auto devicePools)
                {
                    m_mdTlasBufferPool->m_deviceObjects[deviceIndex] = devicePools->GetTlasBufferPool().get();
                    m_mdTlasBufferPool->m_descriptor = devicePools->GetTlasBufferPool()->GetDescriptor();
                });
                return ResultCode::Success;
            });

        m_initialized = true;
    }
} // namespace AZ::RHI
