DEVICE_READ32(PRAMDAC)
{
	DEVICE_READ32_SWITCH() {

	case NV_PRAMDAC_NVPLL_COEFF:
		result = pramdac.core_clock_coeff;
		break;
	case NV_PRAMDAC_MPLL_COEFF:
		result = pramdac.memory_clock_coeff;
		break;
	case NV_PRAMDAC_VPLL_COEFF:
		result = pramdac.video_clock_coeff;
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

	DEVICE_READ32_END(PRAMDAC);
}

DEVICE_WRITE32(PRAMDAC)
{
	switch (addr) {

	uint32_t m, n, p;
	case NV_PRAMDAC_NVPLL_COEFF:
		pramdac.core_clock_coeff = value;

		m = value & NV_PRAMDAC_NVPLL_COEFF_MDIV;
		n = (value & NV_PRAMDAC_NVPLL_COEFF_NDIV) >> 8;
		p = (value & NV_PRAMDAC_NVPLL_COEFF_PDIV) >> 16;

		if (m == 0) {
			pramdac.core_clock_freq = 0;
		}
		else {
			pramdac.core_clock_freq = (NV2A_CRYSTAL_FREQ * n)
				/ (1 << p) / m;
		}

		break;
	case NV_PRAMDAC_MPLL_COEFF:
		pramdac.memory_clock_coeff = value;
		break;
	case NV_PRAMDAC_VPLL_COEFF:
		pramdac.video_clock_coeff = value;
		break;

	default: 
		//DEVICE_WRITE32_REG(pramdac); // Was : DEBUG_WRITE32_UNHANDLED(PRAMDAC);
		break;
	}

	DEVICE_WRITE32_END(PRAMDAC);
}
