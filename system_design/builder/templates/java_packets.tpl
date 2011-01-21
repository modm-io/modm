/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

package {{ package }};

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Packets
{
	/** Every packet has to implement this interface */
	public interface Packet
	{
		/** Get the size of the underlaying native struct in bytes */
		int getSize();
		
		/** 
		 * @brief	Get a byte array containing the values of attributes
		 * 			of the class
		 */
		byte[] getBytes();
		
		/** Convert attributes of the class to a byte buffer */
		abstract ByteBuffer toBuffer(ByteBuffer buffer);
	}
	
	/** Base class for all struct types */
	public static abstract class Struct implements Packet
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
	
	{% for primitive in primitives %}
	public static final class {{ primitive.name }} implements Packet
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
	}
	{% endfor %}
	
	// ------------------------------------------------------------------------
	//                       User defined types
	// ------------------------------------------------------------------------
	
{%- for packet in packets %}
	
	{% if packet.description %}/** {{ packet.description | xpcc.wordwrap(72) | xpcc.indent(1) }} */{% endif %}
	{%- if packet.isStruct %}
	public static {% if (not packet.extending) %}final {% endif %}class {{ packet.flattened().name | typeName }} extends {% if packet.extends %}{{ packet.extends.name | typeName }}{% else %} Struct {% endif %}
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
		
		@Override
		public {% if (not packet.extending) %} final {% endif %} int getSize() {
			return {{ packet.size }};
		}
		
		public static {{ packet.name | typeName }} fromBuffer(ByteBuffer buffer) {
			{% if packet.extending %}
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
			{% else %}
			return new {{ packet.name | typeName }}(buffer);
			{% endif %}
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
			return new {{ packet.name | typeName }}({{ packet.subtype.name | typeObjectName }}.fromBuffer(buffer).value);
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
	}
	{%- endif %}
{%- endfor %}
}
