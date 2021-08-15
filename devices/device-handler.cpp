#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J.
 *
 *    SDR-J is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    SDR-J is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * 	Default () implementation of
 * 	virtual input class
 */
#include	"device-handler.h"

	deviceHandler::deviceHandler () {
}

	deviceHandler::~deviceHandler () {
}

bool	deviceHandler::restartReader	(int32_t freq) {
	(void) freq;
	return false;
}

void	deviceHandler::stopReader	() {
}


void	deviceHandler::setVFOFrequency (int32_t f) {
	(void)f;
}

int32_t	deviceHandler::getVFOFrequency	() {
	return -1;
}

int32_t	deviceHandler::getSamples	(std::complex<float> *v,
	                                               int32_t amount) {
	(void)v; 
	(void)amount; 
	return 0;
}

int32_t	deviceHandler::Samples		() {
	return 0;
}

void	deviceHandler::resetBuffer	() {
}

