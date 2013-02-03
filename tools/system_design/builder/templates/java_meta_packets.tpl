/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

package {{ package }};

import java.util.ArrayList;
import java.util.List;

public class MetaPackets
{
	public static abstract class Packet<T extends Packets.Packet>
	{
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
		
	}
	
	public static class Primitive<T extends Packets.Packet> extends Packet<T> {
		public Primitive(String name, String description, Class<T> type) {
			super(name, description, type);
		}
	}
	
	public static class Struct<T extends Packets.Packet> extends Packet<T> {
		private final List<Packet<?>> elements;
		public Struct(String name, String description, Class<T> type, Packet<?> ... packets) {
			super(name, description, type);
			elements = new ArrayList<Packet<?>>(packets.length);
			for (Packet<?> p : packets)
				elements.add(p);
		}
	}
	
	public static class Enum<T extends Packets.Packet> extends Packet<T> {
		public Enum(String name, String description, Class<T> type) {
			super(name, description, type);
		}
	}
	
	// ------------------------------------------------------------------------
	//                        Primitive types
	// ------------------------------------------------------------------------
	
	//TODO: some structs are extending others
	
	
	public static Primitive<Packets.Void> Void = new Primitive<Packets.Void>("void", "", Packets.Void.class);
	{%- for primitive in primitives %}
	public static Primitive<Packets.{{ primitive.name }}> {{ primitive.name }} = new Primitive<Packets.{{ primitive.name }}>("{{ primitive.type }}", "", Packets.{{ primitive.name }}.class);
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
						{{ element.subtype.name | typeObjectName }}
						{%- endfor %}
						);
	{%- elif packet.isEnum %}
	public static Enum<Packets.{{ packet.name | typeObjectName }}> {{ packet.name | typeObjectName }} = new Enum<Packets.{{ packet.name | typeObjectName }}>(// packet.isEnum
						"{{ packet.flattened().name }}",
						"{%- if packet.description -%}{{ packet.description | inStringDescription }}{%- endif -%}",
						Packets.{{ packet.name | typeObjectName }}.class
						);
						
	{%- elif packet.isTypedef %}
	public static Struct<Packets.{{ packet.name | typeObjectName }}> {{ packet.name | typeObjectName }} = new Struct<Packets.{{ packet.name | typeObjectName }}>(// packet.isTypedef 
						"{{ packet.flattened().name }}",
						"{%- if packet.description -%}{{ packet.description | inStringDescription }}{%- endif -%}",
						Packets.{{ packet.name | typeObjectName }}.class,
						{{ packet.subtype.name | typeObjectName }}
						);
	{%- endif %}
	
{%- endfor %}


	public static Packet<?> getMetaPacket(Packets.Packet packet) {
		if (packet instanceof Packets.Void)
			return Void;
		{%- for primitive in primitives %}
		if (packet instanceof Packets.{{ primitive.name }})
			return {{ primitive.name }};
		{%- endfor %}
		{% for packet in packets %}
		if (packet instanceof Packets.{{ packet.name | typeObjectName }})
			return {{ packet.name | typeObjectName }};
		{%- endfor %}
		throw new RuntimeException("No MetaPacket defined for " + packet.getClass().getName());
	}
}
