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

/** \file IAlignmentContainerFactory.hpp
 *  \brief Interface for some alignments container.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IALIGNMENT_CONTAINER_FACTORY_HPP_
#define _IALIGNMENT_CONTAINER_FACTORY_HPP_

/********************************************************************************/

#include <alignment/core/api/IAlignmentContainer.hpp>

#include <string>
#include <vector>

/********************************************************************************/
namespace alignment {
namespace core      {
/********************************************************************************/

/** \brief Factory
  */
class IAlignmentContainerFactory : public dp::SmartPointer
{
public:

    /** Create a new IAlignmentContainer instance.
     * \return the created container
     */
    virtual IAlignmentContainer* createContainer () = 0;

    /** Read alignments from some uri resource. Different implementations could read from different file format.
     * \param[in]  uri : uri of the resource to be used
     */
    virtual IAlignmentContainer* createContainerFromUri (const std::string& uri, void* context) = 0;
};

/********************************************************************************/
}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _IALIGNMENT_CONTAINER_FACTORY_HPP_ */