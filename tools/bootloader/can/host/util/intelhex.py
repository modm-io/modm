#!/usr/bin/python2

import sys

class HexParserException(Exception):
	""" Ausnahmeklasse fuer den Intel-Hex-Parser """
	pass

class Segment:
	""" Speicher einen String mit Speicherinhalten zusammen mit seiner Startadresse """
	def __init__(self, address = 0, data = ''):
		self.address = address
		self.data = data
	
	def __getitem__(self, index):
		return self.data[index]
	
	def __len__(self):
		return len(self.data)

	def __repr__(self):	
		return "Segment (address = 0x%04x, data=%s)" % (self.address, self.data)


class IntelHexParser:
	""" Liest Intelhex Dateien """

	def __init__(self, filename = ''):
		""" Konstruktor """
		
		self.segments = []

		if filename:
			self.filename = filename
			self.load_file(filename)
		
	def load_file(self, filename):
		""" Liest die Datei in einen internen Puffer """
		try:
			file = open(filename)
			self.load_hex_data(file)
			file.close()
		except IOError:
			raise HexParserException("Could not open file: \"%s\"." % filename)
		except:
			raise HexParserException("No Intel-Hex Format!")

	def load_hex_data(self, file):
		""" liest die Daten aus einer Datei im Intel-Hex Format """
		segmentdata	= []
		currentAddr	= 0
		startAddr	= 0
		
		for line in file:
			l = line.strip("\n\r")
		
			# Leerzeilen herausfiltern
			if len(l) == 0:
				continue
			
			if l[0] != ':' or len(l) < 11: 
				raise HexParserException("File Format Error.")
			
			length 	= int(l[1:3], 16)
			address	= int(l[3:7], 16)
			type 	= int(l[7:9], 16)
			check 	= int(l[-2:], 16)

			if len(l) != (11 + 2*length):
				raise HexParserException("Invaild Line Length.")
		
			# Checksumme ueberpruefen
			bb = [int(l[i:i+2], 16) for i in xrange(1,len(l),2)]
			crc = reduce(lambda x, y: x+y, bb, 0)
			crc &= 0x0FF

			if crc != 0:
				raise HexParserException("Checksum Error.")
			
			if type == 0x00:
				if currentAddr != address:
					if segmentdata:
						self.segments.append( Segment(startAddr, ''.join(segmentdata)) )
					startAddr = currentAddr = address
					segmentdata = []
				for i in range(length):
					segmentdata.append( chr(int(l[9+2*i:11+2*i],16)) )
				currentAddr = length + currentAddr
			elif type == 0x01:
				# Ende der Hexdaten
				if length == 0:
					break
				else:
					raise HexParserException("Invalid End-of-File Record")
			elif type in (0x02, 0x03, 0x04):
				pass
			else:
				sys.stderr.write("Ignored unknown field (type 0x%02x) in ihex file.\n" % type)
		
		if segmentdata:
			self.segments.append( Segment(startAddr, ''.join(segmentdata)) )
	
	def __repr__(self):
		""" Gibt die geladene IntelHex Datei aus """
		buffer = []
		
		if self.segments:
			for segment in self.segments:
				buffer.append("Startadresse: %s, Daten:\n" % segment.address)
				
				counter = 0
				for value in segment:
					buffer.append("%02x " % ord(value))
					counter += 1
					if counter >= 26:
						counter = 0
						buffer.append("\n")
				buffer.append("\n\n")
			buffer.pop()
		
		return ''.join(buffer)

# kleines Beispiel zur Verwendung des Intelhexparsers
#
# Liest eine uebergebene Intelhex-Datei ein und gibt den Inhalt entsprechend
# verarbeitet aus.

if __name__ == '__main__':
	filename = sys.argv[1]
	
	try:
		parser = IntelHexParser(filename)
		print(parser)
	except HexParserException as e:
		# Fehlermeldungen ausgeben
		print(e)

