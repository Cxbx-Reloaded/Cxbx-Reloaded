// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
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
// *  This file is heavily based on code from XQEMU
// *  https://github.com/xqemu/xqemu/blob/master/hw/xbox/nv2a/nv2a_pfifo.c
// *  Copyright (c) 2012 espes
// *  Copyright (c) 2015 Jannik Vogel
// *  Copyright (c) 2018 Matt Borgerson
// *
// *  Contributions for Cxbx-Reloaded
// *  Copyright (c) 2017-2018 Luke Usher <luke.usher@outlook.com>
// *  Copyright (c) 2018 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

typedef struct RAMHTEntry {
	uint32_t handle;
	xbox::addr_xt instance;
	enum FIFOEngine engine;
	unsigned int channel_id : 5;
	bool valid;
} RAMHTEntry;

static RAMHTEntry ramht_lookup(NV2AState *d, uint32_t handle); // forward declaration

/* PFIFO - MMIO and DMA FIFO submission to PGRAPH and VPE */
DEVICE_READ32(PFIFO)
{
    qemu_mutex_lock(&d->pfifo.pfifo_lock);

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
	default:
		DEVICE_READ32_REG(pfifo); // Was : DEBUG_READ32_UNHANDLED(PFIFO);
		break;
	}

    qemu_mutex_unlock(&d->pfifo.pfifo_lock);

	DEVICE_READ32_END(PFIFO);
}

DEVICE_WRITE32(PFIFO)
{
    qemu_mutex_lock(&d->pfifo.pfifo_lock);

	switch(addr) {
		case NV_PFIFO_INTR_0:
			d->pfifo.pending_interrupts &= ~value;
			update_irq(d);
			break;
		case NV_PFIFO_INTR_EN_0:
			d->pfifo.enabled_interrupts = value;
			update_irq(d);
			break;
		default:
			DEVICE_WRITE32_REG(pfifo); // Was : DEBUG_WRITE32_UNHANDLED(PFIFO);
			break;
	}

    qemu_cond_broadcast(&d->pfifo.pusher_cond);
    //qemu_cond_broadcast(&d->pfifo.puller_cond);
	qemu_mutex_unlock(&d->pfifo.pfifo_lock);
	DEVICE_WRITE32_END(PFIFO);
}
static int pfifo_run_puller(NV2AState *d, uint32_t arg_subchannel ,uint32_t arg_method,
    uint32_t arg_parameter, uint32_t *arg_parameters,
    size_t arg_method_count,
    size_t arg_max_lookahead_words)
