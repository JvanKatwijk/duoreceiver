#
/*
 *    Copyright (C) 2013 .. 2017
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
#
#ifndef	__PHASEREFERENCE__
#define	__PHASEREFERENCE__
#include	<QObject>
#include	<cstdio>
#include	<cstdint>
#include	<vector>
#include	"fft.h"
#include	"phasetable.h"
#include	"dab-constants.h"
#include	"dab-params.h"
#include	"process-params.h"
#include	"ringbuffer.h"

class	duoReceiver;

class phaseReference : public QObject, public phaseTable {
Q_OBJECT
public:
			phaseReference 		(duoReceiver *);
			~phaseReference();
	int32_t		findIndex		(std::vector<std::complex<float>>, int);
	int16_t		estimate_CarrierOffset	(std::vector<std::complex<float>>);
	float		estimate_FrequencyOffset (std::vector<std::complex<float>>);
//
	float		phase			(std::vector<std::complex<float>>, int);
//	This one is used in the ofdm decoder
	std::vector<std::complex<float>> refTable;
private:
	dabParams	params;
	common_fft	my_fftHandler;
	RingBuffer<float> *response;
	std::vector<float> phaseDifferences;
	int16_t		diff_length;
	int16_t		depth;
	int32_t		T_u;
	int32_t		T_g;
	int16_t		carriers;

	std::complex<float>	*fft_buffer;
	int32_t		fft_counter;
	int32_t		framesperSecond;	
	int32_t		displayCounter;
signals:
	void		showCorrelation	(int, int);
};
#endif

