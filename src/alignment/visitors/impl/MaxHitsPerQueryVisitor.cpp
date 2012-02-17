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

#include <alignment/visitors/impl/MaxHitsPerQueryVisitor.hpp>

#include <database/api/ISequenceDatabase.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace database;
using namespace alignment;
using namespace alignment::core;

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void MaxHitsPerQueryVisitor::visitQuerySequence   (const database::ISequence* seq)
{
    if (_ref)  { _ref->visitQuerySequence (seq); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void MaxHitsPerQueryVisitor::visitSubjectSequence (const database::ISequence* seq)
{
    _currentHitsNb = 0;

    if (_ref)  { _ref->visitSubjectSequence (seq); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void MaxHitsPerQueryVisitor::visitAlignment (Alignment* align)
{
    _currentHitsNb ++;

    if (_currentHitsNb <= _maxHitsPerQuery  &&  _ref)  { _ref->visitAlignment (align); }
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
