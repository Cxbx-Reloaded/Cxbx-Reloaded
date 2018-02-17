/* USER - PFIFO MMIO and DMA submission area */
DEVICE_READ32(USER)
{
	unsigned int channel_id = addr >> 16;
	assert(channel_id < NV2A_NUM_CHANNELS);

	ChannelControl *control = &d->user.channel_control[channel_id];

	uint32_t channel_modes = d->pfifo.regs[NV_PFIFO_MODE];

	/* PIO Mode */
	if (!channel_modes & (1 << channel_id)) {
		assert(false);
	}

	/* DMA Mode */
	addr &= 0xFFFF;
	DEVICE_READ32_SWITCH() {
		case NV_USER_DMA_PUT:
			result = control->dma_put;
			break;
		case NV_USER_DMA_GET:
			result = control->dma_get;
			break;
		case NV_USER_REF:
			result = control->ref;
			break;
		default:
			DEBUG_READ32_UNHANDLED(USER);
			break;
	}

	DEVICE_READ32_END(USER);
}

DEVICE_WRITE32(USER)
{
	unsigned int channel_id = addr >> 16;
	assert(channel_id < NV2A_NUM_CHANNELS);

	ChannelControl *control = &d->user.channel_control[channel_id];

	uint32_t channel_modes = d->pfifo.regs[NV_PFIFO_MODE];
	if (channel_modes & (1 << channel_id)) {
		/* DMA Mode */
		switch (addr & 0xFFFF) {
		case NV_USER_DMA_PUT:
			control->dma_put = value;

			if (d->pfifo.cache1.push_enabled) {
				pfifo_run_pusher(d);
			}
			break;
		case NV_USER_DMA_GET:
			control->dma_get = value;
			break;
		case NV_USER_REF:
			control->ref = value;
			break;
		default:
			DEBUG_WRITE32_UNHANDLED(USER);
			break;
		}
	} else {
		/* PIO Mode */
		assert(false);
	}

	DEVICE_WRITE32_END(USER);
}
