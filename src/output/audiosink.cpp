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
 *    duoreceiver is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with duoreceiver; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	"audiosink.h"
#include	<stdio.h>
#include	<QDebug>
#include	<QMessageBox>
/*
 */
	audioSink::audioSink	(int16_t latency) {
int32_t	i;
	this	-> latency	= latency;
	this	-> CardRate	= 48000;
	this	-> latency	= latency;
	_O_Buffer		= new RingBuffer<float>(2 * 32768);
	portAudio		= false;
	if (Pa_Initialize () != paNoError) {
	   fprintf (stderr, "Initializing Pa for output failed\n");
	   return;
	}

	portAudio	= true;
	qDebug ("Hostapis: %d\n", Pa_GetHostApiCount ());

	for (i = 0; i < Pa_GetHostApiCount (); i ++)
	   qDebug ("Api %d is %s\n", i, Pa_GetHostApiInfo (i) -> name);

	numofDevices	= Pa_GetDeviceCount ();
	outTable	= new int16_t [numofDevices + 1];
	for (i = 0; i <= numofDevices; i ++)
	   outTable [i] = -1;
	ostream		= nullptr;
}

	audioSink::~audioSink	(void) {
	if ((ostream != nullptr) && !Pa_IsStreamStopped (ostream)) {
	   paCallbackReturn = paAbort;
	   (void) Pa_AbortStream (ostream);
	   while (!Pa_IsStreamStopped (ostream))
	      Pa_Sleep (1);
	}

	if (ostream != nullptr)
	   Pa_CloseStream (ostream);

	if (portAudio)
	   Pa_Terminate ();

	delete	_O_Buffer;
	delete[] outTable;
}


//
bool	audioSink::selectDevice (int16_t odev) {
PaError err;
	if (!isValidDevice (odev))
	   return false;

	fprintf (stderr, "select device with %d (%d)\n", odev, numofDevices);
	if ((ostream != nullptr) && !Pa_IsStreamStopped (ostream)) {
	   paCallbackReturn = paAbort;
	   (void) Pa_AbortStream (ostream);
	   while (!Pa_IsStreamStopped (ostream))
	      Pa_Sleep (1);
	}

	if (ostream != nullptr)
	   Pa_CloseStream (ostream);

	outputParameters. device		= odev;
	outputParameters. channelCount		= 2;
	outputParameters. sampleFormat		= paFloat32;
	outputParameters. suggestedLatency	= 
	                          Pa_GetDeviceInfo (odev) ->
	                                      defaultHighOutputLatency * 4;
//	bufSize	= (int)((float)outputParameters. suggestedLatency);
	bufSize	= latency * 128;

	outputParameters. hostApiSpecificStreamInfo = nullptr;
//
	fprintf (stderr, "Suggested size for outputbuffer = %d\n", bufSize);
	err = Pa_OpenStream (
	             &ostream,
	             NULL,
	             &outputParameters,
	             CardRate,
	             bufSize,
	             0,
	             this	-> paCallback_o,
	             this
	      );

	if (err != paNoError) {
	   qDebug ("Open ostream error\n");
	   return false;
	}
	paCallbackReturn = paContinue;
	err = Pa_StartStream (ostream);
	if (err != paNoError) {
	   qDebug ("Open startstream error\n");
	   return false;
	}
	fprintf (stderr, "stream started\n");
	return true;
}

void	audioSink::restart	(void) {
PaError err;

	fprintf (stderr, "restarting audio\n");
	if (!Pa_IsStreamStopped (ostream))
	   return;

	_O_Buffer	-> FlushRingBuffer ();
	paCallbackReturn = paContinue;
	err = Pa_StartStream (ostream);
	fprintf (stderr, "audio restarted\n");
}

