// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   src->devices->video->EmuNV2A_PFIFO.cpp
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license, or (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not, write to the Free Software Foundation, Inc.,
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  nv2a.cpp is heavily based on code from XQEMU
// *  Copyright(c) 2012 espes
// *  Copyright(c) 2015 Jannik Vogel
// *  https://github.com/espes/xqemu/blob/xbox/hw/xbox/nv2a.c
// *  (c) 2017-2018 Luke Usher <luke.usher@outlook.com>
// *  (c) 2018 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

typedef struct RAMHTEntry {
	uint32_t handle;
	xbaddr instance;
	enum FIFOEngine engine;
	unsigned int channel_id : 5;
	bool valid;
} RAMHTEntry;

static void pfifo_run_pusher(NV2AState *d); // forward declaration
int pfifo_puller_thread(NV2AState *d);
static uint32_t ramht_hash(NV2AState *d, uint32_t handle);
static RAMHTEntry ramht_lookup(NV2AState *d, uint32_t handle); // forward declaration

/* PFIFO - MMIO and DMA FIFO submission to PGRAPH and VPE */
DEVICE_READ32(PFIFO)
{
	DEVICE_READ32_SWITCH() {
	case NV_PFIFO_RAMHT:
		result = 0x03000100; // = NV_PFIFO_RAMHT_SIZE_4K | NV_PFIFO_RAMHT_BASE_ADDRESS(NumberOfPaddingBytes >> 12) | NV_PFIFO_RAMHT_SEARCH_128
		break;
	case NV_PFIFO_RAMFC:
		result = 0x00890110; // = ? | NV_PFIFO_RAMFC_SIZE_2K | ?
		break;
	case NV_PFIFO_INTR_0:
		result = d->pfifo.pending_interrupts;
		break;
	case NV_PFIFO_INTR_EN_0:
		result = d->pfifo.enabled_interrupts;
		break;
	case NV_PFIFO_RUNOUT_STATUS:
		result = NV_PFIFO_RUNOUT_STATUS_LOW_MARK; /* low mark empty */
		break;
	case NV_PFIFO_CACHE1_PUSH0:
		result = d->pfifo.cache1.push_enabled;
		break;
	case NV_PFIFO_CACHE1_PUSH1:
		SET_MASK(result, NV_PFIFO_CACHE1_PUSH1_CHID, d->pfifo.cache1.channel_id);
		SET_MASK(result, NV_PFIFO_CACHE1_PUSH1_MODE, d->pfifo.cache1.mode);
		break;
	case NV_PFIFO_CACHE1_STATUS: {
		qemu_mutex_lock(&d->pfifo.cache1.cache_lock);

		if (d->pfifo.cache1.cache.empty()) {
			result |= NV_PFIFO_CACHE1_STATUS_LOW_MARK; /* low mark empty */
		}

		qemu_mutex_unlock(&d->pfifo.cache1.cache_lock);
		break;
	}
	case NV_PFIFO_CACHE1_DMA_PUSH:
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_PUSH_ACCESS,
			d->pfifo.cache1.dma_push_enabled);
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_PUSH_STATUS,
			d->pfifo.cache1.dma_push_suspended);
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_PUSH_BUFFER, 1); /* buffer emoty */
		break;
	case NV_PFIFO_CACHE1_DMA_STATE:
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE,
			d->pfifo.cache1.method_nonincreasing);
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_STATE_METHOD,
			d->pfifo.cache1.method >> 2);
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_STATE_SUBCHANNEL,
			d->pfifo.cache1.subchannel);
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_STATE_METHOD_COUNT,
			d->pfifo.cache1.method_count);
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_STATE_ERROR,
			d->pfifo.cache1.error);
		break;
	case NV_PFIFO_CACHE1_DMA_INSTANCE:
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_INSTANCE_ADDRESS_MASK,
			d->pfifo.cache1.dma_instance >> 4);
		break;
	case NV_PFIFO_CACHE1_DMA_PUT:
		result = d->user.channel_control[d->pfifo.cache1.channel_id].dma_put;
		break;
	case NV_PFIFO_CACHE1_DMA_GET:
		result = d->user.channel_control[d->pfifo.cache1.channel_id].dma_get;
		break;
	case NV_PFIFO_CACHE1_DMA_SUBROUTINE:
		result = d->pfifo.cache1.subroutine_return
			| (xbaddr)d->pfifo.cache1.subroutine_active;
		break;
	case NV_PFIFO_CACHE1_PULL0: {
		qemu_mutex_lock(&d->pfifo.cache1.cache_lock);
		result = d->pfifo.cache1.pull_enabled;
		qemu_mutex_unlock(&d->pfifo.cache1.cache_lock);
		break;
	}
	case NV_PFIFO_CACHE1_ENGINE: {
		qemu_mutex_lock(&d->pfifo.cache1.cache_lock);

		for (int i = 0; i < NV2A_NUM_SUBCHANNELS; i++) {
			result |= d->pfifo.cache1.bound_engines[i] << (i * 2);
		}
		qemu_mutex_unlock(&d->pfifo.cache1.cache_lock);
		break;
	}
	case NV_PFIFO_CACHE1_DMA_DCOUNT:
		result = d->pfifo.cache1.dcount;
		break;
	case NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW:
		result = d->pfifo.cache1.get_jmp_shadow;
		break;
	case NV_PFIFO_CACHE1_DMA_RSVD_SHADOW:
		result = d->pfifo.cache1.rsvd_shadow;
		break;
	case NV_PFIFO_CACHE1_DMA_DATA_SHADOW:
		result = d->pfifo.cache1.data_shadow;
		break;
	default:
		DEVICE_READ32_REG(pfifo); // Was : DEBUG_READ32_UNHANDLED(PFIFO);
		break;
	}

	DEVICE_READ32_END(PFIFO);
}

