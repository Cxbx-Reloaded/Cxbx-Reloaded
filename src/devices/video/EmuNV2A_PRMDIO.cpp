
// TODO: Remove disabled warning once case are add to PRMDIO switch.
#pragma warning(push)
#pragma warning(disable: 4065)
DEVICE_READ32(PRMDIO)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PRMDIO);
		break;
	}

	DEVICE_READ32_END(PRMDIO);
}
#pragma warning(pop)

// TODO: Remove disabled warning once case are add to PRMDIO switch.
#pragma warning(push)
#pragma warning(disable: 4065)
DEVICE_WRITE32(PRMDIO)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PRMDIO);
		break;
	}

	DEVICE_WRITE32_END(PRMDIO);
}
#pragma warning(pop)
