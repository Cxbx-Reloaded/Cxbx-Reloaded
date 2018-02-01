static void pgraph_context_switch(NV2AState *d, unsigned int channel_id);
static void pgraph_set_context_user(NV2AState *d, uint32_t val);
//static void pgraph_wait_fifo_access(NV2AState *d);
static void pgraph_method(NV2AState *d, unsigned int subchannel, unsigned int graphics_class, unsigned int method, uint32_t parameter);
//static void pgraph_allocate_inline_buffer_vertices(PGRAPHState *pg, unsigned int attr);
//static void pgraph_finish_inline_buffer_vertex(PGRAPHState *pg);
//static void pgraph_shader_update_constants(PGRAPHState *pg, ShaderBinding *binding, bool binding_changed, bool vertex_program, bool fixed_function);
//static void pgraph_bind_shaders(PGRAPHState *pg);
//static bool pgraph_framebuffer_dirty(PGRAPHState *pg);
static bool pgraph_color_write_enabled(PGRAPHState *pg);
static bool pgraph_zeta_write_enabled(PGRAPHState *pg);
//static void pgraph_set_surface_dirty(PGRAPHState *pg, bool color, bool zeta);
//static void pgraph_update_surface_part(NV2AState *d, bool upload, bool color);
static void pgraph_update_surface(NV2AState *d, bool upload, bool color_write, bool zeta_write);
//static void pgraph_bind_textures(NV2AState *d);
//static void pgraph_apply_anti_aliasing_factor(PGRAPHState *pg, unsigned int *width, unsigned int *height);
//static void pgraph_get_surface_dimensions(PGRAPHState *pg, unsigned int *width, unsigned int *height);
//static void pgraph_update_memory_buffer(NV2AState *d, hwaddr addr, hwaddr size, bool f);
//static void pgraph_bind_vertex_attributes(NV2AState *d, unsigned int num_elements, bool inline_data, unsigned int inline_stride);
//static unsigned int pgraph_bind_inline_array(NV2AState *d);
static void load_graphics_object(NV2AState *d, hwaddr instance_address, GraphicsObject *obj);
static GraphicsObject* lookup_graphics_object(PGRAPHState *s, hwaddr instance_address);
//static float convert_f16_to_float(uint16_t f16);
//static float convert_f24_to_float(uint32_t f24);
//static uint8_t cliptobyte(int x);
//static void convert_yuy2_to_rgb(const uint8_t *line, unsigned int ix, uint8_t *r, uint8_t *g, uint8_t* b);
//static uint8_t* convert_texture_data(const TextureShape s, const uint8_t *data, const uint8_t *palette_data, unsigned int width, unsigned int height, unsigned int depth, unsigned int row_pitch, unsigned int slice_pitch);
//static void upload_gl_texture(GLenum gl_target, const TextureShape s, const uint8_t *texture_data, const uint8_t *palette_data);
//static TextureBinding* generate_texture(const TextureShape s, const uint8_t *texture_data, const uint8_t *palette_data);
//static guint texture_key_hash(gconstpointer key);
//static gboolean texture_key_equal(gconstpointer a, gconstpointer b);
//static gpointer texture_key_retrieve(gpointer key, gpointer user_data, GError **error);
//static void texture_key_destroy(gpointer data);
//static void texture_binding_destroy(gpointer data);
//static guint shader_hash(gconstpointer key);
//static gboolean shader_equal(gconstpointer a, gconstpointer b);
static unsigned int kelvin_map_stencil_op(uint32_t parameter);
static unsigned int kelvin_map_polygon_mode(uint32_t parameter);
static unsigned int kelvin_map_texgen(uint32_t parameter, unsigned int channel);
static void pgraph_method_log(unsigned int subchannel, unsigned int graphics_class, unsigned int method, uint32_t parameter);
//static uint64_t fnv_hash(const uint8_t *data, size_t len);
//static uint64_t fast_hash(const uint8_t *data, size_t len, unsigned int samples);

DEVICE_READ32(PGRAPH)
{
	std::lock_guard<std::mutex> lk(d->pgraph.mutex);

	DEVICE_READ32_SWITCH() {
	case NV_PGRAPH_INTR:
		result = d->pgraph.pending_interrupts;
		break;
	case NV_PGRAPH_INTR_EN:
		result = d->pgraph.enabled_interrupts;
		break;
	case NV_PGRAPH_NSOURCE:
		result = d->pgraph.notify_source;
		break;
	case NV_PGRAPH_CTX_USER:
		SET_MASK(result, NV_PGRAPH_CTX_USER_CHANNEL_3D, d->pgraph.context[d->pgraph.channel_id].channel_3d);
		SET_MASK(result, NV_PGRAPH_CTX_USER_CHANNEL_3D_VALID, 1);
		SET_MASK(result, NV_PGRAPH_CTX_USER_SUBCH, d->pgraph.context[d->pgraph.channel_id].subchannel << 13);
		SET_MASK(result, NV_PGRAPH_CTX_USER_CHID, d->pgraph.channel_id);
		break;
	case NV_PGRAPH_TRAPPED_ADDR:
		SET_MASK(result, NV_PGRAPH_TRAPPED_ADDR_CHID, d->pgraph.trapped_channel_id);
		SET_MASK(result, NV_PGRAPH_TRAPPED_ADDR_SUBCH, d->pgraph.trapped_subchannel);
		SET_MASK(result, NV_PGRAPH_TRAPPED_ADDR_MTHD, d->pgraph.trapped_method);
		break;
	case NV_PGRAPH_TRAPPED_DATA_LOW:
		result = d->pgraph.trapped_data[0];
		break;
	case NV_PGRAPH_FIFO:
		SET_MASK(result, NV_PGRAPH_FIFO_ACCESS, d->pgraph.fifo_access);
		break;
	case NV_PGRAPH_CHANNEL_CTX_TABLE:
		result = d->pgraph.context_table >> 4;
		break;
	case NV_PGRAPH_CHANNEL_CTX_POINTER:
		result = d->pgraph.context_address >> 4;
		break;
	default:
		DEVICE_READ32_REG(pgraph); // Was : DEBUG_READ32_UNHANDLED(PGRAPH);
	}

	DEVICE_READ32_END(PGRAPH);
}

static void pgraph_set_context_user(NV2AState *d, uint32_t value)
{
	d->pgraph.channel_id = (value & NV_PGRAPH_CTX_USER_CHID) >> 24;
	d->pgraph.context[d->pgraph.channel_id].channel_3d = GET_MASK(value, NV_PGRAPH_CTX_USER_CHANNEL_3D);
	d->pgraph.context[d->pgraph.channel_id].subchannel = GET_MASK(value, NV_PGRAPH_CTX_USER_SUBCH);
}

