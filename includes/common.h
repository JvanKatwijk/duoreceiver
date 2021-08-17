
#ifndef	__COMMON__
#define	__COMMON__
#include	<complex>

#ifndef	__MINGW32__
#include	"dlfcn.h"
typedef	void	*HINSTANCE;
#else
#include	"windows.h"
#endif
static inline
std::complex<float> cmul (std::complex<float> val, float mult) {
	return std::complex<float> (real (val) * mult,
	                            imag (val) * mult);
}

static inline
std::complex<float> cdiv (std::complex<float> val, float div) {
	return std::complex<float> (real (val) / div,
	                            imag (val) / div);
}

#define	MHz(x)	(1000000 * x)
#define	KHz(x)	(1000 * x)

#endif
