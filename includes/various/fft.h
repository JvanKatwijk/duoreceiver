#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the duoreceiver
 *    duoreceiver is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    duoreceiver is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with duoreceiver; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _COMMON_FFT__
#define	_COMMON_FFT__

//
//	Simple wrapper around fftw
#include	<stdio.h>
#include	<complex>
//
#define	FFTW_MALLOC		fftwf_malloc
#define	FFTW_PLAN_DFT_1D	fftwf_plan_dft_1d
#define FFTW_DESTROY_PLAN	fftwf_destroy_plan
#define	FFTW_FREE		fftwf_free
#define	FFTW_PLAN		fftwf_plan
#define	FFTW_EXECUTE		fftwf_execute
#include	<fftw3.h>
/*
 *	a simple wrapper
 */

class	common_fft {
public:
			common_fft	(int32_t);
			~common_fft	(void);
	std::complex<float>	*getVector	();
	void		do_FFT		();
	void		do_IFFT		();
	void		do_Shift	();
private:
	int32_t		fftSize;
	std::complex<float>	*vector;
	std::complex<float>	*vector1;
	FFTW_PLAN	plan;
	void		Scale		(std::complex<float> *);
};

class	common_ifft {
public:
			common_ifft	(int32_t);
			~common_ifft	(void);
	std::complex<float>	*getVector	(void);
	void		do_IFFT		(void);
private:
	int32_t		fftSize;
	std::complex<float>	*vector;
	FFTW_PLAN	plan;
	void		Scale		(std::complex<float> *);
};

#endif

