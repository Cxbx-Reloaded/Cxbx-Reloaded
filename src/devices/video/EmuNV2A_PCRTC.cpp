DEVICE_READ32(PCRTC)
{
	DEVICE_READ32_SWITCH() {

	case NV_PCRTC_INTR_0:
		result = d->pcrtc.pending_interrupts;
		break;
	case NV_PCRTC_INTR_EN_0:
		result = d->pcrtc.enabled_interrupts;
		break;
	case NV_PCRTC_START:
		result = d->pcrtc.start;
		break;
	default: 
		result = 0;
		//DEVICE_READ32_REG(pcrtc); // Was : DEBUG_READ32_UNHANDLED(PCRTC);
		break;
	}

	DEVICE_READ32_END(PCRTC);
}

DEVICE_WRITE32(PCRTC)
{
	switch (addr) {

	case NV_PCRTC_INTR_0:
		d->pcrtc.pending_interrupts &= ~value;
		update_irq(d);
		break;
	case NV_PCRTC_INTR_EN_0:
		d->pcrtc.enabled_interrupts = value;
		update_irq(d);
		break;
	case NV_PCRTC_START:
        value &= 0x07FFFFFF;
        // assert(val < memory_region_size(d->vram));
		d->pcrtc.start = value;

        NV2A_DPRINTF("PCRTC_START - %x %x %x %x\n",
                d->vram_ptr[value+64], d->vram_ptr[value+64+1],
                d->vram_ptr[value+64+2], d->vram_ptr[value+64+3]);
		break;

	default: 
		DEVICE_WRITE32_REG(pcrtc); // Was : DEBUG_WRITE32_UNHANDLED(PCRTC);
		break;
	}

	DEVICE_WRITE32_END(PCRTC);
}
