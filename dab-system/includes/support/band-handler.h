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

#ifndef	__BANDHANDLER__
#define	__BANDHANDLER__
#include	<cstdint>
#include	<QComboBox>
#include	<QObject>
#include	<QString>
#include	<QSettings>
#include	<stdio.h>
//
//	a simple convenience class
//

typedef struct {
	QString key;
	int	fKHz;
} dabFrequencies;

class bandHandler: public QObject {
Q_OBJECT
public:
		bandHandler	(QSettings *);
		~bandHandler	();
	void    saveSettings	();
	void	setupChannels	(QComboBox *s);
	int32_t Frequency	(QString Channel);

private:
};
#endif

