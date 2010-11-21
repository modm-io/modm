#!/usr/bin/env python

import sys

class ProgressBar:
	""" Erstellt eine text-basierte Statusanzeige.
	
	Um den Fortschrittsbalken anzuzeigen ruft man das Objekt als Funktion auf.
	Das koennte zum Beispiel so aussehen:
    
	>>> foo = ProgressBar(width=40)
	>>> foo(22)
	[=======>         22%                   ]
	
	Beim erstellen kann man ausserdem den Maximal/Minimalwert und die Breite
	angeben. Die Breite entspricht der kompletten Anzeige inklusive der
	oeffnenden bzw. schliessenden Klammern
	"""
	def __init__(self, min = 0, max = 100, width = 80):
		self.progbar = ""
		self.min = min
		self.max = max
		self.width = width
		self.value = 0
		self.update(0)
	
	def update(self, value = 0):
		""" Aktualisiert den Fortschrittsbalken.
		
		Wenn value ausserhalb der Maximalen Grenzen liegt wird es auf den 
		den Minimal bzw. Maximalwert begrenzt.
		"""
		if value < self.min: value = self.min
		if value > self.max: value = self.max
		
		if value == self.value:
			return False
		self.value = value
		# Ausrechnen wieviel Prozent erledigt sind
		percent_done = (float(value - self.min) / 
						float(self.max - self.min)) * 100.0
		percent_done = int(round(percent_done))
		
		max_char = self.width - 2
		num_hashes = int(round((percent_done / 100.0) * max_char))

		if num_hashes == 0:
			self.progbar = "[>%s]" % (' '*(max_char-1))
		elif num_hashes == max_char:
			self.progbar = "[%s]" % ('='*max_char)
		else:
			self.progbar = "[%s>%s]" % ('='*(num_hashes-1),
										' '*(max_char-num_hashes))

		# Prozentanzeige ungefaehr mittig einsetzen.
		percent_position = (len(self.progbar) / 2) - len(str(percent_done)) 
		percent_str     = str(percent_done) + "%"
		self.progbar = ''.join([self.progbar[0:percent_position], percent_str,
								self.progbar[percent_position+len(percent_str):]])
		return True

	def __str__(self):
		""" Gibt den aktuellen Fortschrittsbalken aus. 
		
		Die Laenge des zurueckgegebenen Strings entspricht dem Wert von "width" der
		beim erzeugen des Objekts mitgegeben wurde (Standardwert ist 80).
		"""
		return str(self.progbar)

	def __call__(self, value):
		""" Aktualisiert die Statusanzeige und gibt sie auf den stdout-Stream
		aus falls sie sich geaendert hat.
		
		Schreibt als erstes ein "carrige return" sodass die aktuelle Zeile
		ueberschrieben wird.
		"""
		if self.update(value):
			progbar = str(self)
			sys.stdout.write("\r" + progbar)
			sys.stdout.flush()

# Kleines Testprogramm fuer die ProgressBar-Klasse
#
# Gibt auf der Konsole einen 60 Zeichen breiten Fortschrittsbalken aus
# der langsam von 0 auf 100 % laeuft.

if __name__ == '__main__':
	import time

	bar = ProgressBar(width=60)

	for x in range(0,101):
		time.sleep(0.10)
		bar(x)
	print ""

