/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

package {{ package }};

import {{ package }}.Identifier.Action;
import {{ package }}.Identifier.Event;
import xpcc.Header;

public class Generator {
	public static Packets.Packet generate(Header header, byte[] payload){
		if (header.isAcknowledge)
			return null;
		switch (header.type){
			case REQUEST:
				if (header.destination == Identifier.Component.BROADCAST.id){
					switch (Event.get(header.packetIdentifier)){
					{%- for element in events.iter() %}
						case {{ element.name | enumElement }}:
							{%- if element.type %}
							return Packets.{{ element.type.name | typeObjectName }}.fromBuffer(payload);
							{%- else %}
							return null;
							{%- endif %}
					{%- endfor %}
					}
				}
				else{
					switch (Action.get(header.packetIdentifier)){
					{%- for element in actions.iter() %}
						case {{ element.name | enumElement }}:
							{%- if element.parameterType %}
							return Packets.{{ element.parameterType.name | typeObjectName }}.fromBuffer(payload);
							{%- else %}
							return null;
							{%- endif %}
					{%- endfor %}
					}
				
				}
				break;
			case RESPONSE:
				switch (Action.get(header.packetIdentifier)){
					{%- for element in actions.iter() %}
					case {{ element.name | enumElement }}:
						{%- if element.returnType %}
						return Packets.{{ element.returnType.name | typeObjectName }}.fromBuffer(payload);
						{%- else %}
						return null;
						{%- endif %}
					{%- endfor %}
				}
				break;
			case NEGATIVE_RESPONSE:
				// there are no conventions about returntypes for negative responses yet
				break;
		}
		throw new RuntimeException("No messagetype defined for header: " + header);
	}
}
