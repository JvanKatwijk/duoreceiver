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

#ifndef __AUDIO_SINK__
#define	__AUDIO_SINK__
#include	<QString>
#include	<complex>
#include	<stdio.h>
#include	<sndfile.h>
#include	<QComboBox>
#include	<QObject>
#include	<portaudio.h>
#include	"ringbuffer.h"

class	audioSink: public QObject {
Q_OBJECT
public:
	                audioSink		(int16_t);
			~audioSink		(void);
	bool		setupChannels		(QComboBox *);
	void		stop			();
	void		restart			();
	bool		selectDefaultDevice	(void);
	bool		selectDevice		(int16_t);
	void		audioOutput		(std::complex<float>);
public slots:
	bool		set_streamSelector	(int);
private:
	int16_t		numberofDevices		();
	QString		outputChannelwithRate	(int16_t, int32_t);
	int16_t		invalidDevice		(void);
	bool		isValidDevice		(int16_t);
	int32_t		cardRate		(void);

	bool		OutputrateIsSupported	(int16_t, int32_t);
	int32_t		CardRate;
	int16_t		latency;
	int32_t		size;
	bool		portAudio;
	int16_t		numofDevices;
	int		paCallbackReturn;
	int16_t		bufSize;
	PaStream	*ostream;
	RingBuffer<float>	*_O_Buffer;
	PaStreamParameters	outputParameters;

	int16_t		*outTable;
protected:
static	int		paCallback_o	(const void	*input,
	                                 void		*output,
	                                 unsigned long	framesperBuffer,
	                                 const PaStreamCallbackTimeInfo *timeInfo,
					 PaStreamCallbackFlags statusFlags,
	                                 void		*userData);
};

#endif

