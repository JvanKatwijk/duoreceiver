#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the duoreceiver
 *    duoreceiver is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    duofeceiver is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with duoreceiver; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
//
//
//	The band handler now manages the skipTable
//
#include	"band-handler.h"
#include	"dab-constants.h"
#include	"common.h"
#include	<stdio.h>

static
dabFrequencies frequencies_1 [] = {
{"5A",	174928},
{"5B",	176640},
{"5C",	178352},
{"5D",	180064},
{"6A",	181936},
{"6B",	183648},
{"6C",	185360},
{"6D",	187072},
{"7A",	188928},
{"7B",	190640},
{"7C",	192352},
{"7D",	194064},
{"8A",	195936},
{"8B",	197648},
{"8C",	199360},
{"8D",	201072},
{"9A",	202928},
{"9B",	204640},
{"9C",	206352},
{"9D",	208064},
{"10A",	209936},
{"10B", 211648},
{"10C", 213360},
{"10D", 215072},
{"11A", 216928},
{"11B",	218640},
{"11C",	220352},
{"11D",	222064},
{"12A",	223936},
{"12B",	225648},
{"12C",	227360},
{"12D",	229072},
{"13A",	230784},
{"13B",	232496},
{"13C",	234208},
{"13D",	235776},
{"13E",	237488},
{"13F",	239200},
{nullptr, 0}
};

	bandHandler::bandHandler (QSettings *s) {
	(void)s;
}
//
//	note that saving settings has to be done separately
	bandHandler::~bandHandler () {
}
//
//	The main program calls this once, the combobox will be filled
void	bandHandler::setupChannels (QComboBox *s) {
int16_t	i;
int16_t	c	= s -> count();

//	clear the fields in the comboBox
	for (i = 0; i < c; i ++) 
	   s	-> removeItem (c - (i + 1));
//
//	The table elements are by default all "+";
	for (int i = 0; frequencies_1 [i]. fKHz != 0; i ++)  {
	   s -> insertItem (i, frequencies_1 [i]. key, QVariant (i));
	}
}
//
//	find the frequency for a given channel in a given band
int32_t	bandHandler::Frequency (QString Channel) {
int32_t	tunedFrequency		= 0;
int	i;

	for (i = 0; frequencies_1 [i]. key != nullptr; i ++) {
	   if (frequencies_1 [i]. key == Channel) {
	      tunedFrequency	= KHz (frequencies_1 [i]. fKHz);
	      break;
	   }
	}

	if (tunedFrequency == 0)	// should not happen
	   tunedFrequency = KHz (frequencies_1 [0]. fKHz);

	return tunedFrequency;
}