DEVICE_WRITE32(PGRAPH)
{
	std::lock_guard<std::mutex> lk(d->pgraph.mutex);

	switch (addr) {
	case NV_PGRAPH_INTR:
		d->pgraph.pending_interrupts &= ~value;
		d->pgraph.interrupt_cond.notify_all();
		break;
	case NV_PGRAPH_INTR_EN:
		d->pgraph.enabled_interrupts = value;
		break;
	case NV_PGRAPH_CTX_CONTROL:
		d->pgraph.channel_valid = (value & NV_PGRAPH_CTX_CONTROL_CHID);
		break;
	case NV_PGRAPH_CTX_USER:
		pgraph_set_context_user(d, value);
		break;
	case NV_PGRAPH_INCREMENT:
		if (value & NV_PGRAPH_INCREMENT_READ_3D) {
			SET_MASK(d->pgraph.regs[NV_PGRAPH_SURFACE],
				NV_PGRAPH_SURFACE_READ_3D,
				(GET_MASK(d->pgraph.regs[NV_PGRAPH_SURFACE],
					NV_PGRAPH_SURFACE_READ_3D) + 1)
				% GET_MASK(d->pgraph.regs[NV_PGRAPH_SURFACE],
					NV_PGRAPH_SURFACE_MODULO_3D));

			d->pgraph.flip_3d.notify_all();
		}
		break;
	case NV_PGRAPH_FIFO:
		d->pgraph.fifo_access = GET_MASK(value, NV_PGRAPH_FIFO_ACCESS);
		d->pgraph.fifo_access_cond.notify_all();
		break;
	case NV_PGRAPH_CHANNEL_CTX_TABLE:
		d->pgraph.context_table = (value & NV_PGRAPH_CHANNEL_CTX_TABLE_INST) << 4;
		break;
	case NV_PGRAPH_CHANNEL_CTX_POINTER:
		d->pgraph.context_address =
			(value & NV_PGRAPH_CHANNEL_CTX_POINTER_INST) << 4;
		break;
	case NV_PGRAPH_CHANNEL_CTX_TRIGGER:
		if (value & NV_PGRAPH_CHANNEL_CTX_TRIGGER_READ_IN) {
			printf("PGRAPH: read channel %d context from %0x08X\n",
				d->pgraph.channel_id, d->pgraph.context_address);

			uint8_t *context_ptr = d->pramin.ramin_ptr + d->pgraph.context_address;
			uint32_t context_user = ldl_le_p((uint32_t*)context_ptr);

			printf("    - CTX_USER = 0x%x\n", context_user);


			pgraph_set_context_user(d, context_user);
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

static void pgraph_method(NV2AState *d, unsigned int subchannel, unsigned int method, uint32_t parameter)
{
	std::lock_guard<std::mutex> lk(d->pgraph.mutex);

//	int i;
	GraphicsSubchannel *subchannel_data;
	GraphicsObject *object;

	unsigned int slot;

    PGRAPHState *pg = &d->pgraph;

	assert(pg->channel_valid);
	subchannel_data = &pg->subchannel_data[subchannel];
	object = &subchannel_data->object;

	ContextSurfaces2DState *context_surfaces_2d = &object->data.context_surfaces_2d;
	ImageBlitState *image_blit = &object->data.image_blit;
	KelvinState *kelvin = &object->data.kelvin;

	pgraph_method_log(subchannel, object->graphics_class, method, parameter);

	if (method == NV_SET_OBJECT) {
		subchannel_data->object_instance = parameter;

		//qemu_mutex_lock_iothread();
		load_graphics_object(d, parameter, object);
		//qemu_mutex_unlock_iothread();
		return;
	}

	switch (object->graphics_class) {
	case NV_CONTEXT_SURFACES_2D: {
		switch (method) {
		case NV062_SET_CONTEXT_DMA_IMAGE_SOURCE:
			context_surfaces_2d->dma_image_source = parameter;
			break;
		case NV062_SET_CONTEXT_DMA_IMAGE_DESTIN:
			context_surfaces_2d->dma_image_dest = parameter;
			break;
		case NV062_SET_COLOR_FORMAT:
			context_surfaces_2d->color_format = parameter;
			break;
		case NV062_SET_PITCH:
			context_surfaces_2d->source_pitch = parameter & 0xFFFF;
			context_surfaces_2d->dest_pitch = parameter >> 16;
			break;
		case NV062_SET_OFFSET_SOURCE:
			context_surfaces_2d->source_offset = parameter & 0x07FFFFFF;
			break;
		case NV062_SET_OFFSET_DESTIN:
			context_surfaces_2d->dest_offset = parameter & 0x07FFFFFF;
			break;
		default:
			EmuWarning("EmuNV2A: Unknown NV_CONTEXT_SURFACES_2D Method: 0x%08X\n", method);
		}
	
		break; 
	}
	
	case NV_IMAGE_BLIT: {
		switch (method) {
		case NV09F_SET_CONTEXT_SURFACES:
			image_blit->context_surfaces = parameter;
			break;
		case NV09F_SET_OPERATION:
			image_blit->operation = parameter;
			break;
		case NV09F_CONTROL_POINT_IN:
			image_blit->in_x = parameter & 0xFFFF;
			image_blit->in_y = parameter >> 16;
			break;
		case NV09F_CONTROL_POINT_OUT:
			image_blit->out_x = parameter & 0xFFFF;
			image_blit->out_y = parameter >> 16;
			break;
		case NV09F_SIZE:
			image_blit->width = parameter & 0xFFFF;
			image_blit->height = parameter >> 16;

			/* I guess this kicks it off? */
			if (image_blit->operation == NV09F_SET_OPERATION_SRCCOPY) {

				printf("NV09F_SET_OPERATION_SRCCOPY\n");

				GraphicsObject *context_surfaces_obj = lookup_graphics_object(pg, image_blit->context_surfaces);
				assert(context_surfaces_obj);
				assert(context_surfaces_obj->graphics_class == NV_CONTEXT_SURFACES_2D);

				ContextSurfaces2DState *context_surfaces =	&context_surfaces_obj->data.context_surfaces_2d;

				unsigned int bytes_per_pixel;
				switch (context_surfaces->color_format) {
				case NV062_SET_COLOR_FORMAT_LE_Y8:
					bytes_per_pixel = 1;
					break;
				case NV062_SET_COLOR_FORMAT_LE_R5G6B5:
					bytes_per_pixel = 2;
					break;
				case NV062_SET_COLOR_FORMAT_LE_A8R8G8B8:
					bytes_per_pixel = 4;
					break;
				default:
					printf("Unknown blit surface format: 0x%x\n", context_surfaces->color_format);
					assert(false);
					break;
				}

				xbaddr source_dma_len, dest_dma_len;
				uint8_t *source, *dest;

				source = (uint8_t*)nv_dma_map(d, context_surfaces->dma_image_source,	&source_dma_len);
				assert(context_surfaces->source_offset < source_dma_len);
				source += context_surfaces->source_offset;

				dest = (uint8_t*)nv_dma_map(d, context_surfaces->dma_image_dest,	&dest_dma_len);
				assert(context_surfaces->dest_offset < dest_dma_len);
				dest += context_surfaces->dest_offset;

				printf("  - 0x%tx -> 0x%tx\n", source - MM_SYSTEM_PHYSICAL_MAP,dest - MM_SYSTEM_PHYSICAL_MAP);

				unsigned int y;
				for (y = 0; y<image_blit->height; y++) {
					uint8_t *source_row = source
						+ (image_blit->in_y + y) * context_surfaces->source_pitch
						+ image_blit->in_x * bytes_per_pixel;

					uint8_t *dest_row = dest
						+ (image_blit->out_y + y) * context_surfaces->dest_pitch
						+ image_blit->out_x * bytes_per_pixel;

					memmove(dest_row, source_row,
						image_blit->width * bytes_per_pixel);
				}
			}
			else {
				assert(false);
			}

			break;
		default:
			EmuWarning("EmuNV2A: Unknown NV_IMAGE_BLIT Method: 0x%08X\n", method);
		}
		break;
	}

	case NV_KELVIN_PRIMITIVE: {
		switch (method) {
		case NV097_SET_CONTEXT_DMA_NOTIFIES:
			kelvin->dma_notifies = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_A:
			pg->dma_a = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_B:
			pg->dma_b = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_STATE:
			kelvin->dma_state = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_COLOR:
			/* try to get any straggling draws in before the surface's changed :/ */
			pgraph_update_surface(d, false, true, true);

			pg->dma_color = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_ZETA:
			pg->dma_zeta = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_VERTEX_A:
			pg->dma_vertex_a = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_VERTEX_B:
			pg->dma_vertex_b = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_SEMAPHORE:
			kelvin->dma_semaphore = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_REPORT:
			pg->dma_report = parameter;
			break;
		case NV097_SET_SURFACE_CLIP_HORIZONTAL:
			pgraph_update_surface(d, false, true, true);

			pg->surface_shape.clip_x =
				GET_MASK(parameter, NV097_SET_SURFACE_CLIP_HORIZONTAL_X);
			pg->surface_shape.clip_width =
				GET_MASK(parameter, NV097_SET_SURFACE_CLIP_HORIZONTAL_WIDTH);
			break;
		case NV097_SET_SURFACE_CLIP_VERTICAL:
			pgraph_update_surface(d, false, true, true);

			pg->surface_shape.clip_y =
				GET_MASK(parameter, NV097_SET_SURFACE_CLIP_VERTICAL_Y);
			pg->surface_shape.clip_height =
				GET_MASK(parameter, NV097_SET_SURFACE_CLIP_VERTICAL_HEIGHT);
			break;
		case NV097_SET_SURFACE_FORMAT:
			pgraph_update_surface(d, false, true, true);

			pg->surface_shape.color_format =
				GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_COLOR);
			pg->surface_shape.zeta_format =
				GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_ZETA);
			pg->surface_type =
				GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_TYPE);
			pg->surface_shape.anti_aliasing =
				GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_ANTI_ALIASING);
			pg->surface_shape.log_width =
				GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_WIDTH);
			pg->surface_shape.log_height =
				GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_HEIGHT);
			break;
		case NV097_SET_SURFACE_PITCH:
			pgraph_update_surface(d, false, true, true);

			pg->surface_color.pitch =
				GET_MASK(parameter, NV097_SET_SURFACE_PITCH_COLOR);
			pg->surface_zeta.pitch =
				GET_MASK(parameter, NV097_SET_SURFACE_PITCH_ZETA);
			break;
		case NV097_SET_SURFACE_COLOR_OFFSET:
			pgraph_update_surface(d, false, true, true);

			pg->surface_color.offset = parameter;
			break;
		case NV097_SET_SURFACE_ZETA_OFFSET:
			pgraph_update_surface(d, false, true, true);

			pg->surface_zeta.offset = parameter;
			break;
		case NV097_SET_COMBINER_SPECULAR_FOG_CW0:
			pg->regs[NV_PGRAPH_COMBINESPECFOG0] = parameter;
			break;
		case NV097_SET_COMBINER_SPECULAR_FOG_CW1:
			pg->regs[NV_PGRAPH_COMBINESPECFOG1] = parameter;
			break;
		CASE_4(NV097_SET_TEXTURE_ADDRESS, 64):
			slot = (method - NV097_SET_TEXTURE_ADDRESS) / 64;
			pg->regs[NV_PGRAPH_TEXADDRESS0 + slot * 4] = parameter;
			break;
		case NV097_SET_CONTROL0: {
			pgraph_update_surface(d, false, true, true);

			bool stencil_write_enable =
				parameter & NV097_SET_CONTROL0_STENCIL_WRITE_ENABLE;
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_STENCIL_WRITE_ENABLE,
				stencil_write_enable);

			uint32_t z_format = GET_MASK(parameter, NV097_SET_CONTROL0_Z_FORMAT);
			SET_MASK(pg->regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_Z_FORMAT, z_format);

			bool z_perspective =
				parameter & NV097_SET_CONTROL0_Z_PERSPECTIVE_ENABLE;
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_Z_PERSPECTIVE_ENABLE,
				z_perspective);
			break;
		}

		case NV097_SET_FOG_MODE: {
			/* FIXME: There is also NV_PGRAPH_CSV0_D_FOG_MODE */
			unsigned int mode;
			switch (parameter) {
			case NV097_SET_FOG_MODE_V_LINEAR:
				mode = NV_PGRAPH_CONTROL_3_FOG_MODE_LINEAR; break;
			case NV097_SET_FOG_MODE_V_EXP:
				mode = NV_PGRAPH_CONTROL_3_FOG_MODE_EXP; break;
			case NV097_SET_FOG_MODE_V_EXP2:
				mode = NV_PGRAPH_CONTROL_3_FOG_MODE_EXP2; break;
			case NV097_SET_FOG_MODE_V_EXP_ABS:
				mode = NV_PGRAPH_CONTROL_3_FOG_MODE_EXP_ABS; break;
			case NV097_SET_FOG_MODE_V_EXP2_ABS:
				mode = NV_PGRAPH_CONTROL_3_FOG_MODE_EXP2_ABS; break;
			case NV097_SET_FOG_MODE_V_LINEAR_ABS:
				mode = NV_PGRAPH_CONTROL_3_FOG_MODE_LINEAR_ABS; break;
			default:
				assert(false);
				break;
			}
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_3], NV_PGRAPH_CONTROL_3_FOG_MODE,
				mode);
			break;
		}
		case NV097_SET_FOG_GEN_MODE: {
			unsigned int mode;
			switch (parameter) {
			case NV097_SET_FOG_GEN_MODE_V_SPEC_ALPHA:
				mode = NV_PGRAPH_CSV0_D_FOGGENMODE_SPEC_ALPHA; break;
			case NV097_SET_FOG_GEN_MODE_V_RADIAL:
				mode = NV_PGRAPH_CSV0_D_FOGGENMODE_RADIAL; break;
			case NV097_SET_FOG_GEN_MODE_V_PLANAR:
				mode = NV_PGRAPH_CSV0_D_FOGGENMODE_PLANAR; break;
			case NV097_SET_FOG_GEN_MODE_V_ABS_PLANAR:
				mode = NV_PGRAPH_CSV0_D_FOGGENMODE_ABS_PLANAR; break;
			case NV097_SET_FOG_GEN_MODE_V_FOG_X:
				mode = NV_PGRAPH_CSV0_D_FOGGENMODE_FOG_X; break;
			default:
				assert(false);
				break;
			}
			SET_MASK(pg->regs[NV_PGRAPH_CSV0_D], NV_PGRAPH_CSV0_D_FOGGENMODE, mode);
			break;
		}
		case NV097_SET_FOG_ENABLE:
			/*
			FIXME: There is also:
			SET_MASK(pg->regs[NV_PGRAPH_CSV0_D], NV_PGRAPH_CSV0_D_FOGENABLE,
			parameter);
			*/
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_3], NV_PGRAPH_CONTROL_3_FOGENABLE,
				parameter);
			break;
		case NV097_SET_FOG_COLOR: {
			/* PGRAPH channels are ARGB, parameter channels are ABGR */
			uint8_t red = GET_MASK(parameter, NV097_SET_FOG_COLOR_RED);
			uint8_t green = GET_MASK(parameter, NV097_SET_FOG_COLOR_GREEN);
			uint8_t blue = GET_MASK(parameter, NV097_SET_FOG_COLOR_BLUE);
			uint8_t alpha = GET_MASK(parameter, NV097_SET_FOG_COLOR_ALPHA);
			SET_MASK(pg->regs[NV_PGRAPH_FOGCOLOR], NV_PGRAPH_FOGCOLOR_RED, red);
			SET_MASK(pg->regs[NV_PGRAPH_FOGCOLOR], NV_PGRAPH_FOGCOLOR_GREEN, green);
			SET_MASK(pg->regs[NV_PGRAPH_FOGCOLOR], NV_PGRAPH_FOGCOLOR_BLUE, blue);
			SET_MASK(pg->regs[NV_PGRAPH_FOGCOLOR], NV_PGRAPH_FOGCOLOR_ALPHA, alpha);
			break;
		}
		case NV097_SET_ALPHA_TEST_ENABLE:
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_ALPHATESTENABLE, parameter);
			break;
		case NV097_SET_BLEND_ENABLE:
			SET_MASK(pg->regs[NV_PGRAPH_BLEND], NV_PGRAPH_BLEND_EN, parameter);
			break;
		case NV097_SET_CULL_FACE_ENABLE:
			SET_MASK(pg->regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_CULLENABLE,
				parameter);
			break;
		case NV097_SET_DEPTH_TEST_ENABLE:
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_0], NV_PGRAPH_CONTROL_0_ZENABLE,
				parameter);
			break;
		case NV097_SET_DITHER_ENABLE:
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_DITHERENABLE, parameter);
			break;
		case NV097_SET_LIGHTING_ENABLE:
			SET_MASK(pg->regs[NV_PGRAPH_CSV0_C], NV_PGRAPH_CSV0_C_LIGHTING,
				parameter);
			break;
		case NV097_SET_SKIN_MODE:
			SET_MASK(pg->regs[NV_PGRAPH_CSV0_D], NV_PGRAPH_CSV0_D_SKIN,
				parameter);
			break;
		case NV097_SET_STENCIL_TEST_ENABLE:
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_1],
				NV_PGRAPH_CONTROL_1_STENCIL_TEST_ENABLE, parameter);
			break;
		case NV097_SET_POLY_OFFSET_POINT_ENABLE:
			SET_MASK(pg->regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_POFFSETPOINTENABLE, parameter);
			break;
		case NV097_SET_POLY_OFFSET_LINE_ENABLE:
			SET_MASK(pg->regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_POFFSETLINEENABLE, parameter);
			break;
		case NV097_SET_POLY_OFFSET_FILL_ENABLE:
			SET_MASK(pg->regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_POFFSETFILLENABLE, parameter);
			break;
		case NV097_SET_ALPHA_FUNC:
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_ALPHAFUNC, parameter & 0xF);
			break;
		case NV097_SET_ALPHA_REF:
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_ALPHAREF, parameter);
			break;
		case NV097_SET_BLEND_FUNC_SFACTOR: {
			unsigned int factor;
			switch (parameter) {
			case NV097_SET_BLEND_FUNC_SFACTOR_V_ZERO:
				factor = NV_PGRAPH_BLEND_SFACTOR_ZERO; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE:
				factor = NV_PGRAPH_BLEND_SFACTOR_ONE; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_SRC_COLOR:
				factor = NV_PGRAPH_BLEND_SFACTOR_SRC_COLOR; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_SRC_COLOR:
				factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_SRC_COLOR; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_SRC_ALPHA:
				factor = NV_PGRAPH_BLEND_SFACTOR_SRC_ALPHA; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_SRC_ALPHA:
				factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_SRC_ALPHA; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_DST_ALPHA:
				factor = NV_PGRAPH_BLEND_SFACTOR_DST_ALPHA; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_DST_ALPHA:
				factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_DST_ALPHA; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_DST_COLOR:
				factor = NV_PGRAPH_BLEND_SFACTOR_DST_COLOR; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_DST_COLOR:
				factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_DST_COLOR; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_SRC_ALPHA_SATURATE:
				factor = NV_PGRAPH_BLEND_SFACTOR_SRC_ALPHA_SATURATE; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_CONSTANT_COLOR:
				factor = NV_PGRAPH_BLEND_SFACTOR_CONSTANT_COLOR; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_CONSTANT_COLOR:
				factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_CONSTANT_COLOR; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_CONSTANT_ALPHA:
				factor = NV_PGRAPH_BLEND_SFACTOR_CONSTANT_ALPHA; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_CONSTANT_ALPHA:
				factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_CONSTANT_ALPHA; break;
			default:
				fprintf(stderr, "Unknown blend source factor: 0x%x\n", parameter);
				assert(false);
				break;
			}
			SET_MASK(pg->regs[NV_PGRAPH_BLEND], NV_PGRAPH_BLEND_SFACTOR, factor);

			break;
		}

		case NV097_SET_BLEND_FUNC_DFACTOR: {
			unsigned int factor;
			switch (parameter) {
			case NV097_SET_BLEND_FUNC_DFACTOR_V_ZERO:
				factor = NV_PGRAPH_BLEND_DFACTOR_ZERO; break;
			case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE:
				factor = NV_PGRAPH_BLEND_DFACTOR_ONE; break;
			case NV097_SET_BLEND_FUNC_DFACTOR_V_SRC_COLOR:
				factor = NV_PGRAPH_BLEND_DFACTOR_SRC_COLOR; break;
			case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_SRC_COLOR:
				factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_SRC_COLOR; break;
			case NV097_SET_BLEND_FUNC_DFACTOR_V_SRC_ALPHA:
				factor = NV_PGRAPH_BLEND_DFACTOR_SRC_ALPHA; break;
			case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_SRC_ALPHA:
				factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_SRC_ALPHA; break;
			case NV097_SET_BLEND_FUNC_DFACTOR_V_DST_ALPHA:
				factor = NV_PGRAPH_BLEND_DFACTOR_DST_ALPHA; break;
			case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_DST_ALPHA:
				factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_DST_ALPHA; break;
			case NV097_SET_BLEND_FUNC_DFACTOR_V_DST_COLOR:
				factor = NV_PGRAPH_BLEND_DFACTOR_DST_COLOR; break;
			case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_DST_COLOR:
				factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_DST_COLOR; break;
			case NV097_SET_BLEND_FUNC_DFACTOR_V_SRC_ALPHA_SATURATE:
				factor = NV_PGRAPH_BLEND_DFACTOR_SRC_ALPHA_SATURATE; break;
			case NV097_SET_BLEND_FUNC_DFACTOR_V_CONSTANT_COLOR:
				factor = NV_PGRAPH_BLEND_DFACTOR_CONSTANT_COLOR; break;
			case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_CONSTANT_COLOR:
				factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_CONSTANT_COLOR; break;
			case NV097_SET_BLEND_FUNC_DFACTOR_V_CONSTANT_ALPHA:
				factor = NV_PGRAPH_BLEND_DFACTOR_CONSTANT_ALPHA; break;
			case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_CONSTANT_ALPHA:
				factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_CONSTANT_ALPHA; break;
			default:
				fprintf(stderr, "Unknown blend destination factor: 0x%x\n", parameter);
				assert(false);
				break;
			}
			SET_MASK(pg->regs[NV_PGRAPH_BLEND], NV_PGRAPH_BLEND_DFACTOR, factor);

			break;
		}

		case NV097_SET_BLEND_COLOR:
			pg->regs[NV_PGRAPH_BLENDCOLOR] = parameter;
			break;

		case NV097_SET_BLEND_EQUATION: {
			unsigned int equation;
			switch (parameter) {
			case NV097_SET_BLEND_EQUATION_V_FUNC_SUBTRACT:
				equation = 0; break;
			case NV097_SET_BLEND_EQUATION_V_FUNC_REVERSE_SUBTRACT:
				equation = 1; break;
			case NV097_SET_BLEND_EQUATION_V_FUNC_ADD:
				equation = 2; break;
			case NV097_SET_BLEND_EQUATION_V_MIN:
				equation = 3; break;
			case NV097_SET_BLEND_EQUATION_V_MAX:
				equation = 4; break;
			case NV097_SET_BLEND_EQUATION_V_FUNC_REVERSE_SUBTRACT_SIGNED:
				equation = 5; break;
			case NV097_SET_BLEND_EQUATION_V_FUNC_ADD_SIGNED:
				equation = 6; break;
			default:
				assert(false);
				break;
			}
			SET_MASK(pg->regs[NV_PGRAPH_BLEND], NV_PGRAPH_BLEND_EQN, equation);

			break;
		}

		case NV097_SET_DEPTH_FUNC:
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_0], NV_PGRAPH_CONTROL_0_ZFUNC,
				parameter & 0xF);
			break;

		case NV097_SET_COLOR_MASK: {
			pg->surface_color.write_enabled_cache |= pgraph_color_write_enabled(pg);

			bool alpha = parameter & NV097_SET_COLOR_MASK_ALPHA_WRITE_ENABLE;
			bool red = parameter & NV097_SET_COLOR_MASK_RED_WRITE_ENABLE;
			bool green = parameter & NV097_SET_COLOR_MASK_GREEN_WRITE_ENABLE;
			bool blue = parameter & NV097_SET_COLOR_MASK_BLUE_WRITE_ENABLE;
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_ALPHA_WRITE_ENABLE, alpha);
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_RED_WRITE_ENABLE, red);
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_GREEN_WRITE_ENABLE, green);
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_BLUE_WRITE_ENABLE, blue);
			break;
		}
		case NV097_SET_DEPTH_MASK:
			pg->surface_zeta.write_enabled_cache |= pgraph_zeta_write_enabled(pg);

			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_ZWRITEENABLE, parameter);
			break;
		case NV097_SET_STENCIL_MASK:
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_1],
				NV_PGRAPH_CONTROL_1_STENCIL_MASK_WRITE, parameter);
			break;
		case NV097_SET_STENCIL_FUNC:
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_1],
				NV_PGRAPH_CONTROL_1_STENCIL_FUNC, parameter & 0xF);
			break;
		case NV097_SET_STENCIL_FUNC_REF:
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_1],
				NV_PGRAPH_CONTROL_1_STENCIL_REF, parameter);
			break;
		case NV097_SET_STENCIL_FUNC_MASK:
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_1],
				NV_PGRAPH_CONTROL_1_STENCIL_MASK_READ, parameter);
			break;
		case NV097_SET_STENCIL_OP_FAIL:
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_2],
				NV_PGRAPH_CONTROL_2_STENCIL_OP_FAIL,
				kelvin_map_stencil_op(parameter));
			break;
		case NV097_SET_STENCIL_OP_ZFAIL:
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_2],
				NV_PGRAPH_CONTROL_2_STENCIL_OP_ZFAIL,
				kelvin_map_stencil_op(parameter));
			break;
		case NV097_SET_STENCIL_OP_ZPASS:
			SET_MASK(pg->regs[NV_PGRAPH_CONTROL_2],
				NV_PGRAPH_CONTROL_2_STENCIL_OP_ZPASS,
				kelvin_map_stencil_op(parameter));
			break;

		case NV097_SET_POLYGON_OFFSET_SCALE_FACTOR:
			pg->regs[NV_PGRAPH_ZOFFSETFACTOR] = parameter;
			break;
		case NV097_SET_POLYGON_OFFSET_BIAS:
			pg->regs[NV_PGRAPH_ZOFFSETBIAS] = parameter;
			break;
		case NV097_SET_FRONT_POLYGON_MODE:
			SET_MASK(pg->regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_FRONTFACEMODE,
				kelvin_map_polygon_mode(parameter));
			break;
		case NV097_SET_BACK_POLYGON_MODE:
			SET_MASK(pg->regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_BACKFACEMODE,
				kelvin_map_polygon_mode(parameter));
			break;
		case NV097_SET_CLIP_MIN:
			pg->regs[NV_PGRAPH_ZCLIPMIN] = parameter;
			break;
		case NV097_SET_CLIP_MAX:
			pg->regs[NV_PGRAPH_ZCLIPMAX] = parameter;
			break;
		case NV097_SET_CULL_FACE: {
			unsigned int face;
			switch (parameter) {
			case NV097_SET_CULL_FACE_V_FRONT:
				face = NV_PGRAPH_SETUPRASTER_CULLCTRL_FRONT; break;
			case NV097_SET_CULL_FACE_V_BACK:
				face = NV_PGRAPH_SETUPRASTER_CULLCTRL_BACK; break;
			case NV097_SET_CULL_FACE_V_FRONT_AND_BACK:
				face = NV_PGRAPH_SETUPRASTER_CULLCTRL_FRONT_AND_BACK; break;
			default:
				assert(false);
				break;
			}
			SET_MASK(pg->regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_CULLCTRL,
				face);
			break;
		}
		case NV097_SET_FRONT_FACE: {
			bool ccw;
			switch (parameter) {
			case NV097_SET_FRONT_FACE_V_CW:
				ccw = false; break;
			case NV097_SET_FRONT_FACE_V_CCW:
				ccw = true; break;
			default:
				fprintf(stderr, "Unknown front face: 0x%x\n", parameter);
				assert(false);
				break;
			}
			SET_MASK(pg->regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_FRONTFACE,
				ccw ? 1 : 0);
			break;
		}
		case NV097_SET_NORMALIZATION_ENABLE:
			SET_MASK(pg->regs[NV_PGRAPH_CSV0_C],
				NV_PGRAPH_CSV0_C_NORMALIZATION_ENABLE,
				parameter);
			break;

		case NV097_SET_LIGHT_ENABLE_MASK:
			SET_MASK(pg->regs[NV_PGRAPH_CSV0_D],
				NV_PGRAPH_CSV0_D_LIGHTS,
				parameter);
			break;

		CASE_4(NV097_SET_TEXGEN_S, 16) : {
			slot = (method - NV097_SET_TEXGEN_S) / 16;
			unsigned int reg = (slot < 2) ? NV_PGRAPH_CSV1_A
				: NV_PGRAPH_CSV1_B;
			unsigned int mask = (slot % 2) ? NV_PGRAPH_CSV1_A_T1_S
				: NV_PGRAPH_CSV1_A_T0_S;
			SET_MASK(pg->regs[reg], mask, kelvin_map_texgen(parameter, 0));
			break;
		}
		CASE_4(NV097_SET_TEXGEN_T, 16) : {
			slot = (method - NV097_SET_TEXGEN_T) / 16;
			unsigned int reg = (slot < 2) ? NV_PGRAPH_CSV1_A
				: NV_PGRAPH_CSV1_B;
			unsigned int mask = (slot % 2) ? NV_PGRAPH_CSV1_A_T1_T
				: NV_PGRAPH_CSV1_A_T0_T;
			SET_MASK(pg->regs[reg], mask, kelvin_map_texgen(parameter, 1));
			break;
		}
		CASE_4(NV097_SET_TEXGEN_R, 16) : {
			slot = (method - NV097_SET_TEXGEN_R) / 16;
			unsigned int reg = (slot < 2) ? NV_PGRAPH_CSV1_A
				: NV_PGRAPH_CSV1_B;
			unsigned int mask = (slot % 2) ? NV_PGRAPH_CSV1_A_T1_R
				: NV_PGRAPH_CSV1_A_T0_R;
			SET_MASK(pg->regs[reg], mask, kelvin_map_texgen(parameter, 2));
			break;
		}
		CASE_4(NV097_SET_TEXGEN_Q, 16) : {
			slot = (method - NV097_SET_TEXGEN_Q) / 16;
			unsigned int reg = (slot < 2) ? NV_PGRAPH_CSV1_A
				: NV_PGRAPH_CSV1_B;
			unsigned int mask = (slot % 2) ? NV_PGRAPH_CSV1_A_T1_Q
				: NV_PGRAPH_CSV1_A_T0_Q;
			SET_MASK(pg->regs[reg], mask, kelvin_map_texgen(parameter, 3));
			break;
		}
		CASE_4(NV097_SET_TEXTURE_MATRIX_ENABLE, 4) :
			slot = (method - NV097_SET_TEXTURE_MATRIX_ENABLE) / 4;
		pg->texture_matrix_enable[slot] = parameter;
		break;

		case NV097_SET_TEXGEN_VIEW_MODEL:
			SET_MASK(pg->regs[NV_PGRAPH_CSV0_D], NV_PGRAPH_CSV0_D_TEXGEN_REF, parameter);
			break;
		case NV097_SET_SEMAPHORE_OFFSET:
			kelvin->semaphore_offset = parameter;
			break;
		case NV097_BACK_END_WRITE_SEMAPHORE_RELEASE: {

			pgraph_update_surface(d, false, true, true);

			//qemu_mutex_unlock(&d->pg->lock);
			//qemu_mutex_lock_iothread();

			xbaddr semaphore_dma_len;
			uint8_t *semaphore_data = (uint8_t*)nv_dma_map(d, kelvin->dma_semaphore,
				&semaphore_dma_len);
			assert(kelvin->semaphore_offset < semaphore_dma_len);
			semaphore_data += kelvin->semaphore_offset;

			stl_le_p((uint32_t*)semaphore_data, parameter);

			//qemu_mutex_lock(&d->pg->lock);
			//qemu_mutex_unlock_iothread();

			break;
		}
		default:
			if (method >= NV097_SET_COMBINER_ALPHA_ICW && method <= NV097_SET_COMBINER_ALPHA_ICW + 28) {
				slot = (method - NV097_SET_COMBINER_ALPHA_ICW) / 4;
				pg->regs[NV_PGRAPH_COMBINEALPHAI0 + slot * 4] = parameter;
				break;
			}

			if (method >= NV097_SET_PROJECTION_MATRIX && method <= NV097_SET_PROJECTION_MATRIX + 0x3c) {
				slot = (method - NV097_SET_PROJECTION_MATRIX) / 4;
				// pg->projection_matrix[slot] = *(float*)&parameter;
				unsigned int row = NV_IGRAPH_XF_XFCTX_PMAT0 + slot / 4;
				pg->vsh_constants[row][slot % 4] = parameter;
				pg->vsh_constants_dirty[row] = true;
				break;
			}

			if (method >= NV097_SET_MODEL_VIEW_MATRIX && method <= NV097_SET_MODEL_VIEW_MATRIX + 0xfc) {
				slot = (method - NV097_SET_MODEL_VIEW_MATRIX) / 4;
				unsigned int matnum = slot / 16;
				unsigned int entry = slot % 16;
				unsigned int row = NV_IGRAPH_XF_XFCTX_MMAT0 + matnum * 8 + entry / 4;
				pg->vsh_constants[row][entry % 4] = parameter;
				pg->vsh_constants_dirty[row] = true;
				break;
			}

			if (method >= NV097_SET_INVERSE_MODEL_VIEW_MATRIX && method <= NV097_SET_INVERSE_MODEL_VIEW_MATRIX + 0xfc) {
				slot = (method - NV097_SET_INVERSE_MODEL_VIEW_MATRIX) / 4;
				unsigned int matnum = slot / 16;
				unsigned int entry = slot % 16;
				unsigned int row = NV_IGRAPH_XF_XFCTX_IMMAT0 + matnum * 8 + entry / 4;
				pg->vsh_constants[row][entry % 4] = parameter;
				pg->vsh_constants_dirty[row] = true;
				break;
			}

			if (method >= NV097_SET_COMPOSITE_MATRIX && method <= NV097_SET_COMPOSITE_MATRIX + 0x3c) {
				slot = (method - NV097_SET_COMPOSITE_MATRIX) / 4;
				unsigned int row = NV_IGRAPH_XF_XFCTX_CMAT0 + slot / 4;
				pg->vsh_constants[row][slot % 4] = parameter;
				pg->vsh_constants_dirty[row] = true;
				break;
			}

			if (method >= NV097_SET_TEXTURE_MATRIX && method <= NV097_SET_TEXTURE_MATRIX + 0xfc) {
				slot = (method - NV097_SET_TEXTURE_MATRIX) / 4;
				unsigned int tex = slot / 16;
				unsigned int entry = slot % 16;
				unsigned int row = NV_IGRAPH_XF_XFCTX_T0MAT + tex * 8 + entry / 4;
				pg->vsh_constants[row][entry % 4] = parameter;
				pg->vsh_constants_dirty[row] = true;
				break;
			}

			if (method >= NV097_SET_FOG_PARAMS && method <= NV097_SET_FOG_PARAMS + 8) {
				slot = (method - NV097_SET_FOG_PARAMS) / 4;
				if (slot < 2) {
					pg->regs[NV_PGRAPH_FOGPARAM0 + slot * 4] = parameter;
				}
				else {
					/* FIXME: No idea where slot = 2 is */
				}

				pg->ltctxa[NV_IGRAPH_XF_LTCTXA_FOG_K][slot] = parameter;
				pg->ltctxa_dirty[NV_IGRAPH_XF_LTCTXA_FOG_K] = true;
				break;
			}
				
			/* Handles NV097_SET_TEXGEN_PLANE_S,T,R,Q */
			if (method >= NV097_SET_TEXGEN_PLANE_S && method <=	NV097_SET_TEXGEN_PLANE_S + 0xfc) {
				slot = (method - NV097_SET_TEXGEN_PLANE_S) / 4;
				unsigned int tex = slot / 16;
				unsigned int entry = slot % 16;
				unsigned int row = NV_IGRAPH_XF_XFCTX_TG0MAT + tex * 8 + entry / 4;
				pg->vsh_constants[row][entry % 4] = parameter;
				pg->vsh_constants_dirty[row] = true;
				break;
			}

			if (method >= NV097_SET_FOG_PLANE && method <= NV097_SET_FOG_PLANE + 12) {
				slot = (method - NV097_SET_FOG_PLANE) / 4;
				pg->vsh_constants[NV_IGRAPH_XF_XFCTX_FOG][slot] = parameter;
				pg->vsh_constants_dirty[NV_IGRAPH_XF_XFCTX_FOG] = true;
				break;
			}

			if (method >= NV097_SET_SCENE_AMBIENT_COLOR && method <= NV097_SET_SCENE_AMBIENT_COLOR + 8) {
				slot = (method - NV097_SET_SCENE_AMBIENT_COLOR) / 4;
				// ??
				pg->ltctxa[NV_IGRAPH_XF_LTCTXA_FR_AMB][slot] = parameter;
				pg->ltctxa_dirty[NV_IGRAPH_XF_LTCTXA_FR_AMB] = true;
				break;
			}

			if (method >= NV097_SET_VIEWPORT_OFFSET && method <= NV097_SET_VIEWPORT_OFFSET + 12) {
				slot = (method - NV097_SET_VIEWPORT_OFFSET) / 4;
				pg->vsh_constants[NV_IGRAPH_XF_XFCTX_VPOFF][slot] = parameter;
				pg->vsh_constants_dirty[NV_IGRAPH_XF_XFCTX_VPOFF] = true;
				break; 
			}

			if (method >= NV097_SET_EYE_POSITION  && method <= NV097_SET_EYE_POSITION + 12) {
				slot = (method - NV097_SET_EYE_POSITION) / 4;
				pg->vsh_constants[NV_IGRAPH_XF_XFCTX_EYEP][slot] = parameter;
				pg->vsh_constants_dirty[NV_IGRAPH_XF_XFCTX_EYEP] = true;
				break;
			}

			if (method >= NV097_SET_COMBINER_FACTOR0 && method <= NV097_SET_COMBINER_FACTOR0 + 28) {
				slot = (method - NV097_SET_COMBINER_FACTOR0) / 4;
				pg->regs[NV_PGRAPH_COMBINEFACTOR0 + slot * 4] = parameter;
				break;
			}

			if (method >= NV097_SET_COMBINER_FACTOR1 && method <= NV097_SET_COMBINER_FACTOR1 + 28) {
				slot = (method - NV097_SET_COMBINER_FACTOR1) / 4;
				pg->regs[NV_PGRAPH_COMBINEFACTOR1 + slot * 4] = parameter;
				break;
			}

			if (method >= NV097_SET_COMBINER_ALPHA_OCW && method <= NV097_SET_COMBINER_ALPHA_OCW + 28) {
				slot = (method - NV097_SET_COMBINER_ALPHA_OCW) / 4;
				pg->regs[NV_PGRAPH_COMBINEALPHAO0 + slot * 4] = parameter;
				break;
			}

			if (method >= NV097_SET_COMBINER_COLOR_ICW && method <= NV097_SET_COMBINER_COLOR_ICW + 28) {
				slot = (method - NV097_SET_COMBINER_COLOR_ICW) / 4;
				pg->regs[NV_PGRAPH_COMBINECOLORI0 + slot * 4] = parameter;
				break;
			}

			if (method >= NV097_SET_VIEWPORT_SCALE && method <= NV097_SET_VIEWPORT_SCALE + 12) {
				slot = (method - NV097_SET_VIEWPORT_SCALE) / 4;
				pg->vsh_constants[NV_IGRAPH_XF_XFCTX_VPSCL][slot] = parameter;
				pg->vsh_constants_dirty[NV_IGRAPH_XF_XFCTX_VPSCL] = true;
				break;
			}

			EmuWarning("EmuNV2A: Unknown NV_KELVIN_PRIMITIVE Method: 0x%08X\n", method);
		}
		break;
	}

	default:
		EmuWarning("EmuNV2A: Unknown Graphics Class/Method 0x%08X/0x%08X\n", object->graphics_class, method);
		break;
	}
}

