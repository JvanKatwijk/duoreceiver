#
/*
 *    Copyright (C) 2013
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
 ************************************************************************
 *	may 15 2015. A real improvement on the code
 *	is the addition from Stefan Poeschel to create a
 *	header for the aac that matches, really a big help!!!!
 *
 ************************************************************************
 */
#include	"mp4processor.h"
#include	"duoreceiver.h"
//
#include	<cstring>
#include	"charsets.h"
#include	"pad-handler.h"
//#include	"bitWriter.h"

//
/**
  *	\class mp4Processor is the main handler for the aac frames
  *	the class proper processes input and extracts the aac frames
  *	that are processed by the "faadDecoder" class
  */
	mp4Processor::mp4Processor (duoReceiver	*mr,
	                            int16_t	bitRate,
	                            RingBuffer<int16_t> *b,
	                            uint8_t	procMode)
	                               :my_padhandler (mr),
 	                                my_rsDecoder (8, 0435, 0, 1, 10) {

	myRadioInterface	= mr;
	this	-> procMode	= procMode;
	connect (this, SIGNAL (isStereo (bool)),
	         mr, SLOT (setStereo (bool)));
	aacDecoder		= new faadDecoder (mr, b);
	this	-> bitRate	= bitRate;	// input rate

	superFramesize		= 110 * (bitRate / 8);
	RSDims			= bitRate / 8;
	frameBytes. resize (RSDims * 120);	// input
	outVector . resize (RSDims * 110);
	blockFillIndex	= 0;
	blocksInBuffer	= 0;
	frameCount	= 0;
}

	mp4Processor::~mp4Processor() {
	delete aacDecoder;
}

/**
  *	\brief addtoFrame
  *
  *	a DAB+ superframe consists of 5 consecutive DAB frames 
  *	we add vector for vector to the superframe. Once we have
  *	5 lengths of "old" frames, we check
  *	Note that the packing in the entry vector is still one bit
  *	per Byte, nbits is the number of Bits (i.e. containing bytes)
  *	the function adds nbits bits, packed in bytes, to the frame
  */
void	mp4Processor::addtoFrame (std::vector<uint8_t> V) {
int16_t	i, j;
uint8_t	temp	= 0;
int16_t	nbits	= 24 * bitRate;

	for (i = 0; i < nbits / 8; i ++) {	// in bytes
	   temp = 0;
	   for (j = 0; j < 8; j ++)
	      temp = (temp << 1) | (V [i * 8 + j] & 01);
	   frameBytes [blockFillIndex * nbits / 8 + i] = temp;
	}
//
	blocksInBuffer ++;
	blockFillIndex = (blockFillIndex + 1) % 5;
//
/**
  *	starting for real: check the fire code
  *	if the firecode is OK, we handle the frame
  *	and adjust the buffer here for the next round
  */
	if (fc. check (&frameBytes [blockFillIndex * nbits / 8]) &&
	       (processSuperframe (frameBytes. data(),
	                           blockFillIndex * nbits / 8))) {
//	since we processed a full cycle of 5 blocks, we just start a
//	new sequence, beginning with block blockFillIndex
	   blocksInBuffer	= 0;
	}
	else {
/**
  *	we were wrong, virtual shift to left in block sizes
  */
	   blocksInBuffer  = 4;
	}
}

/**
  *	\brief processSuperframe
  *
  *	First, we know the firecode checker gave green light
  *	We correct the errors using RS
  */
