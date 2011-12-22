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

#ifdef __DARWIN__

#include <os/impl/MacOsThread.hpp>
#include <misc/api/macros.hpp>
#include <memory>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define DEBUG(a)  //printf a

/********************************************************************************/
namespace os { namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
class MacOsThread : public IThread
{
public:
    MacOsThread (void* (mainloop) (void*), void* data)  { pthread_create (&_thread, NULL,  mainloop, data); }
    ~MacOsThread ()  { pthread_detach (_thread);        }
    void join ()     { pthread_join   (_thread, NULL);  }
private:
    pthread_t  _thread;
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
class MacOsSynchronizer : public ISynchronizer
{
public:
    MacOsSynchronizer ()            {  pthread_mutex_init (&_mutex, NULL);  }
    virtual ~MacOsSynchronizer()    {  pthread_mutex_destroy (&_mutex);     }

    void   lock ()  { pthread_mutex_lock   (&_mutex); }
    void unlock ()  { pthread_mutex_unlock (&_mutex); }

private:
    pthread_mutex_t  _mutex;
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IThread* MacOsThreadFactory::newThread (void* (*mainloop) (void*), void* data)
{
    return new MacOsThread (mainloop, data);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ISynchronizer* MacOsThreadFactory::newSynchronizer (void)
{
    return new MacOsSynchronizer ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
size_t MacOsThreadFactory::getNbCores ()
{
    size_t result = 0;

    /** We open the "/proc/cpuinfo" file. */
    FILE* file = fopen ("/proc/cpuinfo", "r");
    if (file)
    {
        char buffer[256];

        while (fgets(buffer, sizeof(buffer), file))
        {
            if (strstr(buffer, "processor") != NULL)  { result ++;  }
        }

        fclose (file);
    }

    if (result==0)  { result = 1; }

    return result;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* __DARWIN__ */