static void pgraph_context_switch(NV2AState *d, unsigned int channel_id)
{
	bool valid = false;

	// Scope the lock so that it gets unlocked at end of this block
	{
		std::lock_guard<std::mutex> lk(d->pgraph.mutex);

		valid = d->pgraph.channel_valid && d->pgraph.channel_id == channel_id;
		if (!valid) {
			d->pgraph.trapped_channel_id = channel_id;
		}
	}

	if (!valid) {
		printf("puller needs to switch to ch %d\n", channel_id);

		//qemu_mutex_lock_iothread();
		d->pgraph.pending_interrupts |= NV_PGRAPH_INTR_CONTEXT_SWITCH;
		update_irq(d);

		std::unique_lock<std::mutex> lk(d->pgraph.mutex);
		//qemu_mutex_unlock_iothread();

		while (d->pgraph.pending_interrupts & NV_PGRAPH_INTR_CONTEXT_SWITCH) {
			d->pgraph.interrupt_cond.wait(lk);
		}
	}
}

static void pgraph_wait_fifo_access(NV2AState *d) {
	std::unique_lock<std::mutex> lk(d->pgraph.mutex);

	while (!d->pgraph.fifo_access) {	
		d->pgraph.fifo_access_cond.wait(lk);
	}
}

static void pgraph_method_log(unsigned int subchannel,	unsigned int graphics_class, unsigned int method, uint32_t parameter) {
	static unsigned int last = 0;
	static unsigned int count = 0;

	if (last == 0x1800 && method != last) {
		printf("d->pgraph method (%d) 0x%08X * %d", subchannel, last, count);
	}
	if (method != 0x1800) {
		const char* method_name = NULL;
		unsigned int nmethod = 0;
		switch (graphics_class) {
		case NV_KELVIN_PRIMITIVE:
			nmethod = method | (0x5c << 16);
			break;
		case NV_CONTEXT_SURFACES_2D:
			nmethod = method | (0x6d << 16);
			break;
		default:
			break;
		}
		/*
		if (nmethod != 0 && nmethod < ARRAY_SIZE(nv2a_method_names)) {
			method_name = nv2a_method_names[nmethod];
		}
		if (method_name) {
			printf("d->pgraph method (%d): %s (0x%x)\n",
				subchannel, method_name, parameter);
		}
		else {
		*/
			printf("d->pgraph method (%d): 0x%x -> 0x%04x (0x%x)\n",
				subchannel, graphics_class, method, parameter);
		//}

	}
	if (method == last) { count++; }
	else { count = 0; }
	last = method;
}

