/* <stdlib.h>
 *
 *	ANSI-C Standard Library definitions
 */
#define	__IEEE_FLOAT	1				// BIGBOY


#ifndef _STDLIB_H
#define _STDLIB_H
# ifdef __cplusplus
extern "C" {
# endif
			/* note: no !defined(__STDC__) handling, deliberately */
#include <defs.h>

#ifndef _STDDEF_H
#include <stddef.h>	/* get size_t, NULL */
#endif


char *itoa(int i, char* string, int radix);

/*
   These exit macro values work with either exit() or return from main(),
   and avoid unnecessary warnings from the DCL command intrepretor.  They
   don't match VAX C's <stdlib.h>, which has 0 and 2 respectively.  Using 0
   for success works with VAXCRTL's exit() but requires additional compiler
   support in main() (which VAX C neglects to provide).  2 always provokes
   "%NONAME-E-NOMSG, Message number 00000002"; similarly for unfixed 0.
 */
#define EXIT_SUCCESS 1			/* SS$_NORMAL, STS$K_SUCCESS */
#define EXIT_FAILURE 0x10000002		/* (STS$K_ERROR | STS$M_INHIB_MSG) */

#define RAND_MAX 2147483647	/* 0x7FFFFFFF */
#define lrand48() rand()

#if (__GNUC__ == 2 && !__STRICT_ANSI__)
#if (__GNUC_MINOR__ >= 5)
void abort(void) __asm ("decc$abort") __attribute__((noreturn));
void exit(int) __asm ("decc$exit") __attribute__((noreturn));
void _exit(int) __attribute__((noreturn));
#else
volatile void abort(void) __asm ("decc$abort");
volatile void exit(int) __asm ("decc$exit");
volatile void _exit(int);
#endif
#else
void abort(void);
void exit(int i);
void _exit(int i);
#endif
int  atexit(void (*)(void));

void *bsearch(const void *,const void *,size_t,size_t,
	      int (*)(const void *,const void *));

void qsort(void *,size_t,size_t,int (*)(const void *,const void *));	// __asm ("decc$qsort");

int  rand(void);
int  srand(int);		/* routine to initialize rand() */

/* These are the functions that actually do things.  The `random', `srandom',
   `initstate' and `setstate' functions are those from BSD Unices.
   The `rand' and `srand' functions are required by the ANSI standard.
   We provide both interfaces to the same random number generator.  */
/* Return a random long integer between 0 and RAND_MAX inclusive.  */
extern long int __random (void);
/* Seed the random number generator with the given number.  */
extern void __srandom (unsigned int __seed);


char *getenv(const char *);	// __asm ("decc$getenv");
int  system(const char *);

/* math related routines */
#ifndef _DIV_T
#define _DIV_T
typedef struct DIV_T { int quot, rem; } div_t;
#endif
#ifndef _LDIV_T
#define _LDIV_T
typedef struct LDIV_T { long quot, rem; } ldiv_t;
#endif

# define ___fdecl(_func,_arglist)
#if __IEEE_FLOAT
# define ___gdecl(_func, _arglist) _func _arglist { __asm{ nop } }
#elif __G_FLOAT
# define ___gdecl(_func, _arglist) _func _arglist __asm("decc$g" #_func)
#else
# error "No floating format defined"
#endif

int	abs(int);
int	atoi (const char *i);
long	atol(const char * i);
double  atof(const char * i);
div_t	div(int i,int i2);
long	labs(long);
ldiv_t	ldiv(long,long);
double	strtod(const char *i,char **i2);
long	strtol(const char *i,char **i2,int i3);
unsigned long strtoul(const char *i,char **i2,int i3);

/* memory manipulation routines; note: cfree() is gone; see also <malloc.h> */
void*	malloc (size_t i);
void*	calloc (size_t i, size_t i2);
void*	realloc (void *i, size_t i2);
void	free (void * i);

#undef ___gdecl
#undef ___fdecl

# ifdef __cplusplus
}
# endif
#endif	/*_STDLIB_H*/
