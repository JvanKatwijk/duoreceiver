#
/*
 *    Copyright (C) 2010, 2011, 2012
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
#include	"fm-processor.h"
#include	"duoreceiver.h"
#include	"device-handler.h"

#define	AUDIO_FREQ_DEV_PROPORTION 0.85f

//
	fmProcessor::fmProcessor (duoReceiver		*RI,
	                          deviceHandler		*vi,
	                          RingBuffer<std::complex<float>> *audioSamples):	
	                          decimatingFilter (15, 192000 / 2, 
	                                            2048000, 10),
	                          my_fmDecoder (RI, audioSamples, 192000) {
	running. store (false);
	this	-> theDevice		= vi;
	this	-> my_fmDriver		= RI;
	this	-> inputRate		= 2048000;
	this	-> fmRate		= 192000;

	this	-> peakLevel		= -100;
	this	-> peakLevelcnt		= 0;
	this	-> max_freq_deviation	= 0.95 * (0.5 * fmRate);
	this	-> norm_freq_deviation	= 0.5 * max_freq_deviation;

	this	-> audioGain		= 1;
	this	-> audioGainAverage	= 1;
	connect (this, SIGNAL (new_audioGain (float)),
	         my_fmDriver, SLOT (new_audioGain (float)));

//	the final decimation step, from 204800 -> 192000 is done
//	by interpolation
	float inval = 2048.0;
	convBufferSize	= 2048;
	for (int i = 0; i < 1920; i ++) {
	   mapTable_int [i] = int (floor (i * (inval / 1920.0)));
	   mapTable_float [i] = i * (inval / 1920.0) - mapTable_int [i];
	}
	convIndex	= 0;
	convBuffer. resize (convBufferSize + 1);
}

	fmProcessor::~fmProcessor (void) {
	if (running. load () && isRunning ()) {
	   running. store (false);
	   while (!isFinished ())
	      usleep (100);
	}
}

void	fmProcessor::stop () {
	if (running. load () && isRunning ()) {
           running. store (false);
           while (!isFinished ())
              usleep (100);
        }
}

//	We have a separate thread for the fm processing

#define	bufferSize	8192
void	fmProcessor::run (void) {
std::complex<float>	dataBuffer	[bufferSize];
std::complex<float>	passBuffer	[1920];

	running. store (true);		// will be set to false elsewhere
	try {
	   int16_t i;
	   std::complex<float> v;
	   while (running. load ()) {
	      while (running. load () &&
	                         (theDevice -> Samples () < bufferSize)) 
	         usleep (100);	// should be enough

	      if (!running. load ())
	         throw (22);

//	This is what we get in, first thing: decimating
//	which - by the way - is a pretty resource consuming operation
	      int32_t amount = theDevice -> getSamples (dataBuffer, bufferSize);
	      for (i = 0; i < amount; i ++) {
	         v =  std::complex<float> (real (dataBuffer [i]) * 100,
	                                   imag (dataBuffer [i]) * 100);
	         if (!decimatingFilter. Pass (v, &v))
	            continue;
	         convBuffer [convIndex ++] = v;
	         if (convIndex > convBufferSize) {
	            for (int j = 0; j < 1920; j ++) {
                       int16_t  inpBase    = mapTable_int [j];
                       float    inpRatio   = mapTable_float [j];
                       passBuffer [j]    =
	                          cmul (convBuffer [inpBase + 1], inpRatio) +
                                  cmul (convBuffer [inpBase], 1 - inpRatio);

	               if (abs (passBuffer [j]) > peakLevel)
	                  peakLevel = abs (passBuffer [j]);
	               if (++ peakLevelcnt >= fmRate / 2) {
	                  float	ratio	= 
	                          max_freq_deviation / norm_freq_deviation;
	                  if (peakLevel > 0)
	                     this -> audioGain	= 
	                        (ratio / peakLevel) / AUDIO_FREQ_DEV_PROPORTION;
	                  if (audioGain <= 0.05)
	                     audioGain = 0.05;
	                  audioGain	= 0.99 * audioGainAverage + 0.01 * audioGain;
	                  audioGainAverage = audioGain;
	                  peakLevelcnt	= 0;
	                  peakLevel	= -100;
	                  emit new_audioGain (audioGain);
	               }
	            }
	            convBuffer [0] = convBuffer [convBufferSize];
	            convIndex = 1;
//
//	decoding will be done in a separate thread
	            my_fmDecoder. doDecode (passBuffer, 1920);
	         }
//
	      } // end main loop
	   } // end "running" loop
	   fprintf (stderr, "normal termination\n");
	}
	catch (int e) {;}
}

bool	fmProcessor::isLocked (void) {
	return false;
}

void	fmProcessor::reset_rds	(void) {
}

