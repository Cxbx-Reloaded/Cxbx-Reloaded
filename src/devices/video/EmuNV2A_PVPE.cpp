
// TODO: Remove disabled warning once case are add to PVPE switch.
#pragma warning(push)
#pragma warning(disable: 4065)
DEVICE_READ32(PVPE)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PVPE); // TODO : DEVICE_READ32_REG(pvpe);
		break;
	}

	DEVICE_READ32_END(PVPE);
}
#pragma warning(pop)

// TODO: Remove disabled warning once case are add to PVPE switch.
#pragma warning(push)
#pragma warning(disable: 4065)
DEVICE_WRITE32(PVPE)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PVPE); // TODO : DEVICE_WRITE32_REG(pvpe);
		break;
	}

	DEVICE_WRITE32_END(PVPE);
}
#pragma warning(pop)
