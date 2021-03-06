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

/** \file ModifierContainerVisitor.hpp
 *  \brief A few implementations of IAlignmentResultVisitor interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _MODIFIER_ALIGNMENT_CONTAINER_VISITOR_HPP_
#define _MODIFIER_ALIGNMENT_CONTAINER_VISITOR_HPP_

/********************************************************************************/

#include <alignment/visitors/impl/HierarchyAlignmentVisitor.hpp>
#include <alignment/core/impl/AbstractAlignmentContainer.hpp>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/** \brief Visitor that fills a list holding all Alignment instances
 */
class ModifierContainerVisitor : public HierarchyAlignmentResultVisitor
{
public:

    /** */
    ModifierContainerVisitor () : _nbRemoved(0) {}

    /** \copydoc IAlignmentResultVisitor::finish */
    void postVisit (core::IAlignmentContainer* result)
    {
        /** Not pretty but avoid to recompute the alignments number after the modification process. */
        core::impl::AbstractAlignmentContainer* res = dynamic_cast<core::impl::AbstractAlignmentContainer*> (result);
        if (res != 0)  {  res->setSize (res->getAlignmentsNumber() - _nbRemoved);  }
    }

protected:
    size_t _nbRemoved;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _MODIFIER_ALIGNMENT_CONTAINER_VISITOR_HPP_ */
