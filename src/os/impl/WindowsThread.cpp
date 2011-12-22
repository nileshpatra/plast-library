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

#ifdef __WINDOWS__

#include <os/impl/WindowsThread.hpp>
#include <misc/api/macros.hpp>
#include <memory>
#include <stdio.h>
#include <string.h>

#include <windows.h>

#define DEBUG(a)  //printf a

/********************************************************************************/
namespace os { namespace impl {
/********************************************************************************/

typedef DWORD (WINAPI * THREADROUTINE)(void *);

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
class WindowsThread : public IThread
{
public:

    WindowsThread (THREADROUTINE mainloop, void* data)
    {
         _thread = CreateThread (NULL, 0, (THREADROUTINE) mainloop, data, 0, NULL);
    }

    ~WindowsThread ()
    {
        TerminateThread (_thread, 0);
        CloseHandle (_thread);
    }

    void join ()
    {
        WaitForSingleObject (_thread, INFINITE);
        CloseHandle (_thread);
    }

private:
    HANDLE  _thread;
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
class WindowsSynchronizer : public ISynchronizer
{
public:
    WindowsSynchronizer ()
    {
        _mutex = CreateMutex (NULL, FALSE, NULL);
    }

    virtual ~WindowsSynchronizer()
    {
        CloseHandle (_mutex);
    }

    void lock ()
    {
        WaitForSingleObject (_mutex, INFINITE);
    }

    void unlock ()
    {
        ReleaseMutex (_mutex);
    }

private:
    HANDLE  _mutex;
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IThread* WindowsThreadFactory::newThread (void* (*mainloop) (void*), void* data)
{
    return new WindowsThread ((THREADROUTINE)mainloop, data);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ISynchronizer* WindowsThreadFactory::newSynchronizer (void)
{
    return new WindowsSynchronizer ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
size_t WindowsThreadFactory::getNbCores ()
{
    size_t result = 0;

    SYSTEM_INFO sysinfo;
    GetSystemInfo (&sysinfo);
    result = sysinfo.dwNumberOfProcessors;

    if (result==0)  { result = 1; }

    return result;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* __WINDOWS__ */