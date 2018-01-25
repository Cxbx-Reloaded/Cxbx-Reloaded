DEVICE_READ32(PRAMIN)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEVICE_READ32_REG(pramin);
		break;
	}

	DEVICE_READ32_END(PRAMIN);
}

DEVICE_WRITE32(PRAMIN)
{
	switch (addr) {
	default:
		DEVICE_WRITE32_REG(pramin);
		break;
	}

	DEVICE_WRITE32_END(PRAMIN);
}
