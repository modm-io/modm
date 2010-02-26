#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright (c) 2009, Roboterclub Aachen e.V.
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
# 
# $Id$
# -----------------------------------------------------------------------------

import os
import builder_base

class JavaBuilder(builder_base.Builder):
	
	VERSION = "$Id$"
	
	def generate(self):
		# check the commandline options
		if not self.options.outpath:
			raise builder.BuilderException("You need to provide an output path!")
		
		template = self.template('templates/java.tpl')
		
		substitutions = {
			'components': self.tree.components,
			'actions': self.tree.components.actions,
			'attributes': self.tree.components.attributes,
			'events': self.tree.components.events,
			'types': self.tree.types
		}
		
		file = os.path.join(self.options.outpath, 'robot_packets.hpp')
		self.write(file, template.render(substitutions))

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	JavaBuilder().generate()


#import base
#import string
#import math
#import pprint
#import rccp_xml

#TEMPLATE_DESCRIPTION = string.Template("""
  #/**
   #* ${desc}
   #*/""")

##class enclosing all types
#TEMPLATE_TYPES = string.Template("""
#package rca09.robot;

#public class Types {
  #public interface RccpType {
    #byte[] getBytes();
  #}
  
  #/* Is extended by Rccp-Struct-types */
  #public static abstract class SuperStruct implements RccpType {
    #abstract byte[][] body();

    #public byte[] getBytes() {
      #byte[][] b = body();

      #int length = 0;
      #for (int i = 0; i < b.length; i++)
        #length += b[i].length;
        
      #int i = 0;
      #int k = 0;
      #byte[] ret = new byte[length];
      #for (int pos = 0; pos < length; pos++) {
        #ret[pos] = b[i][k++];
        #if (k>=b[i].length) {
          #k=0;
          #i++;
        #}
      #}
      #return ret;
    #}
  #}

#${types}
#}
#""")

#TEMPLATE_DEFAULT_EXTEND = "RccpRemoteComponent"

#TEMPLATE_COMPONENT_ABSTRACT = string.Template("""
#package rca09.robot;

#import rca09.robot.Types.*;
#import canCommunications.io.ComRccpClient;
#import canCommunications.io.RccpCommunicatable;
#import canCommunications.io.RccpRemoteComponent;

#public abstract class ${name} extends ${extends} {
#${desc}
  #public ${name}(ComRccpClient client, int id, RccpCommunicatable listener) {
    #super(client, id, listener);
  #}
#${actions}
#${attributes}
#}
#""")

#TEMPLATE_COMPONENT = string.Template("""
#package rca09.robot;

#import rca09.robot.Types.*;
#import canCommunications.io.ComRccpClient;
#import canCommunications.io.RccpCommunicatable;
#import canCommunications.io.RccpRemoteComponent;

#public class ${name} extends ${extends} {
#${desc}
  #public static final int id = ${id};
  #public ${name}(ComRccpClient client, RccpCommunicatable listener) {
    #super(client, id, listener);
  #}
#${actions}
#${attributes}
#}
#""")

#TEMPLATE_ACTION = string.Template("""
#${desc}
  #public void ${name}(${parameter}) {
    #super.sendRPC(${id}, ${payload});
  #}
#""")


#TEMPLATE_ATTRIBUTE_GET = string.Template("""
#${desc}
  #public void get${name}() {
    #super.sendReadMessage(${id});
  #}
#""")

#TEMPLATE_ATTRIBUTE_SET = string.Template("""
#${desc}
  #public void set${name}(${type} payload) {
    #if (payload != null)
      #super.sendWriteMessage(${id}, payload.getBytes());
  #}
#""")


##template for primitive types
#TEMPLATE_PRIMITIVE_CLASS = string.Template("""
#//class
#${desc}
  #public static class ${name} implements RccpType {
    #public ${javatype} value;
    #public ${name}(${javatype} value) {
      #this.value = value;
    #}
    
    #public byte[] getBytes() {
      #${ret}
    #}
  #}
#""")


##template for structs
#TEMPLATE_STRUCT_CLASS = string.Template("""
#//struct class
#${desc}
  #public static class ${name} extends SuperStruct {
#${member}

    #public ${name}() {
    #}

    #byte[][] body() {
      #return new byte[][]{${body_b}};
    #}
  #}
#""")


##template for enum
#TEMPLATE_ENUM_ELEMENT = string.Template("""
    #${name}(${value}),\t\t// ${desc}""")

#TEMPLATE_ENUM_CLASS = string.Template("""
#${desc}
  #public enum ${name} implements RccpType {
