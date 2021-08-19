DUORECEIVER 1.0

COMBINED FM AND DAB MINI RECEIVER

---------------------------------------------------------------------------
---------------------------------------------------------------------------

![duoreceiver](/duoreceiver-1.png?raw=true)
![duoreceiver](/duoreceiver-2.png?raw=true)


I got questions from family members why no combined
FM and DAB receiver was available, since there were
multiple versions of both a DAB decoder and an FM decoder written.
*Duoreceiver* is an answer to that, using the philosophy of the DAB mini
program iy is a "mini" version of a DAB/FM receiver.

The current version supports 
 - the SDRplay devices using the "old" 2.13 interface library
 - the airspy devices
 - rtlsdr sticks

Further devices may be added later on.

*Selecting* a device is automatic,
on program startup, the software will check the availability of
any of the configured devices.
If any of the configured devices
is connected and can be initialized, the duoreceiver program will
detect it and use it as input device.

Note that the SDRplay control widget is included in the main widget,
it merely contains a radio button for switching the agc on or off, 
a spinbox for the IF gain reduction, restricted to the values 20 .. 59
and a spinbox for setting an LNA state.
If the agc is "on", the IF gain reduction spinbox is hidden.

Note that the airspy control widget is complicated and displayed
in a separate widget.

The "control" for the rtlsdr sticks is also included in the
main widget. It is kept simple, just a radio button 
for the agc and a gain setting, using the gains extracted from the
device library

![duoreceiver](/duoreceiver-4.png?raw=true)

------------------------------------------------------------------------
Support for presets
------------------------------------------------------------------------

Both the FM and the DAB subsystems support *presets*.

![duoreceiver](/duoreceiver-3.png?raw=true)

For the FM subsystem, the widget contains button labeled *save frequency*
that - when touched - will open a small widget in which a name may be
given to the currently selected frequency.

*Touching* the name in the preset window will instruct the software to
select the associated frequency. *Touching the name twice in a short period*
will remove the entry from the list.

For the DAB subsystem, the widget contains the combobox labeled *presets*.
Touching the combobox will show the registered presets.
Touching a name in the list will instruct the software to switch to the
service with that name.

*Adding* a service to the list is by touching with the right mouse button
the currently selected service.

On switching off one of the subsystems, the duoreceiver will remember
the frequency for the FM subsystem, or the service for the DAB subsystem.
Next time such a subsystem is selected, the  frequency (for FM)
or the service (for DAB) will be selected automatically.

----------------------------------------------------------------------------
A note on some settings
----------------------------------------------------------------------------

A small "configuration file" is maintained for saving some settings.
The file is named ".duoreceiver.ini" and maintained in the user's
home directory.
This ".ini" file will (a.o) register the subsystem used, and on starting
the program the registered subsystem , either F< or DAB, is activated.

Two other files are maintained
 - a file ".duoreceiver-fm-stations.bin", a file in a binary format containing the
list of saved fm stations with their frequency;
 - a file ".duoreceiver-dab-stations.xml", a file in ASCII format containing
the presets, encoded in XML.

----------------------------------------------------------------------
# Copyright
------------------------------------------------------------------------

        Copyright (C)  2021
        Jan van Katwijk (J.vanKatwijk@gmail.com)
        Lazy Chair Computing

        The Qt-DAB  software and derivatives such as duoreceiver
	is made available under the GPL-2.0.
        The software, is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.


