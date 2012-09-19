% avrdude and libusb
% 
% 

avrdude und libusb
==================

Ohne installierte *libusb* findet avrdude unter Windows keinerlei
USB-Programmer. Es erscheint nur eine Fehlermeldung wie *did not find
any USB device "usb"*.


Installation
------------

<screenshot href="images/windows/libusb_install.png" title="" />

Lade [libusb-win32][libusb] herunter und entpacke es.

<screenshot href="images/windows/libusb_install_wizard.png" title="" />

Schließe den Programmer an und starte den Wizard im *bin* Verzeichnis.

<screenshot href="images/windows/libusb_install_wizard_device_select.png" title="" />

Er sollte darauf in der Liste der gefunden Geräte auftauchen.

<screenshot href="images/windows/libusb_install_wizard_device_install.png" title="" />

Installiere den Treiber für den Programmer.

<screenshot href="images/windows/libusb_install_unsigned_driver.png" title="" />

Windows beschwert sich das der Treiber nicht signiert ist, installiere ihn trotzdem.
Danach muss der Programmer einmal abgezogen und neu anschloßen werden und **avrdude** sollte ihn erkennen.

<clear />

Zurück zur [Installationsanleitung](install_windows.html).


[libusb]: http://sourceforge.net/projects/libusb-win32/files/libusb-win32-releases/1.2.4.0/
