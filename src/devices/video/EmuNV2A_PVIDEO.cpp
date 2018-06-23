static void pvideo_vga_invalidate(NV2AState *d)
{
    int y1 = GET_MASK(d->pvideo.regs[NV_PVIDEO_POINT_OUT(0)],
                      NV_PVIDEO_POINT_OUT_Y);
    int y2 = y1 + GET_MASK(d->pvideo.regs[NV_PVIDEO_SIZE_OUT(0)],
                           NV_PVIDEO_SIZE_OUT_HEIGHT);
    NV2A_DPRINTF("pvideo_vga_invalidate %d %d\n", y1, y2);
	// TODO : vga_invalidate_scanlines(&d->vga, y1, y2);

	// TODO: Remove this when the AMD crash is solved in vblank_thread
	if (d->enable_overlay) // to avoid flickering, only update when there's an overlay
		NV2ADevice::UpdateHostDisplay(d);
}

DEVICE_READ32(PVIDEO)
{
	DEVICE_READ32_SWITCH() {

	case NV_PVIDEO_INTR:
		result = d->pvideo.pending_interrupts;
		break;
	case NV_PVIDEO_INTR_EN:
		result = d->pvideo.enabled_interrupts;
		break;
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
	case NV_PVIDEO_INTR:
		d->pvideo.pending_interrupts &= ~value;
		update_irq(d);
//		qemu_cond_broadcast(&d->pvideo.interrupt_cond);
		break;
	case NV_PVIDEO_INTR_EN:
		d->pvideo.enabled_interrupts = value;
		update_irq(d);
		break;
	case NV_PVIDEO_BUFFER:
		d->pvideo.regs[NV_PVIDEO_BUFFER] = value;
		d->enable_overlay = (value != 0);
		pvideo_vga_invalidate(d);
		break;
	case NV_PVIDEO_STOP:
		d->pvideo.regs[NV_PVIDEO_BUFFER] = 0;
		d->enable_overlay = false;
		pvideo_vga_invalidate(d);
		break;
	default:
		DEVICE_WRITE32_REG(pvideo);
		break;
	}

	DEVICE_WRITE32_END(PVIDEO);
}