#${elements}
    #;
    
    #public final int value;
    #private ${name}(int value) {
      #this.value = value;
    #}
    
    #public byte[] getBytes() {
      #return new byte[]{(byte)value};
    #}
  #}
#""")


##template for typedef
#TEMPLATE_TYPEDEF_PRIMITIVE = string.Template("""
#//typedef
#${desc}
  #public static class ${name} implements RccpType {
    #public ${javatype} value;
    #public ${name}() {
    
    #}

    #public byte[] getBytes() {
      #return new ${type}(value).getBytes();
    #}
  #}

#""")

##template for typedef
#TEMPLATE_TYPEDEF_COMPLEX = string.Template("""
#//typedef
#${desc}
  #public static class ${name} implements RccpType {
    #public ${type} value;
    #public ${name}() {
    
    #}

    #public byte[] getBytes() {
      #return value.getBytes();
    #}
  #}

#""")

#PRIMITIVES = {
	#"int8_t" : "int",
	#"int16_t" : "int",
	#"int32_t" : "int",
	#"uint8_t" : "int",
	#"uint16_t" : "int",
	#"uint32_t" : "int",
	#"float" : "float",
	#"char" : "char",
	#"Bool" : "boolean",
#}

#PRIMITIVES_RETURN = {
	#"int8_t" : "return new byte[]{(byte)value};",
	#"int16_t" : "return new byte[]{(byte)(value),(byte)(value>>8)};",
	#"int32_t" : "return new byte[]{(byte)(value),(byte)(value>>8),(byte)(value>>16),(byte)(value>>24)};",
	#"uint8_t" : "return new byte[]{(byte)value};",
	#"uint16_t" : "return new byte[]{(byte)(value),(byte)(value>>8)};",
	#"uint32_t" : "return new byte[]{(byte)(value),(byte)(value>>8),(byte)(value>>16),(byte)(value>>24)};",
	#"float" : "int i = java.lang.Float.floatToIntBits(value);\nreturn new byte[]{(byte)(i),(byte)(i>>8),(byte)(i>>16),(byte)(i>>24)};",
	#"char" : "return new byte[]{(byte)value};",
	#"Bool" : "return new byte[]{(byte)(value?1:0)};",
#}


#class JavaBuilder(builder.Builder):
	
##	VERSION = "$Id$"
	#VERSION = "Number one"
	
	#def generate(self):
		#generator = TypeGenerator();
		
		#generator.add(self.tree.types)
		#self.write_file(self.options.output+"/Types.java", generator.get_string())
		##print generator.get_string()

		#for component in self.tree.components.itervalues():
			##print component
			##print generator.get_string()
			#generator = ComponentGenerator(component)
			#self.write_file("%s/%s.java" % (self.options.output, to_java_type(component.name)), generator.get_string())

#def to_java_name(name):
	## mixed case with first character lower case
	#return name[0].lower() + name.title().replace(' ', '')[1:]
	
#def to_java_type(str):
	#if str in PRIMITIVES.iterkeys():
		#return str.replace('_t', '').title()
	#else:
		#return str.title().replace(' ', '')

#def to_java_desc(str):
	#return TEMPLATE_DESCRIPTION.substitute({'desc' : str.replace('\n', '\n   * ')}) if str else ""

#class JavaBuilder2:
	#""" Erstellt die Componenten für Java. """
	#def __init__(self, parser):
		#self.parser = parser
	
	#def generate_packets_file(self, filename):
		#generator = TypeGenerator()
		
##		self.constructors = []
		#for type in self.parser.types.itervalues():
##			if type['model'] != 'struct':
##				continue
##			if type['invisible']:
##				print "invisible"
			
			#print "%s" % (type['name'])
			#generator.add(type)
##			print generator.generate(type)
##			pprint.pprint(type)
			
##			self.constructors.append(generator.get_constructor())
			
##			packet_string += str + '\n'
		
		
##		print generator.get_string()
		
		#file = codecs.open(filename, 'w',"utf8")
		#file.write(generator.get_string())
		#file.close()
		
	#def generate_component_file(self, outputdir, component):
##		if component['id'] == 65:
		#generator = ComponentGenerator(component['id'], component['name'])
		#print "%i, %s" % (component['id'], component['name'])
		
		#for attribute in self.tree.component['id'].attributes:
			#generator.add_attribute(attribute)
		
		#for action in component['actions'].itervalues():
			#generator.add_action(action)
		
		#file = codecs.open("%s/%s.java" % (outputdir, to_java_type(component['name'])), 'w',"utf8")
		#file.write(generator.get_string())
		#file.close()
