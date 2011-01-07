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
	{// primitives
		public {{ primitive.javaType }} value;
		
		public {{ primitive.name }}({{ primitive.javaType }} value){
			this.value = value;
		}
		
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
		
		public static {{ primitive.javaType }} fromBuffer(ByteBuffer buffer){
			return {{ primitive | fromBufferMethod }};
		}
		
		public static {{ primitive.javaType }} fromBuffer(byte[] bytes){
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
	public static final class {{ packet.name | typeName }} extends Struct
	{// packet.isStruct
		{%- for element in packet.iter() %}
		public {{ element.type.name | typeName }} {{ element.name | variableName }};
		{%- endfor %}
		
		{{ packet.name | typeName }} (ByteBuffer buffer) {
			{%- for element in packet.iter() %}
			{{ element.name | variableName }} = {{ element.type.name | typeObjectName }}.fromBuffer(buffer);
			{%- endfor %}
		}
		
		public {{ packet.name | typeName }} () {
		}
		
		public final int getSize() {
			return {{ packet.size }};
		}
		
		public static {{ packet.name | typeName }} fromBuffer(ByteBuffer buffer) {
			return new {{ packet.name | typeName }}(buffer);
		}
		
		public static {{ packet.name | typeName }} fromBuffer(byte[] bytes) {
			ByteBuffer buffer = ByteBuffer.wrap(bytes)
							.asReadOnlyBuffer()
							.order(ByteOrder.LITTLE_ENDIAN);
			return fromBuffer(buffer);
		}
		
		public ByteBuffer toBuffer(ByteBuffer buffer) {
			{%- for element in packet.iter() %}
			{{ element | toBufferMethodStructAccess }};
			{%- endfor %}
			
			return buffer;
		}
		
		@Override
		public String toString(){
			StringBuffer buff = new StringBuffer();
			buff.append("(");
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
			value = value&0xff;
			for({{ packet.name | typeName }} i : {{ packet.name | typeName }}.values()){
				if (i.value == value)
					return i;
			}
			throw new RuntimeException("Enumeration value " + value + " does not exist.");
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
		public {{ packet.type.name | typeName }} value;
		
		public {{ packet.name | typeName }}(){
		}
		
		public {{ packet.name | typeName }}({{ packet.type.name | typeName }} value){
			this.value = value;
		}
		
		public final int getSize() {
			return {{ packet.size }};
		}
		
		public static {{ packet.name | typeName }} fromBuffer(ByteBuffer buffer) {
			return new {{ packet.name | typeName }}({{ packet.type.name | typeObjectName }}.fromBuffer(buffer));
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
