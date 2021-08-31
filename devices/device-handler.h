#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the duoreceiver
 *    duoereceiver is free software; you can redistribute it and/or modify
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
 *	We have to create a simple virtual class here, since we
 *	want the interface with different devices (including  filehandling)
 *	to be transparent
 */
#ifndef	__DEVICE_HANDLER__
#define	__DEVICE_HANDLER__

#include	<stdint.h>
#include	<complex>
#include	<QObject>
#include	<QThread>
#include	<QDialog>
#include	"common.h"

#define	NIX		0100
#define	SDRPLAY		0101
#define	DAB_STICK	0102
#define	EXTIO		0104
#define	AIRSPY		0110
#define	PMSDR		0111
#define	HACKRF		0114

//
//	in some cases we anly want to differentiate between sticks
//	and non-sticks

#define	someStick(x)	((x) & 03)

/**
  *	\class virtualInput
  *	base class for devices for the fm software
  *	The class is not completely virtual, since it is
  *	used as a default in case selecting a "real" class did not work out
  */
class	deviceHandler: public QThread {
Q_OBJECT
public:
			deviceHandler 	();
virtual			~deviceHandler 	();
virtual		bool	restartReader	(int32_t);
virtual		void	stopReader	();
virtual		void	setVFOFrequency	(int32_t);
virtual		int32_t	getVFOFrequency	();
virtual		int32_t	getSamples	(std::complex<float> *, int32_t);
virtual		int32_t	Samples		();
virtual		void	resetBuffer	();
};
#endif

