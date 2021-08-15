#
/*
 *    Copyright (C)  2014
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

#include	"fm-demodulator.h"
#include	"common.h"
#include	<stdlib.h>

#define	DCAlpha	0.0001
//
	fm_Demodulator::fm_Demodulator (int rateIn,
	                                std::complex<float> *theTable,
	                                 float K_FM):
	                                      myfm_pll (rateIn, 
	                                      0, 
	                                      -0.95 * (rateIn / 2),
	                                       0.95 * (rateIn / 2),
	                                       0.85 * rateIn,
	                                       theTable) {
	this	-> fm_afc	= 0;
	this	-> fm_cvt	= 0.90 * (rateIn / (M_PI * 150000));
	this	-> K_FM		= K_FM;
}

		fm_Demodulator::~fm_Demodulator (void) {
}

float	fm_Demodulator::demodulate (std::complex<float> z) {
float	res;		
	if (abs (z) <= 0.001)
	   z	= std::complex<float> (0.001, 0.001);
	else 
	   z	= cdiv (z, abs (z));
	myfm_pll. do_pll (z);
	res = myfm_pll. getPhaseIncr ();
	fm_afc	= (1 - DCAlpha) * fm_afc + DCAlpha * res;
	res	= (res - fm_afc) * fm_cvt;
	return res;
}

float	fm_Demodulator::get_dcComponent (void) {
	return fm_afc;
}

