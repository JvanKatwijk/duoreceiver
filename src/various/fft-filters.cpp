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
 */

#include	"fft-filters.h"
#include	"fir-filters.h"
#include	<cstring>

	fftFilter::fftFilter (int32_t size, int16_t degree) {
int32_t	i;

	fftSize		= size;
	filterDegree	= degree;
	OverlapSize	= filterDegree;
	NumofSamples	= fftSize - OverlapSize;

	MyFFT		= new common_fft	(fftSize);
	FFT_A		= MyFFT		-> 	getVector ();
	MyIFFT		= new common_ifft	(fftSize);
	FFT_C		= MyIFFT	->	getVector ();

	FilterFFT	= new common_fft	(fftSize);
	filterVector	= FilterFFT	->	getVector ();
	RfilterVector	= new float [fftSize];

	Overloop	= new std::complex<float> [OverlapSize];
	inp		= 0;
	for (i = 0; i < fftSize; i ++) {
	   FFT_A [i] = 0;
	   FFT_C [i] = 0;
	   filterVector [i] = 0;
	   RfilterVector [i] = 0;
	}
}

	fftFilter::~fftFilter () {
	delete		MyFFT;
	delete		MyIFFT;
	delete		FilterFFT;
	delete[]	RfilterVector;
	delete[]	Overloop;
}

void	fftFilter::setSimple (int32_t low, int32_t high, int32_t rate) {
int32_t i;
BasicBandPass	*BandPass	= new BasicBandPass ((int16_t)filterDegree,
	                                             low, high, rate);

	for (i = 0; i < filterDegree; i ++)
	   filterVector [i] = (BandPass -> getKernel ()) [i];
	memset (&filterVector [filterDegree], 0,
	                (fftSize - filterDegree) * sizeof (std::complex<float>));
	FilterFFT	-> do_FFT ();
	inp		= 0;
	delete	BandPass;
}

void	fftFilter::setBand (int32_t low, int32_t high, int32_t rate) {
int32_t	i;
bandpassFIR	*BandPass	= new bandpassFIR ((int16_t)filterDegree,
	                                           low, high,
	                                           rate);

	for (i = 0; i < filterDegree; i ++)
	   filterVector [i] = (BandPass -> getKernel ()) [i];
//	   filterVector [i] = conj ((BandPass -> getKernel ()) [i]);
	memset (&filterVector [filterDegree], 0,
	                (fftSize - filterDegree) * sizeof (std::complex<float>));
	FilterFFT	-> do_FFT ();
	inp		= 0;
	delete	BandPass;
}

void	fftFilter::setLowPass (int32_t low, int32_t rate) {
int32_t	i;
lowpassFIR	*LowPass	= new lowpassFIR ((int16_t)filterDegree,
	                                          low,
	                                          rate);

	for (i = 0; i < filterDegree; i ++)
	   filterVector [i] = (LowPass -> getKernel ()) [i];
	memset (&filterVector [filterDegree], 0,
	                (fftSize - filterDegree) * sizeof (std::complex<float>));
	FilterFFT	-> do_FFT ();
	inp	= 0;
	delete LowPass;
}

std::complex<float>	fftFilter::Pass (std::complex<float> z) {
std::complex<float>	sample;
int16_t		j;

	sample	= FFT_C [inp];
	FFT_A [inp] = std::complex<float> (real (z), imag (z));

	if (++inp >= NumofSamples) {
	   inp = 0;
	   memset (&FFT_A [NumofSamples], 0,
	               (fftSize - NumofSamples) * sizeof (std::complex<float>));
	   MyFFT	-> do_FFT ();

	   for (j = 0; j < fftSize; j ++) 
	      FFT_C [j] = FFT_A [j] * filterVector [j];

	   MyIFFT	-> do_IFFT ();
	   for (j = 0; j < OverlapSize; j ++) {
	      FFT_C [j] += Overloop [j];
	      Overloop [j] = FFT_C [NumofSamples + j];
	   }
	}

	return sample;
}


