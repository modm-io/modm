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
	
	public interface Action<T extends Packets.Packet> {
		public String getName();
		public String getDescription();
		public Class<T> getParameterType();

		void execute(T packet);
	}

	public class Component {
		private final List<Action<?>> actions;
		private String name;
		private String description;
		
		public Component(String name, String description) {
			this.name = name;
			this.description = description;
			this.actions = new ArrayList<MetaCommunication.Action<?>>();
		}
		
		public String getName() {
			return name;
		}
		
		public Iterable<Action<?>> actions() {
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
				{%- if action.parameterType -%}Packets.{{ action.parameterType.flattened().name | typeObjectName }}{%- else -%}Packets.Void{%- endif -%}>(
				"{{ action.name }}",
				"{%- if action.description -%}{{ action.description | inStringDescription }}{%- endif -%}",
				{%- if action.parameterType -%}Packets.{{ action.parameterType.flattened().name | typeObjectName }}{%- else -%}Packets.Void{%- endif -%}.class) {
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
	
	private abstract class MyAction<T extends Packets.Packet> implements Action<T> {
		private final String name;
		private final String description;
		private final Class<T> parameterType;
		
		public MyAction(String name, String description, Class<T> parameterType) {
			this.name = name;
			this.description = description;
			this.parameterType = parameterType;
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
		public Class<T> getParameterType() {
			return parameterType;
		}
		
		@Override
		public String toString() {
			return String.format("%s(%s)", getName(), parameterType.getSimpleName());
		}
	}
	
}
