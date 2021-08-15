#
/*
 *    Copyright (C) 2015 .. 2017
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
#ifndef	__FM_DECODER__
#define	__FM_DECODER__
#include	<QThread>
#include	<complex>
#include	<atomic>
#include	"common.h"
#include	"ringbuffer.h"
#include	"fir-filters.h"
#include        "fft-filters.h"
#include	"rds-decoder.h"

class	duoReceiver;
class	lowpassFIR;
class	fftFilter;
class	HilbertFilter;
class	pllC;
class	fm_Demodulator;

#define RDS_DECIMATOR           8
#define PILOTFILTER_SIZE        35
#define RDSLOWPASS_SIZE         11
#define HILBERT_SIZE            7
#define RDSBANDFILTER_SIZE      35
#define FFT_SIZE                512
#define PILOT_WIDTH             100
#define RDS_WIDTH               1500
#define LEVEL_SIZE              512
#define LEVEL_FREQ              3
//
#define FM_MONO                 0101
#define FM_STEREO               0102

class fmDecoder: public QThread  {
Q_OBJECT
public:
		fmDecoder	(duoReceiver *,
	                         RingBuffer<std::complex<float>> *,
	                         int32_t);
		~fmDecoder	();
	void	doDecode	(std::complex<float> *, int16_t);
	bool	isLocked	();
	void	stop		();
private:
	duoReceiver	*myDriver;
	RingBuffer<std::complex<float>> *audioBuffer;
	int32_t		currentRate;

	RingBuffer<std::complex<float>> sampleBuffer;
	DecimatingFIR	audioDecimator;
	fftFilter	rdsBandFilter;
	fftFilter	rdsLowPassFilter;
	fftFilter	pilotBandFilter;
	HilbertFilter	theHilbertFilter;
//	rdsDecoder	my_rdsDecoder;

	lowpassFIR	*lrdiffFilter;
	lowpassFIR	*lrplusFilter;
	void		run		(void);
	void		do_stereo	(std::complex<float>,
	                                 std::complex<float> *, float *);
	fm_Demodulator	*myDemodulator;
	pllC		*pilotRecover;
	std::atomic<bool>	running;
	std::complex<float>	*Table;
	float		K_FM;
	float		current_rdsPhase;
	std::complex<float>	*localTable;
	int32_t		localPhase_1;
	int32_t		localPhase_2;
signals:
	void	audiosamplesAvailable	(int);
	void	showLocked		(bool);
};
#endif

