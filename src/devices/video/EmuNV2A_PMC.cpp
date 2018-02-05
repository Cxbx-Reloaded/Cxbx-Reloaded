/* PMC - card master control */
DEVICE_READ32(PMC)
{
	DEVICE_READ32_SWITCH() {
	case NV_PMC_BOOT_0:	// chipset and stepping: NV2A, A02, Rev 0
		result = 0x02A000A2;
		break;
	case NV_PMC_BOOT_1:	// Selects big/little endian mode for the card
		result = 0; // When read, returns 0 if in little-endian mode, 0x01000001 if in big-endian mode.
		break;
	case NV_PMC_INTR_0: // Shows which functional units have pending IRQ
		result = d->pmc.pending_interrupts;
		break;
	case NV_PMC_INTR_EN_0: // Selects which functional units can cause IRQs
		result = d->pmc.enabled_interrupts;
		break;
	default:
		result = 0;
		//DEVICE_READ32_REG(PMC); // Was : DEBUG_READ32_UNHANDLED(PMC);
		break;
	}

	DEVICE_READ32_END(PMC);
}

DEVICE_WRITE32(PMC)
{
	switch(addr) {
	case NV_PMC_INTR_0:
        /* the bits of the interrupts to clear are wrtten */
		d->pmc.pending_interrupts &= ~value;
		update_irq(d);
		break;
	case NV_PMC_INTR_EN_0:
		d->pmc.enabled_interrupts = value;
		update_irq(d);
		break;

	default: 
		//DEVICE_WRITE32_REG(pmc); // Was : DEBUG_WRITE32_UNHANDLED(PMC);
		break;
	}

	DEVICE_WRITE32_END(PMC);
}


