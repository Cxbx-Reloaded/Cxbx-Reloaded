DEVICE_READ32(PRAMIN)
{
	uint32_t result = *((uint32_t*)(d->pramin.ramin_ptr + addr));

	DEVICE_READ32_END(PRAMIN);
}

DEVICE_WRITE32(PRAMIN)
{
	*((uint32_t*)(d->pramin.ramin_ptr + addr)) = value;

	DEVICE_WRITE32_END(PRAMIN);
}
