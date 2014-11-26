/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

package {{ package }};

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class MetaCommunication {
	private final Communication communication;
	private final List<Component> components;
	
	public interface Action<P extends Packets.Packet, T extends MetaPackets.Packet<P>> {
		public String getName();
		public String getDescription();
		public T getParameterType();
		public Component getComponent();

		void execute(P packet);
		void tryExecute(Packets.Packet packet);
	}

	public class Component {
		private final List<Action<?,?>> actions;
		private String name;
		@SuppressWarnings("unused")
		private String description;
		
		public Component(String name, String description) {
			this.name = name;
			this.description = description;
			this.actions = new ArrayList<MetaCommunication.Action<?,?>>();
		}
		
		public String getName() {
			return name;
		}
		
		public Iterable<Action<?,?>> actions() {
			return Collections.unmodifiableList(actions);
		}
		
		@Override
		public String toString() {
			return getName();
		}
	}
	
	public MetaCommunication(Communication communication) {
		this.communication = communication;
		
		components = new ArrayList<MetaCommunication.Component>();
		
		{%- for component in components.iter() %}
		{//{{ component.name }}
			Component c = new Component("{{ component.name }}", "{% if component.description %}{{ component.description | inStringDescription}}{% endif %}");
			components.add(c);
			{%- for action in component.flattened().actions %}
			c.actions.add(new MyAction<
				{% if action.parameterType -%}Packets.{{ action.parameterType.flattened().name | typeObjectName }}{%- else -%}Packets.Void{%- endif -%},
				MetaPackets.Packet<
				{% if action.parameterType -%}Packets.{{ action.parameterType.flattened().name | typeObjectName }}{%- else -%}Packets.Void{%- endif -%}
				>>(
				"{{ action.name }}",
				"{%- if action.description -%}{{ action.description | inStringDescription }}{%- endif -%}",
				{% if action.parameterType -%}MetaPackets.{{ action.parameterType.flattened().name | typeObjectName }}{%- else -%}MetaPackets.Void{%- endif -%},
				c) {
				@Override
				public void execute(
				{%- if action.parameterType %}Packets.{{ action.parameterType.flattened().name | typeObjectName }}{%- else %}Packets.Void{%- endif %} packet){
					MetaCommunication.this.communication.{{ component.name | variableName }}.{{ action.name | variableName }}(packet);
				}
			});
			{%- endfor %}
		}
		{%- endfor %}
	}
	
	public Iterable<Component> components() {
		return Collections.unmodifiableList(components);
	}
	
	private abstract class MyAction<P extends Packets.Packet, T extends MetaPackets.Packet<P>> implements Action<P,T> {
		private final String name;
		private final String description;
		private final T parameterType;
		private final Component component;
		
		public MyAction(String name, String description, T parameterType, Component component) {
			this.name = name;
			this.description = description;
			this.parameterType = parameterType;
			this.component = component;
		}

		@Override
		public String getName() {
			return name;
		}

		@Override
		public String getDescription() {
			return description;
		}
		
		@Override
		public T getParameterType() {
			return parameterType;
		}
		
		@Override
		public String toString() {
			return String.format("%s(%s)", getName(), parameterType.name);
		}
		
		@Override
		public void tryExecute(Packets.Packet packet) {
			if (packet == null)
				throw new NullPointerException("Parameter is null.");
			
			if (packet.getClass() != parameterType.type)
				throw new RuntimeException("Type of value does not match my Type!");
			
			@SuppressWarnings("unchecked")
			P p = (P)packet;
			execute(p);
		}
		
		@Override
		public Component getComponent() {
			return component;
		}
	}
	
}
