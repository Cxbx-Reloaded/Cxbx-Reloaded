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
	res.l.high = (uint32_t)(rh / c);
	res.l.low = (((rh % c) << 32) + (rl & 0xffffffff)) / c;
	return res.ll;
}

/* PTIMER - time measurement and time-based alarms */
static uint64_t ptimer_get_clock(NV2AState * d)
{
	// Get time in nanoseconds
    uint64_t time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	
	return muldiv64(time,
					uint32_t(d->pramdac.core_clock_freq * d->ptimer.numerator),
					CLOCKS_PER_SEC * d->ptimer.denominator);
}

DEVICE_READ32(PTIMER)
{
	DEVICE_READ32_SWITCH() {
	case NV_PTIMER_INTR_0:
		result = d->ptimer.pending_interrupts;
		break;
	case NV_PTIMER_INTR_EN_0:
		result = d->ptimer.enabled_interrupts;
		break;
	case NV_PTIMER_NUMERATOR:
		result = d->ptimer.numerator;
		break;
	case NV_PTIMER_DENOMINATOR:
		result = d->ptimer.denominator;
		break;
	case NV_PTIMER_TIME_0:
		result = (ptimer_get_clock(d) & 0x7ffffff) << 5;
		break;
	case NV_PTIMER_TIME_1:
		result = (ptimer_get_clock(d) >> 27) & 0x1fffffff;
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
		d->ptimer.pending_interrupts &= ~value;
		update_irq(d);
		break;
	case NV_PTIMER_INTR_EN_0:
		d->ptimer.enabled_interrupts = value;
		update_irq(d);
		break;
	case NV_PTIMER_DENOMINATOR:
		d->ptimer.denominator = value;
		break;
	case NV_PTIMER_NUMERATOR:
		d->ptimer.numerator = value;
		break;
	case NV_PTIMER_ALARM_0:
		d->ptimer.alarm_time = value;
		break;
	default: 
		//DEVICE_WRITE32_REG(ptimer); // Was : DEBUG_WRITE32_UNHANDLED(PTIMER);
		break;
	}

	DEVICE_WRITE32_END(PTIMER);
}
