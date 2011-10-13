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

#ifndef _ISEED_MODEL_HPP_
#define _ISEED_MODEL_HPP_

/********************************************************************************/

#include "ISeed.hpp"
#include "ISeedIterator.hpp"
#include "IAlphabet.hpp"
#include "SmartPointer.hpp"
#include "IProperty.hpp"
#include "types.hpp"

/********************************************************************************/
namespace seed {
/********************************************************************************/

class ISeedModel : public dp::SmartPointer
{
public:

    /** Get alphabet. */
    virtual  database::IAlphabet* getAlphabet () = 0;

    /** Get the span (ie. the number of letters of a seed). */
    virtual size_t getSpan () = 0;

    /** Get the number of possible seeds. */
    virtual size_t getSeedsMaxNumber () = 0;

    /** Returns an iterator that loops over seeds in a word (a sequence for instance).  */
    virtual ISeedIterator* createSeedsIterator (const database::IWord& data) = 0;

    /** Returns an iterator that loops over all possible seeds for the model.  */
    virtual ISeedIterator* createAllSeedsIterator () = 0;

    /** Tells whether two seeds are equivalent. */
    virtual bool compare (const ISeed& s1, const ISeed& s2) = 0;

    /** Returns a table holding all seeds.  */
    virtual const database::LETTER* getAllSeedsTable() = 0;

    /** Return properties about the instance. */
    virtual dp::IProperties* getProperties () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEED_MODEL_HPP_  */