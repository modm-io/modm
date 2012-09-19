% Eclipse (DE)
% Fabian Greif
% 

Eclipse als Entwicklungsumgebung
================================

Eclipse ist eine sehr mächtige IDE. Das bedeutet leider auch das es
sehr viel einzustellen gibt. Hat man dies aber einmal erledigt kann man
sehr effizient damit arbeiten.

Einige sehr nützliche Features sind zum Beispiel:

- Im Editor kann durch *Strg + Linksklick* auf einen Funktionsaufruf
  direkt zu der Deklaration der entsprechenden Funktion gesprungen werden.
  Fährt man mit dem Mauszeiger über ein Element, für das Quelltext-Dokumentation
  vorhanden ist, so wird diese in einem Pop-up-Fenster angezeigt.
- Wird der Cursor auf eine Variable gesetzt, so werden diese und alle
  weiteren Vorkommen derselben im Dokument farbig unterlegt, in einer
  Leiste am rechten Rand des Editors werden sie als kleine farbige Striche
  angezeigt, die als Links zu den entsprechenden Stellen im Quelltext fungieren.
- Code-Faltung.
- Unterstützung von Quellcodevervollständigung mittels *Strg + Leertaste*.
- Subversion Integration


Installation
------------

<screenshot href="images/eclipse/eclipse_download.png" />

Lade die aktuellste Version von *[Eclipse IDE for C/C++ Developers][eclipse]*
herunter und installiere sie. Diese Version enthält neben der Kern IDE
auch die [C/C++ Erweitung (CDT)][eclipse].
Hat man schon ein bestehende Eclipse Installation reicht es die CDT-Erweitung
nachzuinstallieren.

<screenshot href="images/eclipse/eclipse_startscreen.png" title="Startscreen" />

Vom Startscreen geht es zur Workbench.


Neues Projekt
-------------

<screenshot href="images/eclipse/eclipse_cpp_project.png" title="" />

Als nächstes muss ein neues C++ Projekt angelegt werden.

<screenshot href="images/eclipse/eclipse_cpp_project_settings.png" title="" />

Wähle zuerst einen Projekt Namen. Hier wird als Beispiel xpcc als Eclipse-Projekt
angelegt, dementsprechend wird als Name *xpcc* verwendet.
	
Will man später die Subversion-Erweitung von Eclipse verwenden ist
es wichtig das Projekt nicht im Workspace anzulegen, sondern an der
Stelle an der die Dateien auch auf der Festplatte liegen. Dazu den Hacken
bei *Use default location* entfernen und passenden Pfad auswählen.

Wähle zuerst *Project type &raquo; Makefile Project &raquo; Empty Project*,
dann *Toolchains &raquo; -- Other Toolchain --*. Die Wahl der Toolchain
beeinflusst die Dateien und Defines die Standardmäßig gesetzt sind.

<screenshot href="images/eclipse/eclipse_editor.png" title="" />

Über den *Project Explorer* kann man jetzt einzelne Quellcode-Dateien
öffnen. Allerdings werden die xpcc-Include-Dateien und Funktionen noch
nicht als solche erkannt.


Weitere Einstellungen
---------------------

<screenshot href="images/eclipse/eclipse_path_xpcc.png" title="" />

Dazu öffnet man *Project &raquo; Properties &raquo; C/C++ General &raquo; Paths and Symbols*. Hier
trägt man den Pfad zum `src`-Ordner von xpcc ein.

<screenshot href="images/eclipse/eclipse_path_avrgcc.png" title="" />

Gleiches macht man jetzt auch noch für die verwendete Toolchain. Für
avr-gcc ist das der `avr/include` Ordner im Installationsordner.
Bei mir ist dies `/opt/avr/avr/include`. Bei Windows könnte
dies `C:\Program Files\Atmel\AVR Studio 5.0\AVR Toolchain\avr\include` sein.

Für Sourcery CodeBench Lite Edition for ARM wäre es je nach Installation
zum Beispiel `/opt/arm-none-eabi/arm-none-eabi/include`.

<screenshot href="images/eclipse/eclipse_path_indexer.png" title="" />

Unter *Symbols* können Defines vorgegeben werden die Eclipse
dann bei der Anzeige berücksichtigt. Hier wurde der Wert `__AVR__`
eingetragen, welcher ansonsten standardmäßig vom Compiler erzeugt wird.

Diese Einstellungen sind aber nicht zwingend notwendig und können auch alle 
später noch geändert werden.

Sind Pfade und Symbole eingestellt muss der Indexer einmal durchlaufen
damit alle Einstellungen übernommen werden. Danach sollten alle Include-Dateien
und xpcc-Funktionen von Eclipse erkannt werden.

<screenshot href="images/eclipse/eclipse_env_path.png" title="" />

Eclipse verwendet eigene Shell. Deswegen werden zum Beispiel Einstellungen die
man in `.bashrc` vornimmt nicht übernommen. Damit Eclipse den Compiler
findet muss dieser entweder in den Standard-Pfaden liegen oder extra eingetragen werden.

