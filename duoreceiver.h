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

#ifndef __DUO_RECEIVER__
#define __DUO_RECEIVER__

#include	<QDialog>
#include	<QComboBox>
#include	<QSettings>
#include	<QFrame>
#include        <QStringList>
#include        <QStandardItemModel>
#include	<QTimer>
#include	<QCloseEvent>
#include	<complex>
#include	"ui_duoreceiver.h"
#include	"fir-filters.h"
#include	"ringbuffer.h"

#include	"process-params.h"
#include        "band-handler.h"
#include	"text-mapper.h"
#include	"process-params.h"
#include	"preset-handler.h"
#include	"audio-base.h"

#include        "popup-keypad.h"
#include        "fm-processor.h"
#include        "squelchClass.h"
#include	<QLineEdit>

class	audioSink;
class	deviceHandler;
class	lowpassFIR;

class	dabProcessor;
class	fmProcessor;
class	programList;

class dabService {
public:
        QString         serviceName;
        uint32_t        SId;
        int             SCIds;
        bool            valid;
};


/*
 */
class duoReceiver: public QDialog,
		      private Ui_duoreceiver {
Q_OBJECT
public:
		duoReceiver		(QSettings	*,
	                                 const QString &,
	                                 const QString &,
	                                 QWidget *parent = NULL);
		~duoReceiver		();

protected:
	bool		eventFilter (QObject *obj, QEvent *event);

private:

	RingBuffer<std::complex<float>> fm_audioBuffer;
        keyPad			mykeyPad;
	squelch			mySquelch;
	presetHandler		my_presetHandler;
	bandHandler		theBand;
	QSettings		*duoSettings;
	RingBuffer<std::complex<float>>
			audioSamples;
	RingBuffer<int16_t>	dab_audioBuffer;
	audioBase		audioHandler;
	lowpassFIR		audioFilter;

	processParams		globals;
//	std::complex<float>	*audioOut;
	audioSink		*soundOut;
	deviceHandler		*theDevice;
	int16_t			*outTable;
	QTimer			displayTimer;
	QTimer			signalTimer;
	QTimer			presetTimer;
	QTimer			startTimer;
	int			audioRate;
	bool			muting;
	uint8_t		decoder;

	int			serviceOrder;
	bool			stereoSetting;
	std::atomic<bool>	running;
	dabProcessor		*my_dabProcessor;
	int			switchTime;

	QStandardItemModel	model;
	std::vector<serviceId>	serviceList;
	bool			isMember (std::vector<serviceId>,
	                                       serviceId);
	std::vector<serviceId>
	  	                insert   (std::vector<serviceId>,
	                                  serviceId, int);
	int16_t		ficBlocks;
	int16_t		ficSuccess;
	int		total_ficError;
	int		total_fics;
	void		cleanScreen		();
	void		start_audioService	(const QString &);
	void		startChannel		(const QString &);
	void		stopChannel		();
	void		stopService		();
	void		startService		(dabService *);
	void		colorService		(QModelIndex ind,
	                                                 QColor c, int pt);
	void		localSelect		(const QString &s);
	bool		doStart			();
	void		hide_for_safety		();
	void		show_for_safety		();

	void		TerminateProcess	();
	void		stop_dabSystem		();
	void		stop_fmSystem		();

	int		fmLow;
	int		fmHigh;
private slots:
	void		updateTimeDisplay	();
	void		set_streamSelector	(int);
	void		audiosamplesAvailable	(int);
	void		closeEvent		(QCloseEvent *event);
	void		handle_muteButton	();
	void		set_fmSystem		();
	void		set_dabSystem		();

public slots:
	void		addtoEnsemble		(const QString &, int);
	void		nameofEnsemble		(int, const QString &);
	void		show_ficSuccess		(bool);
	void		showLabel		(const QString &);
	void		changeinConfiguration	();
	void		dabAudio		(int, int);
	void		setStereo		(bool);

//	Somehow, these must be connected to the GUI
private slots:
	void		handle_nextChannelButton	();
	void		handle_prevChannelButton	();
	void		handle_prevServiceButton	();
        void		handle_nextServiceButton	();
	void		handle_presetSelector	(const QString &);
	void		selectChannel		(const QString &);
	void		selectService		(QModelIndex);
	void		setPresetStation	();

private:
//	for the FM subsystem we have
        fmProcessor	*my_fmProcessor;
	float           Tau;
        float           alpha;
        float           audioGain;
	programList	*myList;
	QLineEdit	*myLine;
public slots:
        void            new_audioGain           (float);
	void		fmAudio			(int);
        void            showLocked              (bool);
        void            clearStationLabel       ();
        void            setStationLabel         (const QString &);
        void            clearRadioText          ();
        void            setRadioText            (const QString &);
        void            setRDSisSynchronized    (bool);
        void            setMusicSpeechFlag      (int);
        void            clearMusicSpeechFlag    (void);
        void            newFrequency            (int);

	void		set_freqSave		();
	void		handle_myLine		();
private slots:
	void            setfmDeemphasis (const QString& s);
        void            handle_freqButton       ();
        void            handle_decrementButton  ();
        void            handle_incrementButton  ();
        void            handle_squelchSlider    (int);
};

#endif