DEVICE_WRITE32(PFIFO)
{
    int i;

	switch(addr) {
		case NV_PFIFO_INTR_0:
			d->pfifo.pending_interrupts &= ~value;
			update_irq(d);
			break;
		case NV_PFIFO_INTR_EN_0:
			d->pfifo.enabled_interrupts = value;
			update_irq(d);
			break;
		case NV_PFIFO_CACHE1_PUSH0:
			d->pfifo.cache1.push_enabled = value & NV_PFIFO_CACHE1_PUSH0_ACCESS;
			break;
		case NV_PFIFO_CACHE1_PUSH1:
			d->pfifo.cache1.channel_id = GET_MASK(value, NV_PFIFO_CACHE1_PUSH1_CHID);
			d->pfifo.cache1.mode = (FifoMode)GET_MASK(value, NV_PFIFO_CACHE1_PUSH1_MODE);
			assert(d->pfifo.cache1.channel_id < NV2A_NUM_CHANNELS);
			break;
		case NV_PFIFO_CACHE1_DMA_PUSH:
			d->pfifo.cache1.dma_push_enabled =
				GET_MASK(value, NV_PFIFO_CACHE1_DMA_PUSH_ACCESS);
			if (d->pfifo.cache1.dma_push_suspended
				&& !GET_MASK(value, NV_PFIFO_CACHE1_DMA_PUSH_STATUS)) {
				d->pfifo.cache1.dma_push_suspended = false;
				pfifo_run_pusher(d);
			}
			d->pfifo.cache1.dma_push_suspended =
				GET_MASK(value, NV_PFIFO_CACHE1_DMA_PUSH_STATUS);
			break;
		case NV_PFIFO_CACHE1_DMA_STATE:
			d->pfifo.cache1.method_nonincreasing =
				GET_MASK(value, NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE);
			d->pfifo.cache1.method =
				GET_MASK(value, NV_PFIFO_CACHE1_DMA_STATE_METHOD) << 2;
			d->pfifo.cache1.subchannel =
				GET_MASK(value, NV_PFIFO_CACHE1_DMA_STATE_SUBCHANNEL);
			d->pfifo.cache1.method_count =
				GET_MASK(value, NV_PFIFO_CACHE1_DMA_STATE_METHOD_COUNT);
			d->pfifo.cache1.error =
				GET_MASK(value, NV_PFIFO_CACHE1_DMA_STATE_ERROR);
			break;
		case NV_PFIFO_CACHE1_DMA_INSTANCE:
			d->pfifo.cache1.dma_instance =
				GET_MASK(value, NV_PFIFO_CACHE1_DMA_INSTANCE_ADDRESS_MASK) << 4;
			break;
		case NV_PFIFO_CACHE1_DMA_PUT:
			d->user.channel_control[d->pfifo.cache1.channel_id].dma_put = value;
			break;
		case NV_PFIFO_CACHE1_DMA_GET:
			d->user.channel_control[d->pfifo.cache1.channel_id].dma_get = value;
			break;
		case NV_PFIFO_CACHE1_DMA_SUBROUTINE:
			d->pfifo.cache1.subroutine_return =
				(value & NV_PFIFO_CACHE1_DMA_SUBROUTINE_RETURN_OFFSET);
			d->pfifo.cache1.subroutine_active =
				(value & NV_PFIFO_CACHE1_DMA_SUBROUTINE_STATE);
			break;
		case NV_PFIFO_CACHE1_PULL0: {
			qemu_mutex_lock(&d->pfifo.cache1.cache_lock);

			if ((value & NV_PFIFO_CACHE1_PULL0_ACCESS)
				&& !d->pfifo.cache1.pull_enabled) {
				d->pfifo.cache1.pull_enabled = true;

				/* the puller thread should wake up */
				qemu_cond_signal(&d->pfifo.cache1.cache_cond);
			}
			else if (!(value & NV_PFIFO_CACHE1_PULL0_ACCESS)
				&& d->pfifo.cache1.pull_enabled) {
				d->pfifo.cache1.pull_enabled = false;
			}

			qemu_mutex_unlock(&d->pfifo.cache1.cache_lock);
			break;
		}
		case NV_PFIFO_CACHE1_ENGINE: {
			qemu_mutex_lock(&d->pfifo.cache1.cache_lock);

			for (i = 0; i < NV2A_NUM_SUBCHANNELS; i++) {
				d->pfifo.cache1.bound_engines[i] = (FIFOEngine)((value >> (i * 2)) & 3);
			}

			qemu_mutex_unlock(&d->pfifo.cache1.cache_lock);
			break;
		}
		case NV_PFIFO_CACHE1_DMA_DCOUNT:
			d->pfifo.cache1.dcount =
				(value & NV_PFIFO_CACHE1_DMA_DCOUNT_VALUE);
			break;
		case NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW:
			d->pfifo.cache1.get_jmp_shadow =
				(value & NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET);
			break;
		case NV_PFIFO_CACHE1_DMA_RSVD_SHADOW:
			d->pfifo.cache1.rsvd_shadow = value;
			break;
		case NV_PFIFO_CACHE1_DMA_DATA_SHADOW:
			d->pfifo.cache1.data_shadow = value;
			break;
		default:
			DEVICE_WRITE32_REG(pfifo); // Was : DEBUG_WRITE32_UNHANDLED(PFIFO);
			break;
	}

	DEVICE_WRITE32_END(PFIFO);
}


