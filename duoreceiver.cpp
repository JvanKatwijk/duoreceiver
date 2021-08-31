#
/*
 *    Copyright (C) 2021
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the  duo receiver
 *    duo receiver is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    duo receiver is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	<Qt>
#include	<QSettings>
#include	<QMessageBox>
#include	<QDebug>
#include	<QDateTime>
#include	"duoreceiver.h"
#include        <fstream>
#include        <iostream>
#include        <numeric>
#include        <unistd.h>
#include        <vector>
#ifdef	__SDRPLAY_V2__
#include	"sdrplay-handler.h"
#endif
#ifdef	__AIRSPY__
#include	"airspy-handler.h"
#endif
#ifdef	__RTLSDR__
#include	"rtlsdr-handler.h"
#endif
#ifdef	__PLUTO__
#include	"pluto-handler.h"
#endif
#include	"audiosink.h"

#include	"dab-processor.h"
dabService      nextService;
dabService      currentService;
//
#include	"program-list.h"

#ifdef __MINGW32__
#include	<iostream>
#include	<windows.h>
#endif
//
#define	DAB_DECODER	0100
#define	FM_DECODER	0200

#define	FM_LOW		86000
#define	FM_HIGH		110000

	duoReceiver::duoReceiver (QSettings	*Si,
	                          const QString &fmStationList,
	                          const QString &presetFile,
	                          QWidget	*parent):
	                                    QDialog (parent) ,
	                                    fm_audioBuffer (32768),
	                                    mykeyPad (this),
	                                    mySquelch (70, 48000 / 20,
	                                               48000 / 10, 48000),
                                            my_presetHandler (this),
                                            theBand (Si),
	                                    audioSamples (32768),
	                                    dab_audioBuffer (32768),
	                                    audioHandler (&audioSamples),
	                                    audioFilter (11, 15000, 48000) {
int16_t	latency		= 1;
QString h;
int	k;
bool	err;
QString	presetName;
	duoSettings	= Si;
	setupUi (this);
//
//	before printing anything, we set
	setlocale (LC_ALL, "");
//
	theDevice		= nullptr;
#ifdef	__PLUTO__
	if (theDevice == nullptr)
	   try {
	      theDevice = new plutoHandler (Si, deviceControlWidget);
	   } catch (int e) {}
#endif
#ifdef	__RTLSDR__
	if (theDevice == nullptr) 
	   try {
	      theDevice	= new rtlsdrHandler (Si, deviceControlWidget);
	   } catch (int e) {}
#endif

#ifdef	__SDRPLAY_V2__
	if (theDevice == nullptr) 
	   try {
	      theDevice	= new sdrplayHandler (Si, deviceControlWidget);
	   } catch (int e) {}
#endif
#ifdef	__AIRSPY__
	if (theDevice == nullptr) 
	   try {
	      theDevice	= new airspyHandler (Si, deviceControlWidget);
	   } catch (int e) {}
#endif
	if (theDevice == nullptr) {
	   QMessageBox::warning (this, tr ("sdr"),
	                               tr ("no device configured\n"));
	   exit (1);
	}

	soundOut		= new audioSink		(latency);
	((audioSink *)soundOut)	-> setupChannels (streamOutSelector);
	h = duoSettings -> value ("audioStream",
	                                       "default"). toString();

        k       = streamOutSelector -> findText (h);
        if (k != -1) {
           streamOutSelector -> setCurrentIndex (k);
           err = !((audioSink *)soundOut) -> selectDevice (k);
        }

        if ((k == -1) || err)
           ((audioSink *)soundOut)      -> selectDefaultDevice();
	
	connect (streamOutSelector, SIGNAL (activated (int)),
                 this,  SLOT (set_streamSelector (int)));
	              
	my_presetHandler. loadPresets (presetFile, presetSelector);

	decoder		= 077;		// nothing
	myList		= new programList (this, fmStationList);
	muting		= false;
	connect (muteButton, SIGNAL (clicked ()),
	         this, SLOT (handle_muteButton ()));
//	create a timer for displaying the "real" time
	displayTimer. setInterval (1000);
	connect (&displayTimer,
	         SIGNAL (timeout ()),
	         this,
	         SLOT (updateTimeDisplay ()));
	displayTimer. start (1000);

	switchTime	= 
	   duoSettings	-> value ("switchTime", 4000). toInt ();
	connect (fmSelector, SIGNAL (clicked ()),
	         this, SLOT (set_fmSystem ()));
	connect (dabSelector, SIGNAL (clicked ()),
	         this, SLOT (set_dabSystem ()));
//
//	allocate the DAB subsystem
        theBand. setupChannels (channelSelector);

        serviceList. clear ();
        model . clear ();
        ensembleDisplay         -> setModel (&model);

	presetTimer. setSingleShot (true);
        presetTimer. setInterval (switchTime);
        connect (&presetTimer, SIGNAL (timeout (void)),
                 this, SLOT (setPresetStation (void)));
//
//
	globals. dabMode	= 1;
        my_dabProcessor = new dabProcessor (this, theDevice, &globals);

	my_fmProcessor	= new fmProcessor (this, theDevice, &fm_audioBuffer);

	qApp    -> installEventFilter (this);

        connect (presetSelector, SIGNAL (activated (const QString &)),
                 this, SLOT (handle_presetSelector (const QString &)));
        connect (prevServiceButton, SIGNAL (clicked ()),
                 this, SLOT (handle_prevServiceButton ()));
        connect (nextServiceButton, SIGNAL (clicked ()),
                 this, SLOT (handle_nextServiceButton ()));
        connect (ensembleDisplay, SIGNAL (clicked (QModelIndex)),
                 this, SLOT (selectService (QModelIndex)));
        connect (nextChannelButton, SIGNAL (clicked (void)),
                 this, SLOT (handle_nextChannelButton (void)));
        connect (prevChannelButton, SIGNAL (clicked (void)),
                 this, SLOT (handle_prevChannelButton (void)));
	nextService. valid	= false;

//
//	connects for the FM subsystem
//
	connect (fmDeemphasisSelector, SIGNAL (activated (const QString &)),
                      this, SLOT (setfmDeemphasis (const QString &)));
        connect (freqDecrementButton, SIGNAL (clicked ()),
                 this, SLOT (handle_decrementButton ()));
        connect (freqIncrementButton, SIGNAL (clicked ()),
                 this, SLOT (handle_incrementButton ()));
        Tau       = 1000000.0 / 50;
        alpha     = 1.0 / (float (192000) / Tau + 1.0);

	duoSettings	-> beginGroup ("FM_SYSTEM");
	fmLow	= duoSettings -> value ("fmLow", FM_LOW). toInt ();
	fmHigh	= duoSettings -> value ("fmLow", FM_HIGH). toInt ();
	duoSettings	-> endGroup ();

	copyrightLabel	-> setToolTip (footText ());

	if (duoSettings -> value ("system", "DAB"). toString () == "DAB")
	   set_dabSystem ();
	else
	   set_fmSystem ();
}

	duoReceiver::~duoReceiver () {
	delete	theDevice;
	delete	soundOut;
	delete	myList;
}

QString duoReceiver::footText () {
        QString versionText = "duoReceiver version: " + QString(CURRENT_VERSION) + "\n";
        versionText += "Built on " + QString(__TIMESTAMP__) + QString (", Commit ") + QString (GITHASH) + "\n";
        versionText += "Copyright Jan van Katwijk, mailto:J.vanKatwijk@gmail.com\n";
        versionText += "Rights of Qt, fftw, portaudio, libfaad gratefully acknowledged\n";
        versionText += "Rights of other contributors gratefully acknowledged";
       return versionText;
}

void	duoReceiver::updateTimeDisplay () {
QDateTime	currentTime = QDateTime::currentDateTime ();
	timeDisplay	-> setText (currentTime.
	                            toString (QString ("dd.MM.yy:hh:mm:ss")));
}
//
//	- low pass filtering
void	duoReceiver::audiosamplesAvailable (int n) {
std::complex<float> buffer [512];
	(void)n;
	while (audioSamples. GetRingBufferReadAvailable () > 512) {
	   audioSamples. getDataFromBuffer (buffer, 512);
	   if (!muting) {
	      for (int i = 0; i < 512; i ++) {
	         std::complex<float> out	=
	                   audioFilter. Pass (buffer [i]);
	         soundOut	-> audioOutput (out);
	      }
	   }
	}
}
//
void    duoReceiver:: set_streamSelector (int k) {
        ((audioSink *)(soundOut)) -> selectDevice (k);
}
//
//	Handling Buttons
void	duoReceiver::handle_muteButton	() {
	muting = !muting;
	muteButton	-> setText (muting ? "MUTING" : "mute");
}

void	duoReceiver::set_dabSystem	() {
int16_t k;
QString h;

	fprintf (stderr, "we gaan voor DAB\n");
	if (decoder == DAB_DECODER)
	   return;
	if (decoder == FM_DECODER)
	   stop_fmSystem ();

	fprintf (stderr, "zetten de sound stop\n");
//	soundOut	-> stop ();
	fprintf (stderr, "zetten de index op 0\n");
	stackedWidget	-> setCurrentIndex (0);
	fprintf (stderr, "en dan?\n");
	currentService. valid   = false;
	duoSettings	-> beginGroup ("DAB_SYSTEM");
	QString presetName           =
	               duoSettings -> value ("presetName", ""). toString();
	if (presetName != "") {
	   nextService. serviceName = presetName;
	   nextService. SId          = 0;
	   nextService. SCIds        = 0;
	   nextService. valid        = true;
	}

	h       = duoSettings -> value ("channel", "12C"). toString();
	k       = channelSelector -> findText (h);
	if (k != -1)
	   channelSelector -> setCurrentIndex (k);

	duoSettings	-> endGroup ();
	if (nextService. valid) {
	   presetTimer. setSingleShot   (true);
	   presetTimer. setInterval     (switchTime);
	   presetTimer. start           (switchTime);
	}
        connect (channelSelector, SIGNAL (activated (const QString &)),
                 this, SLOT (selectChannel (const QString &)));
	soundOut	-> restart ();
	startChannel (channelSelector -> currentText ());
        ficBlocks       = 0;
        ficSuccess      = 0;
	decoder		= DAB_DECODER;
	functionDisplay -> setText ("DAB");
	running. store (true);
}

void	duoReceiver::stop_dabSystem	() {
	if (decoder != DAB_DECODER)
	   return;
	theDevice	-> stopReader ();
	soundOut	-> stop ();
	my_dabProcessor	-> stop ();
	presetTimer. stop ();
	duoSettings	-> beginGroup ("DAB_SYSTEM");
	duoSettings	-> setValue ("channel", channelSelector -> currentText ());
	if (currentService. valid) {
	   duoSettings -> setValue ("presetName", currentService. serviceName);
	}
	duoSettings	-> endGroup ();
	stopChannel ();
        disconnect (channelSelector, SIGNAL (activated (const QString &)),
                    this, SLOT (selectChannel (const QString &)));
	my_presetHandler. savePresets (presetSelector);
	decoder		= 077;
	running. store (false);
}

void	duoReceiver::set_fmSystem	() {
	if (decoder == DAB_DECODER)
	   stop_dabSystem ();

	soundOut	-> stop ();
	stackedWidget	-> setCurrentIndex (1);
	connect (freqButton, SIGNAL (clicked ()),
                 this, SLOT (handle_freqButton ()));
        connect (squelchSlider, SIGNAL (valueChanged (int)),
                 this, SLOT (handle_squelchSlider (int)));
        connect (freqSave, SIGNAL (clicked (void)),
                 this, SLOT (set_freqSave (void)));
        myList  -> show ();
        myLine  = nullptr;

	setfmDeemphasis         (fmDeemphasisSelector   -> currentText ());
	audioGain               = 1.0;
	duoSettings	-> beginGroup ("FM_SYSTEM");
	int frequency	=
	         duoSettings -> value ("fmFrequency", 94700). toInt ();
	int squelchSetting =
	         duoSettings -> value ("squelchSetting", 70). toInt ();
	squelchSlider	-> setValue (squelchSetting);
	theDevice	-> restartReader (KHz (frequency));
	lcd_Frequency	-> display (frequency);
	duoSettings	-> endGroup ();

	soundOut	-> restart ();
	my_fmProcessor	-> start ();
	functionDisplay -> setText ("FM");
	decoder = FM_DECODER;
}

void	duoReceiver::stop_fmSystem	() {
	if (decoder != FM_DECODER)
	   return;
	theDevice	-> stopReader ();
	soundOut	-> stop ();
	myList		-> saveTable ();
	duoSettings	-> beginGroup ("FM_SYSTEM");
	duoSettings -> setValue ("fmFrequency", 
	                         theDevice -> getVFOFrequency () / 1000);
	duoSettings	-> endGroup ();
	mykeyPad. hidePad ();
	myList	-> hide ();
        disconnect (freqSave, SIGNAL (clicked (void)),
                 this, SLOT (set_freqSave (void)));
	disconnect (freqButton, SIGNAL (clicked ()),
                    this, SLOT (handle_freqButton ()));
        disconnect (squelchSlider, SIGNAL (valueChanged (int)),
                    this, SLOT (handle_squelchSlider (int)));
	my_fmProcessor	-> stop ();
	decoder		= 077;
}

#include <QCloseEvent>
void duoReceiver::closeEvent (QCloseEvent *event) {

        QMessageBox::StandardButton resultButton =
                        QMessageBox::question (this, "wfm-rpi",
                                               tr("Are you sure?\n"),
                                               QMessageBox::No | QMessageBox::Yes,
                                               QMessageBox::Yes);
        if (resultButton != QMessageBox::Yes) {
           event -> ignore();
        } else {
	   TerminateProcess ();
           event -> accept ();
        }
}

void	duoReceiver::TerminateProcess	() {
	if (decoder == DAB_DECODER) {
	   stop_dabSystem ();
	   duoSettings	-> setValue ("system", "DAB");
	}
	else
	if (decoder == FM_DECODER) {
	   stop_fmSystem ();
	   duoSettings -> setValue ("system", "FM");
	}
	duoSettings -> setValue ("audioStream",
	                       streamOutSelector -> currentText ());
}

//
void	duoReceiver::dabAudio     (int amount, int rate) {
        if (running. load ()) {
           int16_t vec [amount];
           while (dab_audioBuffer. GetRingBufferReadAvailable() >
	                        (uint32_t)amount) {
              dab_audioBuffer. getDataFromBuffer (vec, amount);
              audioHandler. audioOut (vec, amount, rate);
           }
           emit audiosamplesAvailable (1);
        }
}

//	a slot, called by the fic/fib handlers
void	duoReceiver::addtoEnsemble (const QString &serviceName,
	                                             int32_t SId) {
	serviceId ed;
	ed. name = serviceName;
	ed. SId	= SId;
	if (!my_dabProcessor -> is_audioService (serviceName))
	   return;
	if (isMember (serviceList, ed))
	   return;
	serviceList = insert (serviceList, ed, serviceOrder);

	model. clear ();
	for (const auto serv : serviceList)
	   model. appendRow (new QStandardItem (serv. name));
        int row = model. rowCount ();
        for (int i = 0; i < row; i ++) {
           model. setData (model. index (i, 0),
                      QFont ("Cantarell", 10), Qt::FontRole);
        }

        ensembleDisplay -> setModel (&model);
}
//
//	The ensembleId is written as hexadecimal, however, the 
//	number display of Qt is only 7 segments ...
static
QString hextoString (int v) {
char t [4];
QString res;
int     i;
	for (i = 0; i < 4; i ++) {
	   t [3 - i] = v & 0xF;
	   v >>= 4;
	}
	for (i = 0; i < 4; i ++) {
	   QChar c = t [i] <= 9 ? (char) ('0' + t [i]) : (char)('A'+ t [i] - 10);
	   res. append (c);
	}
	return res;
}

///	a slot, called by the fib processor
void	duoReceiver::nameofEnsemble (int id, const QString &v) {
	if (!running. load())
	   return;
	ensembleId      -> setAlignment(Qt::AlignLeft);
        ensembleId      -> setText (v + QString (":") + hextoString (id));
}

void	duoReceiver::show_ficSuccess (bool b) {
	if (b) 
	   ficSuccess ++;
	if (++ficBlocks >= 25) {
	   if (ficSuccess > 24)
	      ficLabel ->
	          setStyleSheet ("QLabel {background-color : green; color: white}");
	   else
	      ficLabel ->
	          setStyleSheet ("QLabel {background-color : red; color: white}");
	   ficBlocks	= 0;
	   ficSuccess	= 0;
	}
}

void	duoReceiver::showLabel	(const QString &s) {
	dynamicLabel	-> setText (s);
	dynamicLabel -> setWordWrap (true);
}

void	duoReceiver::setStereo	(bool s) {
        if (stereoSetting == s)
           return;

	stereoLabel   -> setStyleSheet (s ?
                         "QLabel {background-color: green; color : white}":
                         "QLabel {background-color: red; color : white}");
	stereoLabel   -> setText (s ? "stereo" : "mono");
        stereoSetting = s;

}

bool	duoReceiver::eventFilter (QObject *obj, QEvent *event) {
	if (!running. load ())
	   return QWidget::eventFilter (obj, event);

	if (event -> type () == QEvent::KeyPress) {
	   QKeyEvent *ke = static_cast <QKeyEvent *> (event);
	   if (ke -> key () == Qt::Key_Return) {
	      presetTimer. stop ();
	      nextService. valid = false;
	      QString serviceName =
	         ensembleDisplay -> currentIndex ().
	                             data (Qt::DisplayRole). toString ();
	      if (currentService. serviceName != serviceName) {
//	         fprintf (stderr, "currentservice = %s (%d)\n",
//	                       currentService. serviceName. toLatin1 (). data (),
//	                                    currentService. valid);
	         stopService ();	// basically redundant
	         selectService (ensembleDisplay -> currentIndex ());
	      }
           }
        }
        else
	if ((obj == this -> ensembleDisplay -> viewport()) &&
	    (event -> type () == QEvent::MouseButtonPress )) {
	   QMouseEvent *ev = static_cast<QMouseEvent *>(event);
	   if (ev -> buttons() & Qt::RightButton) {
	      audiodata ad;
	      QString serviceName =
	           this -> ensembleDisplay -> indexAt (ev -> pos()). data().toString();
	      if (serviceName. at (1) == ' ')
	         return true;
	      my_dabProcessor -> dataforAudioService (serviceName, &ad);
	      if (ad. defined && (serviceLabel -> text () == serviceName)) {
	         presetData pd;
	         pd. serviceName	= serviceName;
	         pd. channel		= channelSelector -> currentText ();
	         QString itemText	= pd. channel + ":" + pd. serviceName;
	         for (int i = 0; i < presetSelector -> count (); i ++)
	            if (presetSelector -> itemText (i) == itemText)
	               return true;
	         presetSelector -> addItem (itemText);
	         return true;
	      }
	      else {
	      }
	   }
	}
	return QWidget::eventFilter (obj, event);
}

//
void	duoReceiver::handle_presetSelector (const QString &s) {
        presetTimer. stop ();
        if ((s == "Presets") || (presetSelector -> currentIndex () == 0))
           return;
        localSelect (s);
}

void	duoReceiver::localSelect (const QString &s) {
	QStringList list = s.split (":", QString::SkipEmptyParts);
	if (list. length () != 2)
	   return;
	QString channel = list. at (0);
	QString service	= list. at (1);
	if (channel == channelSelector -> currentText ()) {
	   stopService ();
	   dabService s;
	   my_dabProcessor -> getParameters (service, &s. SId, &s. SCIds);
	   if (s. SId == 0) {
              QMessageBox::warning (this, tr ("Warning"),
                                 tr ("insufficient data for this program\n"));
              return;
           }

	   s. serviceName = service;
	   startService (&s);
	   return;
	}
//
//	The hard part is stopping the current service,
//      selecting a new channel,
//      waiting a while
//      trying to start the selected service
	disconnect (channelSelector, SIGNAL (activated (const QString &)),
	            this, SLOT (selectChannel (const QString &)));
	int k           = channelSelector -> findText (channel);
	if (k != -1) {
	   channelSelector -> setCurrentIndex (k);
	}
	else 
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("Incorrect preset\n"));
	connect (channelSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (selectChannel (const QString &)));
	if (k == -1)
	   return;

	stopChannel ();		// this will handle stopping the service
	nextService. valid = true;
	nextService. serviceName	= service;
	nextService. SId		= 0;
	nextService. SCIds		= 0;
	presetTimer. setSingleShot (true);
	presetTimer. setInterval (switchTime);
	presetTimer. start (switchTime);
	startChannel	(channelSelector -> currentText ());
}

void	duoReceiver::stopService	() {
	presetTimer. stop ();
	presetSelector -> setCurrentIndex (0);
	if (currentService. valid) {
	   audiodata ad;
	   my_dabProcessor -> dataforAudioService (currentService. serviceName,
	                                                  &ad);
	   my_dabProcessor -> stopService (&ad);
	   usleep (1000);
	   QString serviceName = currentService. serviceName;
	   for (int i = 0; i < model. rowCount (); i ++) {
	      QString itemText =
	          model. index (i, 0). data (Qt::DisplayRole). toString ();
	      if (itemText == serviceName) {
	         colorService (model. index (i, 0), Qt::black, 10);
	         break;
	      }
	   }
	}
	currentService. valid	= false;
	cleanScreen	();
}
//
void	duoReceiver::selectService (QModelIndex ind) {
QString	currentProgram = ind. data (Qt::DisplayRole). toString();
	stopService 	();		// if any

	dabService s;
	my_dabProcessor -> getParameters (currentProgram, &s. SId, &s. SCIds);
	if (s. SId == 0) {
	   QMessageBox::warning (this, tr ("Warning"),
 	                         tr ("insufficient data for this program\n"));	
	   return;
	}

	s. serviceName = currentProgram;
	startService (&s);
}
//
void	duoReceiver::startService (dabService *s) {
QString serviceName	= s -> serviceName;

	if (currentService. valid) {
	   fprintf (stderr, "Niet verwacht, service %s is still valid\n",
	                    currentService. serviceName. toUtf8 (). data ());
	   stopService ();
	}

	ficBlocks		= 0;
	ficSuccess		= 0;
	currentService		= *s;
	currentService. valid	= false;
	int rowCount		= model. rowCount ();
	for (int i = 0; i < rowCount; i ++) {
	   QString itemText =
	           model. index (i, 0). data (Qt::DisplayRole). toString ();
	   if (itemText == serviceName) {
	      colorService (model. index (i, 0), Qt::red, 13);
	      serviceLabel	-> setStyleSheet ("QLabel {color : black}");
	      serviceLabel	-> setText (serviceName);
	      if (my_dabProcessor -> is_audioService (serviceName)) {
	         start_audioService (serviceName);
	         currentService. valid = true;
	         currentService. serviceName = serviceName;
	      }
	      else
	         fprintf (stderr, "%s not supported\n",
	                            serviceName. toUtf8 (). data ());
	      return;
	   }
	}
}

void    duoReceiver::colorService (QModelIndex ind, QColor c, int pt) {
	QMap <int, QVariant> vMap = model. itemData (ind);
	vMap. insert (Qt::ForegroundRole, QVariant (QBrush (c)));
	model. setItemData (ind, vMap);
	model. setData (ind, QFont ("Cantarell", pt), Qt::FontRole);
}
//
void	duoReceiver::cleanScreen	() {
	serviceLabel		-> setText ("");
	dynamicLabel		-> setText ("");
	presetSelector		-> setCurrentIndex (0);
	stereoLabel	-> setStyleSheet (
	                     "QLabel {background-color: white; color : black}");
	stereoLabel	-> setText ("");
	stereoSetting		= false;
}

void	duoReceiver::start_audioService (const QString &serviceName) {
audiodata ad;

	my_dabProcessor -> dataforAudioService (serviceName, &ad);
	if (!ad. defined) {
	   QMessageBox::warning (this, tr ("Warning"),
 	                         tr ("insufficient data for this program\n"));
	   return;
	}
	serviceLabel -> setText (serviceName);
	serviceLabel -> setAlignment(Qt::AlignLeft);

	ad. procMode	= __ONLY_SOUND;
	my_dabProcessor -> set_audioChannel (&ad, &dab_audioBuffer);
//	activate sound
}

void	duoReceiver::handle_prevServiceButton	() {
	presetTimer. stop ();
	presetSelector -> setCurrentIndex (0);
	nextService. valid	= false;

	if (!currentService. valid)
	   return;

	QString oldService	= currentService. serviceName;
	disconnect (prevServiceButton, SIGNAL (clicked ()),
	            this, SLOT (handle_prevServiceButton ()));
	stopService  ();
	if ((serviceList. size () != 0) &&
	                    (oldService != "")) {
	   for (int i = 0; i < (int)(serviceList. size ()); i ++) {
	      if (serviceList. at (i). name == oldService) {
	         colorService (model. index (i, 0), Qt::black, 10);
	         i = i - 1;
	         if (i < 0)
	            i = serviceList. size () - 1;
	         dabService s;
	         s. serviceName = serviceList. at (i). name;
	         my_dabProcessor ->
	                  getParameters (s. serviceName, &s. SId, &s. SCIds);
	         if (s. SId == 0) {
                    QMessageBox::warning (this, tr ("Warning"),
                                 tr ("insufficient data for this program\n"));
	            break;
                 }

	         startService (&s);
	         break;
	      }
	   }
	}
	connect (prevServiceButton, SIGNAL (clicked ()),
	         this, SLOT (handle_prevServiceButton ()));
}

void	duoReceiver::handle_nextServiceButton	() {
	presetTimer. stop ();
	presetSelector -> setCurrentIndex (0);
	nextService. valid	= false;

	if (!currentService. valid)
	   return;

	QString oldService = currentService. serviceName;
	disconnect (nextServiceButton, SIGNAL (clicked ()),
	            this, SLOT (handle_nextServiceButton ()));
	stopService ();
	if ((serviceList. size () != 0) && (oldService != "")) {
	   for (int i = 0; i < (int)(serviceList. size ()); i ++) {
	      if (serviceList. at (i). name == oldService) {
	         colorService (model. index (i, 0), Qt::black, 10);
	         i = i + 1;
	         if (i >= (int)(serviceList. size ()))
	            i = 0;
	         dabService s;
	         s. serviceName = serviceList. at (i). name;
	         my_dabProcessor ->
	                 getParameters (s. serviceName, &s. SId, &s. SCIds);
	         if (s. SId == 0) {
	            QMessageBox::warning (this, tr ("Warning"),
	                      tr ("insufficient data for this program\n"));
	            break;
                 }

	         startService (&s);
	         break;
	      }
	   }
	}
	connect (nextServiceButton, SIGNAL (clicked ()),
	         this, SLOT (handle_nextServiceButton ()));
}

void	duoReceiver::setPresetStation () {
	if (ensembleId -> text () == QString ("")) {
	   QMessageBox::warning (this, tr ("Warning"),
	                          tr ("Oops, ensemble not yet recognized\nselect service manually\n"));
	   return;
	}

	if (!nextService. valid)
	   return;

	QString presetName	= nextService. serviceName;
	for (const auto& service: serviceList) {
	   if (service. name. contains (presetName)) {
	      dabService s;
	      s. serviceName = presetName;
	      my_dabProcessor	-> getParameters (presetName, &s. SId, &s. SCIds);
	      if (s. SId == 0) {
	         QMessageBox::warning (this, tr ("Warning"),
	                        tr ("insufficient data for this program\n"));
	         return;
	      }
	      s. serviceName = presetName;
	      startService (&s);
	      return;
	   }
	}
	nextService. valid = false;
//
//	not found, no service selected
	fprintf (stderr, "presetName %s not found\n",
	                      presetName. toUtf8 (). data ());
}

void	duoReceiver::startChannel (const QString &channel) {
int	tunedFrequency	=
	         theBand. Frequency (channel);
	theDevice		-> stopReader ();
	theDevice		-> restartReader (tunedFrequency);
	my_dabProcessor		-> start (tunedFrequency);
	show_for_safety	();
}

void	duoReceiver::stopChannel	() {
	if ((theDevice == nullptr) || (my_dabProcessor == nullptr))
	   return;
	ficSuccess		= 0;
	ficBlocks		= 0;
	presetTimer. stop ();
        disconnect (presetSelector, SIGNAL (activated (const QString &)),
                 this, SLOT (handle_presetSelector (const QString &)));
	presetSelector		-> setCurrentIndex (0);
        connect (presetSelector, SIGNAL (activated (const QString &)),
                 this, SLOT (handle_presetSelector (const QString &)));
	hide_for_safety		();
//
//	The service(s) - if any - is stopped by halting the dabProcessor
	my_dabProcessor		-> stop ();
	theDevice		-> stopReader ();
	usleep (1000);
	currentService. valid	= false;
	nextService. valid	= false;
//	the visual elements
	serviceList. clear ();
	model. clear ();
	ensembleDisplay		-> setModel (&model);
	cleanScreen	();
}


void    duoReceiver::selectChannel (const QString &channel) {
	stopChannel ();
	startChannel (channel);
}

void	duoReceiver::handle_nextChannelButton () {
int     currentChannel  = channelSelector -> currentIndex ();
	stopChannel ();
	currentChannel ++;
	if (currentChannel >= channelSelector -> count ())
	   currentChannel = 0;
	disconnect (channelSelector, SIGNAL (activated (const QString &)),
	            this, SLOT (selectChannel (const QString &)));
	channelSelector -> setCurrentIndex (currentChannel);
	connect (channelSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (selectChannel (const QString &)));
	startChannel (channelSelector -> currentText ());
}

void	duoReceiver::handle_prevChannelButton () {
int     currentChannel  = channelSelector -> currentIndex ();
	stopChannel ();
	currentChannel --;
	if (currentChannel < 0)
	   currentChannel =  channelSelector -> count () - 1;
	disconnect (channelSelector, SIGNAL (activated (const QString &)),
	            this, SLOT (selectChannel (const QString &)));
	channelSelector -> setCurrentIndex (currentChannel);
	connect (channelSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (selectChannel (const QString &)));
	startChannel (channelSelector -> currentText ());
}

bool	duoReceiver::isMember (std::vector<serviceId> a,
	                                     serviceId b) {
	for (const auto serv : a)
	   if (serv. name == b. name)
	      return true;
	return false;
}

std::vector<serviceId>
	duoReceiver::insert (std::vector<serviceId> l,
	                        serviceId n, int order) {
std::vector<serviceId> k;
	if (l . size () == 0) {
	   k. push_back (n);
	   return k;
	}
	int 	baseN		= 0;
	QString baseS		= "";
	bool	inserted	= false;
	for (const auto serv : l) {
	   if (!inserted &&
	         (order == ID_BASED ?
	             ((baseN < (int)n. SId) && (n. SId <= serv. SId)):
	             ((baseS < n. name) && (n. name < serv. name)))) {
	      k. push_back (n);
	      inserted = true;
	   }
	   baseS	= serv. name;
	   baseN	= serv. SId;
	   k. push_back (serv);
	}
	if (!inserted)
	   k. push_back (n);
	return k;
}

void	duoReceiver::hide_for_safety () {
	prevServiceButton	->	hide ();
        nextServiceButton	->	hide ();
}

void	duoReceiver::show_for_safety () {
	prevServiceButton	->	show ();
        nextServiceButton	->	show ();
}

void	duoReceiver::changeinConfiguration() {
	if (running. load ()) {
	   dabService s;
	   if (currentService. valid) { 
	      s = currentService;
	      stopService     ();
	   }
	   fprintf (stderr, "change detected\n");
//
//	we rebuild the services list from the fib and
//	then we (try to) restart the service
	   serviceList	= my_dabProcessor -> getServices (serviceOrder);
	   model. clear	();
	   for (const auto serv : serviceList)
	      model. appendRow (new QStandardItem (serv. name));
	   int row = model. rowCount ();
	   for (int i = 0; i < row; i ++) {
	      model. setData (model. index (i, 0),
	      QFont ("Cantarell", 10), Qt::FontRole);
	   }
	   ensembleDisplay -> setModel (&model);
//
//	and restart the one that was running
	   if (s. valid) {
	      if (s. SCIds != 0) { // secondary service may be gone
	         if (my_dabProcessor -> findService (s. SId, s. SCIds) ==
	                                                   s. serviceName) {
	            startService (&s);
	            return;
	         }
	         else {
	            s. SCIds = 0;
	            s. serviceName =
	                  my_dabProcessor -> findService (s. SId, s. SCIds);
	         }
	      }
//	checking for the main service
	      if (s. serviceName != 
	                 my_dabProcessor -> findService (s. SId, s. SCIds)) {
	         QMessageBox::warning (this, tr ("Warning"),
                                tr ("insufficient data for this program\n"));
                 return;
              }

	      startService (&s);
	   }
	}
}
//
//		handling FM subsystem

void	duoReceiver::new_audioGain	(float v) {
	audioGain	= v;
}
//      When pressing the freqButton, a form will appear on
//      which the "user" may key in a frequency.
//
//      If it is already there, pressing the button (for the second time)
//      will hide the keypad again
void    duoReceiver::handle_freqButton (void) {
        if (mykeyPad. isVisible ())
           mykeyPad. hidePad ();
        else
           mykeyPad. showPad ();
}
//
//	This function os for FM only, so we check the frequency range
void	duoReceiver::newFrequency	(int freq) {
	if (freq < KHz (fmLow))
	   return;
	if (freq > KHz (fmHigh))
	   return;
	theDevice	-> setVFOFrequency (freq);
	lcd_Frequency	-> display (freq / 1000);
}

void	duoReceiver::handle_decrementButton	() {
int	freq	= theDevice -> getVFOFrequency	();
	newFrequency (freq - KHz (100));
}

void	duoReceiver::handle_incrementButton	() {
int	freq	= theDevice -> getVFOFrequency	();
	newFrequency (freq + KHz (100));
}

void    duoReceiver::showLocked (bool locked) {
        if (locked)
           pll_isLocked -> setStyleSheet ("QLabel {background-color:green}");
        else
           pll_isLocked -> setStyleSheet ("QLabel {background-color:red}");
}

void	duoReceiver::clearStationLabel () {
	stationLabelTextBox	-> setText ("");
}
//
void	duoReceiver::setStationLabel (const QString &s) {
	stationLabelTextBox	-> setText (s);
}

void	duoReceiver::setMusicSpeechFlag (int n) {
	if (n != 0)
	   speechLabel -> setText (QString ("music"));
	else
	   speechLabel -> setText (QString ("speech"));
}

void	duoReceiver::clearMusicSpeechFlag (void) {
	speechLabel	-> setText (QString (""));
}

void	duoReceiver::clearRadioText () {
	radioTextBox	-> setText (QString (""));
}
//
void	duoReceiver::setRadioText (const QString &s) {
	radioTextBox	-> setWordWrap (true);
	radioTextBox	-> setText (s);
}

void	duoReceiver::setRDSisSynchronized (bool syn) {
	if (!syn)
	   rdsSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	else
	   rdsSyncLabel -> setStyleSheet ("QLabel {background-color:green}");
}

void    duoReceiver::handle_squelchSlider (int n) {
        mySquelch. setSquelchLevel (n);
}

void	duoReceiver::fmAudio	(int n) {
static float xkm1	= 0;
static float ykm1	= 0;
std::complex<float> buf1 [512];
	(void)n;

	while (fm_audioBuffer. GetRingBufferReadAvailable () > 512) {
	   fm_audioBuffer. getDataFromBuffer (buf1, 512);
	   for (int i = 0; i < 512; i ++) {
	      std::complex<float> temp =
	             std::complex<float> (real (buf1 [i]) - imag (buf1 [i]),
	                              - (-real (buf1 [i]) + imag (buf1 [i])));
	      float Re, Im;
//	deemphasize
              Re        = xkm1 = (real (temp) - xkm1) * alpha + xkm1;
              Im        = ykm1 = (imag (temp) - ykm1) * alpha + ykm1;
	      Re        *= audioGain;
	      Im        *= audioGain;
	      buf1 [i]	= mySquelch. do_squelch (std::complex<float> (Re, Im));
	   }
	   audioSamples. putDataIntoBuffer (buf1, 512);
	   audiosamplesAvailable (512);
	}
}

//	Deemphasis	= 50 usec (3183 Hz, Europe)
//	Deemphasis	= 75 usec (2122 Hz US)
void	duoReceiver::setfmDeemphasis	(const QString& s) {
float	Tau;
int16_t	v	= s. toInt ();
	switch (v) {
	   default:
	      v	= 1;
	   case	1:
	   case 50:
	   case 75:
//	pass the Tau
	      Tau	= 1000000.0 / v;
	      alpha	= 1.0 / (float (192000) / Tau + 1.0);
	}
}


void    duoReceiver::set_freqSave   (void) {
        myLine  = new QLineEdit ();
        myLine  -> show ();
        connect (myLine, SIGNAL (returnPressed (void)),
                 this, SLOT (handle_myLine (void)));
}

void	duoReceiver::handle_myLine (void) {
int32_t freq    = theDevice -> getVFOFrequency ();
QString programName     = myLine -> text ();
        myList  -> addRow (programName, QString::number (freq / KHz (1)));
        delete myLine;
        myLine  = nullptr;
}

