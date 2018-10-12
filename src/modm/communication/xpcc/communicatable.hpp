/*
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_COMMUNICATABLE_HPP
#define	XPCC_COMMUNICATABLE_HPP

namespace xpcc
{
	/**
	 * \brief	Base class for all classed which need to communicate
	 *
	 * Needed to have a common base class for the callback classes.
	 *
	 * \ingroup	modm_communication_xpcc
	 */
	class Communicatable
	{
	};
}

#endif // XPCC_COMMUNICATABLE_HPP
