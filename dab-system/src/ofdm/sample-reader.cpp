#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the duoreceiver
 *    duoreceiver-DAB is free software; you can redistribute it and/or modify
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
#
#include	"sample-reader.h"
#include	"duoreceiver.h"

static
std::complex<float> oscillatorTable [INPUT_RATE];

	sampleReader::sampleReader (duoReceiver *mr,
	                            deviceHandler *theDevice) {
	this	-> theDevice	= theDevice;
	currentPhase	= 0;
	sLevel		= 0;
        for (int i = 0; i < INPUT_RATE; i ++)
           oscillatorTable [i] = std::complex<float>
	                            (cos (2.0 * M_PI * i / INPUT_RATE),
                                     sin (2.0 * M_PI * i / INPUT_RATE));

	running. store (true);
}

	sampleReader::~sampleReader() {
}

void	sampleReader::setRunning (bool b) {
	running. store (b);
}

float	sampleReader::get_sLevel() {
	return sLevel;
}

std::complex<float> sampleReader::getSample (int32_t phaseOffset) {
std::complex<float> temp;

	if (!running. load())
	   throw 21;

	while ((theDevice -> Samples () < 1) && running. load()) {
	   usleep (10);
	}
	
	if (!running. load())	
	   throw 20;
//
//	so here, bufferContent > 0
	theDevice -> getSamples (&temp, 1);
//	OK, we have a sample!!
//	first: adjust frequency. We need Hz accuracy
	currentPhase	-= phaseOffset;
	currentPhase	= (currentPhase + INPUT_RATE) % INPUT_RATE;

	temp		*= oscillatorTable [currentPhase];
	sLevel		= 0.00001 * jan_abs (temp) + (1 - 0.00001) * sLevel;
	return temp;
}

void	sampleReader::getSamples (std::complex<float>  *v,
	                          int32_t n, int32_t phaseOffset) {
int32_t		i;

	if (!running. load())
	   throw 21;
	while ((theDevice -> Samples () < n) && running. load()) {
	   usleep (10);
	}

	if (!running. load())	
	   throw 20;
//
	n	= theDevice -> getSamples (v, n);
//	OK, we have samples!!
//	first: adjust frequency. We need Hz accuracy
	for (i = 0; i < n; i ++) {
	   currentPhase	-= phaseOffset;
//
//	Note that "phase" itself might be negative
	   currentPhase	= (currentPhase + INPUT_RATE) % INPUT_RATE;
	   v [i]	*= oscillatorTable [currentPhase];
	   sLevel	= 0.00001 * jan_abs (v [i]) + (1 - 0.00001) * sLevel;
	}
}

