#!/usr/bin/env python2

# -----------------------------------------------------------------------------
class BaseFilter:
	"""Base class for message filter
	
	Will accept all messages if used.
	"""	
	def __init__(self, callback):
		self.callback = callback
	
	def getCallback(self):
		return self.callback
	
	def check(self, message):
		"""Checks if a message matches the criteria"""
		return True

# -----------------------------------------------------------------------------
class AttributeFilter(BaseFilter):
	"""Universal filter that checks whether the attributes of the message class
	meets a list of criteria.
	"""
	def __init__(self, callback, criteria):
		"""
		- 'criteria' is a list of (attribute name, value) tuples which
		will be tested. If the message matches all attributes it will be
		accepted and the callback will be executed."""
		BaseFilter.__init__(self, callback)
		
		self.criteria = criteria
	
	def check(self, message):
		"""Checks if a message matches the criteria"""
		try:
			for attr, value in self.criteria:
				if getattr(message, attr) != value:
					return False
		except AttributeError:
			return False
		return True


# -----------------------------------------------------------------------------
class CanFilter(BaseFilter):
	"""A Filter specifically suited for CAN messages
	"""
	def __init__(self, callback, id = 0, extended = True, rtr = False):
		BaseFilter.__init__(self, callback)
		
		self.id = id
		self.extended = extended
		self.rtr = rtr
	
	def check(self, message):
		if message.id == self.id and \
			message.extended == self.extended and \
			message.rtr == self.rtr:
			return True
		return False


# -----------------------------------------------------------------------------
if __name__ == '__main__':
	import time
	
	class Message:
		def __init__(self):
			self.id = 1000
			self.extended = False
			self.rtr = False
	
	filter1 = AttributeFilter(None, [["id", 1000], ["extended", False], ["rtr", False]])
	filter2 = CanFilter(None, id = 1000, extended = False)
	
	msg = Message()
	
	times = 100000
	start = time.time()
	for i in xrange(times):
		filter1.check(msg)
	mid = time.time()
	for i in xrange(times):
		filter2.check(msg)
	end = time.time()
	
	print(mid - start)
	print(end - mid)