//static void pfifo_run_puller(NV2AState *d)
{
    int num_processed = 0;
    uint32_t *pull0 = &d->pfifo.regs[NV_PFIFO_CACHE1_PULL0/4];
    uint32_t *pull1 = &d->pfifo.regs[NV_PFIFO_CACHE1_PULL1/4];
    uint32_t *engine_reg = &d->pfifo.regs[NV_PFIFO_CACHE1_ENGINE/4];

    uint32_t *status = &d->pfifo.regs[NV_PFIFO_CACHE1_STATUS/4];
    uint32_t *get_reg = &d->pfifo.regs[NV_PFIFO_CACHE1_GET/4];
    uint32_t *put_reg = &d->pfifo.regs[NV_PFIFO_CACHE1_PUT/4];

    // TODO
    // CacheEntry working_cache[NV2A_CACHE1_SIZE];
    // int working_cache_size = 0;
    // pull everything into our own queue

    // TODO think more about locking

    while (true) {
        if (!GET_MASK(*pull0, NV_PFIFO_CACHE1_PULL0_ACCESS)) return num_processed;

        /* empty cache1 */
        if (*status & NV_PFIFO_CACHE1_STATUS_LOW_MARK) break;

        uint32_t get = *get_reg;
        uint32_t put = *put_reg;

        assert(get < 128*4 && (get % 4) == 0);
        uint32_t method_entry = d->pfifo.regs[NV_PFIFO_CACHE1_METHOD/4 + get/4];// /4?
        uint32_t parameter = d->pfifo.regs[NV_PFIFO_CACHE1_DATA/4 + get/4];// /4?

        uint32_t *parameters=&(d->pfifo.regs[NV_PFIFO_CACHE1_DATA/4 + get/4]);// /4?

        uint32_t new_get = (get+4) & 0x1fc;
        *get_reg = new_get;

        if (new_get >= put) {
            // set low mark
            *status |= NV_PFIFO_CACHE1_STATUS_LOW_MARK;
			//sleep for a while
			Sleep(1);
        }

        if (*status & NV_PFIFO_CACHE1_STATUS_HIGH_MARK) {
            // unset high mark
            *status &= ~NV_PFIFO_CACHE1_STATUS_HIGH_MARK;
            // signal pusher
            qemu_cond_signal(&d->pfifo.pusher_cond);            
        }

        uint32_t subchannel = GET_MASK(method_entry, NV_PFIFO_CACHE1_METHOD_SUBCHANNEL); 
        uint32_t method = method_entry & NV_PFIFO_CACHE1_METHOD_ADDRESS; // (Same as NV_PFIFO_CACHE1_DMA_STATE_METHOD) Masking is more efficient than GET_MASK(method_entry, NV_PFIFO_CACHE1_DMA_STATE_METHOD) << 2; //code from run_pusher compare method and method_new
        uint32_t method_count =	GET_MASK(method_entry, NV_PFIFO_CACHE1_DMA_STATE_METHOD_COUNT);//code from run_pusher

        // NV2A_DPRINTF("pull %d 0x%08X 0x%08X - subch %d\n", get/4, method_entry, parameter, subchannel);

        if (method == 0) {
            RAMHTEntry entry = ramht_lookup(d, parameter);
            assert(entry.valid);

            // assert(entry.channel_id == state->channel_id);

            assert(entry.engine == ENGINE_GRAPHICS);


            /* the engine is bound to the subchannel */
            assert(subchannel < 8);
            SET_MASK(*engine_reg, 3 << (4*subchannel), entry.engine);
            SET_MASK(*pull1, NV_PFIFO_CACHE1_PULL1_ENGINE, entry.engine);
            // NV2A_DPRINTF("engine_reg1 %d 0x%08X\n", subchannel, *engine_reg);


            // TODO: this is fucked
            qemu_mutex_lock(&d->pgraph.pgraph_lock);
            //make pgraph busy
            qemu_mutex_unlock(&d->pfifo.pfifo_lock);

            pgraph_switch_context(d, entry.channel_id);
            pgraph_wait_fifo_access(d);
            //pgraph_handle_method(
            //	d,						//NV2AState
            //	subc,					//Subchannel
            //	mthd,					//command word
            //	word,					//first parameter
            //	dma_get,				//parameters, pointer to 1st parameter, which is exact parameter in the args.
            //	mcnt,					//method count
            //	max_words_available);   //words still available in the pushbuffer

            num_processed = pgraph_handle_method(
                d,						//NV2AState
                subchannel,				//NV2AState
                0,						//command word
                entry.instance,			//first parameter
                &entry.instance,		//parameters, pointer to 1st parameter, which is exact parameter in the args.
                MIN(method_count, 1),						//method count
                MAX(method_count, 1));						//words still available in the pushbuffer
            //LOG_TEST_CASE("Pushbuffer method handling from EmuNV2a_PFIFO.CPP::pfifo_run_pusher()!!");
            // make pgraph not busy
            qemu_mutex_unlock(&d->pgraph.pgraph_lock);
            qemu_mutex_lock(&d->pfifo.pfifo_lock);

        } else if (method >= 0x100) {
            // method passed to engine

            /* methods that take objects.
             * TODO: Check this range is correct for the nv2a */
            if (method >= 0x180 && method < 0x200) {
                //qemu_mutex_lock_iothread();
                RAMHTEntry entry = ramht_lookup(d, parameter);
                assert(entry.valid);
                // assert(entry.channel_id == state->channel_id);
                parameter = entry.instance;
                //qemu_mutex_unlock_iothread();
            }

            enum FIFOEngine engine = (enum FIFOEngine)GET_MASK(*engine_reg, 3 << (4*subchannel));
            // NV2A_DPRINTF("engine_reg2 %d 0x%08X\n", subchannel, *engine_reg);
            assert(engine == ENGINE_GRAPHICS);
            SET_MASK(*pull1, NV_PFIFO_CACHE1_PULL1_ENGINE, engine);

            // TODO: this is fucked
            qemu_mutex_lock(&d->pgraph.pgraph_lock);
            //make pgraph busy
            qemu_mutex_unlock(&d->pfifo.pfifo_lock);

            pgraph_wait_fifo_access(d);
            
            //pgraph_handle_method(
            //	d,						//NV2AState
            //	subc,					//Subchannel
            //	mthd,					//command word
            //	word,					//first parameter
            //	dma_get,				//parameters, pointer to 1st parameter, which is exact parameter in the args.
            //	mcnt,					//method count
            //	max_words_available);   //words still available in the pushbuffer

            num_processed = pgraph_handle_method(
                d,
                subchannel,
                method,
                parameter,
                parameters,
                MIN(method_count,1),
                MAX(method_count,1));
            //LOG_TEST_CASE("Pushbuffer method handling from EmuNV2a_PFIFO.CPP::pfifo_run_pusher()!!");

            // make pgraph not busy
            qemu_mutex_unlock(&d->pgraph.pgraph_lock);
            qemu_mutex_lock(&d->pfifo.pfifo_lock);
        } else {
            assert(false);
        }

    }
    return num_processed;
}

