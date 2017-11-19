#!/usr/bin/env python3
# -*- coding: utf-8 -*-

class ParserError(Exception):
	
	def __init__(self, elem, msg):
		""" Emit a error message
		
		elem	- element that reports the error
		msg		- error message
		
		"""
		self.elem = elem
		Exception.__init__(self, msg)
		
	def __str__(self):
		msg = ""
		if self.elem is not None:
			msg += "in %s, " % self.elem
		msg += Exception.__str__(self)
		return msg