##		component_list[component['id']] = component['name'].replace(' ', '_').upper()
	
	#def generate_component_files(self, outputdir):
##		print "hallo"
		#for board in self.parser.boards.itervalues():
			#for component in board['components']:
				#if not component['invisible']:
					#self.generate_component_file(outputdir, component)
	
	#def make(self, outputdir):
		#filename = outputdir+"/Types.java"
		#print filename
		#self.generate_packets_file(filename)
		#self.generate_component_files(outputdir)
##		pprint.pprint(self.parser.boards)
##		time.sleep(10)
		
		
#class ComponentGenerator:
	#""" Create definitions strings
	#"""
	#def __init__(self, component):
		#self.subs = {}
		#self.subs['name'] = to_java_type(component.name)
		#self.subs['id'] = component.id
		#self.subs['desc'] = to_java_desc(component.desc)
		#self.subs['actions'] = ""
		#self.subs['attributes'] = ""
		#self.subs['extends'] = component.extends if component.extends else TEMPLATE_DEFAULT_EXTEND
		#if component.extends:
			#print component
		#self.component = component
		#for action in component.actions.itervalues():
			#self.add_action(action);
		#for attribute in component.attributes.itervalues():
			#self.add_attribute(attribute);
		
	#def add_attribute(self, attribute):
		#subs = {}
		#subs['name'] = to_java_type(attribute.name)
		#subs['id'] = attribute.id
		#subs['type'] = to_java_type(attribute.type)
		#subs['desc'] = to_java_desc(attribute.desc)
		
		#self.subs['attributes'] += TEMPLATE_ATTRIBUTE_GET.substitute(subs)

		#if not attribute.readonly:
			#self.subs['attributes'] += TEMPLATE_ATTRIBUTE_SET.substitute(subs)
		
	
	#def add_action(self, action):
		#subs = {}
		#subs['name'] = to_java_name(action.name)
		#subs['id'] = action.id
		#subs['desc'] = to_java_desc(action.desc)
		
		#if action.param_type:
			#subs['parameter'] = to_java_type(action.param_type) + " payload"
			#subs['payload'] = "payload.getBytes()"
		#else:
			#subs['parameter'] = ""
			#subs['payload'] = "new byte[] {}"
			
		#self.subs['actions'] += TEMPLATE_ACTION.substitute(subs)
		
	#def get_string(self):
		#if not self.component.abstract:
			#return TEMPLATE_COMPONENT.substitute(self.subs)
		#else:
			#return TEMPLATE_COMPONENT_ABSTRACT.substitute(self.subs)
	
	
#class TypeGenerator:
	#""" Create definitions strings
	#"""
	#def __init__(self):
		#self.string = ""
		#self._add_primitives();
	
	#def _add_primitives(self):
		#for primitive in PRIMITIVES.iterkeys():
			#subs = {}
			#subs['name'] = self._type_mangling(primitive)
			#subs['ret'] = PRIMITIVES_RETURN[primitive].replace('\n','\n      ')
			#subs['javatype'] = PRIMITIVES[primitive];
			#subs['desc'] = to_java_desc("Primitive " + primitive);
			##self.string += self._get_class(name,PRIMITIVES[primitive],PRIMITIVES_RETURN[primitive])
			#self.string += TEMPLATE_PRIMITIVE_CLASS.substitute(subs)

	#def add(self, types):
		#for type in types.itervalues():
			#if type.name not in PRIMITIVES.iterkeys():
				#if type.__class__ == rccp_xml.type.Typedef:
					#str = self._generate_typedef(type)
					#self.string += str + "\n"
				#elif type.__class__ == rccp_xml.type.Enum:
					#str = self._generate_enum(type)
					#self.string += str + "\n"
				#elif type.__class__ == rccp_xml.type.Struct:
					#str = self._generate_struct(type)
					#self.string += str + "\n"
				##elif type.__class__ == rccp_xml.type.BuiltIn:
				##	str = self._generate_typedef(type)
				##	self.string += str + "\n"
					
	#def get_string(self):
		#return TEMPLATE_TYPES.substitute({'types':self.string})
	
	#def _generate_struct(self, struct):
		#member_list = []
		#return_body = []
		
		#for element in struct.iter():
			#elemSubs={}
			#if element.type[-1] == "]":
				#type, dummy, num = element.type.partition('[')
				#try:
					#num = int(num[:-1], 0)
				#except ValueError:
					#num = None
			#else:
				#type = element.type
				#num = None
			
			#elemSubs['type'] = self._type_mangling(type)
			#elemSubs['name'] = self._variable_mangling(element.name)
			##line desc
			#elemSubs['desc'] = "\t\t//!< %s" % element.desc if element.desc else ""
			
			
			
			#if num != None:
				#t = string.Template("${type} ${name};${desc} // array[${num}]")
				#elemSubs['num'] = num
				#member_list.append( t.substitute(elemSubs))