int pfifo_puller_thread(NV2AState *d)
{
    //hacked code. 
    return NULL;

    uint32_t subchannel=0;
    uint32_t method=0;
    uint32_t parameter=0;
    uint32_t *parameters=0;
    size_t method_count=0;
    size_t max_lookahead_words=0;
    int num_word_processed = 0;

    g_AffinityPolicy->SetAffinityOther();
    CxbxSetThreadName("Cxbx NV2A FIFO puller");

    glo_set_current(d->pgraph.gl_context);

    qemu_mutex_lock(&d->pfifo.pfifo_lock);
    while (true) {
        //pfifo_run_puller(d);
        //shall remove this call and pfifo_puller_thread() together, let run_pusher() call run_puller().
        num_word_processed=pfifo_run_puller(
            d,
            subchannel,
            method,
            parameter,
            parameters,
            method_count,
            max_lookahead_words);
		//no need to wait here. just keet polling pfifo_run_puller()
        //qemu_cond_wait(&d->pfifo.puller_cond, &d->pfifo.pfifo_lock);

        if (d->exiting) {
            break;
        }
    }
    //qemu_mutex_unlock(&d->pfifo.pfifo_lock);

    glo_set_current(NULL); // Cxbx addition

    return NULL;
}


/* If NV097_FLIP_STALL was executed, check if the flip has completed.
 * This will usually happen in the VSYNC interrupt handler.
 */
static bool pgraph_is_flip_stall_complete(NV2AState *d)
{
    PGRAPHState *pg = &d->pgraph;

    NV2A_DPRINTF("flip stall read: %d, write: %d, modulo: %d\n",
		pg->KelvinPrimitive.SetFlipRead,
		pg->KelvinPrimitive.SetFlipWrite,
		pg->KelvinPrimitive.SetFlipModulo);

    //uint32_t s = pg->regs[NV_PGRAPH_SURFACE];
    if (pg->KelvinPrimitive.SetFlipRead
        != pg->KelvinPrimitive.SetFlipWrite) {
        return true;
    }

    return false;
}

/**/
extern void EmuExecutePushBufferRaw
(
	void *pPushData,
	uint32_t uSizeInBytes,
	uint32_t **p_dma_get,
	uint32_t **p_dma_put,
	uint8_t *dma
);

