/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

package {{ package }};

import xpcc.DefaultXpccMessage;
import xpcc.Header;
import xpcc.HeaderType;
import xpcc.Sender;

public class Communication {
	private final Identifier.Component me;
	private final Sender sender;
	
	{% for component in components.iter() %}
	{% if component.description %}/** {{ component.description | xpcc.wordwrap(72) | xpcc.indent(1) }} */{% endif %}
	public final {{ component.name | typeName }} {{ component.name | variableName }} = new {{ component.name | typeName }}();
	{% endfor %}

	public Communication(Identifier.Component me, Sender sender) {
		this.me = me;
		this.sender = sender;
	}

	/** Every component has to extend this class */
	public abstract class Component{
		/** The Identifier of Component */
		public final Identifier.Component id;
		
		private Component(Identifier.Component id) {
			this.id = id;
		}
	}
	
{%- for component in components.iter() %}
	
	{% if component.description %}/** {{ component.description | xpcc.wordwrap(72) | xpcc.indent(1) }} */{% endif %}
	public class {{ component.name | typeName }} extends Component {
		private {{ component.name | typeName }}() {
			super (Identifier.Component.{{ component.flattened().name | enumElement }});
		}
		
		{% for action in component.flattened().actions %}
		{% if action.description -%}
		{% if action.parameterType -%}
		/** {{ action.description | xpcc.wordwrap(72) | xpcc.indent(2) }}
		@param packet Packets.{{ action.parameterType.flattened().name | typeObjectName }}
		 */
		{% else -%}
		/** {{ action.description | xpcc.wordwrap(72) | xpcc.indent(2) }} */
		{% endif -%}
		{% else -%}
		{% if action.parameterType -%}
		/**
		@param packet Packets.{{ action.parameterType.flattened().name | typeObjectName }}
		 */
		{% endif -%}
		{% endif -%}
		{% if action.parameterType -%}
		public void {{ action.name | variableName }} (Packets.{{ action.parameterType.flattened().name | typeObjectName }} packet) {
			Header header = new Header(HeaderType.REQUEST, false, this.id.id, me.id, Identifier.Action.{{ action.name | enumElement }}.id);
			
			DefaultXpccMessage message = new DefaultXpccMessage(header, packet.getBytes());
			sender.sendPacket(message);
		}
		{% else -%}
		public void {{ action.name | variableName }} () {
			Header header = new Header(HeaderType.REQUEST, false, this.id.id, me.id, Identifier.Action.{{ action.name | enumElement }}.id);
			
			DefaultXpccMessage message = new DefaultXpccMessage(header, null);
			sender.sendPacket(message);
		}
		public void {{ action.name | variableName }} (Packets.Void packet) {
			{{ action.name | variableName }}();
		}
		{% endif %}
		{%- endfor %}
		
	}

	
{%- endfor %}
}
