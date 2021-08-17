#
/*
 *    Copyright (C) 2014 .. 2017
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
#ifndef	__MP4PROCESSOR__
#define	__MP4PROCESSOR__
/*
 * 	Handling superframes for DAB+ and delivering
 * 	frames into the ffmpeg or faad decoding library
 */
//
#include	"dab-constants.h"
#include	<cstdio>
#include	<cstdint>
#include	<vector>
#include	"audio-base.h"
#include	"frame-processor.h"
#include	"firecode-checker.h"
#include	"reed-solomon.h"
#include	<QObject>
#include	"pad-handler.h"

#ifdef	__WITH_FDK_AAC__
#include	"fdk-aac.h"
#else
#include	"faad-decoder.h"
#endif

class	duoReceiver;

class	mp4Processor : public QObject, public frameProcessor {
Q_OBJECT
public:
			mp4Processor	(duoReceiver *,
	                                 int16_t,
	                                 RingBuffer<int16_t> *,
	                                 uint8_t procMode = 1);
			~mp4Processor();
	void		addtoFrame	(std::vector<uint8_t>);
private:
	duoReceiver	*myRadioInterface;
	padHandler	my_padhandler;
	bool		processSuperframe (uint8_t [], int16_t);
	uint8_t		procMode;
	int16_t		superFramesize;
	int16_t		blockFillIndex;
	int16_t		blocksInBuffer;
	int16_t         frameCount;
        int16_t         frameErrors;
        int16_t         rsErrors;
        int16_t         aacErrors;
        int16_t         aacFrames;
        int16_t         successFrames;
        int16_t         charSet;
	int		goodFrames;
	int		totalCorrections;
	int16_t		bitRate;
	std::vector<uint8_t> frameBytes;
	std::vector<uint8_t> outVector;
	int16_t		RSDims;
	int16_t		au_start	[10];
	firecode_checker	fc;
	reedSolomon	my_rsDecoder;
//	and for the aac decoder
	faadDecoder	*aacDecoder;
signals:
	void		isStereo	(bool);
	void		newFrame	(bool);
};

#endif