static void pfifo_run_pusher(NV2AState *d)
{
    //for dma access to pull pushbuffer data.
    uint32_t *p_push0 = &d->pfifo.regs[NV_PFIFO_CACHE1_PUSH0/4];
    uint32_t *p_push1 = &d->pfifo.regs[NV_PFIFO_CACHE1_PUSH1/4];
    uint32_t *p_dma_subroutine = &d->pfifo.regs[NV_PFIFO_CACHE1_DMA_SUBROUTINE/4];
    uint32_t *p_dma_state = &d->pfifo.regs[NV_PFIFO_CACHE1_DMA_STATE/4];
    uint32_t *p_dma_push = &d->pfifo.regs[NV_PFIFO_CACHE1_DMA_PUSH/4];
    uint32_t **p_dma_get = (uint32_t **)&d->pfifo.regs[NV_PFIFO_CACHE1_DMA_GET/4];
    uint32_t **p_dma_put = (uint32_t **)&d->pfifo.regs[NV_PFIFO_CACHE1_DMA_PUT/4];
    uint32_t *p_dma_dcount = &d->pfifo.regs[NV_PFIFO_CACHE1_DMA_DCOUNT/4];
    //for the PFIFO cache access to push data to puller.
    uint32_t *p_status = &d->pfifo.regs[NV_PFIFO_CACHE1_STATUS/4];
    uint32_t *p_get_reg = &d->pfifo.regs[NV_PFIFO_CACHE1_GET/4];
    uint32_t *p_put_reg = &d->pfifo.regs[NV_PFIFO_CACHE1_PUT/4];

    if (!GET_MASK(*p_push0, NV_PFIFO_CACHE1_PUSH0_ACCESS)) return;
    if (!GET_MASK(*p_dma_push, NV_PFIFO_CACHE1_DMA_PUSH_ACCESS)) return;

    /* suspended */
    if (GET_MASK(*p_dma_push, NV_PFIFO_CACHE1_DMA_PUSH_STATUS)) return;

    // TODO: should we become busy here??
    // NV_PFIFO_CACHE1_DMA_PUSH_STATE _BUSY

    unsigned int channel_id = GET_MASK(*p_push1,
                                       NV_PFIFO_CACHE1_PUSH1_CHID);


    /* Channel running DMA */
    uint32_t channel_modes = d->pfifo.regs[NV_PFIFO_MODE/4];
    assert(channel_modes & (1 << channel_id));

    assert(GET_MASK(*p_push1, NV_PFIFO_CACHE1_PUSH1_MODE)
            == NV_PFIFO_CACHE1_PUSH1_MODE_DMA);

    /* We're running so there should be no pending errors... */
    assert(GET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_ERROR)
            == NV_PFIFO_CACHE1_DMA_STATE_ERROR_NONE);

    hwaddr dma_instance =
        GET_MASK(d->pfifo.regs[NV_PFIFO_CACHE1_DMA_INSTANCE/4],
                 NV_PFIFO_CACHE1_DMA_INSTANCE_ADDRESS_MASK) << 4; // TODO : Use NV_PFIFO_CACHE1_DMA_INSTANCE_ADDRESS_MOVE?

    hwaddr dma_len;
    uint8_t *dma = (uint8_t*)nv_dma_map(d, dma_instance, &dma_len);

	//the dma_get and dma_put here are offset from start of dma context, the segment base is dma, which is supposed to be 0x80000000.
	uint32_t *dma_get = *p_dma_get;
	uint32_t *dma_put = *p_dma_put;