/* pusher should be fine to run from a mimo handler
* whenever's it's convenient */
static void pfifo_run_pusher(NV2AState *d) {
	uint8_t channel_id;
	ChannelControl *control;
	Cache1State *state;
	CacheEntry *command;
	uint8_t *dma;
	xbaddr dma_len;
	uint32_t word;

	/* TODO: How is cache1 selected? */
	state = &d->pfifo.cache1;
	channel_id = state->channel_id;
	control = &d->user.channel_control[channel_id];

	if (!state->push_enabled)
		return;


	/* only handling DMA for now... */

	/* Channel running DMA */
	uint32_t channel_modes = d->pfifo.regs[NV_PFIFO_MODE];
	assert(channel_modes & (1 << channel_id));
	assert(state->mode == FIFO_DMA);

	if (!state->dma_push_enabled)
		return;
	if (state->dma_push_suspended)
		return;

	/* We're running so there should be no pending errors... */
	assert(state->error == NV_PFIFO_CACHE1_DMA_STATE_ERROR_NONE);

	dma = (uint8_t*)nv_dma_map(d, state->dma_instance, &dma_len);

	NV2A_DPRINTF("DMA pusher: max 0x%08X, 0x%08X - 0x%08X\n",
		dma_len, control->dma_get, control->dma_put);

	/* based on the convenient pseudocode in envytools */
	while (control->dma_get != control->dma_put) {
		if (control->dma_get >= dma_len) {

			state->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_PROTECTION;
			break;
		}

		word = ldl_le_p((uint32_t*)(dma + control->dma_get));
		control->dma_get += 4;

		if (state->method_count) {
			/* data word of methods command */
			state->data_shadow = word;

			command = (CacheEntry*)g_malloc0(sizeof(CacheEntry));
			command->method = state->method;
			command->subchannel = state->subchannel;
			command->nonincreasing = state->method_nonincreasing;
			command->parameter = word;

			qemu_mutex_lock(&state->cache_lock);
			state->cache.push(command);
			qemu_cond_signal(&state->cache_cond);
			qemu_mutex_unlock(&state->cache_lock);

			if (!state->method_nonincreasing) {
				state->method += 4;
			}
			state->method_count--;
			state->dcount++;
		} else {
			/* no command active - this is the first word of a new one */
			state->rsvd_shadow = word;
			/* match all forms */
			if ((word & 0xe0000003) == 0x20000000) {
				/* old jump */
				state->get_jmp_shadow = control->dma_get;
				control->dma_get = word & 0x1fffffff;
				NV2A_DPRINTF("pb OLD_JMP 0x%08X\n", control->dma_get);
			} else if ((word & 3) == 1) {
				/* jump */
				state->get_jmp_shadow = control->dma_get;
				control->dma_get = word & 0xfffffffc;
				NV2A_DPRINTF("pb JMP 0x%08X\n", control->dma_get);
			} else if ((word & 3) == 2) {
				/* call */
				if (state->subroutine_active) {
					state->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_CALL;
					break;
				}
				state->subroutine_return = control->dma_get;
				state->subroutine_active = true;
				control->dma_get = word & 0xfffffffc;
				NV2A_DPRINTF("pb CALL 0x%08X\n", control->dma_get);
			} else if (word == 0x00020000) {
				/* return */
				if (!state->subroutine_active) {
					state->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_RETURN;
					break;
				}
				control->dma_get = state->subroutine_return;
				state->subroutine_active = false;
				NV2A_DPRINTF("pb RET 0x%08X\n", control->dma_get);
			} else if ((word & 0xe0030003) == 0) {
				/* increasing methods */
				state->method = word & 0x1fff;
				state->subchannel = (word >> 13) & 7;
				state->method_count = (word >> 18) & 0x7ff;
				state->method_nonincreasing = false;
				state->dcount = 0;
			} else if ((word & 0xe0030003) == 0x40000000) {
				/* non-increasing methods */
				state->method = word & 0x1fff;
				state->subchannel = (word >> 13) & 7;
				state->method_count = (word >> 18) & 0x7ff;
				state->method_nonincreasing = true;
				state->dcount = 0;
			} else {
				NV2A_DPRINTF("pb reserved cmd 0x%08X - 0x%08X\n",
					control->dma_get, word);
				state->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_RESERVED_CMD;
				break;
			}
		}
	}

	NV2A_DPRINTF("DMA pusher done: max 0x%08X, 0x%08X - 0x%08X\n",
		dma_len, control->dma_get, control->dma_put);

	if (state->error) {
		NV2A_DPRINTF("pb error: %d\n", state->error);
		assert(false);

		state->dma_push_suspended = true;

		d->pfifo.pending_interrupts |= NV_PFIFO_INTR_0_DMA_PUSHER;
		update_irq(d);
	}
}

