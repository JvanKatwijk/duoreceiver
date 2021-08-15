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
#ifndef	__MSC_HANDLER__
#define	__MSC_HANDLER__

#include	<QThread>
#include	<QWaitCondition>
#include	<QSemaphore>
#include	<QMutex>
#include	<atomic>
#include	<cstdio>
#include	<cstdint>
#include	<cstdio>
#include	<vector>
#include	"dab-constants.h"
#include	"dab-params.h"
#include        "fft.h"
#include        "ringbuffer.h"
#include        "phasetable.h"
#include        "freq-interleaver.h"

class	duoReceiver;
class	Backend;

class mscHandler: public QThread  {
public:
			mscHandler		(duoReceiver *, uint8_t);
			~mscHandler		();
	void		processBlock_0		(std::complex<float> *);
	void		process_Msc		(std::complex<float> *, int);
	bool		set_Channel		(descriptorType *,
	                                           RingBuffer<int16_t> *,
	                                           RingBuffer<uint8_t> *);
//
//	
	void		reset_Channel		();
	void		stopService		(descriptorType *);
	void		reset_Buffers		();
private:
	void		process_mscBlock	(std::vector<int16_t>, int16_t);
	duoReceiver	*myRadioInterface;
	RingBuffer<uint8_t>	*dataBuffer;
	dabParams	params;
	common_fft	my_fftHandler;
	std::complex<float>     *fft_buffer;
	std::vector<complex<float>>     phaseReference;

        interLeaver     myMapper;
	QMutex		locker;
	bool		audioService;
	std::vector<Backend *>theBackends;
	std::vector<int16_t> cifVector;
	int16_t		cifCount;
	int16_t		blkCount;
	std::atomic<bool> work_to_be_done;
	int16_t		BitsperBlock;
	std::vector<int16_t> ibits;

	int16_t		numberofblocksperCIF;
	int16_t		blockCount;
        void            processMsc	(int32_t n);
        QMutex          helper;
	int		nrBlocks;
        void            processBlock_0	();
        std::vector<std::vector<std::complex<float> > > command;
        int16_t         amount;
	void            run();
        QSemaphore      bufferSpace;
        QWaitCondition  commandHandler;
        std::atomic<bool>       running;
};

#endif


