/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

package rca10.robot;

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
	}
	
	/** Base class for all struct types */
	public static abstract class Struct implements Packet
	{
		/** Read attributes from a byte buffer */
		abstract ByteBuffer fromBuffer(ByteBuffer buf);
		
		/** Convert attributes of the class to a byte buffer */
		abstract ByteBuffer toBuffer(ByteBuffer buffer);
		
		abstract public int getSize();
		
		// TODO benutze ByteBuffer.wrap(byte[] array) um einen Konstruktor
		// zu bauen der die Werte aus einem Array liest
		
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
	{
		public {{ primitive.javaType }} value;
		
		public final int getSize() {
			return {{ primitive.size }};
		}
		
		public byte[] getBytes() {
			ByteBuffer buffer = ByteBuffer.allocate({{ primitive.size }});
			buffer.order(ByteOrder.LITTLE_ENDIAN);
			
			{% if primitive.equivalent == "boolean" -%}
			return buffer.put((byte) ((value == true) ? 1 : 0)).array();
			{%- else -%}
			return buffer.put{{ primitive.accessor }}(({{ primitive.equivalent }}) value).array();
			{%- endif %}
		}
	}
	{% endfor %}
	
	// ------------------------------------------------------------------------
	//                       User defined types
	// ------------------------------------------------------------------------
	
{%- for packet in packets %}
	
	{% if packet.description %}/** {{ packet.description | xpcc.wordwrap(72) | xpcc.indent(1) }} */{% endif %}
	{%- if packet.isStruct %}
	public static final class {{ packet.name | typeName }} extends Struct
	{
		{%- for element in packet.iter() %}
		public {{ element.type.name | typeName }} {{ element.name | variableName }};
		{%- endfor %}
		
		public final int getSize() {
			return {{ packet.size }};
		}
		
		ByteBuffer fromBuffer(ByteBuffer buffer) {
			{%- for element in packet.iter() %}
			{{ element | fromBufferMethod }}
			{%- endfor %}
			
			return buffer;
		}
		
		ByteBuffer toBuffer(ByteBuffer buffer) {
			{%- for element in packet.iter() %}
			{{ element | toBufferMethod }}
			{%- endfor %}
			
			return buffer;
		}
	}
	{%- elif packet.isEnum %}
	public enum {{ packet.name | typeName }} implements Packet
	{
	{%- for element in packet.iter() %}
		{{ element.name | enumElement }}({{ element.value }}){% if loop.last %};{% else %},{% endif %}
	{%- endfor %}
		
		public final int getSize() {
			return {{ packet.size }};
		}
		
		public final int value;
		private {{ packet.name | typeName }}(int value) {
			this.value = value;
		}
		
		public byte[] getBytes() {
			return new byte[] { (byte) value };
		}
	}
	{%- elif packet.isTypedef %}
	public static final class {{ packet.name | typeName }} extends Struct
	{
		public {{ packet.type.name | typeName }} value;
		
		public final int getSize() {
			return {{ packet.size }};
		}
		
		ByteBuffer fromBuffer(ByteBuffer buffer) {
			{{ packet | fromBufferMethod("value") }}
			return buffer;
		}
		
		ByteBuffer toBuffer(ByteBuffer buffer) {
			{{ packet | toBufferMethod("value") }}
			return buffer;
		}
	}
	{%- endif %}
{%- endfor %}
}
