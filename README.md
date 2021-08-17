DUORECEIVER

COMBINED FM AND DAB MINI RECEIVER

---------------------------------------------------------------------------
---------------------------------------------------------------------------

![duoreceiver](/duoreceiver-1.png?raw=true)
![duoreceiver](/duoreceiver-2.png?raw=true)


I got questions from family members why therewas not a combined
FM and DAB receiver.
Duoreceiver is an answer to that, using the philosophy of the DAB mini
program a "mini" version of a DAB/FM receiver is implemented.

The current version supports 
 - the SDRplay devices using the "old" 2.13 interface library
 - the airspy devices
 - rtlsdr sticks

Selecting a device is automatic, if one of the configured devices
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

- generating a windows version
- generating an appImage

