static inline uint64_t muldiv64(uint64_t a, uint32_t b, uint32_t c)
{
	union {
		uint64_t ll;
		struct {
			uint32_t low, high;
		} l;
	} u, res;
	uint64_t rl, rh;

	u.ll = a;
	rl = (uint64_t)u.l.low * (uint64_t)b;
	rh = (uint64_t)u.l.high * (uint64_t)b;
	rh += (rl >> 32);
	res.l.high = rh / c;
	res.l.low = (((rh % c) << 32) + (rl & 0xffffffff)) / c;
	return res.ll;
}

/* PIMTER - time measurement and time-based alarms */
static uint32_t ptimer_get_clock()
{
	// Get time in nanoseconds
	long int time = static_cast<long int>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
	
	return muldiv64(time, pramdac.core_clock_freq * ptimer.numerator, CLOCKS_PER_SEC * ptimer.denominator);
}

DEVICE_READ32(PTIMER)
{
	DEVICE_READ32_SWITCH() {
	case NV_PTIMER_INTR_0:
		result = ptimer.pending_interrupts;
		break;
	case NV_PTIMER_INTR_EN_0:
		result = ptimer.enabled_interrupts;
		break;
	case NV_PTIMER_NUMERATOR:
		result = ptimer.numerator;
		break;
	case NV_PTIMER_DENOMINATOR:
		result = ptimer.denominator;
		break;
	case NV_PTIMER_TIME_0:
		result = (ptimer_get_clock() & 0x7ffffff) << 5;
		break;
	case NV_PTIMER_TIME_1:
		result = (ptimer_get_clock() >> 27) & 0x1fffffff;
		break;
	default: 
		result = 0;
		//DEVICE_READ32_REG(ptimer); // Was : DEBUG_READ32_UNHANDLED(PTIMER);
		break;
	}

	DEVICE_READ32_END(PTIMER);
}


DEVICE_WRITE32(PTIMER)
{
	switch (addr) {

	case NV_PTIMER_INTR_0:
		ptimer.pending_interrupts &= ~value;
		update_irq();
		break;
	case NV_PTIMER_INTR_EN_0:
		ptimer.enabled_interrupts = value;
		update_irq();
		break;
	case NV_PTIMER_DENOMINATOR:
		ptimer.denominator = value;
		break;
	case NV_PTIMER_NUMERATOR:
		ptimer.numerator = value;
		break;
	case NV_PTIMER_ALARM_0:
		ptimer.alarm_time = value;
		break;
	default: 
		//DEVICE_WRITE32_REG(ptimer); // Was : DEBUG_WRITE32_UNHANDLED(PTIMER);
		break;
	}

	DEVICE_WRITE32_END(PTIMER);
}
