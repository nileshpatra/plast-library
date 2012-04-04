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

/** \file IProperty.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Property definition
 *
 *  This file contains facilities for managing [key,value] sets.
 *
 *  The idea is to have a generic way for holding information. For instance, this could be used:
 *     - for externalizing some information from some classes
 *     - managing command line options
 */

#ifndef _IPROPERTY_H_
#define _IPROPERTY_H_

#include <designpattern/api/SmartPointer.hpp>

#include <string>
#include <list>
#include <set>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/********************************************************************************/
/** \brief Design Pattern tools (Observer, SmartPointer, Command...) */
namespace dp {
/********************************************************************************/

/** \brief  Definition of a property as a [key,value] entry with a given depth.
 *
 * Define what a property can be. This is an extension of the [key,value] concept
 * since we add a notion a depth to this couple, which makes possible to have a tree
 * vision of a simple list of [depth,key,value] entries.
 */
struct IProperty
{
    /** Constructor.
     * \param[in] aDepth : depth of the [key,value]
     * \param[in] aKey   : the key
     * \param[in] aValue : the value
     */
    IProperty (size_t aDepth, const std::string& aKey, const std::string& aValue)
        : depth(aDepth), key(aKey), value(aValue)  {}

    /** Depth of the property. 0 should mean root property. */
    size_t      depth;

    /** Key of the property as a string. */
    std::string key;

    /** Value of the property as a string. */
    std::string value;

    /** Returns the value of the property as a C++ string.
     * \return the value
     */
    const std::string&  getValue  ()  { return value;                }

    /** Returns the value of the property as an integer (it supposes that the string represents an integer).
     * \return the value
     */
    long                getInt    ()  { return atol (value.c_str()); }

    /** Returns the value of the property as a C string.
     * \return the value
     */
    const char*         getString ()  { return value.c_str();        }

    /** */
    std::string toString ()
    {
        std::stringstream ss;
        ss << "[PROPERTY " << depth << ": " << key << " : " << value << "]";
        return ss.str();
    }
};

/********************************************************************************/

/** \brief Visitor for a IProperty instance.
 *
 *  Define a Design Pattern Visitor for the IProperty instance.
 *
 *  In this case, we have only the IProperty class to visit (not a true classes hierarchy like one can
 *  find more classically for the Visitor DP), but we add two methods, one called before the IProperty
 *  visit, and one called after.
 *
 *  It is defined as a SmartPointer for easing instance life cycle management.
 */
class IPropertiesVisitor : public SmartPointer
{
public:

    /** Called before the true visit of the IProperty instance. */
    virtual void visitBegin    () = 0;

    /** Visit of the IProperty instance.
     * \param[in] prop : the instance to be visited.
     */
    virtual void visitProperty (IProperty* prop) = 0;

    /** Called after the true visit of the IProperty instance. */
    virtual void visitEnd      () = 0;
};

/********************************************************************************/

/** \brief Container of IProperty instances with DP Visitor capability.
 *
 *  This interface merely defines a container of IProperty instances; it contains
 *  several 'add' methods for adding IProperty instances into the container.
 *
 *  It is possible to retrieve a specific IProperty instance given a key.
 *
 *  The main method is 'accept'; its purpose is to visit each contained IProperty instance.
 *  Note that the only way to iterate the whole IProperty set is to define its own IPropertiesVisitor
 *  class and make it accepted by the IProperties instance; the 'visitProperty' method should be then
 *  called for each IProperty instance.
 *
 *  It is defined as a SmartPointer for easing instance life cycle management.
 *
 * Sample of use:
 * \code
 * void sample ()
 * {
 *      // we create a IProperties instance.
 *      IProperties* props = new Properties ();
 *
 *      // we add some entries
 *      props->add (0, "root", "");
 *      props->add (1, "loud",   "len=%d", 3);
 *      props->add (1, "louder", "great");
 *      props->add (1, "stop",   "[x,y]=[%f,%f]", 3.14, 2.71);
 *
 *      // we create some visitor for dumping the props into a XML file
 *      IPropertiesVisitor* v = new XmlDumpPropertiesVisitor ("/tmp/test.xml");
 *
 *      // we accept the visitor; after that, the output file should have been created.
 *      props->accept (v);
 * }
 * \endcode
 *
 *  \see IProperty
 *  \see IPropertiesVisitor
 */
class IProperties : public SmartPointer
{
public:

    /** Accept a visitor (should loop over all IProperty instances).
     * \param[in] visitor : visitor to be accepted
     */
    virtual void accept (IPropertiesVisitor* visitor) = 0;

    /** Add a IProperty instance given a depth, a key and a value provided in a printf way.
     * \param[in] depth  : depth of the property to be added
     * \param[in] aKey   : key of the property to be added
     * \param[in] format : define the format of the value of the property, the actual value being defined by the ellipsis
     * \return a IProperty instance is created and returned as result of the method.
     *
     */
    virtual IProperty* add (size_t depth, const std::string& aKey, const char* format=0, ...) = 0;

    /** Add a IProperty instance given a depth, a key and a value.
     * \param[in] depth  : depth of the property to be added
     * \param[in] aKey   : key of the property to be added
     * \param[in] aValue : value (as a string) of the property to be added
     * \return a IProperty instance is created and returned as result of the method.
     */
    virtual IProperty* add (size_t depth, const std::string& aKey, const std::string& aValue) = 0;

    /** Add all the IProperty instances contained in the provided IProperties instance. Note that a depth is provided
     *  and is added to the depth of each added IProperty instance.
     * \param[in] depth : depth to be added to each depth of added instances.
     * \param[in] prop  : instance holding IProperty instances to be added
     */
    virtual void       add (size_t depth, IProperties* prop) = 0;

    /** Merge the IProperty instances contained in the provided IProperties instance.
     * \param[in] prop  : instance holding IProperty instances to be added
     */
    virtual void  merge (IProperties* prop) = 0;

    /** Returns the IProperty instance given a key.
     * \param[in] key : the key
     * \return the IProperty instance if found, 0 otherwise.
     */
    virtual IProperty* getProperty (const std::string& key) = 0;

    /** Clone the instance
     * \return the cloned instance.
     */
    virtual IProperties* clone () = 0;

    /** Distribute arguments that are comma separated list.
     * \return the list of distributed IProperties instances.
     */
    virtual std::list<IProperties*> map (const char* separator) = 0;

    /** Get the know keys.
     * \return the set of keys
     */
    virtual std::set<std::string> getKeys () = 0;

    /** Move the item (given its key) to the front of the container.
     * \param[in] key : the key of the item to be moved.
     */
    virtual void setToFront (const std::string& key) = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IPROPERTY_H_ */
