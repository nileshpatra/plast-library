/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.0, released July  2011                                        *
 *   Copyright (c) 2011                                                      *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the CECILL version 2 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   CECILL version 2 License for more details.                              *
 *****************************************************************************/

#include "DefaultOsFactory.hpp"

#include "LinuxThread.hpp"
#include "LinuxTime.hpp"

/********************************************************************************/
namespace os {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IThreadFactory&  DefaultFactory::getThreadFactory ()
{
    return LinuxThreadFactory::singleton();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ITime& DefaultFactory::getTimeFactory   ()
{
    return LinuxTime::singleton();
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/