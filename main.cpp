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
 *
 *	Main program
 */
#include	<QApplication>
#include	<QSettings>
#include	<unistd.h>
#include	<QDir>
#include	"duoreceiver.h"

#define	DEFAULT_INI	".duoreceiver.ini"
#define	FM_STATION_LIST	".duoreceiver-fm-stations.bin"
#define	DAB_PRESETS	".duofreceiver-dab-presets.xml"

int	main (int argc, char **argv) {
QSettings	*ISettings;		/* .ini file	*/
duoReceiver	*MyRadioInterface;
QString iniFile = QDir::homePath ();
QString	fmStationList	= QDir::homePath ();
QString dabPresets	= QDir::homePath ();

	iniFile. append ("/");
	iniFile. append (DEFAULT_INI);
	iniFile	= QDir::toNativeSeparators (iniFile);

	fmStationList. append ("/");
        fmStationList. append (FM_STATION_LIST);
        fmStationList = QDir::toNativeSeparators (fmStationList);

        dabPresets. append ("/");
        dabPresets. append (DAB_PRESETS);
        dabPresets = QDir::toNativeSeparators (dabPresets);

/*
 *	... and the settings of the "environment"
 */
	ISettings	= new QSettings (iniFile, QSettings::IniFormat);
/*
 *	Before we connect control to the gui, we have to
 *	instantiate
 */
	QGuiApplication::setAttribute (Qt::AA_EnableHighDpiScaling);
	QApplication a (argc, argv);
	a. setWindowIcon (QIcon (":/FM-Radio.ico"));

	MyRadioInterface = new duoReceiver (ISettings,
	                                    fmStationList, dabPresets);
	MyRadioInterface -> show ();
	a. exec ();
/*
 *	done:
 */
	fflush (stdout);
	fflush (stderr);
	delete MyRadioInterface;
	delete ISettings;
	fprintf (stderr, "It is done\n");
}