int pfifo_puller_thread(NV2AState *d)
{
	CxbxSetThreadName("Cxbx NV2A FIFO");

	Cache1State *state = &(d->pfifo.cache1);

	glo_set_current(d->pgraph.gl_context);

	while (true) {
		qemu_mutex_lock(&state->cache_lock);

		while (state->cache.empty() || !state->pull_enabled) {
			qemu_cond_wait(&state->cache_cond, &state->cache_lock);

			if (d->exiting) {
				qemu_mutex_unlock(&state->cache_lock);
				glo_set_current(NULL);
				return 0;
			}
		}

		// Copy cache to working_cache
		while (!state->cache.empty()) {
			state->working_cache.push(state->cache.front());
			state->cache.pop();
		}

		qemu_mutex_unlock(&state->cache_lock);

		qemu_mutex_lock(&d->pgraph.lock);

		while (!state->working_cache.empty()) {
			CacheEntry* command = state->working_cache.front();
			state->working_cache.pop();

			if (command->method == 0) {
				// qemu_mutex_lock_iothread();
				RAMHTEntry entry = ramht_lookup(d, command->parameter);
				assert(entry.valid);

				assert(entry.channel_id == state->channel_id);
				// qemu_mutex_unlock_iothread();

				switch (entry.engine) {
				case ENGINE_GRAPHICS:
					pgraph_switch_context(d, entry.channel_id);
					pgraph_wait_fifo_access(d);
					pgraph_handle_method(d, command->subchannel, 0, entry.instance);
					break;
				default:
					assert(false);
					break;
				}

				/* the engine is bound to the subchannel */
				qemu_mutex_lock(&state->cache_lock);
				state->bound_engines[command->subchannel] = entry.engine;
				state->last_engine = entry.engine;
				qemu_mutex_unlock(&state->cache_lock);
			}
			else if (command->method >= 0x100) {
				/* method passed to engine */

				uint32_t parameter = command->parameter;

				/* methods that take objects.
				* TODO: Check this range is correct for the nv2a */
				if (command->method >= 0x180 && command->method < 0x200) {
					//qemu_mutex_lock_iothread();
					RAMHTEntry entry = ramht_lookup(d, parameter);
					assert(entry.valid);
					assert(entry.channel_id == state->channel_id);
					parameter = entry.instance;
					//qemu_mutex_unlock_iothread();
				}

				// state->cache_lock.lock();
				enum FIFOEngine engine = state->bound_engines[command->subchannel];
				// state->cache_lock.unlock();

				switch (engine) {
				case ENGINE_GRAPHICS:
					pgraph_wait_fifo_access(d);
					pgraph_handle_method(d, command->subchannel,
									command->method, parameter);
					break;
				default:
					// TODO: FIx this 
					// assert(false);
					break;
				}

				// state->cache_lock.lock();
				state->last_engine = state->bound_engines[command->subchannel];
				// state->cache_lock.unlock();
			}
			else
				NV2A_DPRINTF("FIFO: Unknown Method - 0x%08X\n",
						command->method);

			g_free(command);
		}

		qemu_mutex_unlock(&d->pgraph.lock);
	}

	return 0;
}

