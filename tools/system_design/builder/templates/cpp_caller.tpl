/*
 * WARNING: This file is generated automatically from cpp_xpcc_task_caller.tpl.
 * Do not edit! Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

#ifndef {{ namespace | upper }}_CPP_COMMUNICATION_XPCC_TASK_CALLER_HPP
#define {{ namespace | upper }}_CPP_COMMUNICATION_XPCC_TASK_CALLER_HPP

#include "identifier.hpp"
#include "packets.hpp"

#include <xpcc/communication/xpcc/communicatable_task.hpp>
#include <xpcc/processing/protothread.hpp>
#include <xpcc/processing/resumable.hpp>

namespace {{ namespace }}
{

namespace caller
{

{%- for component in components.iter() %}
{% if component.description %}/** {{ component.description | xpcc.wordwrap(72) | xpcc.indent(2) }} */{% endif %}
class {{ component.name | CamelCase }}
{
public:
	{%- for action in component.flattened().actions %}
	{%- if action.description %}
	{%- if action.parameterType %}
	/** {{ action.description | xpcc.wordwrap(72) | xpcc.indent(2) }}
	Parameter: {{ namespace }}::packet::{{ action.parameterType.flattened().name | CamelCase }}&
	 */
	{%- else %}
	/** {{ action.description | xpcc.wordwrap(72) | xpcc.indent(2) }} */
	{%- endif %}
	{%- else %}
	{%- if action.parameterType %}
	/**
	Parameter: {{ namespace }}::packet::{{ action.parameterType.flattened().name | CamelCase }}&
	 */
	{%- endif %}
	{%- endif %}
	class {{ action.name | CamelCase }} : public xpcc::CommunicatableTask, private xpcc::pt::Protothread, private xpcc::Resumable<1>
	{
	public:
		{%- if action.parameterType %}
			{%- if action.parameterType.isBuiltIn %}
		typedef {{ action.parameterType.name | CamelCase }} ParameterType;
			{%- else %}
		typedef {{ namespace }}::packet::{{ action.parameterType.name | CamelCase }} ParameterType;
			{%- endif %}
		{%- endif %}

		{%- if action.returnType %}
			{%- if action.returnType.isBuiltIn %}
		typedef {{ action.returnType.name | CamelCase }} ReturnType;
			{%- else %}
		typedef {{ namespace }}::packet::{{ action.returnType.name | CamelCase }} ReturnType;
			{%- endif %}
		{%- else %}
		typedef void ReturnType;
		{%- endif %}

		{{ action.name | CamelCase }}( xpcc::Communicator *c ) :
			CommunicatableTask( c ),
			responseCallback( this, &{{ action.name | CamelCase }}::responseHandler ),
			waitForResponse( false )
		{
			this->stop();		/* stopping the Proto-Thread */
		}

		inline {{ action.name | CamelCase }}*
		start({% if action.parameterType %} const ParameterType& parameter {% endif %})
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
		const ReturnType& getResponsePayload()
		{
			return responsePayload;
		}
		{%- endif %}

		{% if action.returnType %}
		const xpcc::ActionResult<ReturnType*>
		{%- else %}
		const xpcc::ActionResult<void>
		{%- endif %}
		getResult()
		{
			if (responseHeader.type == xpcc::Header::RESPONSE)
				{%- if action.returnType %}
				return &responsePayload;	/* returns positive response with payload */
				{%- else %}
				return xpcc::Response::Positive; /* returns positive response without payload */
				{%- endif %}
			return xpcc::Response::Negative;
		}

		{% if action.returnType %}
		xpcc::ActionResponse<ReturnType*>
		{%- else %}
		xpcc::ActionResponse<void>
		{%- endif %}
		execute({% if action.parameterType %} const ParameterType& parameter {% endif %})
		{
			RF_BEGIN(0);

			start({%- if action.parameterType %} parameter {%- endif %});

			RF_WAIT_WHILE(this->run());

			RF_END_RETURN(getResult());
		}

	private:
		xpcc::ResponseCallback responseCallback;

		void
		responseHandler(
		{%- if action.returnType %}
				const xpcc::Header& header,
				const ReturnType *payload)
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
					{{ namespace }}::component::Identifier::{{ component.name | CAMELCASE }},
					{{ namespace }}::action::Identifier::{{ action.name | CAMELCASE }},
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
		ParameterType parameter;
		{%- endif %}

		// return
		{%- if action.returnType %}
		ReturnType responsePayload;
		{%- endif %}
		xpcc::Header responseHeader;

		// internal
		bool waitForResponse;
	};

	{%- endfor %}

}; // class {{ component.name | CamelCase }}
{%- endfor %}

} // namespace caller

} // namespace {{ namespace }}

#endif // {{ namespace | upper }}_CPP_COMMUNICATION_XPCC_TASK_CALLER_HPP