##				if type == "char":
##					member_list.append("%s %s[%i];%s" % (type, name, num, desc))
##					parameter_list.append("const %s *const %s_" % (type, name))
##					
##					init_body.append(u"\n\t\tmemcpy(%s, %s_, %i);\n\t\t%s[%i] = '\\0';\n\t" % \
##							(name, name, num, name, num - 1))
##					default_init_body.append(u"\n\t\tmemset(%s, 0, %i);\n\t" % (name, num))
##				else:
##					raise Exception("FIXME: Array behandlung ist noch unvollständig!")
			#else:
##				if type in rccp_parser.BUILDIN_TYPES:
				#if type in PRIMITIVES.iterkeys():
					#body = string.Template("\n          new ${type}(${name}).getBytes()")
					#member = string.Template("public ${javatype} ${name};${desc}")
					#elemSubs['javatype'] = PRIMITIVES[type]
					
					#return_body.append( body.substitute(elemSubs))
					#member_list.append( member.substitute(elemSubs))
##					if type == "float":
##						member_list.append( "public float %s;%s" % (name_, desc_))
##						default_init_list.append("%s(0.0)" % name)
##					elif type == "char":
##						member_list.append( "public char %s;%s" % (name_, desc_))
##						default_init_list.append("%s(\"\")" % name)
##					else:
##						default_init_list.append("%s(0)" % name)
				#else:
					#body = string.Template("\n          ${name}.getBytes()")
					#member = string.Template("public ${type} ${name};${desc}")
					
					#return_body.append( body.substitute(elemSubs))
					#member_list.append( member.substitute(elemSubs))
				
##				init_list.append("%s(%s_)" % (name, name))
##				parameter_list.append("%s %s_" % (type, name))
		
##		default_init = ', '.join(default_init_list)
##		if default_init:
##			default_init = ": " + default_init
		
		#substitutions = {
			#"desc" : to_java_desc(struct.desc),
			#"name" : self._type_mangling(struct.name),
##			"default_init" : default_init,
##			"parameter" : ', '.join(parameter_list),
##			"init" : ', '.join(init_list),
			#"member" : member_list,
##			"init_body" : ''.join(init_body),
##			"default_init_body": ''.join(default_init_body),
			#"body_b" : "%s" % ','.join(return_body),
		#}
		
		#substitutions['member'] = "    " + "\n    ".join(substitutions['member'])
		#return TEMPLATE_STRUCT_CLASS.substitute(substitutions)
	
	
	#def _generate_enum(self, enum):
		#str = ""
		#if self._type_mangling(enum.name) == 'PowerSink':
			#pass
		#for element in enum.iter():
			#elemSubs = {}
			#elemSubs['name'] = element.name
			##linedesc
			#elemSubs['desc'] = element.desc.replace('\n', ' ') if element.desc else ""
				
			#try:
				#elemSubs['value'] = "0x%x" % int(element.value)
			#except ValueError:
				#elemSubs['value'] = element.value
			#str += TEMPLATE_ENUM_ELEMENT.substitute(elemSubs)
		
		#subs = {}
		#subs['desc'] = to_java_desc(enum.desc)
		#subs['name'] = self._type_mangling(enum.name)
		#subs['elements'] = str

		#return TEMPLATE_ENUM_CLASS.substitute(subs)
	
	#def _generate_typedef(self, type):
		##subs = {'time': self.time}
		#subs = {}
		#subs['name'] = self._type_mangling(type.name)
		#subs['type'] = self._type_mangling(type.type)
		#subs['desc'] = to_java_desc(type.desc)
		#if type.type in PRIMITIVES:
			#subs['javatype'] = PRIMITIVES[type.type]
			#str = TEMPLATE_TYPEDEF_PRIMITIVE.substitute(subs)
		#else:
			#str = TEMPLATE_TYPEDEF_COMPLEX.substitute(subs)

		#return str

	#def _variable_mangling(self, name):
			#return to_java_name(name)
	
	#def _type_mangling(self, str):
		#return to_java_type(str)
