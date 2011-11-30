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

/** \file FileLineIterator.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Implementation of an Iterator that loops over the line of a file.
 */

#ifndef _FILE_ITERATOR_HPP_
#define _FILE_ITERATOR_HPP_

#include <designpattern/api/Iterator.hpp>
#include <os/impl/DefaultOsFactory.hpp>
#include <string.h>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief Iterator that loops over the line of a file.
 *
 *  This iterator loops over the line of a file.
 *  One must give the name of the file to be iterated and the maximum size of a read line.
 *
 *  It inherits from Iterator class by expliciting the type (char*) of the iterated items.
 *  Some inlined methods for optimization.
 *
 *  Code sample:
 *  \code
 *  void sample ()
 *  {
 *      // We create a file iterator. Maximum line size is 256 characters
 *      FileLineIterator it ("/tmp/afile.txt", 256);
 *
 *      // We loop each line of the file
 *      for (it.first(); !it.isDone(); it.next())
 *      {
 *          // We get the current read line
 *          char* line = it.currentItem ();
 *      }
 *  }
 *  \endcode
 *
 *  This is used for instance for parsing FASTA files.
 */
class FileLineIterator : public Iterator<char*>
{
public:

    /** Constructor. Note that one can provide a range [offset0,offset1] of characters to be read within the file.
     * \param[in]  filename     : file name of the file to be iterated
     * \param[in]  lineMaxSize  : number of characters to be read in a line
     * \param[in]  offset0      : if not 0, provides the first character index to be read in the file
     * \param[in]  offset1      : if not 0, provides the last character index to be read in the file
     */
    FileLineIterator (const char* filename, size_t lineMaxSize, u_int64_t offset0=0, u_int64_t offset1=0);

    /** Destructor. */
    virtual ~FileLineIterator ();

    /** \copydoc Iterator<char*>::first */
    void first();

    /** \copydoc Iterator<char*>::next */
    dp::IteratorStatus next()
    {
        if (_file)
        {
            if (_file->gets (_line, _lineMaxSize) == NULL)
            {
                _eof = true;
            }
            else
            {
                _readCurrentSize = strlen (_line);
                _readTotalSize  += _readCurrentSize;
                _eof = (_readTotalSize > _range);

                // don't take the ending '\n'
                _line[--_readCurrentSize] = 0;
            }
        }
        return ITER_UNKNOWN;
    }

    /** \copydoc Iterator<char*>::isDone */
    bool isDone()          { return _eof;  }

    /** \copydoc Iterator<char*>::currentItem */
    char* currentItem()  {  return _line;  }

    /** Returns the size of the currently read line.
     * \return current read line size
     */
    u_int64_t getCurrentReadSize ()  { return _readCurrentSize; }

private:

    /** URI of the file to be iterated. */
    std::string _filename;

    /** Maximum size of a line. */
    size_t      _lineMaxSize;

    /** Instance of the file to be read. */
    os::IFile* _file;
    char*  _line;

    /** First character to be read in the file. */
    u_int64_t _offset0;

    /** Last character to be read in the file. */
    u_int64_t _offset1;

    /** Number of characters to be read in the file. */
    u_int64_t _range;

    /** Total number of read characters. */
    u_int64_t _readTotalSize;

    /** Size of the currently read line. */
    u_int64_t _readCurrentSize;

    /** Tells if the iteration is finished or not. */
    bool _eof;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _FILE_ITERATOR_HPP_ */
