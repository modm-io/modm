/*
 * WARNING: This file is generated automatically from cpp_xpcc_task_caller.tpl.
 * Do not edit! Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

#ifndef ROBOT__CPP_COMMUNICATION_XPCC_TASK_CALLER_HPP
#define ROBOT__CPP_COMMUNICATION_XPCC_TASK_CALLER_HPP

#include "identifier.hpp"
#include "packets.hpp"

#include <xpcc/communication/xpcc/communicatable_task.hpp>
#include <xpcc/processing/protothread.hpp>

namespace robot_call_tasks
{

{%- for component in components.iter() %}
{% if component.description %}/** {{ component.description | xpcc.wordwrap(72) | xpcc.indent(2) }} */{% endif %}
class {{ component.name | CamelCase }}
{
public:
	{% for action in component.flattened().actions %}
	
	{% if action.description -%}
	{% if action.parameterType -%}
	/** {{ action.description | xpcc.wordwrap(72) | xpcc.indent(2) }}
	Parameter: robot::packet::{{ action.parameterType.flattened().name | CamelCase }}&
	 */
	{% else -%}
	/** {{ action.description | xpcc.wordwrap(72) | xpcc.indent(2) }} */
	{% endif -%}
	{% else -%}
	{% if action.parameterType -%}
	/**
	Parameter: robot::packet::{{ action.parameterType.flattened().name | CamelCase }}&
	 */
	{% endif -%}
	{% endif -%}
	class {{ action.name | CamelCase }} : public xpcc::CommunicatableTask, private xpcc::pt::Protothread
	{
	public:
		{%- if action.parameterType %}
		{%- if action.parameterType.isBuiltIn %}
		typedef {{ action.parameterType.name | CamelCase }} ParameterType;
		{%- else %}
		typedef robot::packet::{{ action.parameterType.name | CamelCase }} ParameterType;
		{%- endif %}
		{%- endif %}
		{%- if action.returnType %}
		{%- if action.returnType.isBuiltIn %}
		typedef {{ action.returnType.name | CamelCase }} ReturnType;
		{%- else %}
		typedef robot::packet::{{ action.returnType.name | CamelCase }} ReturnType;
		{%- endif %}
		{%- endif %}
		
		{{ action.name | CamelCase }}( xpcc::Communicator *c ) :
			CommunicatableTask( c ),
			responseCallback( this, &{{ action.name | CamelCase }}::responseHandler ),
			waitForResponse( false )
		{
			this->stop();		/* stopping the Proto-Thread */
		}

		inline {{ action.name | CamelCase }}*
		start (
				{%- if action.parameterType %}
				{%- if action.parameterType.isBuiltIn %}
				const {{ action.parameterType.name | CamelCase }}& parameter
				{%- else -%}
				const robot::packet::{{ action.parameterType.name | CamelCase }}& parameter
				{%- endif -%}
				{%- endif -%}
				)
		{
			{% if action.parameterType -%}
			this->parameter = parameter;
			
			{% endif -%}
			this->waitForResponse = false;
		
			this->restart();	/* restarting Proto-Thread */
			return this;
		}
		
		virtual bool
		isFinished()
		{
			return !this->isRunning();
		}
		

		virtual void
		update()
		{
			this->run();
		}
		
		xpcc::Header getResponseHeader()
		{
			return responseHeader;
		}
		{%- if action.returnType %}
		
		{% if action.returnType.isBuiltIn -%}
		const {{ action.returnType.name | CamelCase }}& getResponsePayload()
		{% else -%}
		const robot::packet::{{ action.returnType.name | CamelCase }}& getResponsePayload()
		{% endif -%}
		{
			return responsePayload;
		}
		{%- endif %}
		
	private:
		xpcc::ResponseCallback responseCallback;

		void
		responseHandler(
		{%- if action.returnType %}
		{%- if action.returnType.isBuiltIn %}
				const xpcc::Header& header,
				const {{ action.returnType.name | CamelCase }} *payload)
		{%- else %}
				const xpcc::Header& header,
				const robot::packet::{{ action.returnType.name | CamelCase }} *payload)
		{%- endif %}
		{%- else %}
				const xpcc::Header& header)
		{%- endif %}
		{
			this->waitForResponse = false;
			this->responseHeader = header;
			{%- if action.returnType %}
			this->responsePayload = *payload;
			{%- endif %}
		}
	
	public:
		bool
		run()
		{
			PT_BEGIN();
		
		//	XPCC_LOG_INFO << XPCC_FILE_INFO << " run : {{ action.name | CamelCase }} " << xpcc::endl;
		
			this->waitForResponse = true;
		
			parent->callAction(
					robot::component::Identifier::{{ component.name | CAMELCASE }},
					robot::action::Identifier::{{ action.name | CAMELCASE }},
					{% if action.parameterType -%}
					this->parameter,
					{% endif -%}
					this->responseCallback );
		
			PT_WAIT_UNTIL( !this->waitForResponse );
		
		//	XPCC_LOG_INFO << XPCC_FILE_INFO << " ready : {{ action.name | CamelCase }} " << xpcc::endl;
		
			PT_END();
		
			return false;
		}
		
	private:
		{%- if action.parameterType %}
		
		// parameter
		{% if action.parameterType.isBuiltIn -%}
		{{ action.parameterType.name | CamelCase }} parameter;
		{% else -%}
		robot::packet::{{ action.parameterType.name | CamelCase }} parameter;
		{%- endif %}
		{%- endif %}

		// return
		{%- if action.returnType %}
		{% if action.returnType.isBuiltIn -%}
		{{ action.returnType.name | CamelCase }} responsePayload;
		{% else -%}
		robot::packet::{{ action.returnType.name | CamelCase }} responsePayload;
		{%- endif %}
		{%- endif %}
		xpcc::Header responseHeader;

		// internal
		bool waitForResponse;
	};
	
	{%- endfor %}

}; // class {{ component.name | CamelCase }}
{%- endfor %}

} // namespace robot_call_tasks

#endif // ROBOT__CPP_COMMUNICATION_XPCC_TASK_CALLER_HPP