bool	mp4Processor::processSuperframe (uint8_t frameBytes [], int16_t base) {
uint8_t		num_aus;
int16_t		i, j, k;
uint8_t		rsIn	[120];
uint8_t		rsOut	[110];
int		tmp;
stream_parms    streamParameters;

/**
  *	apply reed-solomon error repar
  *	OK, what we now have is a vector with RSDims * 120 uint8_t's
  *	the superframe, containing parity bytes for error repair
  *	take into account the interleaving that is applied.
  */
	for (j = 0; j < RSDims; j ++) {
	   int16_t ler	= 0;
	   for (k = 0; k < 120; k ++) 
	      rsIn [k] = frameBytes [(base + j + k * RSDims) % (RSDims * 120)];
	   ler = my_rsDecoder. dec (rsIn, rsOut, 135);
	   if (ler < 0) {
//	      fprintf (stderr, "RS failure\n");
	      return false;
	   }

	   for (k = 0; k < 110; k ++) 
	      outVector [j + k * RSDims] = rsOut [k];
	}

//	bits 0 .. 15 is firecode
//	bit 16 is unused
	streamParameters. dacRate = (outVector [2] >> 6) & 01;	// bit 17
	streamParameters. sbrFlag = (outVector [2] >> 5) & 01;	// bit 18
	streamParameters. aacChannelMode
	                          = (outVector [2] >> 4) & 01;	// bit 19
	streamParameters. psFlag  = (outVector [2] >> 3) & 01;	// bit 20
	streamParameters. mpegSurround	= (outVector [2] & 07);	// bits 21 .. 23
//
//	added for the aac file writer
	streamParameters. CoreSrIndex	=
	              streamParameters. dacRate ?
	                            (streamParameters. sbrFlag ? 6 : 3) :
	                            (streamParameters. sbrFlag ? 8 : 5);
	streamParameters. CoreChConfig	=
	              streamParameters. aacChannelMode ? 2 : 1;

	streamParameters. ExtensionSrIndex =
	              streamParameters. dacRate ? 3 : 5;

	switch (2 * streamParameters. dacRate + streamParameters. sbrFlag) {
	  default:		// cannot happen
	   case 0:
	      num_aus = 4;
	      au_start [0] = 8;
	      au_start [1] = outVector [3] * 16 + (outVector [4] >> 4);
	      au_start [2] = (outVector [4] & 0xf) * 256 +
	                      outVector [5];
	      au_start [3] = outVector [6] * 16 +
	                     (outVector [7] >> 4);
	      au_start [4] = 110 *  (bitRate / 8);
	      break;
//
	   case 1:
	      num_aus = 2;
	      au_start [0] = 5;
	      au_start [1] = outVector [3] * 16 +
	                     (outVector [4] >> 4);
	      au_start [2] = 110 *  (bitRate / 8);
	      break;
//
	   case 2:
	      num_aus = 6;
	      au_start [0] = 11;
	      au_start [1] = outVector [3] * 16 + (outVector [4] >> 4);
	      au_start [2] = (outVector [4] & 0xf) * 256 + outVector [ 5];
	      au_start [3] = outVector [6] * 16 + (outVector [7] >> 4);
	      au_start [4] = (outVector [7] & 0xf) * 256 + outVector [8];
	      au_start [5] = outVector [9] * 16 + (outVector [10] >> 4);
	      au_start [6] = 110 *  (bitRate / 8);
	      break;
//
	   case 3:
	      num_aus = 3;
	      au_start [0] = 6;
	      au_start [1] = outVector [3] * 16 + (outVector [4] >> 4);
	      au_start [2] = (outVector [4] & 0xf) * 256 + outVector [5];
	      au_start [3] = 110 * (bitRate / 8);
	      break;
	}
/**
  *	OK, the result is N * 110 * 8 bits (still single bit per byte!!!)
  *	extract the AU's, and prepare a buffer,  with the sufficient
  *	lengthy for conversion to PCM samples
  */
	for (i = 0; i < num_aus; i ++) {
	   int16_t	aac_frame_length;

///	sanity check 1
	   if (au_start [i + 1] < au_start [i]) {
//	      fprintf (stderr, "%d %d (%d)\n", au_start [i], au_start [i + 1], i);
//	should not happen, all errors were corrected
	      return false;
	   }

	   aac_frame_length = au_start [i + 1] - au_start [i] - 2;
//	just a sanity check
	   if ((aac_frame_length >=  960) || (aac_frame_length < 0)) {
//	      fprintf (stderr, "aac_frame_length = %d\n", aac_frame_length);
//	      return false;
	   }

//	but first the crc check
	   if (check_crc_bytes (&outVector [au_start [i]],
	                                aac_frame_length)) {
//
//	first handle the pad data if any
	      if (((outVector [au_start [i + 0]] >> 5) & 07) == 4) {
	         int16_t count = outVector [au_start [i] + 1];
	         uint8_t buffer [count];
	         memcpy (buffer, &outVector [au_start [i] + 2], count);
	         uint8_t L0	= buffer [count - 1];
	         uint8_t L1	= buffer [count - 2];
	         my_padhandler. processPAD (buffer, count - 3, L1, L0);
	      }
//
	      uint8_t theAudioUnit [2 * 960 + 10];	// sure, large enough

	      memcpy (theAudioUnit,
	                    &outVector [au_start [i]], aac_frame_length);
	      memset (&theAudioUnit [aac_frame_length], 0, 10);

	      tmp = aacDecoder -> MP42PCM (&streamParameters,
	                                      theAudioUnit,
	                                      aac_frame_length);
	      emit isStereo ((streamParameters. aacChannelMode == 1) ||
	                        (streamParameters. psFlag == 1));
	   }
	   else {
	      fprintf (stderr, "CRC failure with dab+ frame %d (%d)\n",
	                                          i, num_aus);
//
//	what would happen if the errors were in the 10 parity bytes
//	rather than in the 110 payload bytes?
	   }
	}
	return true;
}