Dazu *C/C++ Build &raquo; Environment* eine neue Variable `PATH`
mit dem Inhalt `${PATH}` anlegen.

<screenshot href="images/eclipse/eclipse_path_check.png" title="" />

Eclipse übernimmt dann den aktuellen Wert von `PATH`. Man sollte
aber noch einmal überprüfen ob der Pfad zum Compiler wirklich enthalten ist. Dazu editiert
man nochmal die eben angelegte Variante. Eclipse sollte jetzt den Wert von PATH aufgelöst haben
und einzelne Verzeichnisse eingetragen haben. Im Beispiel sieht man das 
`/opt/avr/bin` enthalten ist. Unter Windows müsste es enstprechend
`C:\Program Files\Atmel\AVR Studio 5.0\AVR Toolchain\bin` sein.


Build-Configuration
-------------------

Als nächstes geht es darum den Code aus Eclipse heraus compilieren zu lassen.

<screenshot href="images/eclipse/eclipse_build_settings.png" title="" />

Da wir nicht Make sondern SCons verwenden muss der Hacken bei *Use default
build command* entfernt werden und `scons` als *Build command*
eintragen werden. Unter Windows muss diese `scons.bat` sein!

Ins *Build directory* kommt der Pfad zur SConstruct-Datei des
Projektes. Dadurch lässt sich das Projekt später per *Strg + B* bauen.
Eine andere Möglichkeit dazu sind Make-Targets die TODO

<screenshot href="images/eclipse/eclipse_build_behavior.png" title="" />

Im Reiter *Behaviour* wird bei *Clean* ein `-c` eingetragen.

Um das compilieren zu Beschleunigen kann man, je nach Anzahl der
verbauten CPUs des Rechners, ein Häckchen bei *Use parallel build*
setzten und bei *use parallel jobs* zum Beispiel 2 eintragen. Man muss
nur beachten immer eine Zahl vorzugeben, *Use optimal jobs number*
funktioniert bei SCons **nicht**!

<screenshot href="images/eclipse/eclipse_build_success.png" title="" />

Drückt man nun *Strg + B* sollte jetzt im *Console*-Reiter die
Ausgabe von SCons durchlaufen und das gesamte Projekt gebaut werden.

Hat man innerhalb eines Eclipse-Projektes mehrere Projekte
Mikrocontroller-Projekte die einzeln kompiliert werden sollen gibt
es zwei Möglichkeiten:  
Entweder man legt für Projekt eine eigne Build-Configuration an
oder man verwendet Make-Targets. Schauen wir zuerst nach dem
Build-Configurations:

- Gehe zu *Project &raquo; Properties &raquo; C/C++ Build*
  und klick auf *Manage Configurations*
- Erstelle mit *New* eine neue Konfiguration
- Wähle bei *Copy settings from &raquo; Exisiting Configuration*
  die Konfiguration die der neu zu erstellenden am ähnlichsten
  ist. Das erspart etwas Arbeit.
- Zurück bei *C/C++ Build* wähle bei *Configuration*
  die neu erzeugte Konfiguration aus und passe den Wert
  für *Build directory* an.
- Falls SCons noch weitere Paramter mitgegeben werden sollen
  (beispielsweise `scons target=pc`) ändere auch
  noch den Wert für *Build command*.

Über *Manage Configurations &raquo; Set Active* können die
verschiedenen Konfigurationen dann ausgewählt werden. Strg + B
erstellt dann die aktuelle Konfiguration.


Make Targets
------------

<screenshot href="images/eclipse/eclipse_make_target.png" title="" />

Eine weitere Möglichkeit sind zum starten von SCons sind Make-Targets.
Insbesondere für zusätzliche Aktionen die ausgeführt werden sollen, wie
zum Beispiel das Programmieren aus Eclipse heraus, eigenen sie sich sehr
gut.

Man wählt im *Project-Explorer* das Verzeichnis aus in dem später SCons
ausgeführt werden soll und erstellt ein neues Target.

<screenshot href="images/eclipse/eclipse_make_target_all.png" title="" />

Da wir vorher schon beim *Build command* SCons eingetragen haben
wird dies direkt hier übernommen.


<screenshot href="images/eclipse/eclipse_make_target_clean.png" title="" />

Genauso kann man ein `clean`- oder `program`-Target
anlegen.

<screenshot href="images/eclipse/eclipse_make_target_clean2.png" title="" />

Die Targets tauchen dann im Reiter *Make Targets* auf.


Hinweise
--------

- Falls ein Fehler wie `sh: avr-g++: not found` auftritt,
  überprüfen ob unter *Project &raquo; Properties &raquo; Build C/C++ &raquo; Environment*
  ein korrekter Wert für `PATH` eingetragen ist (siehe oben).

<clear />

Zurück zur [Installationsanleitung](install_windows.html).


[eclipse]: http://eclipse.org/downloads/
