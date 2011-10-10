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

#include "UngapAlignmentResult.hpp"
#include "IAlignmentSplitter.hpp"

#include "DefaultOsFactory.hpp"

#include <iostream>
#include "macros.hpp"

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace database;
using namespace dp;
using namespace os;

//#define VERBOSE

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
UngapAlignmentResult::UngapAlignmentResult (size_t nbQuerySequences)
    : _synchro (0), _nbAlignments(0), _diagGlobal(0),
      _listGaplessAlign(0), _listGaplessAlignSize(0)
{
    _synchro = DefaultFactory::singleton().getThreadFactory().newSynchronizer();

    /** We could set this parameter according to some maximum memory size for the list to be created. */
    _DIVDIAG = 10;

    size_t diagLength = 0;
    size_t k = nbQuerySequences;

    diagLength = 1;
    while (diagLength < k)     {  diagLength = diagLength << 1;  }

    if  (diagLength<1000000)   {  diagLength = diagLength << 3;  }

    _diagGlobal = diagLength - 1;

    k = _diagGlobal/_DIVDIAG + 10;

    _listGaplessAlign = (LISTGAP **) calloc (k, sizeof(LISTGAP*));

    _listGaplessAlignSize = 0;

    DEBUG (("UngapAlignmentResult::UngapAlignmentResult:  nbQuerySequences=%ld  _DIVDIAG=%d  _diagGlobal=%d   listSize=%d\n",
        nbQuerySequences, _DIVDIAG, _diagGlobal, k*sizeof(LISTGAP*)
    ));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
UngapAlignmentResult::~UngapAlignmentResult ()
{
    DEBUG (("UngapAlignmentResult::~UngapAlignmentResult: _listGaplessAlignSize=%ld\n", _listGaplessAlignSize));

    if (_synchro)  { delete _synchro; }

    if (_listGaplessAlign)
    {
        size_t k = _diagGlobal/_DIVDIAG + 10;
        for(size_t i=0; i<k; i++)
        {
            if (_listGaplessAlign[i]==NULL)   {  continue;  }

            LISTGAP* gl_next = 0;
            for (LISTGAP* gl = _listGaplessAlign[i];  gl != NULL;  )
            {
                gl_next = gl->next;
                free (gl);
                gl = gl_next;
            }
            _listGaplessAlign[i] = NULL;
        }

        free (_listGaplessAlign);
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool UngapAlignmentResult::insert (Alignment& align, void* context)
{
    bool result = false;

    IAlignmentSplitter* splitter = (IAlignmentSplitter*) context;

    if (splitter != 0)
    {
        int  splittab[1000];  //memset (splittab, 0, sizeof(splittab));

        int q_start=0, q_stop=0, s_start=0, s_stop=0;

        size_t nbAlign = splitter->splitAlign (align, splittab);

        /** A little check. */
        if (nbAlign%4 == 0)
        {
            for (size_t i=0; i<nbAlign; i=i+4)
            {
                q_start = align._queryStartInDb   + splittab[i+2];
                q_stop  = align._queryStartInDb   + splittab[i+0];

                s_start = align._subjectStartInDb + splittab[i+3];
                s_stop  = align._subjectStartInDb + splittab[i+1];

                if (q_stop - q_start >= 4)  // TO BE MODIFIED: 4
                {
                    result = addDiag (q_start, q_stop, s_start, s_stop, align._occur2->sequence.index);
                }
            }
        }
    }

    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool UngapAlignmentResult::doesExist (
    const indexation::ISeedOccurrence* subjectOccur,
    const indexation::ISeedOccurrence* queryOccur
)
{
    bool result = true;

    int d = 0;

    /** Shortcut (and optimization). */
    u_int64_t diff = queryOccur->offsetInDatabase - subjectOccur->offsetInDatabase;

    if (diff == 0)  {  d = (diff & _diagGlobal) + queryOccur->sequence.index % _DIVDIAG;  }
    else            {  d = (diff & _diagGlobal);  }

    int dd = d/_DIVDIAG;

    LISTGAP* gl = _listGaplessAlign[dd];

    if (gl==NULL)   {  result = false;  }

    if (result == true)
    {
#if 0
        /** We may want to lock the iteration of the list. In doing so, we may avoid some cell creations,
         * but we may slow down concurrent thread that access the list.
         */
        LocalSynchronizer local (_synchro);
#endif

        while ( (gl!=NULL) &&  ( (d<gl->diag) || ( (gl->diag==d) && (queryOccur->offsetInDatabase > gl->stop)) ) )
            gl = gl -> next;

        if (gl==NULL)   { result = false;  }
    }

    if (result == true)
    {
        if (d>gl->diag)   { result = false; }
    }

    if (result == true)
    {
        if ((gl->diag==d) && (queryOccur->offsetInDatabase<gl->start))    { result = false;  }
    }

    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool UngapAlignmentResult::addDiag (int q_start, int q_stop, int s_start, int s_stop, int seqIdx)
{
    bool alreadyExist = false;

    int d,k;

    LISTGAP* gl      = 0;
    LISTGAP* ngl     = 0;
    LISTGAP* prev_gl = 0;

    if (q_start == s_start)  {  d = ((q_start - s_start) & _diagGlobal)  + (seqIdx % _DIVDIAG);  }
    else                     {  d =  (q_start - s_start) & _diagGlobal;  }

    gl = _listGaplessAlign [d/_DIVDIAG];

    k=0;

    while ((gl!=NULL) && ( (d<gl->diag) || ((gl->diag==d)&&(q_start>=gl->stop))) )
    {
        prev_gl=gl;
        gl = gl -> next;
        k++;
    }

    // we can add a new alignment to the list (ngl) if it's satisfied one of the following conditions
    // gl = NULL  : no alignment identified
    // d>gl->diag : as the diagonals are sorted in ascending order, we have not met d = gl->diag
    // d=gl->diag et index1>gl->stop : a diagonal exists, but the index is outside the scope
    // k indicates the number of alignment on which stopped (k = 0 -> empty list)

    if ((gl==NULL) || (d>gl->diag) || ((gl->diag==d) && (q_start<gl->start)))
    {
        if ((ngl = (LISTGAP *) malloc(sizeof(LISTGAP)))==NULL) fprintf(stderr,"ERROR MALLOC MEMORY!");
        ngl->diag  = d;
        ngl->start = q_start;
        ngl->stop  = q_stop;

        // 4 cas sont a considerer suivant : k et gl
        if (gl == NULL)
        {
            if (k==0)
            {
                // gl = NULL et k = 0  --> empty list : connection to ngl (first element)
                ngl->next = (LISTGAP *) NULL;

                _synchro->lock ();
                _listGaplessAlign[d/_DIVDIAG] = ngl;
                _synchro->unlock ();
            }
            else
            {
                // gl = NULL et k = 1 --> insert ngl at the end of list
                ngl->next =  (LISTGAP *) NULL;

                _synchro->lock ();
                prev_gl->next = ngl;
                _synchro->unlock ();
            }
        }
        else
        {
            if (k==0)
            {
                // gl <> NULL et k = 0 --> insert ngl at beginning of list
                ngl->next = gl;

                _synchro->lock ();
                _listGaplessAlign[d/_DIVDIAG] = ngl;
                _synchro->unlock ();
            }
            else
            {
                // gl <> NULL et k > 0 --> insert ngl before gl
                ngl->next = gl;

                _synchro->lock ();
                prev_gl->next = ngl;
                _synchro->unlock ();
            }
        }

        _listGaplessAlignSize++;

        alreadyExist = false;
    }

    else
    {
        alreadyExist = true;
    }

    return alreadyExist;
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
