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

/** \file AlignmentSplitter.hpp
 *  \brief Implementation providing means for splitting a gap alignment into ungap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ALIGNMENT_SPLITTER_HPP_
#define _ALIGNMENT_SPLITTER_HPP_

/********************************************************************************/

#include <alignment/tools/api/IAlignmentSplitter.hpp>

/********************************************************************************/
namespace alignment {
namespace tools     {
namespace impl      {
/********************************************************************************/

/** \brief Implementation of IAlignmentResult for splitting a gap alignment into ungap alignments.
 *
 * This implementation uses dynamic programming for providing the service.
 */
class AlignmentSplitter : public IAlignmentSplitter
{
public:

    /** Constructor.
     * \param[in] scoreMatrix   : score matrix to be used
     * \param[in] openGapCost   : cost for opening a gap
     * \param[in] extendGapCost : cost for extending a gap
     */
    AlignmentSplitter (algo::core::IScoreMatrix* scoreMatrix, int openGapCost, int extendGapCost);

    /** Destructor. */
    virtual ~AlignmentSplitter ();

    /** \copydoc IAlignmentSplitter::splitAlign(Alignment&,int*) */
    size_t splitAlign (
        const database::LETTER* subjectSeq,
        const database::LETTER* querySeq,
        u_int32_t  subjectStartInSeq,
        u_int32_t  subjectEndInSeq,
        u_int32_t  queryStartInSeq,
        u_int32_t  queryEndInSeq,
        u_int32_t* splittab,
        u_int32_t& identity,
        u_int32_t& nbGap,
        u_int32_t& nbMis,
        u_int32_t& alignSize,
        database::LETTER* subjectAlign,
        database::LETTER* queryAlign
    );

    /** \copydoc IAlignmentSplitter::splitAlign(Alignment&,int*) */
    size_t splitAlign (core::Alignment& align,  u_int32_t* splittab);

    /** \copydoc IAlignmentSplitter::computeInfo */
    void computeInfo (core::Alignment& align);

private:

    algo::core::IScoreMatrix* _scoreMatrix;
    void setScoreMatrix (algo::core::IScoreMatrix* scoreMatrix)  { SP_SETATTR (scoreMatrix); }

    int _openGapCost;
    int _extendGapCost;

    int16_t** _matrix_H;
    int16_t** _matrix_E;
    int16_t** _matrix_F;

    int _DefaultAlignSize;
    int _MaxAlignSize;

    int16_t** newMatrix  (int nrows, int ncols);
    void      freeMatrix (int16_t*** mat);
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_SPLITTER_HPP_ */