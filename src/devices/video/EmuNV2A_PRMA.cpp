
// TODO: Remove disabled warning once case are add to PRMA switch.
#pragma warning(push)
#pragma warning(disable: 4065)
DEVICE_READ32(PRMA)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PRMA); // TODO : DEVICE_READ32_REG(prma);
		break;
	}

	DEVICE_READ32_END(PRMA);
}
#pragma warning(pop)

// TODO: Remove disabled warning once case are add to PRMA switch.
#pragma warning(push)
#pragma warning(disable: 4065)
DEVICE_WRITE32(PRMA)
{
	switch(addr) {
	default: 
		DEBUG_WRITE32_UNHANDLED(PRMA); // TODO : DEVICE_WRITE32_REG(prma);
		break;
	}

	DEVICE_WRITE32_END(PRMA);
}
#pragma warning(pop)
