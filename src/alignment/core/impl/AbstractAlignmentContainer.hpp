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

/** \file AbstractAlignmentContainer.hpp
 *  \brief Abstract implementation of IAlignmentContainer interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ABSTRACT_ALIGNMENT_CONTAINER_HPP_
#define _ABSTRACT_ALIGNMENT_CONTAINER_HPP_

/********************************************************************************/

#include <os/api/IThread.hpp>

#include <alignment/core/api/IAlignmentContainer.hpp>

/********************************************************************************/
namespace alignment {
namespace core      {
namespace impl      {
/********************************************************************************/

/** \brief Abstract implementation of IAlignmentResult interface.
 *
 * This implementation implements a few methods of the IAlignmentResult interface.
 *
 * It provides some synchronization scheme for protecting the instances from
 * concurrent accesses (which can occur since an instance may be shared by different
 * IHitIterator instances, each one iterated in different threads).
 *
 * It is still abstract since methods like insert() and shrink() still have to be
 * implemented by subclasses.
 */
class AbstractAlignmentContainer : public IAlignmentContainer
{
public:

    /** Constructor. */
    AbstractAlignmentContainer ();

    /** Destructor. */
    virtual ~AbstractAlignmentContainer ();

    /** \copydoc IAlignmentResult::getProperties */
    dp::IProperties* getProperties (const std::string& root);

    /** \copydoc IAlignmentResult::getSize */
    u_int32_t getSize ()  { return _nbAlignments; }

    /** Ugly... */
    void setSize (u_int32_t s)  { _nbAlignments = s; }

protected:

    /** Synchronizer for preventing for concurrent accesses. */
    os::ISynchronizer* _synchro;

    /** */
    u_int32_t _nbAlignments;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_ALIGNMENT_CONTAINER_HPP_ */