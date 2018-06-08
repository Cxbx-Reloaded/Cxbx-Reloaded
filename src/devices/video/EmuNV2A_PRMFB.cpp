
// TODO: Remove disabled warning once case are add to PRMFB switch.
#pragma warning(push)
#pragma warning(disable: 4065)
DEVICE_READ32(PRMFB)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PRMFB); // TODO : DEVICE_READ32_REG(prmfb);
		break;
	}

	DEVICE_READ32_END(PRMFB);
}
#pragma warning(pop)

// TODO: Remove disabled warning once case are add to PRMFB switch.
#pragma warning(push)
#pragma warning(disable: 4065)
DEVICE_WRITE32(PRMFB)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PRMFB); // TODO : DEVICE_WRITE32_REG(prmfb);
		break;
	}

	DEVICE_WRITE32_END(PRMFB);
}
#pragma warning(pop)
