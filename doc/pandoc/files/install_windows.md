% Installationsanleitung für Windows

Diese Anleitung ist für Windows 7 geschrieben, sollte sich aber
für ältere Versionen (Windows XP und Vista) mit leichten Änderungen
genauso anwenden lassen.

Klick auf die Bilder um sie zu vergrößern. Mit den Pfeiltasten
kann man zusätzlich zwischen den Bildern umschalten.


Schritt 1: Hol dir den Code
---------------------------

<screenshot href="images/windows/tortoisesvn_download.png" title="Installiere TortoiseSVN" />

Installiere [TortoiseSVN][].

<screenshot href="images/windows/tortoisesvn_context_menu.jpg" title="TortoiseSVN: Kontextmenü" />

Erstelle einen neuen Ordner und benutze das Rechtsklick-Kontextmenü
von TortoiseSVN um einen Checkout in dem Ordner zu machen. Hier
wird `xpcc` als Ordnername verwendet.

<screenshot href="images/windows/tortoisesvn_checkout_parameter.jpg" title="TortoiseSVN: Einstellen des SVN Servers" />

Die URL um den aktuellsten Stand herunterzuladen lautet
`https://xpcc.svn.sourceforge.net/svnroot/xpcc/trunk`.
Man muss beachten, dass sich diese Version aktuell gerade in der Entwicklung befindet,
es können sich also immer Änderungen ergeben. Die Doxygen-Dokumentation
wird aus dieser Version erstellt.

Unter `https://xpcc.svn.sourceforge.net/svnroot/xpcc/tags/`
liegen die letzten stabilen Versionen. Aktuell ist dies
`https://xpcc.svn.sourceforge.net/svnroot/xpcc/tags/1.1/`.

<screenshot href="images/windows/tortoisesvn_checkout_progress.jpg" title="TortoiseSVN: Herunterladen des Codes" />

Danach sollte TortoiseSVN das gesamte Projekt herunterladen.

Ab jetzt können Änderungen mit *Rechtsklick &raquo; SVN Update* heruntergeladen
werden.


Schritt 2: Installiere die Build-Umgebung (Python & Scons)
----------------------------------------------------------

<screenshot href="images/windows/python_download.png" title="Download Python" />

Wir verwenden *SCons* anstelle von *Make* um die
Programme zu compilieren.
Da SCons auf Python basiert muss man zuerst den Windows Installer von
[Python][python] herunterladen und installieren.

Man sollte die aktuellste 2.x Version von Python in 32-Bit verwenden.
Aktuell ist dies Version 2.7.2 (die Datei heißt dann "Python 2.7.2 Windows Installer"). Andere
Versionen könnten auch funktionieren, sind aber bisher nicht getestet.

<screenshot href="images/windows/python_install.jpg" title="Installiere Python" />

Installiere Python und merk dir den Installations-Pfad.

<screenshot href="images/windows/python_path_prepare.jpg" title="Füge Python dem Systempfad hinzu" />

Um den Befehl `scons` verfügbar zu machen muss der
System-Pfad geändert werden. Gehe zu
*Start &raquo; Systemsteuerung &raquo; System &raquo; Erweiterte Systemeinstellungen*

<screenshot href="images/windows/python_path.jpg" title="Füge Python dem Systempfad hinzu" />

Das Verzeichnis von Python muss der PATH-Variable hinzugefügt werden. Bei einer
Standard-Installation ist der Pfad `C:\Program Files\Python27`

Also hinzufügen (getrennt durch ";")
`C:\Program Files\Python27`
`C:\Program Files\Python27\Scripts`

Zum Beispiel: 
`C:\Program Files\Python27;C:\Program Files\Python27\Scripts;`

<screenshot href="images/windows/scons_download.png" title="Installiere SCons" />

Lade [SCons][scons] herunter und installiere es.


Schritt 3: Entwicklungsumgebung für AVRs
----------------------------------------

<screenshot href="images/windows/winavr_download.png" title="Installiere WinAVR" />

[WinAVR][winavr] ist ein Program-Paket das die gesamte Entwicklungsumgebung
für die AVRs enthält. Lade die aktuellste Version herunter und installiere
sie. Zum Zeitpunkt des Schreibens dieser Anleitung war
dies [WinAVR-20100110][winavr_install].

<screenshot href="images/windows/cmd.jpg" title="Öffnen die Kommandozeile" />

Um zu überprüfen das alles richtig eingerichtet ist werden
wir eines der Beispiel-Programme compilieren. Öffne dazu die Kommandozeile
indem du `cmd` in die Suchleiste des Startmenüs eintippst.
(Später werden wir noch komfortablere Methoden vorstellen um SCons
auszuführen. Zum Testen ist es allerdings einfacher so).

<screenshot href="images/windows/cmd_gpio_scons.jpg" title="Führe SCons aus" />

Navigiere zum GPIO Bespiel in `xpcc/examples/avr/gpio/basic`
und führe `scons` aus. Falls scons nicht gefunden wird
überprüfe den Systempfad. Mit `echo %PATH%` kannst du
den aktuellen Wert ausgeben lassen. Das `Scripts` Verzeichnis
aus dem Python-Pfad und der Python-Pfad muss darin enthalten sein.

<screenshot href="images/windows/cmd_gio_scons_error_missing_visual_studio.jpg"	title="SCons beschwert sich eventuell über ein fehlendes Visual Studio. Ignoriere diese Warnung." />

Falls es eine Warnung über ein nicht gefundenes Visual Studio gibt,
ignoriere diese. Dies ist ein Fehler innerhalb von SCons und hat
nichts zu sagen.

