#!/usr/bin/env python
# -*- coding: utf-8 -*-

class ParserError(Exception):
	
	def __init__(self, elem, msg):
		""" Emit a error message
		
		elem	- element that reports the error
		msg		- error message
		
		"""
		self.elem = elem
		Exception.__init__(self, msg)
