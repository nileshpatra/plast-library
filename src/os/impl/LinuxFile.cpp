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

#ifdef __LINUX__

#include <os/impl/LinuxFile.hpp>
#include <os/impl/CommonOsImpl.hpp>

#include <iostream>
#include <sstream>

/********************************************************************************/
namespace os { namespace impl {
/********************************************************************************/

/** */
class LinuxFile : public CommonFile
{
public:

    LinuxFile (const char* path, const char* mode, bool temporary) : CommonFile (path, mode, temporary)  {}

    ~LinuxFile ()
    {
        if (_temporary)
        {
            std::stringstream ss;
            ss << "/bin/rm " << _path;
            system (ss.str().c_str());
        }
    }

    int seeko (u_int64_t offset, int whence)  {  return (_handle==0  ?  -1 : fseeko64 (_handle, offset, whence) );  }

    u_int64_t tell ()  { return (_handle==0 ? 0 : ftello64 (_handle)); }
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IFile* LinuxFileFactory::newFile (const char *path, const char *mode, bool temporary)
{
    return new LinuxFile (path, mode, temporary);
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /*  __LINUX__  */
