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

#ifndef	__FM_DEMODULATOR__
#define	__FM_DEMODULATOR__

#include        <stdint.h>
#include        "Xtan2.h"
#include        "pllC.h"
#include	<complex>


class	fm_Demodulator {
private:
	int32_t		rateIn;
	float	fm_afc;
	float	fm_cvt;
	float	K_FM;
	pllC	myfm_pll;
public:
		fm_Demodulator	(int32_t	Rate_in,
	                         std::complex<float>	*Table,
	                         float	K_FM);
		~fm_Demodulator	(void);
	float	demodulate	(std::complex<float>);
	float	get_dcComponent	(void);
};
#endif

