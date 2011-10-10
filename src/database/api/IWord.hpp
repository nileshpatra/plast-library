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

#ifndef _WORD_HPP_
#define _WORD_HPP_

/********************************************************************************/

#include "IAlphabet.hpp"
#include "MemoryAllocator.hpp"
#include <stddef.h>
#include <string>
#include <vector>
#include <stdio.h>

/********************************************************************************/
namespace database {
/********************************************************************************/

/** We define what a word is: some raw content encoded in some encoding scheme. */
struct IWord
{
    os::Vector<LETTER> letters;
    Encoding           encoding;

    /** Constructors. */
    IWord ()                : letters(0),         encoding(SUBSEED)       {}
    IWord (const IWord& w)  : letters(w.letters), encoding(w.encoding)    {}

    IWord (size_t size, const LETTER* buf=0)  : letters(size), encoding(SUBSEED)
    {
        if (buf != 0)  {  for (size_t i=0; i<size; i++)  { letters.data[i] = buf[i]; }  }
    }

    /** */
    IWord& operator= (const IWord& w)
    {
        if (this != &w)
        {
            letters  = w.letters;
            encoding = w.encoding;
        }

        return *this;
    }

    /** Update instance by providing a reference to a buffer. */
    void setReference (size_t size, LETTER* aBuffer)
    {
        letters.setReference (size, aBuffer);
    }

    /** */
    bool operator== (const IWord& w) const
    {
        return (encoding != w.encoding) &&  (letters == w.letters);
    }

    /** */
    std::string toString () const
    {
        std::string result (letters.size, 'X');

        const LETTER* convert = EncodingManager::singleton().getEncodingConversion (encoding, ASCII);

        if (convert != 0)
        {
            for (size_t i=0; i<letters.size; i++)
            {
                result[i] =  convert[(int)letters.data[i]];
            }
        }
        else
        {
            //TBD we should launch some exception instead
            printf ("ERROR... NO CONVERSION TABLE (encoding=%d)\n", encoding);
        }
        return result;
    }
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _WORD_HPP_ */
