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
 *
 *	Once the bits are "in", interpretation and manipulation
 *	should reconstruct the data blocks.
 *	Ofdm_decoder is called for Block_0 and the FIC blocks,
 *	its invocation results in 2 * Tu bits
 */
#include	<vector>
#include	"ofdm-decoder.h"
#include	"duoreceiver.h"
//#include	"phasetable.h"
#include	"fic-handler.h"
//#include	"msc-handler.h"
#include	"freq-interleaver.h"
#include	"dab-params.h"

/**
  *	\brief ofdmDecoder
  *	The class ofdmDecoder is
  *	taking the data from the ofdmProcessor class in, and
  *	will extract the Tu samples, do an FFT and extract the
  *	carriers and map them on (soft) bits
  */
	ofdmDecoder::ofdmDecoder	(duoReceiver *mr) :
	                                    my_fftHandler (T_U),
	                                    myMapper (1) {
	this	-> myRadioInterface	= mr;
//
	fft_buffer			= my_fftHandler. getVector();
	phaseReference			.resize (T_U);
}

	ofdmDecoder::~ofdmDecoder() {
}
//
void	ofdmDecoder::stop() {
}

void	ofdmDecoder::reset() {
}

/**
  */
void	ofdmDecoder::processBlock_0 (
	                std::vector <std::complex<float> > buffer) {
	memcpy (fft_buffer, buffer. data(),
	                             T_U * sizeof (std::complex<float>));

	my_fftHandler. do_FFT();
/**
  *	we are now in the frequency domain, and we keep the carriers
  *	as coming from the FFT as phase reference.
  */
	memcpy (phaseReference. data (), fft_buffer,
	                   T_U * sizeof (std::complex<float>));
}
//
/**
  *	for the other blocks of data, the first step is to go from
  *	time to frequency domain, to get the carriers.
  *	we distinguish between FIC blocks and other blocks,
  *	only to spare a test. The mapping code is the same
  */

void	ofdmDecoder::decode (std::vector <std::complex<float>> buffer,
	                     int32_t blkno, int16_t *ibits) {
std::complex<float> conjVector [T_U];

	memcpy (fft_buffer, &((buffer. data()) [T_G]),
	                               T_U * sizeof (std::complex<float>));

//fftlabel:
/**
  *	first step: do the FFT
  */
	my_fftHandler. do_FFT();
/**
  *	a little optimization: we do not interchange the
  *	positive/negative frequencies to their right positions.
  *	The de-interleaving understands this
  */
//toBitsLabel:
/**
  *	Note that from here on, we are only interested in the
  *	"carriers", the useful carriers of the FFT output
  */
	for (int i = 0; i < CARRIERS; i ++) {
	   int16_t	index	= myMapper.  mapIn (i);
	   if (index < 0) 
	      index += T_U;
/**
  *	decoding is computing the phase difference between
  *	carriers with the same index in subsequent blocks.
  *	The carrier of a block is the reference for the carrier
  *	on the same position in the next block
  */
	   std::complex<float>	r1 = fft_buffer [index] *
	                                    conj (phaseReference [index]);
	   conjVector [index] = r1;
	   float ab1	= jan_abs (r1);
//	split the real and the imaginary part and scale it
//	we make the bits into softbits in the range -127 .. 127
	   ibits [i]		=  - real (r1) / ab1 * 127.0;
	   ibits [CARRIERS + i] =  - imag (r1) / ab1 * 127.0;
	}

	memcpy (phaseReference. data(), fft_buffer,
	                            T_U * sizeof (std::complex<float>));
}