static bool pgraph_color_write_enabled(PGRAPHState *pg)
{
	return pg->regs[NV_PGRAPH_CONTROL_0] & (
		NV_PGRAPH_CONTROL_0_ALPHA_WRITE_ENABLE
		| NV_PGRAPH_CONTROL_0_RED_WRITE_ENABLE
		| NV_PGRAPH_CONTROL_0_GREEN_WRITE_ENABLE
		| NV_PGRAPH_CONTROL_0_BLUE_WRITE_ENABLE);
}

static bool pgraph_zeta_write_enabled(PGRAPHState *pg)
{
	return pg->regs[NV_PGRAPH_CONTROL_0] & (
		NV_PGRAPH_CONTROL_0_ZWRITEENABLE
		| NV_PGRAPH_CONTROL_0_STENCIL_WRITE_ENABLE);
}

static void pgraph_update_surface(NV2AState *d, bool upload,
	bool color_write, bool zeta_write)
{
	printf("TODO: pgraph_update_surface\n");
}

static void load_graphics_object(NV2AState *d, xbaddr instance_address, GraphicsObject *obj)
{
	uint8_t *obj_ptr;
	uint32_t switch1, switch2, switch3;
	
	assert(instance_address < d->pramin.ramin_size);
	obj_ptr = d->pramin.ramin_ptr + instance_address;

	switch1 = ldl_le_p((uint32_t*)obj_ptr);
	switch2 = ldl_le_p((uint32_t*)(obj_ptr + 4));
	switch3 = ldl_le_p((uint32_t*)(obj_ptr + 8));

	obj->graphics_class = switch1 & NV_PGRAPH_CTX_SWITCH1_GRCLASS;

	/* init graphics object */
	switch (obj->graphics_class) {
	case NV_KELVIN_PRIMITIVE:
		// kelvin->vertex_attributes[NV2A_VERTEX_ATTR_DIFFUSE].inline_value = 0xFFFFFFF;
		break;
	default:
		break;
	}
}

