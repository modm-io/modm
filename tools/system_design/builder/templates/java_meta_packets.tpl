/*
 * WARNING: This file is generated automatically from java_meta_packets.tpl.
 * Do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

package {{ package }};

import java.util.ArrayList;
import java.util.Collections;
import java.util.EnumSet;
import java.util.List;

public class MetaPackets
{
	public static abstract class Packet<T extends Packets.Packet>
	{
		/** Name as it is defined in Communication */
		public final String name;
		public final String description;
		public final Class<T> type;
		public Packet(String name, String description, Class<T> type) {
			this.name = name;
			this.description = description;
			this.type = type;
		}
		
		@Override
		public String toString(){
			return "" + getClass().getSimpleName();
		}
		
		/**
		 * @param values Packets, from which the packet can be created.
		 * @return a new created Packet from given Values, or passed value, in
		 * case of a primitive
		 * @throws NullPointerException if one of values is null.
		 */
		public abstract T getPacket(Packets.Packet ...values);
		
		/**
		 * @param packet
		 * @return Array of sub Packets, from which this packet can be re created, or
		 * the passed packet, if it has no subpackets.
		 * @throws NullPointerException if packet is null
		 */
		public abstract Packets.Packet[] getValues(T packet);
	}
	
	public static abstract class Primitive<T extends Packets.Packet> extends Packet<T> {
		public Primitive(String name, String description, Class<T> type) {
			super(name, description, type);
		}
		
		public abstract T getPacketFromPrimitive(Object value);
	}
	
	public static abstract class Struct<T extends Packets.Packet> extends Packet<T> {
		private final List<StructElement<?>> elements;
		public Struct(String name, String description, Class<T> type, StructElement<?> ... packets) {
			super(name, description, type);
			elements = new ArrayList<StructElement<?>>(packets.length);
			for (StructElement<?> p : packets)
				elements.add(p);
		}
		public Iterable<StructElement<?>> elements() {
			return Collections.unmodifiableList(elements);
		}
	}
	
	public static abstract class Enum<T extends Packets.Packet> extends Packet<T> {
		public Enum(String name, String description, Class<T> type) {
			super(name, description, type);
		}
		
		public abstract Iterable<T> elements();
	}
	
	private static class EnumImpl<T extends java.lang.Enum<T> & Packets.Packet> extends Enum<T> {
		public EnumImpl(String name, String description, Class<T> type) {
			super(name, description, type);
		}
		
		@Override
		public Packets.Packet[] getValues(T packet) {
			if (packet == null)
				throw new NullPointerException();
			return new Packets.Packet[] {packet};
		}
		
		@Override
		public T getPacket(Packets.Packet... values) {
			@SuppressWarnings("unchecked")
			T t = (T)values[0];
			if (t == null)
				throw new NullPointerException();
			return t;
		}
		
		@Override
		public Iterable<T> elements() {
			return EnumSet.allOf(type);
		}
	}
	
	public static class StructElement<T extends Packets.Packet>
	{
		/** Name of the Field */
		public final String name;
		public final Packet<T> packet;
		public StructElement(String name, Packet<T> packet) {
			this.name = name;
			this.packet = packet;
		}
	}
	
	// ------------------------------------------------------------------------
	//                        Primitive types
	// ------------------------------------------------------------------------
	
	//TODO: some structs are extending others
	
	
	public static Primitive<Packets.Void> Void = new Primitive<Packets.Void>("Void", "", Packets.Void.class) {
		@Override
		public Packets.Void getPacket(Packets.Packet... values) {
			return new Packets.Void();
		}
		@Override
		public Packets.Packet[] getValues(Packets.Void packet) {
			return new Packets.Packet[]{};
		};
		
		@Override
		public rca.robot.Packets.Void getPacketFromPrimitive(Object value) {
			return new Packets.Void();
		}
	};
	{%- for primitive in primitives %}
	public static Primitive<Packets.{{ primitive.name }}> {{ primitive.name }} = new Primitive<Packets.{{ primitive.name }}>("{{ primitive.type }}", "", Packets.{{ primitive.name }}.class) {
		@Override
		public Packets.{{ primitive.name }} getPacket(Packets.Packet... values) {
			return (Packets.{{ primitive.name }})(values[0]);
		}
		@Override
		public Packets.Packet[] getValues(Packets.{{ primitive.name }} packet) {
			return new Packets.Packet[]{packet};
		};
		
		@Override
		public Packets.{{ primitive.name }} getPacketFromPrimitive(Object value) {
			return new Packets.{{ primitive.name }}(({{ primitive.javaWrapper }})value);
		}
	};
	{%- endfor %}
	
	// ------------------------------------------------------------------------
	//                       User defined types
	// ------------------------------------------------------------------------
	