//	if (dma_get == nullptr || dma_put==nullptr) {//zero offset is valid, these are offset to the dma base.
//		NV2A_DPRINTF("PFIFO run_pusher dma_get/put nullptr! get 0x%08X, put 0x%08X\n", dma_get,dma_put);
//		return;
//	}
	if (dma_get >= dma_put) {
		//no data available pushbuffer yet, or xbox d3d hasn't kickoff pushbuffer yet. shall we add sleep() or wait() here?
		NV2A_DPRINTF("PFIFO run_pusher dma_get==dma_put ! get 0x%08X, put 0x%08X\n", dma_get, dma_put);
		//
		//dma_get = dma_put;
		//dma starvin, shall we sleep for a while?
		return;
	}

	if (dma_get >= (uint32_t *)dma_len) {
		//this check is probably redundant. 
		NV2A_DPRINTF("PFIFO run_pusher dma_get>=dma_len ! get 0x%08X, put 0x%08X\n", dma_get, dma_len);
		//assert(false);
		//SET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_ERROR,
			//NV_PFIFO_CACHE1_DMA_STATE_ERROR_PROTECTION);
		dma_get = dma_put;
		return;
	}

	EmuExecutePushBufferRaw
    (
        (void  *)((uint32_t)*p_dma_get+ (uint32_t)dma),	//use dma starting adder as pushbuffer starting address
        (uint32_t)*p_dma_put- (uint32_t)*p_dma_get,		//use dma length as pushbuffer size.
        p_dma_get,										//pass in pointer to dma_get, be awared dma_get is offset value to the dma base (0x80000000).
        p_dma_put,										//pass in pointer to dma_put, could be useless. be awared dma_put is offset value to the dma base (0x80000000).
		dma												//pass in dma base address (0x80000000),
	);

    /* based on the convenient pseudocode in envytools */
    //don't need these logic below anymore, the same logic is inside ExecutePushbufferRaw(). we'll handle the same logic in one place. it's easier to focus there.
  #if(0)
    while (true) {
        uint32_t dma_get = *p_dma_get;
        uint32_t dma_put = *p_dma_put;
        if (dma_get == dma_put) break;
        if (dma_get >= dma_len) {
            assert(false);
            SET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_ERROR,
                     NV_PFIFO_CACHE1_DMA_STATE_ERROR_PROTECTION);
            break;
        }
        uint32_t * word_ptr = (uint32_t*)(dma + dma_get);    //parameters
        uint32_t word = ldl_le_p((uint32_t*)(dma + dma_get));//parameter
        dma_get += 4;

        uint32_t method_type =
            GET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE);
        uint32_t method_subchannel =
            GET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_SUBCHANNEL);
        uint32_t method =
            GET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_METHOD) << 2;
        uint32_t method_count =
            GET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_METHOD_COUNT);

        uint32_t subroutine_state =
            GET_MASK(*p_dma_subroutine, NV_PFIFO_CACHE1_DMA_SUBROUTINE_STATE);

        if (method_count) {
            /* full */
            if (*p_status & NV_PFIFO_CACHE1_STATUS_HIGH_MARK) return;


            /* data word of methods command */
            d->pfifo.regs[NV_PFIFO_CACHE1_DMA_DATA_SHADOW] = word;

            uint32_t put = *p_put_reg;
            uint32_t get = *p_get_reg;

            assert((method & 3) == 0);
            uint32_t method_entry = 0;
            SET_MASK(method_entry, NV_PFIFO_CACHE1_METHOD_ADDRESS, method >> 2);
            SET_MASK(method_entry, NV_PFIFO_CACHE1_METHOD_TYPE, method_type);
            SET_MASK(method_entry, NV_PFIFO_CACHE1_METHOD_SUBCHANNEL, method_subchannel);

            // NV2A_DPRINTF("push %d 0x%08X 0x%08X - subch %d\n", put/4, method_entry, word, method_subchannel);

            assert(put < 128*4 && (put%4) == 0);
            d->pfifo.regs[NV_PFIFO_CACHE1_METHOD + put*2] = method_entry;
            d->pfifo.regs[NV_PFIFO_CACHE1_DATA + put*2] = word;

            uint32_t new_put = (put+4) & 0x1fc;
            *p_put_reg = new_put;
            if (new_put == get) {
                // set high mark
                *p_status |= NV_PFIFO_CACHE1_STATUS_HIGH_MARK;
            }
            if (*p_status & NV_PFIFO_CACHE1_STATUS_LOW_MARK) {
                // unset low mark
                *p_status &= ~NV_PFIFO_CACHE1_STATUS_LOW_MARK;
                // signal puller
                qemu_cond_signal(&d->pfifo.puller_cond);
            }

            if (method_type == NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE_INC) {
                SET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_METHOD,
                         (method + 4) >> 2);
            }
            SET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_METHOD_COUNT,
                     method_count - 1);

            (*p_dma_dcount)++;
        } else {
            /* no command active - this is the first word of a new one */
            d->pfifo.regs[NV_PFIFO_CACHE1_DMA_RSVD_SHADOW] = word;

            /* match all forms */
            if ((word & 0xe0000003) == 0x20000000) {
                /* old jump */
                d->pfifo.regs[NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW] =
                    dma_get;
                dma_get = word & 0x1fffffff;
                NV2A_DPRINTF("pb OLD_JMP 0x%08X\n", dma_get);
            } else if ((word & 3) == 1) {
                /* jump */
                d->pfifo.regs[NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW] =
                    dma_get;
                dma_get = word & 0xfffffffc;
                NV2A_DPRINTF("pb JMP 0x%08X\n", dma_get);
            } else if ((word & 3) == 2) {
                /* call */
                if (subroutine_state) {
                    SET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_ERROR,
                             NV_PFIFO_CACHE1_DMA_STATE_ERROR_CALL);
                    break;
                } else {
                    *p_dma_subroutine = dma_get;
                    SET_MASK(*p_dma_subroutine,
                             NV_PFIFO_CACHE1_DMA_SUBROUTINE_STATE, 1);
                    dma_get = word & 0xfffffffc;
                    NV2A_DPRINTF("pb CALL 0x%08X\n", dma_get);
                }
            } else if (word == 0x00020000) {
                /* return */
                if (!subroutine_state) {
                    SET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_ERROR,
                             NV_PFIFO_CACHE1_DMA_STATE_ERROR_RETURN);
                    // break;
                } else {
                    dma_get = *p_dma_subroutine & 0xfffffffc;
                    SET_MASK(*p_dma_subroutine,
                             NV_PFIFO_CACHE1_DMA_SUBROUTINE_STATE, 0);
                    NV2A_DPRINTF("pb RET 0x%08X\n", dma_get);
                }
            } else if ((word & 0xe0030003) == 0) {
                /* increasing methods */
                SET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_METHOD,
                         (word & 0x1fff) >> 2 );
                SET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_SUBCHANNEL,
                         (word >> 13) & 7);
                SET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_METHOD_COUNT,
                         (word >> 18) & 0x7ff);
                SET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE,
                         NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE_INC);
                *p_dma_dcount = 0;
            } else if ((word & 0xe0030003) == 0x40000000) {
                /* non-increasing methods */
                SET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_METHOD,
                         (word & 0x1fff) >> 2 );
                SET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_SUBCHANNEL,
                         (word >> 13) & 7);
                SET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_METHOD_COUNT,
                         (word >> 18) & 0x7ff);
                SET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE,
                         NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE_NON_INC);
                *p_dma_dcount = 0;
            } else {
                NV2A_DPRINTF("pb reserved cmd 0x%08X - 0x%08X\n",
                             dma_get, word);
                SET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_ERROR,
                         NV_PFIFO_CACHE1_DMA_STATE_ERROR_RESERVED_CMD);
                // break;
                assert(false);
            }
        }

        *p_dma_get = dma_get;

        if (GET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_ERROR)) {
            break;
        }
    }

    // NV2A_DPRINTF("DMA pusher done: max 0x%08X, 0x%08X - 0x%08X\n",
    //      dma_len, control->dma_get, control->dma_put);

    uint32_t error = GET_MASK(*p_dma_state, NV_PFIFO_CACHE1_DMA_STATE_ERROR);
    if (error) {
        NV2A_DPRINTF("pb error: %d\n", error);
        assert(false);

        SET_MASK(*p_dma_push, NV_PFIFO_CACHE1_DMA_PUSH_STATUS, 1); /* suspended */

        // d->pfifo.pending_interrupts |= NV_PFIFO_INTR_0_DMA_PUSHER;
        // update_irq(d);
    }
  #endif
}

