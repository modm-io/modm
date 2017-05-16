%# Copyright (c) 2014, Kevin Läufer
%#
%# This file is part of the modm project.
%#
%# This Source Code Form is subject to the terms of the Mozilla Public
%# License, v. 2.0. If a copy of the MPL was not distributed with this
%# file, You can obtain one at http://mozilla.org/MPL/2.0/.
%# ----------------------------------------------------------------------------

digraph include {
	rankdir=LR;
	graph [label="Includes relative to '{{ label_path }}'", splines=ortho, nodesep=0.8]
	node [style="rounded,filled", width=0, height=0, shape=box, fillcolor="#E5E5E5"]
	{%- for e in edges %}
	"{{ e[0] }}" -> "{{ e[1] }}";
	{%- endfor %}
	{% if edges|length < 1 %}
	"No Includes"
	{% endif %}
}
