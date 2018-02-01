DEVICE_READ32(PRMCIO)
{
	// vga_ioport_read :
	DEVICE_READ32_SWITCH() {
	case VGA_CRT_IM:
	case VGA_CRT_IC:
		result = d->prmcio.cr_index;
		break;
	case VGA_CRT_DM:
	case VGA_CRT_DC:
		result = d->prmcio.cr[d->prmcio.cr_index];
	
		printf("vga: read CR%x = 0x%02x\n", d->prmcio.cr_index, result);
		break;
	default:
		DEBUG_READ32_UNHANDLED(PRMCIO);
		printf("vga: UNHANDLED ADDR %s\n", addr);
		break;
	}

	DEVICE_READ32_END(PRMCIO);
}

DEVICE_WRITE32(PRMCIO)
{
	switch (addr) {
#if 0 // TODO : Enable
    case VGA_ATT_W:
        /* Cromwell sets attrs without enabling VGA_AR_ENABLE_DISPLAY
         * (which should result in a blank screen).
         * Either nvidia's hardware is lenient or it is set through
         * something else. The former seems more likely.
         */
        if (d->vga.ar_flip_flop == 0) {
            value |= VGA_AR_ENABLE_DISPLAY;
        }
        break;
#endif
	// vga_ioport_write :
	case VGA_CRT_IM:
	case VGA_CRT_IC:
		d->prmcio.cr_index = value;
		break;
	case VGA_CRT_DM:
	case VGA_CRT_DC:
		printf("vga: write CR%x = 0x%02x\n", d->prmcio.cr_index, value);

		/* handle CR0-7 protection */
		if ((d->prmcio.cr[VGA_CRTC_V_SYNC_END] & VGA_CR11_LOCK_CR0_CR7) &&
			d->prmcio.cr_index <= VGA_CRTC_OVERFLOW) {
			/* can always write bit 4 of CR7 */
			if (d->prmcio.cr_index == VGA_CRTC_OVERFLOW) {
				d->prmcio.cr[VGA_CRTC_OVERFLOW] = (d->prmcio.cr[VGA_CRTC_OVERFLOW] & ~0x10) |
					(value & 0x10);
				EmuWarning("TODO: vbe_update_vgaregs");
				//vbe_update_vgaregs();
			}
			return;
		}

		d->prmcio.cr[d->prmcio.cr_index] = value;
		EmuWarning("TODO: vbe_update_vgaregs");
		//vbe_update_vgaregs();

		switch (d->prmcio.cr_index) {
			case VGA_CRTC_H_TOTAL:
			case VGA_CRTC_H_SYNC_START:
			case VGA_CRTC_H_SYNC_END:
			case VGA_CRTC_V_TOTAL:
			case VGA_CRTC_OVERFLOW:
			case VGA_CRTC_V_SYNC_END:
			case VGA_CRTC_MODE:
				// TODO: s->update_retrace_info(s);
				EmuWarning("TODO: update_retrace_info");
				break;
			}
		break;
	default:
		DEBUG_WRITE32_UNHANDLED(PRMCIO); // TODO : DEVICE_WRITE32_REG(prmcio);
		break;
	}

	DEVICE_WRITE32_END(PRMCIO);
}
