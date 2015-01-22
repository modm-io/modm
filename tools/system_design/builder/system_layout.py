#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright (c) 2012, Roboterclub Aachen e.V.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of the Roboterclub Aachen e.V. nor the
#    names of its contributors may be used to endorse or promote products
#    derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# -----------------------------------------------------------------------------

import os
import sys
import builder_base

import numpy as np

from pyx import *
from pyx.connector import arc, curve

sys.path = [os.path.join(os.path.dirname(__file__), '..')] + sys.path
from xmlparser.utils import SingleAssignDictionary

class SystemLayoutBuilder(builder_base.Builder):
	"""
	Generate a visual map of all containers, components and events. 
	
	A common call would be like:
	$python system_layout.py ../../../../roboter/2012_captain/software/global/xml/robot.xml -o /tmp  -s simulator -s "drive simulation" -s "External"
	
	A PDF called system.pdf is generated in /tmp/system.pdf.
	
	If you want to display actions add the -a switch.
	"""
	
	VERSION = "0.1"
	
	def get_y(self, event):
	    """
	    Get the vertical coordinate for an event line
	    """
	    return 8.0 - (self.eventsSorted.index(event) * 0.1)

	def get_component_width(self, component):
		"""
		Get the horizontal width of a component.
		"""
		
		# width of text
		text_width = text.text(0, 0, r"\bf %s" % component, self.textattrs).width.x
		
		# number of arrows
		nr = len(component.events.subscribe) + len(component.events.publish)
		
		# Add actions if desired
		if self.options.actions:
			nr = nr + len(component.actions)
			
		return max((text_width * 15) + 0.1, 0.1 * nr)
		
	
	def get_container_width(self, container):
		"""
		Get the horizontal width of a container. 
		"""
		
		width = 0
		for component in container.components:
		    # include actions from abstract component
		    component = component.flattened()
		
		    width = width + self.get_component_width(component) + 0.1
		
		return width
	  
	def get_graph_width(self, tree):
	    width = 0
	    for container in tree.container:
	        # skip containers that the user asked to skip
	        if self.options.skipList is not None and container.name in self.options.skipList:
	            continue
	
	        width = width + self.get_container_width(container) + 0.1
	    
	    return width 

	def generate(self):
		# check the commandline options
		if not self.options.outpath:
			raise builder_base.BuilderException("You need to provide an output path!")
		
		# Initialise 
		unit.set(uscale=7)
		
		text.set(mode="latex")
		text.preamble(r"\usepackage{helvet}")
		text.preamble(r"\renewcommand*\familydefault{\sfdefault}")
		text.preamble(r"\normalfont")

		
		c = canvas.canvas()
		
		### Object to draw
		eventArrowsSource = []
		eventCirclesSource = []
		eventTextSource = []
	
		eventCirclesSinkless = []
		eventCirclesSourcless = []
		
		eventArrowsSink = []
		eventCirclesSink = []
		eventTextSink = []
		actionArrows = []
		actionText = []

		### Text Attributes
		self.textattrs                   = [text.halign.center, text.vshift.middlezero]
		self.textcontainerattrs          = [text.halign.left,   text.vshift.middlezero]
		self.texteventattrs              = [text.halign.left,   text.vshift(-0.6)]
		self.texteventrotattrs           = [text.halign.left,   text.vshift(-0.6), trafo.rotate(90), color.rgb.red]
		self.texteventsubscribedrotattrs = [text.halign.left,   text.vshift(-0.6), trafo.rotate(90), color.rgb.blue]
		self.textactionrotattrs          = [text.halign.left,   text.vshift(-0.6), trafo.rotate(90), color.rgb(0.0, 0.5, 0.1)]


		# raster for design
		debug = False
		if debug:
			for x in np.arange(0, 20, 0.05):
				c.stroke(path.line(x, 10, x, 0), 
					   [style.linewidth.thin, color.gray(0.8)])
			
			for y in np.arange(0, 10, 0.05):
				c.stroke(path.line(0, y, 20, y), 
					   [style.linewidth.thin, color.gray(0.8)])
		
			for x in np.arange(0, 20, 0.5):
				c.stroke(path.line(x, 10, x, 0), 
					   [style.linewidth.thin, color.gray(0.5)])
			
			for y in np.arange(0, 10, 0.5):
				c.stroke(path.line(0, y, 20, y), 
					   [style.linewidth.thin, color.gray(0.5)])

		
		# Sorted events
		# Sort events by publishing component. Makes graph more structured. 
		self.eventsSorted = []

		print("Analysing containers:")
		for container in self.tree.container:
			print " * " + container.name

		print("Done. Creating graph")

		for container in self.tree.container:
			if container.name is None:
				continue
			if self.options.skipList is not None and container.name in self.options.skipList:
				continue
			for component in container.components:
				# include actions from abstract component
				component = component.flattened()
				
				for event in component.events.publish:
					# add only once
					if not (event in self.eventsSorted):
						self.eventsSorted.append(event)

		# Add sourceless events to the end
		for event in self.tree.events:
			if not (event in self.eventsSorted):
				self.eventsSorted.append(event)
		
		# Draw light gray horizontal lines for all events		
		graph_width = self.get_graph_width(self.tree) + 2.1
		for event in self.tree.events:
			event_y = self.get_y(event)
			c.stroke(path.line(0, event_y, graph_width, event_y),
					 [style.linewidth.THick, color.grey(0.90)])
			
			# Draw Id and name of event
			A = text.text(0, event_y, r"\bf \texttt{[0x%02x]} %s" % (event.id, event.name), self.texteventattrs)
			c.insert(A)

			# Draw type of Event
			if event.type is not None:
				A = text.text(1.1, event_y, r"%s" % (event.type.name), self.texteventattrs)
				c.insert(A)

		# Legend
		A = text.text(  0, 8.1, r"\textbf{%s}" % ("Id"), self.texteventattrs)
		c.insert(A)

		A = text.text(0.175, 8.1, r"\textbf{%s}" % ("Name"), self.texteventattrs)
		c.insert(A)

		A = text.text(1.1, 8.1, r"\textbf{%s}" % ("Type"), self.texteventattrs)
		c.insert(A)

		sinklessDict   = dict()
		sourcelessDict = dict()
			
		# Write sink and sourceless events
		for event in self.tree.events:
			sourceless = True
			sinkless   = True
			for component in self.tree.components:
				# include actions from abstract component
				component = component.flattened()
		
				if self.options.skipList is not None and component.name in self.options.skipList:
					continue
				for event_cmp in component.events.publish:
					if event_cmp == event:
						sourceless = False
				for event_cmp in component.events.subscribe:
					if event_cmp == event:
						sinkless = False
			if sourceless:
				A = text.text(1.8, self.get_y(event), r"\bf sourceless", self.texteventattrs)
				c.insert(A)
			sourcelessDict[event.name] = sourceless
			if sinkless:
				A = text.text(2.1, self.get_y(event), r"\bf sinkless", self.texteventattrs)
				c.insert(A)
			sinklessDict[event.name] = sinkless


		# Empty directories to find out from where to where a thick line
		# for events must be drawn. 
		eventsLeft = SingleAssignDictionary('EventLeft')
		eventsRight = dict()

		# Write what was skipped
		c.insert(text.text(1, 9.8, r"\bf System Design generated from base file", self.textattrs))
		c.insert(text.text(1, 9.7, r"\bf %s" % self.xmlfile.replace('_', '\_'), self.textattrs))
		c.insert(text.text(1, 9.5, r"\bf Skipped containers:", self.textattrs))
		y = 9.40
		if self.options.skipList is not None:			
			for s in self.options.skipList:	   
				A = text.text(1, y, r"\bf %s" % s, self.textattrs)
				c.insert(A)
				y = y - .1

		# Draw containers at the top
		container_x = 2.5
		for container in self.tree.container:
			# skip containers that are requested to skip
			if self.options.skipList is not None and container.name in self.options.skipList:
				continue
					
			A = text.text(container_x + 0.15, 9.90, r"\bf %s" % container, self.textcontainerattrs)
			
			c.draw(path.rect(container_x + 0.05, 9.65, self.get_container_width(container), 0.30), 
				   [deco.stroked(), deco.filled([color.grey(0.85)])])
			c.insert(A)
	
			component_x = container_x		
			container_x = container_x + self.get_container_width(container) + 0.1
			
			
			for component in container.components:
				# include actions from abstract component
				component = component.flattened()
				
				# 			A = text.text(0, event_y, r"\bf \texttt{[0x%02x]} %s" % (event.id, event.name), self.texteventattrs)
				A = text.text(component_x + self.get_component_width(component)/2 + 0.1, 9.8, r"\bf \texttt{[0x%02x]} %s" % (component.id, component.name), self.textattrs)
				
				c.draw(path.rect(component_x + 0.1, 9.75, self.get_component_width(component), 0.10),
					   [deco.stroked(), deco.filled([color.grey(0.65)])])
				c.insert(A)
	
				event_x = component_x + 0.15
				component_x = component_x + self.get_component_width(component) + 0.1
				
				# Draw vertical lines for published events		
				for event in component.events.publish:
					eventArrowsSource.append(path.line(event_x, 9.75, event_x, self.get_y(event)))
					eventCirclesSource.append([event_x, 9.75])
					if sinklessDict[event.name]:
						eventCirclesSinkless.append([event_x, self.get_y(event)])
					
					# write name of event
					A = text.text(event_x, 8.1, r"\bf \texttt{[0x%02x]} %s" % (event.id, event.name), self.texteventrotattrs)				
					c.insert(A)
					
					# Store most left position
					try:
						eventsLeft[event.name] = event_x
					except:
						pass
					
					# this could be the most right position
					eventsRight[event.name] = event_x
					
					event_x = event_x + 0.1
					
				for event in component.events.subscribe:
					eventArrowsSink.append(path.line(event_x, self.get_y(event), event_x, 9.75))
					if sourcelessDict[event.name]:
						eventCirclesSourcless.append([event_x, self.get_y(event)])
					else:				
						eventCirclesSink.append([event_x, self.get_y(event)])
	
					
					A = text.text(event_x, 8.1, r"\bf \texttt{[0x%02x]} %s" % (event.id, event.name), self.texteventsubscribedrotattrs)
					c.insert(A)
				   
					# Store most left position
					try:
						eventsLeft[event.name] = event_x
					except:
						pass
	
					eventsRight[event.name] = event_x
	
					event_x = event_x + 0.1
					
				### Draw actions if selected
				if self.options.actions:
					action_x = event_x
					for action in component.actions:
						c.stroke(path.line(action_x, 8.05, action_x, 9.75),
								 [style.linewidth.THick, color.rgb(0.0, 0.5, 0.1),
								  deco.earrow([deco.stroked([color.rgb(0.0, 0.5, 0.1), style.linejoin.round]),
											   deco.filled([color.rgb(0.0, 0.5, 0.1)])], size=0.05)])
										
						A = text.text(action_x, 8.1, r"\bf %s" % action.name, self.textactionrotattrs)
						c.insert(A)
					   
						action_x = action_x + 0.1
	

		### Now we know from where to where to draw the thick lines
        # horizontal lines for events
		#    eventsLeft = SingleAssignDictionary('EventLeft')
		#    eventsRight = SingleAssignDictionary('EventRight')
    
		for event in self.tree.events:
			event_y = self.get_y(event)
			try:
			    event_x_left = eventsLeft[event.name]
			    event_x_right = eventsRight[event.name]
			    c.stroke(path.line(event_x_left, event_y, event_x_right, event_y),
			             [style.linewidth.THick, color.grey(0.40)])
			except:
			    pass
		
		
		### Draw everything in the right order
		for p in eventArrowsSource:
			c.stroke(p, [style.linewidth.THick, color.rgb.red,
						 deco.earrow([deco.stroked([color.rgb.red, style.linejoin.round]),
									  deco.filled([color.rgb.red])], size=0.05)])
	
		for p in eventArrowsSink:
			c.stroke(p, [style.linewidth.THick, color.rgb.blue,
						 deco.earrow([deco.stroked([color.rgb.blue, style.linejoin.round]),
									  deco.filled([color.rgb.blue])], size=0.05)])
		
		for x, y in eventCirclesSource:
			c.fill(path.circle(x, y, 0.02), [color.rgb.red])
		
		for x, y in eventCirclesSink:
			c.fill(path.circle(x, y, 0.02), [color.rgb.blue])
		
		for x, y in eventCirclesSinkless:
			c.stroke(path.circle(x, y, 0.02), [style.linewidth.THick, color.rgb.red])
	 
		for x, y in eventCirclesSourcless:
			c.stroke(path.circle(x, y, 0.02), [style.linewidth.THick, color.rgb.blue])
		
		
		### Write the PDF
		if os.path.splitext(self.options.outpath)[1] == '':
			file = os.path.join(self.options.outpath, 'system.pdf')
		else:
			file = self.options.outpath
			
		c.writePDFfile(file)
		
		print "The system layout is shown in ", file

	def setup(self, optparser):
		optparser.add_option(
				"-a", "--actions",
				action="store_true",
				dest = "actions",
				default = False,
				help = "Also display actions of components [optional]")
		
		optparser.add_option(
							"-s", "--skip",
							action = "append",
							dest = "skipList",
							help = "Skip containers, e.g. simulator.")

if __name__ == '__main__':
	SystemLayoutBuilder().run()