void	audioSink::stop	(void) {
	if (Pa_IsStreamStopped (ostream))
	   return;

	paCallbackReturn	= paAbort;
	(void)Pa_StopStream	(ostream);
	while (!Pa_IsStreamStopped (ostream))
	   Pa_Sleep (1);
}
//
//	helper
bool	audioSink::OutputrateIsSupported (int16_t device, int32_t Rate) {
PaStreamParameters *outputParameters =
	           (PaStreamParameters *)alloca (sizeof (PaStreamParameters)); 

	outputParameters -> device		= device;
	outputParameters -> channelCount	= 2;	/* I and Q	*/
	outputParameters -> sampleFormat	= paFloat32;
	outputParameters -> suggestedLatency	= 0;
	outputParameters -> hostApiSpecificStreamInfo = NULL;

	return Pa_IsFormatSupported (NULL, outputParameters, Rate) ==
	                                          paFormatIsSupported;
}
/*
 * 	... and the callback
 */
int	audioSink::paCallback_o (
		const void*			inputBuffer,
                void*				outputBuffer,
		unsigned long			framesPerBuffer,
		const PaStreamCallbackTimeInfo	*timeInfo,
	        PaStreamCallbackFlags		statusFlags,
	        void				*userData) {
RingBuffer<float>	*outB;
float	*outp		= (float *)outputBuffer;
audioSink *ud		= reinterpret_cast <audioSink *>(userData);
uint32_t	actualSize;
uint32_t	i;
	(void)statusFlags;
	(void)inputBuffer;
	(void)timeInfo;
	if (ud -> paCallbackReturn == paContinue) {
	   outB = (reinterpret_cast <audioSink *> (userData)) -> _O_Buffer;
	   actualSize = outB -> getDataFromBuffer (outp, 2 * framesPerBuffer);
	   for (i = actualSize; i < 2 * framesPerBuffer; i ++)
	      outp [i] = 0;
	}
	return ud -> paCallbackReturn;
}

void	audioSink::audioOutput	(std::complex<float> b) {
float temp [2];
	temp [0] 	= real (b);
	temp [1]	= imag (b);
	_O_Buffer	-> putDataIntoBuffer (temp, 2);
}

QString audioSink::outputChannelwithRate (int16_t ch, int32_t rate) {
const PaDeviceInfo *deviceInfo;
QString name = QString ("");

	if ((ch < 0) || (ch >= numofDevices))
	   return name;

	deviceInfo = Pa_GetDeviceInfo (ch);
	if (deviceInfo == NULL)
	   return name;
	if (deviceInfo -> maxOutputChannels <= 0)
	   return name;

	if (OutputrateIsSupported (ch, rate))
	   name = QString (deviceInfo -> name);
	return name;
}

int16_t	audioSink::invalidDevice	(void) {
	return numofDevices + 128;
}

bool	audioSink::isValidDevice (int16_t dev) {
	return 0 <= dev && dev < numofDevices;
}

bool	audioSink::selectDefaultDevice (void) {
	return selectDevice (Pa_GetDefaultOutputDevice ());
}

int32_t	audioSink::cardRate	(void) {
	return 48000;
}

bool	audioSink::setupChannels (QComboBox *streamOutSelector) {
uint16_t	ocnt	= 1;
uint16_t	i;

	for (i = 0; i <  numofDevices; i ++) {
	   const QString so = 
	             outputChannelwithRate (i, CardRate);
	   qDebug ("Investigating Device %d\n", i);

	   if (so != QString ("")) {
	      streamOutSelector -> insertItem (ocnt, so, QVariant (i));
	      outTable [ocnt] = i;
	      qDebug (" (output):item %d wordt stream %d (%s)\n", ocnt , i,
	                      so. toLatin1 ().data ());
	      ocnt ++;
	   }
	}

	qDebug () << "added items to combobox";
	return ocnt > 1;
}

bool	audioSink::set_streamSelector (int idx) {
int16_t	outputDevice;

	if (idx == 0)
	   return false;

	outputDevice = outTable [idx];
	if (!isValidDevice (outputDevice)) {
	   return false;
	}

	stop	();
	if (!selectDevice (outputDevice)) {
	   fprintf (stderr, "error selecting device\n");
	   selectDefaultDevice ();
	   return false;
	}

	qWarning () << "selected output device " << idx << outputDevice;
	return true;
}
//
int16_t	audioSink::numberofDevices	(void) {
	return numofDevices;
}