static GraphicsObject* lookup_graphics_object(PGRAPHState *s,xbaddr instance_address)
{
	int i;
	for (i = 0; i<NV2A_NUM_SUBCHANNELS; i++) {
		if (s->subchannel_data[i].object_instance == instance_address) {
			return &s->subchannel_data[i].object;
		}
	}
	return NULL;
}

static unsigned int kelvin_map_stencil_op(uint32_t parameter)
{
	unsigned int op;
	switch (parameter) {
	case NV097_SET_STENCIL_OP_V_KEEP:
		op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_KEEP; break;
	case NV097_SET_STENCIL_OP_V_ZERO:
		op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_ZERO; break;
	case NV097_SET_STENCIL_OP_V_REPLACE:
		op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_REPLACE; break;
	case NV097_SET_STENCIL_OP_V_INCRSAT:
		op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_INCRSAT; break;
	case NV097_SET_STENCIL_OP_V_DECRSAT:
		op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_DECRSAT; break;
	case NV097_SET_STENCIL_OP_V_INVERT:
		op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_INVERT; break;
	case NV097_SET_STENCIL_OP_V_INCR:
		op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_INCR; break;
	case NV097_SET_STENCIL_OP_V_DECR:
		op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_DECR; break;
	default:
		assert(false);
		break;
	}
	return op;
}

