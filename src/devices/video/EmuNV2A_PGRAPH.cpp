DEVICE_READ32(PGRAPH)
{
	std::lock_guard<std::mutex> lk(pgraph.mutex);

	DEVICE_READ32_SWITCH() {
	case NV_PGRAPH_INTR:
		result = pgraph.pending_interrupts;
		break;
	case NV_PGRAPH_INTR_EN:
		result = pgraph.enabled_interrupts;
		break;
	case NV_PGRAPH_NSOURCE:
		result = pgraph.notify_source;
		break;
	case NV_PGRAPH_CTX_USER:
		SET_MASK(result, NV_PGRAPH_CTX_USER_CHANNEL_3D, pgraph.context[pgraph.channel_id].channel_3d);
		SET_MASK(result, NV_PGRAPH_CTX_USER_CHANNEL_3D_VALID, 1);
		SET_MASK(result, NV_PGRAPH_CTX_USER_SUBCH, pgraph.context[pgraph.channel_id].subchannel << 13);
		SET_MASK(result, NV_PGRAPH_CTX_USER_CHID, pgraph.channel_id);
		break;
	case NV_PGRAPH_TRAPPED_ADDR:
		SET_MASK(result, NV_PGRAPH_TRAPPED_ADDR_CHID, pgraph.trapped_channel_id);
		SET_MASK(result, NV_PGRAPH_TRAPPED_ADDR_SUBCH, pgraph.trapped_subchannel);
		SET_MASK(result, NV_PGRAPH_TRAPPED_ADDR_MTHD, pgraph.trapped_method);
		break;
	case NV_PGRAPH_TRAPPED_DATA_LOW:
		result = pgraph.trapped_data[0];
		break;
	case NV_PGRAPH_FIFO:
		SET_MASK(result, NV_PGRAPH_FIFO_ACCESS, pgraph.fifo_access);
		break;
	case NV_PGRAPH_CHANNEL_CTX_TABLE:
		result = pgraph.context_table >> 4;
		break;
	case NV_PGRAPH_CHANNEL_CTX_POINTER:
		result = pgraph.context_address >> 4;
		break;
	default:
		DEVICE_READ32_REG(pgraph); // Was : DEBUG_READ32_UNHANDLED(PGRAPH);
	}

	DEVICE_READ32_END(PGRAPH);
}

static void pgraph_set_context_user(uint32_t value)
{
	pgraph.channel_id = (value & NV_PGRAPH_CTX_USER_CHID) >> 24;
	pgraph.context[pgraph.channel_id].channel_3d = GET_MASK(value, NV_PGRAPH_CTX_USER_CHANNEL_3D);
	pgraph.context[pgraph.channel_id].subchannel = GET_MASK(value, NV_PGRAPH_CTX_USER_SUBCH);
}

DEVICE_WRITE32(PGRAPH)
{
	std::lock_guard<std::mutex> lk(pgraph.mutex);

	switch (addr) {
	case NV_PGRAPH_INTR:
		pgraph.pending_interrupts &= ~value;
		pgraph.interrupt_cond.notify_all();
		break;
	case NV_PGRAPH_INTR_EN:
		pgraph.enabled_interrupts = value;
		break;
	case NV_PGRAPH_CTX_CONTROL:
		pgraph.channel_valid = (value & NV_PGRAPH_CTX_CONTROL_CHID);
		break;
	case NV_PGRAPH_CTX_USER:
		pgraph_set_context_user(value);
		break;
	case NV_PGRAPH_INCREMENT:
		if (value & NV_PGRAPH_INCREMENT_READ_3D) {
			SET_MASK(pgraph.regs[NV_PGRAPH_SURFACE],
				NV_PGRAPH_SURFACE_READ_3D,
				(GET_MASK(pgraph.regs[NV_PGRAPH_SURFACE],
					NV_PGRAPH_SURFACE_READ_3D) + 1)
				% GET_MASK(pgraph.regs[NV_PGRAPH_SURFACE],
					NV_PGRAPH_SURFACE_MODULO_3D));

			pgraph.flip_3d.notify_all();
		}
		break;
	case NV_PGRAPH_FIFO:
		pgraph.fifo_access = GET_MASK(value, NV_PGRAPH_FIFO_ACCESS);
		pgraph.fifo_access_cond.notify_all();
		break;
	case NV_PGRAPH_CHANNEL_CTX_TABLE:
		pgraph.context_table = (value & NV_PGRAPH_CHANNEL_CTX_TABLE_INST) << 4;
		break;
	case NV_PGRAPH_CHANNEL_CTX_POINTER:
		pgraph.context_address =
			(value & NV_PGRAPH_CHANNEL_CTX_POINTER_INST) << 4;
		break;
	case NV_PGRAPH_CHANNEL_CTX_TRIGGER:
		if (value & NV_PGRAPH_CHANNEL_CTX_TRIGGER_READ_IN) {
			printf("PGRAPH: read channel %d context from %0x08X\n",
				pgraph.channel_id, pgraph.context_address);

			uint8_t *context_ptr = (uint8_t*)(pramin.memory + pgraph.context_address);
			uint32_t context_user = ldl_le_p((uint32_t*)context_ptr);

			printf("    - CTX_USER = 0x%x\n", context_user);


			pgraph_set_context_user(context_user);
		}
		if (value & NV_PGRAPH_CHANNEL_CTX_TRIGGER_WRITE_OUT) {
			/* do stuff ... */
		}

		break;
	default: 
		DEVICE_WRITE32_REG(pgraph); // Was : DEBUG_WRITE32_UNHANDLED(PGRAPH);
		break;
	}

	DEVICE_WRITE32_END(PGRAPH);
}
