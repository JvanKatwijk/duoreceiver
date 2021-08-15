#
/*
 *    Copyright (C) 2015 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *	  based on cuteSDR (c) M Wheatly 2011
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

#ifndef	__RDS_DECODER__
#define	__RDS_DECODER__

#include	<QObject>
#include	"rds-group.h"
#include	"rds-blocksynchronizer.h"
#include	"rds-groupdecoder.h"
#include	"fft.h"
#include	"iir-filters.h"
//#include	"sincos.h"

class	duoReceiver;

class	rdsDecoder : public QObject {
Q_OBJECT
public:
		rdsDecoder	(duoReceiver *, int32_t);
		~rdsDecoder	();
	void	doDecode	(float);
	void	reset		();
private:
	void			processBit	(bool);
	int32_t			sampleRate;
	duoReceiver		*myRadioInterface;
	RDSGroup		*my_rdsGroup;
	rdsBlockSynchronizer	*my_rdsBlockSync;
	rdsGroupDecoder		*my_rdsGroupDecoder;
	bool			prevBit;

	float			*rdsBuffer;
	float			*rdsKernel;
	int16_t			ip;
	int16_t			rdsfilterSize;
	float			Match		(float);
	BandPassIIR		*sharpFilter;
	float			rdsLastSyncSlope;
	float			rdsLastSync;
	float			rdsLastData;
	bool			previousBit;
	float			*syncBuffer;
	int16_t			p;
};

#endif