static unsigned int kelvin_map_polygon_mode(uint32_t parameter)
{
	unsigned int mode;
	switch (parameter) {
	case NV097_SET_FRONT_POLYGON_MODE_V_POINT:
		mode = NV_PGRAPH_SETUPRASTER_FRONTFACEMODE_POINT; break;
	case NV097_SET_FRONT_POLYGON_MODE_V_LINE:
		mode = NV_PGRAPH_SETUPRASTER_FRONTFACEMODE_LINE; break;
	case NV097_SET_FRONT_POLYGON_MODE_V_FILL:
		mode = NV_PGRAPH_SETUPRASTER_FRONTFACEMODE_FILL; break;
	default:
		assert(false);
		break;
	}
	return mode;
}

static unsigned int kelvin_map_texgen(uint32_t parameter, unsigned int channel)
{
	assert(channel < 4);
	unsigned int texgen;
	switch (parameter) {
	case NV097_SET_TEXGEN_S_DISABLE:
		texgen = NV_PGRAPH_CSV1_A_T0_S_DISABLE; break;
	case NV097_SET_TEXGEN_S_EYE_LINEAR:
		texgen = NV_PGRAPH_CSV1_A_T0_S_EYE_LINEAR; break;
	case NV097_SET_TEXGEN_S_OBJECT_LINEAR:
		texgen = NV_PGRAPH_CSV1_A_T0_S_OBJECT_LINEAR; break;
	case NV097_SET_TEXGEN_S_SPHERE_MAP:
		assert(channel < 2);
		texgen = NV_PGRAPH_CSV1_A_T0_S_SPHERE_MAP; break;
	case NV097_SET_TEXGEN_S_REFLECTION_MAP:
		assert(channel < 3);
		texgen = NV_PGRAPH_CSV1_A_T0_S_REFLECTION_MAP; break;
	case NV097_SET_TEXGEN_S_NORMAL_MAP:
		assert(channel < 3);
		texgen = NV_PGRAPH_CSV1_A_T0_S_NORMAL_MAP; break;
	default:
		assert(false);
		break;
	}
	return texgen;
}