int pfifo_pusher_thread(NV2AState *d) // thread fire up at nv2a.cpp::void NV2ADevice::Init() Line#122
{
    g_AffinityPolicy->SetAffinityOther();
    CxbxSetThreadName("Cxbx NV2A FIFO pusher");

    qemu_mutex_lock(&d->pfifo.pfifo_lock);
    while (true) {
        pfifo_run_pusher(d);
        qemu_cond_wait(&d->pfifo.pusher_cond, &d->pfifo.pfifo_lock);

        if (d->exiting) {
            break;
        }
    }
    qemu_mutex_unlock(&d->pfifo.pfifo_lock);

    return 0;
}

unsigned int ramht_size(NV2AState *d)
{
	return 
		1 << (GET_MASK(d->pfifo.regs[NV_PFIFO_RAMHT/4], NV_PFIFO_RAMHT_SIZE_MASK) + 12);
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

    unsigned int channel_id = GET_MASK(d->pfifo.regs[NV_PFIFO_CACHE1_PUSH1/4],
                                       NV_PFIFO_CACHE1_PUSH1_CHID);
    hash ^= channel_id << (bits - 4);

	return hash;
}

static RAMHTEntry ramht_lookup(NV2AState *d, uint32_t handle)
{
	uint32_t hash = ramht_hash(d, handle);
	assert(hash * 8 < ramht_size(d));

	xbox::addr_xt ramht_address =
		GET_MASK(d->pfifo.regs[NV_PFIFO_RAMHT/4],
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
