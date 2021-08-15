#
/*
 *    Copyright (C) 2015 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the  duoreceiver
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
#ifndef	__KEYPAD__
#define	__KEYPAD__
#include	<QWidget>
#include	<QGridLayout>
#include	<QButtonGroup>
#include	<QPushButton>
#include	<QLCDNumber>

class	duoReceiver;

class	keyPad:QObject {
Q_OBJECT
public:
		keyPad		(duoReceiver *);
		~keyPad		();
	void	showPad		();
	void	hidePad		();
	bool	isVisible	();
private slots:
	void	collectData	(int);
private:
	duoReceiver	*myRadio;
	QWidget		*theFrame;
	QGridLayout	*theLayout;
	QButtonGroup	*thePad;
	QPushButton		*zeroButton;
	QPushButton		*oneButton;
	QPushButton		*twoButton;
	QPushButton		*threeButton;
	QPushButton		*fourButton;
	QPushButton		*fiveButton;
	QPushButton		*sixButton;
	QPushButton		*sevenButton;
	QPushButton		*eightButton;
	QPushButton		*nineButton;
	QPushButton		*KHzButton;
	QPushButton		*MHzButton;
	QPushButton		*clearButton;
	QPushButton		*correctButton;
	QLCDNumber		*theDisplay;
	int		panel;
	bool		shown;
signals:
	void		newFrequency	(int);
};

#endif

