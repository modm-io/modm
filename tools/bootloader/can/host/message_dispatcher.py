#!/usr/bin/env python3

# -----------------------------------------------------------------------------
class MessageDispatcher:
	
	def __init__(self, filterList = None):
		"""Constructor"""
		self.filter = []
		if filterList:
			for filter in filterList:
				self.addFilter(filter)
	
	def addFilter(self, filter):
		"""Add a filter
		
		The filter-object must feature a check(message) method which returns
		True or False whether the callback should be called or not and a
		getCallback() method to retrieve this callback function.
		"""
		self.filter.append(filter)
	
	def removeFilter(self, filter):
		"""Remove this Filter"""
		self.filter.remove(filter)
	
	def send(self, message):
		pass
	
	def _processMessage(self, message):
		"""Check all filter for this message and call the callback
		functions for those how matches.
		"""
		for filter in self.filter:
			if filter.check(message):
				self._executeCallback(filter.getCallback(), message)
	
	def _executeCallback(self, callback, message):
		"""Call a callback function."""
		callback(message)
