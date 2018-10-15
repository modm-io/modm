# Assertions

These functions allow you to assert a condition at runtime and define
failure handlers in your application that can decide what to do with this
assertion and provide custom functionality.

Each assertion has the form `modm_assert(condition, module, location, failure)`,
where the condition is a boolean and rest are strings, so that a simple
string compare can be used to match for module, location or failure.
For example, the identifier `"can", "init", "timeout"` describes a timeout
failure in the CAN initializer function.
The assert `modm_assert_debug(condition, module, location, failure)` is only
available on debug builds and is removed from the code for a release build.

The user can define one or multiple assertion handlers in any part of the
application using the `MODM_ASSERTION_HANDLER(function)` macro.
All assertion handlers will be executed when an assertion fails anywhere in
the code and get passed the identifier string.

!!!note
	The order of assertion handler execution is undefined and must not been
	relied upon for any functionality!

!!!warning
	Assertion handlers may be executed in interrupt context!

Depending on the information in the failure identifier, the assertion handler
returns `Abandonment::DontCare` if the failure is not of interest, or
`Abandonment::Ignore` for recoverable failures, or `Abandonment::Fail` for
failures that do not allow normal program continuation.
The program is aborted, if any assertion handler returns `Abandonment::Fail`,
all assertion handlers return `Abandonment::DontCare` or no assertion
handlers have been defined in the application.
Only if one or many assertion handlers return `Abandonment::Ignore` and the
remainder returns `Abandonment::DontCare`, only then is the assertion ignored.

!!!note
	It is intended that the assertion handlers do not block (forever), so that
	all assertion handlers can get called.

On program abandonment `modm_abandon(module, location, failure)` is called,
which exits the program silently by default.
Only on hosted an formatted error string is output by default.
It is therefore recommended to overwrite this function on embedded targets
for custom behavior like blinking an LED and printing to a serial connection.

!!!warning
	The abandonment handler may also be executed in interrupt context!