<screenshot href="images/windows/cmd_gpio_scons_success.jpg" title="Kompilieren war erfolgreich" />

Wenn alles korrekt eingerichtet ist sollte die Ausgabe wie
im Screenshot aussehen. Glückwunsch, du hast dein erstes Program
mithilfe von XPCC compiliert.

Alle jetzt folgenden Schritte sind optional.


Schritt 3.1: Installiere AVR Studio
-----------------------------------

<screenshot href="images/windows/avrstudio_download.png" title="Lade AVR Studio 4.18 herunter" />

Selbst wenn du AVR Studio nicht als Entwicklungsumgebung nutzen
willst brauchst du es um an die Treiber für die Atmel Programmier-Tools
wie AVRISP mkII oder AVR JTAGICE mkII zu kommen.

Um das AVR Studio herunter zu laden muss man ein Registierungsformular
ausfüllen. Es muss eine gültige E-Mail Adresse angegeben werden, da
der Download-Link dorthin geschickt wird. Alternativ sind im
[microcontroller.net Wiki][avrstudio] aber auch die Direktlinks auf die
verschieden Versionen aufgelistet. Damit spart man sich die Registrierung
und kann direkt loslegen.

<screenshot href="images/windows/avrstudio_jungo_driver.jpg" title="Installiere den Jungo Driver" />

Bei der Installation des AVR Studios sollte man auf jeden Fall
den Jungo-Treiber mit installieren.

Um USB-Programmer mit **avrdude** verwenden zu können muss zusätzlich
die **libusb** installiert werden. Die Vorgehensweise dazu ist auf einer
[extra Seite](install_libusb.html) beschrieben.


Schritt 4: Installiere eine IDE
-------------------------------

Da es gerade bei der IDE sehr viele verschiedene Vorlieben gibt wollen
wir hier drei populäre Entwicklungsumgebungen vorstellen und zeigen
wie XPCC dort eingebunden werden kann.

- [Programmers Notepad 2](install_ide_pn2.html)
- [AVR Studio 4](install_ide_avrstudio4.html)
- [Eclipse](install_ide_eclipse.html)


Schritt 5: Erweiterte Build-Umgebung
------------------------------------

<screenshot href="images/windows/python_setuptools_download.png" title="Installiere setuptools" />

Für das Kommunikations Framework nutzen wir zwei zusätzliche
Python Module: lxml und jinja2.
Der einfachste Weg sie zu installieren ist es *easy_install* zu verwenden. 
Dieses ist Teil des [setuptools][] Paketes.

Installiere [setuptools][] für die installiere Version von Python.

<screenshot href="images/windows/python_setuptools_rights.jpg" title="Setze die Zugriffsrecht für den Python-Ordner" />

Mit Windows 7 hat Windows ein besseres Rechte-Mangement eingeführt. 
Allerdings scheint easy_install daran noch nicht angepasst zu sein. Damit
es funktioniert braucht der Benutzer Schreibrechte für das Python
Verzeichnis.

Gehe zum Python Ordner, *Rechtsklick &raquo; Eigenschaften &raquo;
Sicherheit* und erlaube Vollzugriff für den aktuellen Benutzer.

<screenshot href="images/windows/python_lxml.jpg" title="Installiere lxml" />

Öffne die Kommandozeile und führe `easy_install lxml==2.3` aus.

<screenshot href="images/windows/python_jinja2.jpg" title="Installiere jinja2" />

Als nächstes führe `easy_install jinja2` aus um die Jinja2 Template Engine
zu installieren.

<screenshot href="images/windows/cmd_communication_scons.jpg" title="Überprüfe ob sich das Kommunikations-Beispiel kompilieren lässt." />

Um zu testen das alles funktioniert gehe zu `xpcc/examples/avr/communication/sender`
und führe SCons aus um das Program zu compilieren.

<screenshot href="images/windows/cmd_communication_scons_success.jpg" title="Kompilieren war erfolgreich" />

Wenn SCons keine Fehler ausgibt und die Programgröße angezeigt
wird ist alles in Ordnung.


Schritt 6: (optional) Unittests & Visual Studio
-----------------------------------------------

<screenshot href="images/windows/vc++_download.png" title="Lade Visual C++ herunter" />

Um die XPCC Unittests auf dem PC auszuführen kann die 
Visual C++ in der Express Version verwendet werden. Diese bekommt
man frei auf der Microsoft Webseite.

<screenshot href="images/windows/cmd_unittest_scons.jpg" title="Kompiliere und starte die Unittests" />

Gehe zum XPCC Stammordner und führe `scons unittest` aus.

<screenshot href="images/windows/cmd_unittest_success.jpg" title="Übersicht Unittests" />

SCons wird dann alle für diese Platform verfügbaren Unittests
kompilieren und ausführen.

Als Ergebnis bekommt man eine List mit allen Fehlern sowie eine
Übersicht über die bestanderen Tests. Im Idealfall sollten natürlich
gar keine Fehler vorhanden sein ;-)


[python]: http://www.python.org/download/
[scons]: http://www.scons.org/download.php
[setuptools]: http://pypi.python.org/pypi/setuptools
[avrstudio]: http://www.mikrocontroller.net/articles/AVR-Studio
[winavr]: http://sourceforge.net/projects/winavr/files/WinAVR/
[winavr_install]: http://sourceforge.net/projects/winavr/files/WinAVR/20100110/WinAVR-20100110-install.exe/download
[TortoiseSVN]: http://tortoisesvn.net/downloads
