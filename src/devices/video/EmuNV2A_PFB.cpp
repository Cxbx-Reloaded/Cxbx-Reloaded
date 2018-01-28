DEVICE_READ32(PFB)
{
	DEVICE_READ32_SWITCH() {
	case NV_PFB_CFG0:
        /* 3-4 memory partitions. The debug bios checks this. */
		result = 3; // = NV_PFB_CFG0_PART_4
		break;
	case NV_PFB_CSTATUS:
	{
		if (g_bIsChihiro || g_bIsDebug) { result = CONTIGUOUS_MEMORY_CHIHIRO_SIZE; break; }
		result = CONTIGUOUS_MEMORY_XBOX_SIZE;
	}
	break;
	case NV_PFB_WBC:
		result = 0; // = !NV_PFB_WBC_FLUSH /* Flush not pending. */
		break;
	default:
		DEVICE_READ32_REG(pfb);
		break;
	}

	DEVICE_READ32_END(PFB);
}

DEVICE_WRITE32(PFB)
{
	switch (addr) {
	default:
		DEVICE_WRITE32_REG(pfb);
		break;
	}

	DEVICE_WRITE32_END(PFB);
}
