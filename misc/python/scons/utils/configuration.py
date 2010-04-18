
import ConfigParser

# -----------------------------------------------------------------------------
class ParserException(Exception):
	pass

# -----------------------------------------------------------------------------
class Parser(ConfigParser.RawConfigParser):
	
	def read(self, filename):
		return ConfigParser.RawConfigParser.read(self, filename)
	
	def get(self, section, option, default=None):
		try:
			return ConfigParser.RawConfigParser.get(self, section, option)
		except (ConfigParser.NoOptionError,
				ConfigParser.NoSectionError), e:
			if default != None:
				return default
			else:
				raise ParserException(e)
	
	def getboolean(self, section, option, default=None):
		try:
			return ConfigParser.RawConfigParser.getboolean(self, section, option)
		except (ConfigParser.NoOptionError,
				ConfigParser.NoSectionError,
				ParserException), e:
			if default != None:
				return default
			else:
				raise ParserException(e)
	
	def items(self, section):
		try:
			return ConfigParser.RawConfigParser.items(self, section)
		except (ConfigParser.NoOptionError,
				ConfigParser.NoSectionError), e:
			raise ParserException(e)
