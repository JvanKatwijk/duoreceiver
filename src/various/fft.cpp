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
#include	"fft.h"
#include	<cstring>
/*
 */

	common_fft::common_fft (int32_t fftSize) {
int32_t	i;

	this	-> fftSize = fftSize;

	vector	= (std::complex<float> *)
	           FFTW_MALLOC (sizeof (std::complex<float>) * fftSize);
	for (i = 0; i < fftSize; i ++)
	   vector [i] = 0;
	plan	= FFTW_PLAN_DFT_1D (fftSize,
	                            reinterpret_cast <fftwf_complex *>(vector),
	                            reinterpret_cast <fftwf_complex *>(vector),
	                            FFTW_FORWARD, FFTW_ESTIMATE);
}

	common_fft::~common_fft () {
	   FFTW_DESTROY_PLAN (plan);
	   FFTW_FREE (vector);
}

std::complex<float>	*common_fft::getVector () {
	return vector;
}

void	common_fft::do_FFT () {
	FFTW_EXECUTE (plan);
}

void	common_fft::do_IFFT () {
        for (int i = 0; i < fftSize; i ++)
           vector [i] = conj (vector [i]);
        FFTW_EXECUTE (plan);
        for (int i = 0; i < fftSize; i ++)
           vector [i] = conj (vector [i]);
}

void	common_fft::do_Shift (void) {
std::complex<float>	*v = (std::complex<float> *)alloca (fftSize * sizeof (std::complex<float>));

	memcpy (v, vector, fftSize * sizeof (std::complex<float>));
	memcpy (vector, &v [fftSize / 2], fftSize / 2 * sizeof (std::complex<float>));
	memcpy (&vector [fftSize / 2], v, fftSize / 2 * sizeof (std::complex<float>));
}
	
void	common_fft::Scale (std::complex<float> *Data) {
const float  Factor = 1.0 / float (fftSize);
int32_t	Position;

	// scale all entries
	for (Position = 0; Position < fftSize; Position ++)
	   Data [Position] *= Factor;
}

/*
 * 	and a wrapper for the inverse transformation
 */
	common_ifft::common_ifft (int32_t fftSize) {
int32_t	i;

//	if ((fftSize & (fftSize - 1)) == 0)
	   this	-> fftSize = fftSize;
//	else
//	   this -> fftSize = 4096;	/* just a default	*/

	vector	= (std::complex<float> *)FFTW_MALLOC (sizeof (std::complex<float>) * fftSize);
	for (i = 0; i < fftSize; i ++)
	   vector [i] = 0;
	plan	= FFTW_PLAN_DFT_1D (fftSize,
	                            reinterpret_cast <fftwf_complex *>(vector),
	                            reinterpret_cast <fftwf_complex *>(vector),
	                            FFTW_BACKWARD, FFTW_ESTIMATE);
}

	common_ifft::~common_ifft () {
	   FFTW_DESTROY_PLAN (plan);
	   FFTW_FREE (vector);
}

std::complex<float>	*common_ifft::getVector () {
	return vector;
}

void	common_ifft::do_IFFT () {
	FFTW_EXECUTE	(plan);
	Scale		(vector);
}

void	common_ifft::Scale (std::complex<float> *Data) {
const float  Factor = 1.0 / float (fftSize);
int32_t	Position;

	// scale all entries
	for (Position = 0; Position < fftSize; Position ++)
	   Data [Position] *= Factor;
}


