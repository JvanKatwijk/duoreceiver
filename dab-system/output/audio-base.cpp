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

#include	"audio-base.h"
#include	<cstdio>

/*
 *	The class is the abstract sink for the data generated
 *	It will handle the "dumping" though and assume that 
 *	someone else will handle the PCM samples, all made
 *	on a rate of 48000
 */
	audioBase::audioBase(RingBuffer<std::complex<float>> *out):
	                              converter_16 (16000, 48000, 2 * 1600),
	                              converter_24 (24000, 48000, 2 * 2400),
	                              converter_32 (32000, 48000, 2 * 3200) {
	this	-> out	= out;
}

	audioBase::~audioBase() {
}

//
//	This one is a hack for handling different baudrates coming from
//	the aac decoder. call is from the GUI, triggered by the
//	aac decoder or the mp3 decoder
void	audioBase::audioOut	(int16_t *V, int32_t amount, int32_t rate) {
//int16_t V [rate / 5];

	switch (rate) {
	   case 16000:	
	      audioOut_16000 (V, amount / 2);
	      return;
	   case 24000:
	      audioOut_24000 (V, amount / 2);
	      return;
	   case 32000:
	      audioOut_32000 (V, amount / 2);
	      return;
	   default:
	   case 48000:
	      audioOut_48000 (V, amount / 2);
	      return;
	}
}
//
//	scale up from 16 -> 48
//	amount gives number of pairs
void	audioBase::audioOut_16000	(int16_t *V, int32_t amount) {
std::complex<float> outputBuffer [converter_16. getOutputsize()];
float      buffer       [2 * converter_16. getOutputsize()];
int16_t	i, j;
int32_t	result;

	for (i = 0; i < amount; i ++)
	   if (converter_16.
	            convert (std::complex<float> (V [2 * i] / 32767.0,
	                                          V [2 * i + 1] / 32767.0),
	                              outputBuffer, &result)) {
	      for (j = 0; j < result; j ++) {
	         buffer [2 * j    ] = real (outputBuffer [j]);
	         buffer [2 * j + 1] = imag (outputBuffer [j]);
	      }
	   
	      out -> putDataIntoBuffer (buffer, result);
	   }
}

//	scale up from 24000 -> 48000
//	amount gives number of pairs
void	audioBase::audioOut_24000	(int16_t *V, int32_t amount) {
std::complex<float> outputBuffer [converter_24. getOutputsize()];
float      buffer       [2 * converter_24. getOutputsize()];
int16_t	i, j;
int32_t	result;

	for (i = 0; i < amount; i ++)
	   if (converter_24.
	            convert (std::complex<float> (V [2 * i] / 32767.0,
	                                          V [2 * i + 1] / 32767.0),
	                              outputBuffer, &result)) {
	      for (j = 0; j < result; j ++) {
	         buffer [2 * j    ] = real (outputBuffer [j]);
	         buffer [2 * j + 1] = imag (outputBuffer [j]);
	      }
	   
	      out	-> putDataIntoBuffer (buffer, result);
	   }
}

//	scale up from 32000 -> 48000
//	amount is number of pairs
void	audioBase::audioOut_32000	(int16_t *V, int32_t amount) {
std::complex<float> outputBuffer [converter_32. getOutputsize()];
float      buffer       [2 * converter_32. getOutputsize()];
int32_t	i, j;
int32_t	result;

	for (i = 0; i < amount; i ++) {
	   if (converter_32.
	            convert (std::complex<float> (V [2 * i] / 32767.0,
	                                          V [2 * i + 1] / 32767.0),
	                              outputBuffer, &result)) {
	      for (j = 0; j < result; j ++) {
	         buffer [2 * j    ] = real (outputBuffer [j]);
	         buffer [2 * j + 1] = imag (outputBuffer [j]);
	      }
	   
	      out -> putDataIntoBuffer (buffer, result);
	   }
	}
}

void	audioBase::audioOut_48000	(int16_t *V, int32_t amount) {
float buffer [2 * amount];
int32_t	i;

	for (i = 0; i < amount; i ++) {
	   buffer [2 * i]	= V [2 * i] / 32767.0;
	   buffer [2 * i + 1]	= V [2 * i + 1] / 32767.0;
	}

	out	-> putDataIntoBuffer (buffer, amount);
}
//
