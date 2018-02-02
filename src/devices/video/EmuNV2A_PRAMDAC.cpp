DEVICE_READ32(PRAMDAC)
{
	DEVICE_READ32_SWITCH() {

	case NV_PRAMDAC_NVPLL_COEFF:
		result = d->pramdac.core_clock_coeff;
		break;
	case NV_PRAMDAC_MPLL_COEFF:
		result = d->pramdac.memory_clock_coeff;
		break;
	case NV_PRAMDAC_VPLL_COEFF:
		result = d->pramdac.video_clock_coeff;
		break;
	case NV_PRAMDAC_PLL_TEST_COUNTER:
		/* emulated PLLs locked instantly? */
		result = NV_PRAMDAC_PLL_TEST_COUNTER_VPLL2_LOCK
			| NV_PRAMDAC_PLL_TEST_COUNTER_NVPLL_LOCK
			| NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCK
			| NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCK;
		break;

	default: 
		//DEVICE_READ32_REG(pramdac); // Was : DEBUG_READ32_UNHANDLED(PRAMDAC);
		break;
	}

    /* Surprisingly, QEMU doesn't handle unaligned access for you properly */
    // result >>= 32 - 8 * size - 8 * (addr & 3);
	
	DEVICE_READ32_END(PRAMDAC);
}

DEVICE_WRITE32(PRAMDAC)
{
	switch (addr) {

	uint32_t m, n, p;
	case NV_PRAMDAC_NVPLL_COEFF:
		d->pramdac.core_clock_coeff = value;

		m = value & NV_PRAMDAC_NVPLL_COEFF_MDIV;
		n = (value & NV_PRAMDAC_NVPLL_COEFF_NDIV) >> 8;
		p = (value & NV_PRAMDAC_NVPLL_COEFF_PDIV) >> 16;

		if (m == 0) {
			d->pramdac.core_clock_freq = 0;
		} else {
			d->pramdac.core_clock_freq = (NV2A_CRYSTAL_FREQ * n)
				/ (1 << p) / m;
		}

		break;
	case NV_PRAMDAC_MPLL_COEFF:
		d->pramdac.memory_clock_coeff = value;
		break;
	case NV_PRAMDAC_VPLL_COEFF:
		d->pramdac.video_clock_coeff = value;
		break;

	default: 
		//DEVICE_WRITE32_REG(pramdac); // Was : DEBUG_WRITE32_UNHANDLED(PRAMDAC);
		break;
	}

	DEVICE_WRITE32_END(PRAMDAC);
}
