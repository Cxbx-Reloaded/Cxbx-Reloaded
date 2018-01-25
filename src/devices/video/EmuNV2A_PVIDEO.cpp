/*
static void pvideo_vga_invalidate(NV2AState *d)
{
    int y1 = GET_MASK(d->pvideo.regs[NV_PVIDEO_POINT_OUT],
                      NV_PVIDEO_POINT_OUT_Y);
    int y2 = y1 + GET_MASK(d->pvideo.regs[NV_PVIDEO_SIZE_OUT],
                           NV_PVIDEO_SIZE_OUT_HEIGHT);
    NV2A_DPRINTF("pvideo_vga_invalidate %d %d\n", y1, y2);
    vga_invalidate_scanlines(&d->vga, y1, y2);
}

*/
DEVICE_READ32(PVIDEO)
{
	DEVICE_READ32_SWITCH() {

	case NV_PVIDEO_STOP:
		result = 0;
		break;
	default:
		DEVICE_READ32_REG(pvideo);
		break;
	}

	DEVICE_READ32_END(PVIDEO);
}

DEVICE_WRITE32(PVIDEO)
{
	switch (addr) {
	case NV_PVIDEO_BUFFER:
		pvideo.regs[addr] = value;
		// TODO: vga.enable_overlay = true;
		// pvideo_vga_invalidate(d);
		break;
	case NV_PVIDEO_STOP:
		pvideo.regs[NV_PVIDEO_BUFFER] = 0;
		// TODO: vga.enable_overlay = false;
		//pvideo_vga_invalidate(d);
		break;
	default:
		DEVICE_WRITE32_REG(pvideo);
		break;
	}

	DEVICE_WRITE32_END(PVIDEO);
}

