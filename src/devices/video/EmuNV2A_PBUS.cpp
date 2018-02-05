/* PBUS - bus control */
DEVICE_READ32(PBUS)
{
	DEVICE_READ32_SWITCH() {
	case NV_PBUS_PCI_NV_0:
		result = 0x10de;	// PCI_VENDOR_ID_NVIDIA	(?where to return PCI_DEVICE_ID_NVIDIA_NV2A = 0x01b7)
        // TODO : result = pci_get_long(d->dev.config + PCI_VENDOR_ID);
		break;
	case NV_PBUS_PCI_NV_1:
		result = 1; // NV_PBUS_PCI_NV_1_IO_SPACE_ENABLED
        // TODO : result = pci_get_long(d->dev.config + PCI_COMMAND);
		break;
	case NV_PBUS_PCI_NV_2:
		result = (0x02 << 24) | 161; // PCI_CLASS_DISPLAY_3D (0x02) Rev 161 (0xA1) 
        // TODO : result = pci_get_long(d->dev.config + PCI_CLASS_REVISION);
		break;

	default: 
		DEBUG_READ32_UNHANDLED(PBUS); // TODO : DEVICE_READ32_REG(pbus);
		break;
	}

	DEVICE_READ32_END(PBUS);
}

DEVICE_WRITE32(PBUS)
{
	switch(addr) {
	case NV_PBUS_PCI_NV_1:
		// TODO : Handle write on NV_PBUS_PCI_NV_1 with  1 (NV_PBUS_PCI_NV_1_IO_SPACE_ENABLED) + 4 (NV_PBUS_PCI_NV_1_BUS_MASTER_ENABLED)
        // pci_set_long(d->dev.config + PCI_COMMAND, val);
		break;
	default: 
		DEBUG_WRITE32_UNHANDLED(PBUS); // TODO : DEVICE_WRITE32_REG(pbus);
		break;
	}

	DEVICE_WRITE32_END(PBUS);
}