unsigned int ramht_size(NV2AState *d)
{
	return 
		1 << (GET_MASK(d->pfifo.regs[NV_PFIFO_RAMHT], NV_PFIFO_RAMHT_SIZE_MASK) + 12);
}

static uint32_t ramht_hash(NV2AState *d, uint32_t handle)
{
	/* XXX: Think this is different to what nouveau calculates... */
	unsigned int bits = ffs(ramht_size(d)) - 2;

	uint32_t hash = 0;
	while (handle) {
		hash ^= (handle & ((1 << bits) - 1));
		handle >>= bits;
	}
	hash ^= d->pfifo.cache1.channel_id << (bits - 4);

	return hash;
}

static RAMHTEntry ramht_lookup(NV2AState *d, uint32_t handle)
{
	uint32_t hash = ramht_hash(d, handle);
	assert(hash * 8 < ramht_size(d));

	uint32_t ramht_address =
		GET_MASK(d->pfifo.regs[NV_PFIFO_RAMHT],
			NV_PFIFO_RAMHT_BASE_ADDRESS_MASK) << 12;

	uint8_t *entry_ptr = d->pramin.ramin_ptr + ramht_address + hash * 8;

	uint32_t entry_handle = ldl_le_p((uint32_t*)entry_ptr);
	uint32_t entry_context = ldl_le_p((uint32_t*)(entry_ptr + 4));

	RAMHTEntry entry;
	entry.handle = entry_handle;
	entry.instance = (entry_context & NV_RAMHT_INSTANCE) << 4;
	entry.engine = (FIFOEngine)((entry_context & NV_RAMHT_ENGINE) >> 16);
	entry.channel_id = (entry_context & NV_RAMHT_CHID) >> 24;
	entry.valid = entry_context & NV_RAMHT_STATUS;

	return entry;
}
