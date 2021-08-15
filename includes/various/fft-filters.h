#
/*
 *    Copyright (C) 2021
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
 *
 */
#ifndef	__FFT_FILTER__
#define	__FFT_FILTER__

#include	"common.h"
#include	"fft.h"
#include	<complex>

class	fftFilter {
public:
			fftFilter	(int32_t, int16_t);
			~fftFilter	(void);
	void		setBand		(int32_t, int32_t, int32_t);
	void		setSimple	(int32_t, int32_t, int32_t);
	void		setLowPass	(int32_t, int32_t);
	std::complex<float>	Pass	(std::complex<float>);

private:
	int32_t		fftSize;
	int16_t		filterDegree;
	int16_t		OverlapSize;
	int16_t		NumofSamples;
	common_fft	*MyFFT;
	std::complex<float>	*FFT_A;
	common_ifft	*MyIFFT;
	std::complex<float>	*FFT_C;
	common_fft	*FilterFFT;
	std::complex<float>	*filterVector;
	float	*RfilterVector;
	std::complex<float>	*Overloop;
	int32_t		inp;
};

#endif
