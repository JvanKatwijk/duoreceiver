#
/*
 *    Copyright (C) 2015 .. 2020
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
#ifndef	__DAB_PROCESSOR__
#define	__DAB_PROCESSOR__
/*
 *	dabProcessor is the embodying of all functionality related
 *	to the actal DAB processing.
 */
#include	"dab-constants.h"
#include	<QThread>
#include	<QObject>
#include	<QByteArray>
#include	<QStringList>
#include	<vector>
#include	<cstdint>
#include	"sample-reader.h"
#include	"phasereference.h"
#include	"ofdm-decoder.h"
#include	"fic-handler.h"
#include	"msc-handler.h"
#include	"ringbuffer.h"
#include	"device-handler.h"
//

class	duoReceiver;
class	dabParams;
class	processParams;

class dabProcessor: public QThread {
Q_OBJECT
public:
		dabProcessor  	(duoReceiver *,
	                         deviceHandler	*,
	                         processParams *);
		~dabProcessor			();
	void		start			(int32_t);
	void		stop			();
//
//	inheriting from our delegates
//	for the ficHandler:
	QString		findService		(uint32_t, int);
	void		getParameters		(const QString &,
	                                         uint32_t *, int *);
	std::vector<serviceId>	getServices	(int);
	bool		is_audioService		(const QString &s);
	bool		is_packetService	(const QString &s);
        void		dataforAudioService     (const QString &,
	                                             audiodata *);
//	for the mscHandler
	void		reset_Services		();
	void		stopService		(descriptorType *);
	bool		set_audioChannel	(audiodata *,
	                                             RingBuffer<int16_t> *);
private:
	int		frequency;
	int		threshold;
	int		totalFrames;
	int		goodFrames;
	int		badFrames;
	dabParams	params;
	sampleReader	myReader;
//	duoreceiver	*myRadioInterface;
	ficHandler	my_ficHandler;
	mscHandler	my_mscHandler;
	phaseReference	phaseSynchronizer;
	ofdmDecoder	my_ofdmDecoder;

	int16_t		attempts;
	bool		scanMode;
	int32_t		T_null;
	int32_t		T_u;
	int32_t		T_s;
	int32_t		T_g;
	int32_t		T_F;
	int32_t		nrBlocks;
	int32_t		carriers;
	int32_t		carrierDiff;
	int16_t		fineOffset;
	int32_t		coarseOffset;
	QByteArray	transmitters;
	bool		correctionNeeded;
	std::vector<std::complex<float>	>ofdmBuffer;
	bool		wasSecond		(int16_t, dabParams *);
virtual	void		run();
signals:
	void		setSynced		(bool);
};
#endif

