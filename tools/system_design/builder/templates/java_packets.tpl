%# Copyright (c) 2010-2011, Fabian Greif
%# Copyright (c) 2010-2011, 2013-2014, Georgi Grinshpun
%# Copyright (c) 2015, Niklas Hauser
%#
%# This file is part of the modm project.
%#
%# This Source Code Form is subject to the terms of the Mozilla Public
%# License, v. 2.0. If a copy of the MPL was not distributed with this
%# file, You can obtain one at http://mozilla.org/MPL/2.0/.
%# ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

package {{ package }};

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import javax.xml.bind.annotation.XmlRootElement;

public class Packets
{
	/** Every packet has to implement this interface */
	public interface Packet
	{
		/** Get the size of the underlying native struct in bytes 
		 * @return size in bytes */
		int getSize();
		
		/** 
		 * Get a byte array containing the values of attributes
		 * of the class
		 * @return array of bytes
		 */
		byte[] getBytes();
		
		/** Puts (which means appends) attributes of the class to given byte buffer.
		 * @param buffer It must have enough space for this packet.
		 * @return passed buffer
		 * @see ByteBuffer#put(byte)*/
		ByteBuffer toBuffer(ByteBuffer buffer);
	}
	
	/** Base class for all struct types */
	public static abstract class Struct implements Packet, Cloneable
	{
		public byte[] getBytes() {
			ByteBuffer buffer = ByteBuffer.allocate(this.getSize());
			buffer.order(ByteOrder.LITTLE_ENDIAN);
			
			return this.toBuffer(buffer).array();
		}
	}
	
	// ------------------------------------------------------------------------
	//                        Primitive types
	// ------------------------------------------------------------------------
	
	public static final class Void implements Packet, Cloneable
	{// manual primitive
		public Void(){
		}
		
		@Override
		public final int getSize() {
			return 0;
		}
		
		public byte[] getBytes() {
			return new byte[0];
		}
		
		public ByteBuffer toBuffer(ByteBuffer buffer){
			return buffer;
		}
		
		public static Void fromBuffer(ByteBuffer buffer){
			return new Void();
		}
		
		public static Void fromBuffer(byte[] bytes){
			return new Void();
		}
		
		@Override
		public String toString(){
			return "void";
		}
		
		@Override
		public Void clone() throws CloneNotSupportedException {
			return (Void)super.clone();
		}
	}
	
	{% for primitive in primitives %}
	public static final class {{ primitive.name }} implements Packet, Cloneable
	{// primitives
		public {{ primitive.javaType }} value;
		
		public {{ primitive.name }}({{ primitive.javaType }} value){
			this.value = value;
		}
		
		@Override
		public final int getSize() {
			return {{ primitive.size }};
		}
		
		public byte[] getBytes() {
			ByteBuffer buffer = ByteBuffer.allocate({{ primitive.size }});
			buffer.order(ByteOrder.LITTLE_ENDIAN);
			
			return toBuffer(buffer).array();
		}
		
		public ByteBuffer toBuffer(ByteBuffer buffer){
			return {{ primitive | toBufferMethod("value") }};
		}
		
		public static {{ primitive.name | typeObjectName }} fromBuffer(ByteBuffer buffer){
			return {{ primitive | fromBufferMethod }};
		}
		
		public static {{ primitive.name | typeObjectName }} fromBuffer(byte[] bytes){
			ByteBuffer buffer = ByteBuffer.wrap(bytes)
							.asReadOnlyBuffer()
							.order(ByteOrder.LITTLE_ENDIAN);
			return fromBuffer(buffer);
		}
		
		@Override
		public String toString(){
			return "" + value;
		}
		
		@Override
		public {{ primitive.name }} clone() throws CloneNotSupportedException {
			return ({{ primitive.name }})super.clone();
		}
	}
	{% endfor %}
	
	// ------------------------------------------------------------------------
	//                       User defined types
	// ------------------------------------------------------------------------
	
{%- for packet in packets %}
	
