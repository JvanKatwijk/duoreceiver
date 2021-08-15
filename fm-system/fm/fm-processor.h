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
 */

#ifndef	__FM_PROCESSOR__
#define	__FM_PROCESSOR__

#include	<QThread>
#include	<QObject>
#include	<atomic>
#include	<complex>
#include	"common.h"
#include	"fir-filters.h"
#include	"ringbuffer.h"
#include	"fm-decoder.h"

class		duoReceiver;
class		deviceHandler;

class	fmProcessor:public QThread {
Q_OBJECT
public:
			fmProcessor (duoReceiver	*,
	                             deviceHandler *,
	                             RingBuffer<std::complex<float>> *);
	        	~fmProcessor ();
	void		stop		();		// stop the processor

	bool		isLocked	();
	void		reset_rds	();
private:
	duoReceiver	*my_fmDriver;
	deviceHandler	*theDevice;
	DecimatingFIR	decimatingFilter;
	fmDecoder	my_fmDecoder;
	void		run			(void);
	int32_t		inputRate;
	int32_t		fmRate;
	int32_t		decimatingScale;
	int16_t         convBufferSize;
        int16_t         convIndex;
        std::vector <std::complex<float> >   convBuffer;
        int16_t         mapTable_int   [1920];
        float           mapTable_float [1920];
	std::atomic<bool>	running;
	float		peakLevel;
	int16_t		peakLevelcnt;
	float		audioGain;
	float		audioGainAverage;
	float		max_freq_deviation;
	float		norm_freq_deviation;
signals:
	void		new_audioGain		(float);
};

#endif

