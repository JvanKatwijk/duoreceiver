#
/*
 *    Copyright (C) 2021
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
#ifndef	__PLL_CH__
#define	__PLL_CH__
/*
 */
#include	<complex>
#include	"Xtan2.h"


class	pllC {
private:
	std::complex<float>	*Table;
	std::complex<float>	pll_Delay;
	int32_t		rate;
	double		NcoPhase;
	double		phaseIncr;
	double		NcoHLimit;
	double		NcoLLimit;
	double		pll_Alpha;
	double		pll_Beta;
	double		phaseError;
	compAtan	myAtan;
	bool		locked;
public:
			pllC (int32_t	rate,
	                float freq, float lofreq, float hifreq,
	                float bandwidth,
	                std::complex<float> *Table);

		        ~pllC (void);

	void		do_pll 		(std::complex<float> signal);
	std::complex<float>	getDelay	();
	float		getPhaseIncr	();
	float		getNco		();
	float		getPhaseError	();
	bool		isLocked	();
};

#endif

