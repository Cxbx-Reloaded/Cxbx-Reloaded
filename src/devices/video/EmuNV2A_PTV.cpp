
// TODO: Remove disabled warning once case are add to PTV switch.
#pragma warning(push)
#pragma warning(disable: 4065)
DEVICE_READ32(PTV)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PTV); // TODO : DEVICE_READ32_REG(ptv);
		break;
	}

	DEVICE_READ32_END(PTV);
}
#pragma warning(pop)

// TODO: Remove disabled warning once case are add to PTV switch.
#pragma warning(push)
#pragma warning(disable: 4065)
DEVICE_WRITE32(PTV)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PTV); // TODO : DEVICE_WRITE32_REG(ptv);
		break;
	}

	DEVICE_WRITE32_END(PTV);
}
#pragma warning(pop)
