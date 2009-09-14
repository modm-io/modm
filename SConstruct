#!/usr/bin/env python
#
# $Id$

import yaml
config = yaml.load(open('config.cfg'))

#import pprint
#pprint.pprint(cfg)

if config['target'] == 'avr':
	env = Environment(
			ARCHITECTURE=config['target'],
			DEVICE=config['avr']['device'],
			AVRDUDE=config['avr']['avrdude'],
			CLOCK=config['avr']['clock'],
			tools=['avr'],
			toolpath=['misc/scons/'])
elif config['target'] == 'pc':
	env = Environment(
			ARCHITECTURE=config['target'],
			tools=['pc'],
			toolpath=['misc/scons/'])
else:
	print "please specify a valid target (avr|pc)"
	Exit(1)
#print env.Dump()

options = config.get('library', {})
SConscript('src/SConscript',
			src='src',
			variant_dir='build', 
			exports=['env', 'options'], 
			duplicate=False)
