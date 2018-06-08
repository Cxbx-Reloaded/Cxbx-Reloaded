
// TODO: Remove disabled warning once case are add to PSTRAPS switch.
#pragma warning(push)
#pragma warning(disable: 4065)
DEVICE_READ32(PSTRAPS)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PSTRAPS);
		break;
	}

	DEVICE_READ32_END(PSTRAPS);
}
#pragma warning(pop)

// TODO: Remove disabled warning once case are add to PSTRAPS switch.
#pragma warning(push)
#pragma warning(disable: 4065)
DEVICE_WRITE32(PSTRAPS)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PSTRAPS);
		break;
	}

	DEVICE_WRITE32_END(PSTRAPS);
}
#pragma warning(pop)
