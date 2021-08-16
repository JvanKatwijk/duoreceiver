DUORECEIVER

COMBINED FM AND DAB MINI RECEIVER

(work in progress)
---------------------------------------------------------------------------

![duoreceiver](/duoreceiver-1.png?raw=true)
![duoreceiver](/duoreceiver-2.png?raw=true)


I got questions from family members why therewas not a combined
FM and DAB receiver.
Duoreceiver is an answer to that, using the philosophy of the DAB mini
program a "mini" version of a DAB/FM receiver is implemented.

The current version supports 
 - the SDRplay devices using the "old" 2.13 interface library
 - the airspy devices. 

Note that the airspy control widget is complicated and displayed
in a separate widget, while the SDRplay control widget is made part
of the main widget.

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

-------------------------------------------------------------------------
Support for other devices
-------------------------------------------------------------------------

Next step will be to add the DABstick to the list of supported devices