{% for packet in packets %}
	{% if packet.description %}
	/** {{ packet.description | inStringDescription }} */{%- endif %}
	{%- if packet.isStruct %}
	public static Struct<Packets.{{ packet.name | typeObjectName }}> {{ packet.flattened().name | typeObjectName }} = new Struct<Packets.{{ packet.name | typeObjectName }}>(// packet.isStruct
						"{{ packet.flattened().name }}",
						"{%- if packet.description -%}{{ packet.description | inStringDescription }}{%- endif -%}",
						Packets.{{ packet.name | typeObjectName }}.class
						{%- for element in packet.iter() %},
						new StructElement<Packets.{{ element.subtype.name | typeObjectName }}>("{{ element.name }}", {{ element.subtype.name | typeObjectName }})
						{%- endfor %}
						) {
		@Override
		public Packets.{{ packet.name | typeObjectName }} getPacket(Packets.Packet... values) {
			Packets.{{ packet.name | typeObjectName }} p = new Packets.{{ packet.name | typeObjectName }}();
			{% for element in packet.iter() %}
			if (values[{{ loop.index-1 }}] == null)
				throw new NullPointerException();
			{%- if element.subtype.type.isBuiltIn or element.subtype.type.name == 'Bool' %}
			p.{{ element.name | variableName }} = ((Packets.{{ element.subtype.name | typeObjectName }})values[{{ loop.index-1 }}]).value;
			{%- else %}
			p.{{ element.name | variableName }} = (Packets.{{ element.subtype.name | typeJavaObjectName }})values[{{ loop.index-1 }}];
			{%- endif %}
			{%- endfor %}

			return p;
		}

		@Override
		public Packets.Packet[] getValues(Packets.{{ packet.name | typeObjectName }} packet) {
			if (packet == null)
				throw new NullPointerException();
		
			return new Packets.Packet[]{
			{%- for element in packet.iter() %}
			{%- if element.subtype.type.isBuiltIn or element.subtype.type.name == 'Bool' %}
				new Packets.{{ element.subtype.name | typeObjectName }} (packet.{{ element.name | variableName }}){% if not loop.last %},{% endif %}
			{%- else %}
				packet.{{ element.name | variableName }}{% if not loop.last %},{% endif %}
			{%- endif %}
			{%- endfor %}
			};
		}
	};
	{%- elif (packet.isEnum or packet.isEnumClass) %}
	public static EnumImpl<Packets.{{ packet.name | typeObjectName }}> {{ packet.name | typeObjectName }} = new EnumImpl<Packets.{{ packet.name | typeObjectName }}>(// packet.isEnum
						"{{ packet.flattened().name }}",
						"{%- if packet.description -%}{{ packet.description | inStringDescription }}{%- endif -%}",
						Packets.{{ packet.name | typeObjectName }}.class
						);
						
	{%- elif packet.isTypedef %}
	public static Struct<Packets.{{ packet.name | typeObjectName }}> {{ packet.name | typeObjectName }} = new Struct<Packets.{{ packet.name | typeObjectName }}>(// packet.isTypedef 
						"{{ packet.flattened().name }}",
						"{%- if packet.description -%}{{ packet.description | inStringDescription }}{%- endif -%}",
						Packets.{{ packet.name | typeObjectName }}.class,
						new StructElement<Packets.{{ packet.subtype.name | typeObjectName }}>("Value", {{ packet.subtype.name | typeObjectName }})
						) {
		@Override
		public Packets.{{ packet.name | typeObjectName }} getPacket(Packets.Packet... values) {
			Packets.{{ packet.name | typeObjectName }} p = new Packets.{{ packet.name | typeObjectName }}();
			if (values[0] == null)
				throw new NullPointerException();
			{%- if packet.subtype.type.isBuiltIn or packet.subtype.type.name == 'Bool' %}
			p.value = ((Packets.{{ packet.subtype.name | typeObjectName }})values[0]).value;
			{%- else %}
			p.value = (Packets.{{ packet.subtype.name | typeJavaObjectName }})values[0];
			{%- endif %}
			return p;
		}

		@Override
		public Packets.Packet[] getValues(Packets.{{ packet.name | typeObjectName }} packet) {
			if (packet == null)
				throw new NullPointerException();
		
			return new Packets.Packet[]{
			{%- if packet.subtype.type.isBuiltIn or packet.subtype.type.name == 'Bool' %}
				new Packets.{{ packet.subtype.name | typeObjectName }} (packet.value)
			{%- else %}
				packet.value
			{%- endif %}
			};
		}
	};
	{%- endif %}
	
{%- endfor %}


	@SuppressWarnings("unchecked")
	public static <T extends Packets.Packet> Packet<T> getMetaPacket(Class<T> packetType) {
		if (packetType == Packets.Void.class)
			return (Packet<T>)Void;
		{%- for primitive in primitives %}
		if (packetType == Packets.{{ primitive.name }}.class)
			return (Packet<T>){{ primitive.name }};
		{%- endfor %}
		{% for packet in packets %}
		if (packetType == Packets.{{ packet.name | typeObjectName }}.class)
			return (Packet<T>){{ packet.name | typeObjectName }};
		{%- endfor %}
		throw new RuntimeException("No MetaPacket defined for " + packetType.getName());
	}
}
