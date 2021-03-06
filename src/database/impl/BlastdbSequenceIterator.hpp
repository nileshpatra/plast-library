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

/** \file BlastdbSequenceIterator.hpp
 *  \brief Sequence iterator that parses BLAST database files format
 *  \date 23/02/2014
 *  \author sbrillet
 */

#ifndef _BLASTDB_ITERATOR_HPP_
#define _BLASTDB_ITERATOR_HPP_

/********************************************************************************/

#include <database/impl/BlastdbDatabaseQuickReader.hpp>
#include <database/impl/BlastdbFileIndexReader.hpp>
#include <database/impl/AbstractSequenceIterator.hpp>
#include <database/impl/DatabaseUtility.hpp>
#include <designpattern/impl/FileLineIterator.hpp>
#include <misc/api/types.hpp>
#include <map>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/
#define BLAST_ASN1_SEQ_ID_GENERAL		0xAA

/** \brief Sequence iterator that parses BLAST Database files
 *
 *  This class implements the ISequenceIterator interface by iterating sequences
 *  read from a BLAST Database files. These files are generated thanks to the makeblastdb tool
 *  from the FASTA file format
 *
 *	The index file, sequence file and header file are the three files needed to
 * 	extract sequences from the BLAST database. For protein databases these files end with the extensions “.pin”,
 *	“.psq” and “.phr” respectively. For DNA databases the extensions are “.nin”, “.nsq” and “.nhr” respectively
 *
 *  \code
 *  void foo ()
 *  {
 *      // We create a BLAST Db format iterator
 *      BlastdbSequenceIterator itSeq ("myDb");
 *      for (itSeq.first(); !itSeq.isDone(); itSeq.next())
 *      {
 *          // We can retrieve the current sequence
 *          const ISequence* seq = itSeq.currentItem();
 *      }
 *  }
 *  \endcode
 *
 */
class BlastdbSequenceIterator : public AbstractSequenceIterator
{
public:

    /** Constructor.
     * \param[in] filename : path of the BLAST files to be read (name without extension)
     * 						 This name needs to be the same for the index, header and sequence file
     * \param[in] commentMaxSize : maximum size of sequences comments
     * \param[in] offset0 : starting offset in the sequence file (.psq or .nsq file)
     * \param[in] offset1 : ending offset in the sequence file (.psq or .nsq file)
     */
    BlastdbSequenceIterator (
        const char* filename,
        size_t commentMaxSize = 2*1024,
        u_int64_t offset0 = 0,
        u_int64_t offset1 = 0
    );

    /** Destructor. */
    virtual ~BlastdbSequenceIterator ();

    /** \copydoc AbstractSequenceIterator::first */
    void first();

    /** \copydoc AbstractSequenceIterator::next */
    dp::IteratorStatus next();

    /** \copydoc AbstractSequenceIterator::isDone */
    bool isDone()  { return _isDone;  }

    /** \copydoc AbstractSequenceIterator::currentItem */
    ISequence* currentItem() { return getBuilder()->getSequence(); }

    /** \copydoc AbstractSequenceIterator::clone */
    ISequenceIterator* clone () { return 0; }

    /** \copydoc ISequenceIterator::transformComment */
    std::string transformComment (const char* comment);

private:
    /** List of index files to be read. */
    std::list<BlastdbFileIndexReader*> _filesIndex;

    /** First character to be read in the file. */
    u_int64_t _offset0;

    /** Last character to be read in the file. */
    u_int64_t _offset1;

    /** Maximum size of a sequence comment. */
    size_t _commentMaxSize;

    /** Total number of read characters. */
    u_int64_t _readTotalSize;

    /** Size of the currently read line. */
    u_int64_t _readCurrentSize;

    /** Size of the current file. */
    u_int64_t _fileCurrentSize;

    /** Size of all files already parsed. */
    u_int64_t _cummulatedFilesLength;

    /** the current index file */
    BlastdbFileIndexReader* _currentIndexFile;

    /** the current sequence file */
    os::IMemoryFile* _currentSequenceFile;

    /** */
    std::list<BlastdbFileIndexReader*>::iterator _filesIterator;

    /** offset index to read the different offset in the sequence, header and ambiguity table. */
    u_int32_t _offsetReadIndex;

    /** local pointer to read the sequence data, it is usefull to keep the address locally and skip the polymorphism cost. */
    const char* _data;

    /** Tells whether the iteration is finished or not. */
    bool _isDone;

    /** Tells if the iteration is finished or not. */
    bool _eof;

    /** Quick reader Type **/
    DatabaseLookupType::QuickReaderType_e _dbType;

    /** FirstOfsset in the data file. it is used for the map file */
    u_int64_t _firstOffset;

    /** Name of the header file. */
    std::string _currentHeaderFileName;

    /** List of index files to be read. */
    std::map<std::string,os::IMemoryFile*> _filesHeaderIndex;

    /** Returns false if eof. */
    bool retrieveNextFile ();

    /** Read the index file and header file name to construct the comment string
     *  \param[in] firstOffset : offset in the data file
     */

    void readIndexFileAndCreateHeaderFile (u_int64_t firstOffset);

};

/********************************************************************************/

/** \brief Implementation of ISequenceIteratorFactory interface.
 *
 * This implementation creates BlastdbSequenceIterator instances.
 */
class BlastdbSequenceIteratorFactory  : public ISequenceIteratorFactory
{
public:
    /** \copydoc ISequenceIteratorFactory::createSequenceIterator */
    virtual ISequenceIterator* createSequenceIterator (const std::string& uri, const misc::Range64& range)
    {
        return new BlastdbSequenceIterator (uri.c_str(), SEQUENCE_MAX_COMMENT_SIZE, range.begin, range.end);
    }
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _BLASTDB_ITERATOR_HPP_ */
