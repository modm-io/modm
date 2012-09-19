% AVR Studio (DE)
% 
%

AVR Studio als Entwicklungsumgebung
===================================


AVR Studio 4 bietet zwar keine Unterstützung für C++ in Form
von Syntax-Highlighing oder ähnlichem, enthält dafür aber einen Simulator.
Mit ein wenig Phantasie kann man damit sogar seinen C++-Code simulieren.

Als komplette Entwicklungsumgebung eignet sich das AVR Studio damit
eher nicht, trotzdem wollen wir kurz zeigen wie man ein XPCC Projekt
damit einrichtet.


Neues Projekt anlegen
---------------------

<screenshot href="images/windows/avrstudio_project_new.png" title="" />

Als erstes muss ein neues Projekt angelegt werden.

<screenshot href="images/windows/avrstudio_project_config.png" title="" />
		
Beim Auswählen des Projekt-Names muss man darauf achten, dass
dieser genau dem in der Konfigurations-Datei (*project.cfg*) eingestellten Namen
entspricht.

Stimmen die beiden Namen nicht überein erkennt AVR Studio später
die compilierten Dateien nicht.

<screenshot href="images/windows/avrstudio_device_select.png" title="" />
		
Für den Simulator muss natürlich auch noch der richtige AVR
ausgewählt werden. Auch hier muss der Eintrag mit dem aus der
Konfigurationsdatei übereinstimmen!


Konfiguration
-------------

<screenshot href="images/windows/avrstudio_config.png" title="" />

<screenshot href="images/windows/avrstudio_new_config.png" title="" />

<screenshot href="images/windows/avrstudio_external_makefile.png" title="" />

<screenshot href="images/windows/avrstudio_compile_result.png" title="" />

<clear />

Zurück zur [Installationsanleitung](install_windows.html).
