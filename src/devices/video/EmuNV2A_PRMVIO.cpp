
// TODO: Remove disabled warning once case are add to PRMVIO switch.
#pragma warning(push)
#pragma warning(disable: 4065)
DEVICE_READ32(PRMVIO)
{
	// vga_ioport_read
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PRMVIO); // TODO : DEVICE_READ32_REG(prmvio);
		break;
	}

	DEVICE_READ32_END(PRMVIO);
}
#pragma warning(pop)

// TODO: Remove disabled warning once case are add to PRMVIO switch.
#pragma warning(push)
#pragma warning(disable: 4065)
DEVICE_WRITE32(PRMVIO)
{
	// vga_ioport_write
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PRMVIO); // TODO : DEVICE_WRITE32_REG(prmvio);
		break;
	}

	DEVICE_WRITE32_END(PRMVIO);
}
#pragma warning(pop)
