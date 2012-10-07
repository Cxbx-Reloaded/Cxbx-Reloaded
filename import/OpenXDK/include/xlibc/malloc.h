
#ifndef		__OPENXDK_MALLOC__
#define		__OPENXDK_MALLOC__

#ifdef	__cplusplus
extern "C" {
#endif

//dummy malloc to make bitmap lib compile :P

void *malloc(size_t);
void free(void *);

#ifdef	__cplusplus
};
#endif

#endif
