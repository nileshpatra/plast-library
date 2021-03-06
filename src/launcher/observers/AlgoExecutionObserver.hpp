/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

/** \file AlgoExecutionObserver.hpp
 *  \brief Abstract class for observer interested by PLAST algorithm progression
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ALGO_EXECUTION_OBSERVER_HPP
#define _ALGO_EXECUTION_OBSERVER_HPP

/********************************************************************************/

#include <launcher/observers/AbstractObserver.hpp>

#include <stdio.h>
#include <string>

/********************************************************************************/
/** \brief PLAST command line. */
namespace launcher {
/** \brief Observers definitions */
namespace observers {
/********************************************************************************/

/** \brief Observer that displays some verbose messages during algorithm execution
 *
 * This implementation will dump textual messages in a FILE descriptor in response
 * to the reception of AlgoEventWithStatus events.
 *
 * By default, the output file is the standard output.
 */
class AlgoVerboseObserver : public AbstractObserver
{
public:

    /** Constructor.
     * \param[in] file : file where to display the information
     */
    AlgoVerboseObserver (FILE* file=stdout);

    /** \copydoc AbstractObserver::update */
    void update (dp::EventInfo* evt, dp::ISubject* subject);

protected:
    FILE* _file;
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALGO_EXECUTION_OBSERVER_HPP */