	{% if packet.description %}/** {{ packet.description | xpcc.wordwrap(72) | xpcc.indent(1) }} */{% endif %}
	{%- if packet.isStruct %}
	@XmlRootElement
	public static {% if (not packet.extending) %}final {% endif %}class {{ packet.flattened().name | typeName }} extends {% if packet.extends %}{{ packet.extends.name | typeName }}{% else %}Struct{% endif %}
	{// packet.isStruct
		{%- for element in packet.iter() %}
		public {{ element.subtype.name | typeName }} {{ element.name | variableName }};
		{%- endfor %}
		
		{{ packet.name | typeName }} (ByteBuffer buffer) {
			{%- if packet.extends %}
			super(buffer);
			{%- endif %}
			{%- for element in packet.iter() %}
			{%- if ((element.subtype.name| typeObjectName) in primitives) %}
			{{ element.name | variableName }} = {{ element.subtype.name | typeObjectName }}.fromBuffer(buffer).value;
			{%- else %}
			{{ element.name | variableName }} = {{ element.subtype.name | typeObjectName }}.fromBuffer(buffer);
			{%- endif %}
			{%- endfor %}
		}
		
		public {{ packet.name | typeName }} () {
		}

		{%- if packet.elements.__len__()>0 %}
		public {{ packet.name | typeName }} (
				{%- for element in packet.iter() %}
				{{ element.subtype.name | typeName }} {{ element.name | variableName }}{% if not loop.last %},{% endif %}
				{%- endfor %}) {
			{%- for element in packet.iter() %}
			this.{{ element.name | variableName }} = {{ element.name | variableName }};
			{%- endfor %}
		}
		{%- endif %}
		
		@Override
		public {% if (not packet.extending) %} final {% endif %} int getSize() {
			return {{ packet.size }};
		}
		
		public static {{ packet.name | typeName }} fromBuffer(ByteBuffer buffer) {
			{%- if packet.extending %}
			{{ packet.elements[0].subtype.name | typeName }} type = {{ packet.elements[0].subtype.name | typeName }}.fromValue(buffer.get());
			buffer.rewind();
			switch(type){
			{%- for child in packet.extending %}
				case {{ child.typeIdentifier.name | enumElement }}:
					return new {{ child.flattened().name | typeName }}(buffer);
			{%- endfor %}
				default:
					throw new RuntimeException("No extending type from struct '{{ packet.flattened().name | typeName }}' for TypeIdentifier " + type + " defined.");
			}
			{%- else %}
			return new {{ packet.name | typeName }}(buffer);
			{%- endif %}
		}
		
		public static {{ packet.name | typeName }} fromBuffer(byte[] bytes) {
			ByteBuffer buffer = ByteBuffer.wrap(bytes)
							.asReadOnlyBuffer()
							.order(ByteOrder.LITTLE_ENDIAN);
			return fromBuffer(buffer);
		}
		
		public ByteBuffer toBuffer(ByteBuffer buffer) {
			{%- for element in packet.flattened().iter() %}
			{{ element | toBufferMethodStructAccess }};
			{%- endfor %}
			
			return buffer;
		}
		
		@Override
		public String toString(){
			StringBuffer buff = new StringBuffer();
			buff.append("(");
			{%- if packet.extends %}
			buff.append(super.toString());
			{%- endif %}
			{%- for element in packet.iter() %}
			buff.append({{ element.name | variableName }}{% if loop.last %}+")"{% else %}+" "{% endif %});
			{%- endfor %}
			return buff.toString();
		}
		
		@Override
		public {{ packet.name | typeName }} clone() throws CloneNotSupportedException {
			{{ packet.name | typeName }} o = ({{ packet.name | typeName }})super.clone();
			{%- for element in packet.iter() %}
			{%- if element.subtype.type.isStruct %}
			o.{{ element.name | variableName }} = {{ element.name | variableName }}.clone();
			{%- endif %}
			{%- endfor %}
			
			return o;
		}
	}
	{%- elif packet.isEnum %}
	public enum {{ packet.name | typeName }} implements Packet
	{// packet.isEnum
	{%- for element in packet.iter() %}
		{{ element.name | enumElement }}({{ element.value }}){% if loop.last %};{% else %},{% endif %}
	{%- endfor %}
		public final int value;
		
		private {{ packet.name | typeName }}(int value) {
			this.value = value;
		}
		
		@Override
		public final int getSize() {
			return {{ packet.size }};
		}
		
		@Override
		public byte[] getBytes() {
			return new byte[] { (byte) value };
		}
		
		public static {{ packet.name | typeName }} fromValue(int value) {
			switch (value){
			{%- for element in packet.iter() %}
				case {{ element.value }}:
					return {{ element.name | enumElement }};
			{%- endfor %}
				default:
					throw new RuntimeException("Enumeration value " + value + " does not exist.");
			}

		}
		
		public static {{ packet.name | typeName }} fromBuffer(byte[] bytes) {
			ByteBuffer buffer = ByteBuffer.wrap(bytes)
							.asReadOnlyBuffer()
							.order(ByteOrder.LITTLE_ENDIAN);
			return fromBuffer(buffer);
		}
		
		public static {{ packet.name | typeName }} fromBuffer(ByteBuffer buffer) {
			return fromValue(buffer.get()&0xff);
		}
		
		public ByteBuffer toBuffer(ByteBuffer buffer) {
			buffer.put((byte)value);
			return buffer;
		}
	}
	{%- elif packet.isTypedef %}
	public static final class {{ packet.name | typeName }} extends Struct
	{// packet.isTypedef
		public {{ packet.subtype.name | typeName }} value;
		
		public {{ packet.name | typeName }}(){
		}
		
		public {{ packet.name | typeName }}({{ packet.subtype.name | typeName }} value){
			this.value = value;
		}
		
		@Override
		public final int getSize() {
			return {{ packet.size }};
		}
		
		public static {{ packet.name | typeName }} fromBuffer(ByteBuffer buffer) {
			{%- if packet.subtype.type.isStruct %}
			return new {{ packet.name | typeName }}({{ packet.subtype.name | typeObjectName }}.fromBuffer(buffer));
			{%- else %}
			return new {{ packet.name | typeName }}({{ packet.subtype.name | typeObjectName }}.fromBuffer(buffer).value);
			{%- endif %}
		}
		
		public static {{ packet.name | typeName }} fromBuffer(byte[] bytes) {
			ByteBuffer buffer = ByteBuffer.wrap(bytes)
							.asReadOnlyBuffer()
							.order(ByteOrder.LITTLE_ENDIAN);
			return fromBuffer(buffer);
		}
		
		@Override
		public ByteBuffer toBuffer(ByteBuffer buffer) {
			return {{ packet | toBufferMethodStructAccess("value") }};
		}
		
		@Override
		public String toString(){
			return "" + value;
		}
		
		@Override
		public {{ packet.name | typeName }} clone() throws CloneNotSupportedException {
			{{ packet.name | typeName }} o = ({{ packet.name | typeName }})super.clone();
			{%- if packet.subtype.type.isStruct %}
			o.value = value.clone();
			{%- endif %}
			
			return o;
		}
	}
	{%- endif %}
{%- endfor %}
}
