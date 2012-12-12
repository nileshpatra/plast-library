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

/** \file CompareResultVisitor.hpp
 *  \brief A few implementations of IAlignmentResultVisitor interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _COMPARE_RESULT_VISITOR_HPP_
#define _COMPARE_RESULT_VISITOR_HPP_

/********************************************************************************/

#include <alignment/core/api/IAlignmentContainer.hpp>
#include <alignment/visitors/impl/AdapterAlignmentVisitor.hpp>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/** \brief Visitor that fills a list holding all Alignment instances
 *
 * This visitor dumps alignments into a list
 */
class CompareResultVisitor : public core::IAlignmentContainerVisitor
{
public:

    CompareResultVisitor (core::IAlignmentContainer* dbComp)
        : _dbComp(dbComp), _commonSize(0), _specificSize(0), _qrySeq(0), _sbjSeq(0)  { }

    /** */
    virtual ~CompareResultVisitor () { }

    /** */
    void visitQuerySequence   (const database::ISequence* seq,   const misc::ProgressInfo& progress)
    {
        _qrySeq = seq;
    }

    /** */
    void visitSubjectSequence (const database::ISequence* seq,   const misc::ProgressInfo& progress)
    {
        _sbjSeq = seq;

        if (_dbComp->getContainer(_qrySeq, _sbjSeq) == 0)
        {
            _specificSize++;
        }
        else
        {
            _commonSize++;
        }
    }

    /** */
    void visitAlignment (core::Alignment* align, const misc::ProgressInfo& progress)  {}

    /** */
    void postVisit  (core::IAlignmentContainer* result)  {}

    /** \copydoc IAlignmentContainerVisitor::visitAlignmentsList */
    void visitAlignmentsList (
        const database::ISequence* qry,
        const database::ISequence* sbj,
        std::list<core::Alignment>& alignments
    ) {}

    /** \copydoc IAlignmentContainerVisitor::finalize */
    void finalize (void)  { }

    /** \copydoc IAlignmentContainerVisitor::getPosition */
    u_int64_t getPosition ()  { return 0; }

    size_t getTotalSize    ()  { return _commonSize + _specificSize; }
    size_t getCommonSize   ()  { return _commonSize;   }
    size_t getSpecificSize ()  { return _specificSize; }

    double getCommonPercentage ()  {  return (double)_commonSize  /  (double) (_commonSize + _specificSize);  }

private:

    core::IAlignmentContainer* _dbComp;

    size_t _commonSize;
    size_t _specificSize;

    const database::ISequence* _qrySeq;
    const database::ISequence* _sbjSeq;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _COMPARE_RESULT_VISITOR_HPP_ */
