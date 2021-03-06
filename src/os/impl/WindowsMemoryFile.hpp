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

/** \file WindowsMemoryFile.hpp
 *  \date 28/03/2014
 *  \author sbrillet
 *  \brief Windows abstraction of Memory file management.
 */

#ifndef _WINDOWS_MEMORYFILE_HPP_
#define _WINDOWS_MEMORYFILE_HPP_

/********************************************************************************/

#include <os/api/IMemoryFile.hpp>

/********************************************************************************/
namespace os {
/** \brief Implementation of Operating System abstraction layer */
namespace impl {
/********************************************************************************/

/** \brief factory that creates IMemoryFile instance for Windows OS.
 *
 *  Factory that creates IMemoryFile instances.
 */
class WindowsMemoryFileFactory : public IMemoryFileFactory
{
public:

    /** Destructor. */
    virtual ~WindowsMemoryFileFactory () {}

    /** \copydoc IMemoryFileFactory::newFile */
    IMemoryFile* newFile (const char *path, bool map);
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _WINDOWS_MEMORYFILE_HPP_ */
