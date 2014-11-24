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

/** \file LinuxMemoryFile.hpp
 *  \date 28/03/2014
 *  \author sbrillet
 *  \brief Linux abstraction of Memory file management.
 */

#ifndef _LINUX_MEMORYFILE_HPP_
#define _LINUX_MEMORYFILE_HPP_

/********************************************************************************/

#include <os/api/IMemoryFile.hpp>

/********************************************************************************/
namespace os {
/** \brief Implementation of Operating System abstraction layer */
namespace impl {
/********************************************************************************/

/** \brief factory that creates IMemoryFile instance for Linux OS.
 *
 *  Factory that creates IMemoryFile instances.
 */
class LinuxMemoryFileFactory : public IMemoryFileFactory
{
public:

    /** Destructor. */
    virtual ~LinuxMemoryFileFactory () {}

    /** \copydoc IMemoryFileFactory::newFile */
    IMemoryFile* newFile (const char *path, bool map);
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _LINUX_MEMORYFILE_HPP_ */