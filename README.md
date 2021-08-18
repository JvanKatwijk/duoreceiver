DUORECEIVER

COMBINED FM AND DAB MINI RECEIVER

---------------------------------------------------------------------------
---------------------------------------------------------------------------

![duoreceiver](/duoreceiver-1.png?raw=true)
![duoreceiver](/duoreceiver-2.png?raw=true)


I got questions from family members why there was no combined
FM and DAB receiver.
*Duoreceiver* is an answer to that, using the philosophy of the DAB mini
program a "mini" version of a DAB/FM receiver is implemented.

The current version supports 
 - the SDRplay devices using the "old" 2.13 interface library
 - the airspy devices
 - rtlsdr sticks

Selecting a device is automatic on program startup.
On program startup, the software will check the availability of
any of the configured devices.
If one of the configured devices
is connected, the software will detect it and use it as input device.

Note that the SDRplay control widget is included in the main widget,
it merely contains a radio button for switching the agc on or off, and
a spinbox for the IF gain reduction, restricted to the values 20 .. 59.

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

Adding a service to the list is by touching with the right mouse button
the currently selected service.

----------------------------------------------------------------------------
ToDo
----------------------------------------------------------------------------

- generating an appImage

----------------------------------------------------------------------------
A note on some settings
----------------------------------------------------------------------------

A small "configuration file" is maintained for saving some settings.
The file is names ".duoreceiver.ini" and maintained in the user's
home directory.
The ".ini" file will register the subsystem used, and on starting
the program the registered subsystem , either F< or DAB, is activated.

Two other files are maintained
 - a file ".duoreceiver-fm-stations.bin", a file in a binary format containing the
list of saved fm stations with their frequency;
 - a file ".duoreceiver-dab-stations.xml", a file in ASCII format containing
the presets, encoded in XML.

