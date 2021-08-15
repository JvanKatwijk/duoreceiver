#
/*
 *    Copyright (C) 2015
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
#include	"fm-decoder.h"
#include	"duoreceiver.h"
#include	"fm-demodulator.h"
#include	"fft-filters.h"
#include	"fir-filters.h"
#include	"rds-decoder.h"
#include	"pllC.h"
#include	<stdlib.h>

#define	PILOT_FREQUENCY		19000
#define	LRDIFF_FREQUENCY	(2 * PILOT_FREQUENCY)
#define	RDS_FREQUENCY		(3 * PILOT_FREQUENCY)
#define	OMEGA_DEMOD		(2 * M_PI / currentRate)
#define	OMEGA_PILOT		PILOT_FREQUENCY * OMEGA_DEMOD
#define	OMEGA_RDS		RDS_FREQUENCY * OMEGA_DEMOD
#define	PILOT_DELAY		(FFT_SIZE - PILOTFILTER_SIZE) * OMEGA_PILOT

//	
//	In order to spread the load of fm processing over more than a single
//	thread we have 
//	- fmprocessor running in its thread, it will receive the
//	samples and decimate them
//	- fmdecoder running in its thread, it will decode the
//	fm signal on the lower rate return audio

	fmDecoder::fmDecoder (duoReceiver	*mr,
	                      RingBuffer<std::complex<float>> *audioBuffer,
	                      int32_t		currentRate) :
		                           sampleBuffer (8 * 32768),
	                                   audioDecimator (11,
	                                        12000 / 2, 192000, 4),
	                                   pilotBandFilter (FFT_SIZE,
	                                           PILOTFILTER_SIZE),
	                                   theHilbertFilter (HILBERT_SIZE,
	                                                currentRate / 2 - 1,
	                                                currentRate),
		                           rdsLowPassFilter (FFT_SIZE,
                                                             RDSLOWPASS_SIZE),
		                           rdsBandFilter (FFT_SIZE,
                                                             RDSLOWPASS_SIZE) {
int	i;
	this	-> myDriver	= mr;
	this	-> audioBuffer	= audioBuffer;
	this	-> currentRate	= currentRate;
	connect (this, SIGNAL (audiosamplesAvailable (int)),
	         mr, SLOT (fmAudio (int)));
	connect (this,  SIGNAL (showLocked (bool)),
	         mr, SLOT (showLocked (bool)));

	Table			= new std::complex<float> [currentRate];
	for (i = 0; i < currentRate; i ++)
	   Table [i] = std::complex<float> (cos (2 * M_PI * i / currentRate),
	                                    sin (2 * M_PI * i / currentRate));
	pilotBandFilter. setBand (PILOT_FREQUENCY - PILOT_WIDTH / 2,
		                            PILOT_FREQUENCY + PILOT_WIDTH / 2,
		                            currentRate);
        rdsLowPassFilter. setLowPass (RDS_WIDTH, currentRate);
        rdsBandFilter. setBand (RDS_FREQUENCY - RDS_WIDTH / 2,
	                        RDS_FREQUENCY + RDS_WIDTH / 2,
	                        currentRate);
//
//
//	the constant K_FM is still subject to many questions
	float	F_G	= 0.65 * currentRate / 2; // highest freq in message
	float	Delta_F	= 0.90 * currentRate / 2;
	float	B_FM	= 2 * (Delta_F + F_G);
	K_FM		= 2 * M_PI / currentRate * B_FM;
	myDemodulator		= new fm_Demodulator (192000,
	                                              Table, K_FM);

	pilotRecover		= new pllC (currentRate,
	                                    PILOT_FREQUENCY,
	                                    PILOT_FREQUENCY - 80,
	                                    PILOT_FREQUENCY + 80,
	                                    20,
	                                    Table);
	
	lrdiffFilter		= new lowpassFIR (5, 15000, currentRate);
	lrplusFilter		= new lowpassFIR (5, 15000, currentRate);

	current_rdsPhase	= 0;
	start ();
}

	fmDecoder::~fmDecoder () {
	running. store (false);
	while (!isFinished ())
	   usleep (100);
	
	delete	pilotRecover;
	delete	myDemodulator;
	delete	lrdiffFilter;
	delete	lrplusFilter;
	delete[] Table;
}

void	fmDecoder::stop	(void) {
	running. store (false);
	while (!isFinished ())
	   usleep (100);
}

void	fmDecoder::doDecode (std::complex<float> *v, int16_t size) {
	sampleBuffer. putDataIntoBuffer (v, size);
}

#define	SIZE	(8192)
void	fmDecoder::run (void) {
std::complex<float>	fmData [SIZE];
std::complex<float>	audioSample;
float		rdsSample;
int		decimationCount	= 0;
rdsDecoder	 my_rdsDecoder (myDriver, 192000 / RDS_DECIMATOR);
int32_t		totalAmount	= 0;
std::complex<float>	localBuffer [2048];

	running. store (true);
	try {
	   int16_t i;
	   int16_t	localBufferp	= 0;
//
//	we get SIZE samples in in app SIZE * 1000 / currentRate msec
//	for 8192 and 192000 that means app 50 msec
	   while (running. load ()) {
	      while (running. load () &&
	         sampleBuffer.  GetRingBufferReadAvailable () < SIZE)
	         usleep (20000);
	      if (!running. load ())
	        throw (22);
	      int16_t amount = 
	                sampleBuffer.  getDataFromBuffer (fmData, SIZE);
	      for (int i = 0; i < amount; i ++) {
	         do_stereo (fmData [i], &audioSample, &rdsSample);
//	It is assumed that the gui thread will deal with filtering etc
	         if (audioDecimator. Pass  (audioSample, &audioSample)) {
	            localBuffer [localBufferp ++] = audioSample;
	            if (localBufferp >= 2048) {
	               audioBuffer -> putDataIntoBuffer (localBuffer, 2048);
	               emit audiosamplesAvailable (2048);
	               localBufferp = 0;
	            }
	         }
	         if (++decimationCount >= RDS_DECIMATOR) {
	            my_rdsDecoder. doDecode (rdsSample);
	            decimationCount	= 0;
	         }
	      }
	      totalAmount += amount;
	      if (totalAmount > currentRate) {
	         totalAmount = 0;
	         showLocked (isLocked ()); 
	      }
	   }
	}
	catch (int e) {;}
}

void	fmDecoder::do_stereo (std::complex<float>	in,
	                      std::complex<float>	*audioOut,
	                      float	*rdsValue) {

float	LRPlus	= 0;
std::complex<float>	LRDiff;
std::complex<float>	pilot;
std::complex<float>	rdsBase;
float	currentPilotPhase;

//	first step: get the demodulated signal
//
//	It consists of LRPlus, pilot, LRDiff and rds components
	float demod		= myDemodulator -> demodulate (in);
	LRPlus			= demod;
	rdsBase	=
	pilot	=
	LRDiff	= 		theHilbertFilter. Pass (demod, demod);
/*
 *	get the phase for the "carrier to be inserted" right
 */
	pilot			= pilotBandFilter. Pass (pilot);
	pilotRecover		-> do_pll (pilot);
	currentPilotPhase	= pilotRecover -> getNco ();

//	for the actual phase we should take into account
//	the delay caused by the FIR bandfilter
	currentPilotPhase	+= PILOT_DELAY;
//
//	shift the LRDiff signal down to baseband	38Khz
	int32_t thePhase	= currentPilotPhase / (2 * M_PI) * currentRate;
	LRDiff  *= conj (Table [(2 * thePhase) % currentRate]);

//	get rid of junk
	LRDiff			= lrdiffFilter -> Pass (LRDiff);
//	.... and for the LplusR as well
	LRPlus			= lrplusFilter -> Pass (LRPlus);
	float v			= imag (LRDiff) - real (LRDiff);
	*audioOut		= std::complex<float> (LRPlus, 2 * v);

	rdsBase *= conj (Table [(3 * thePhase) % currentRate]);
        *rdsValue = 10 * imag (rdsLowPassFilter. Pass (rdsBase));
}

bool	fmDecoder::isLocked (void) {
	if (!running. load ())
	   return false;
	return (pilotRecover -> isLocked ());
}

