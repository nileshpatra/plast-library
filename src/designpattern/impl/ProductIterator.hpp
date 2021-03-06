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

/** \file ProductIterator.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Implementation of an Iterator that cross products two iterators.
 */

#ifndef _PRODUCT_ITERATOR_HPP_
#define _PRODUCT_ITERATOR_HPP_

#include <designpattern/api/Iterator.hpp>
#include <utility>
#include <list>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief Iterator over two iterators.
 *
 * We define a "product" iterator for two iterators, i.e. it will loop each possible
 * couple of the two provided iterators.
 *
 *  It is useful for having only one loop instead of two loops. Note however that it
 *  may still be more efficient to have two loops. The ProductIterator is just here
 *  for easing the product iteration on small sets.
 */
template <class T1, class T2> class ProductIterator : public Iterator<std::pair<T1,T2>&>
{
public:

    /** Constructor.
     * \param[in] it1 : first iterator.
     * \param[in] it2 : second iterator.
     */
    ProductIterator (Iterator<T1>* it1, Iterator<T2>* it2)  : _it1(it1), _it2(it2), _isDone(false)
    {
    }

    /** Destructor. */
    virtual ~ProductIterator ()
    {
    }

    /** \copydoc Iterator::first */
    void first()
    {
        /** We go to the beginning of the two iterators. */
        _it1->first();
        _it2->first();

        /** We use a specific attribute to keep track of the finish status of the iterator.
         *  This is merely an optimization in order not to call too often the "isDone" method
         *  on the two iterators.  */
        _isDone = false;
    }

    /** \copydoc Iterator::next */
    dp::IteratorStatus next()
    {
        /** We go to the next item of the second iterator. */
        _it2->next ();

        /** We check that it is not done. */
        if (_it2->isDone())
        {
            /** We go to the next item of the first iterator. */
            _it1->next ();

            if (! _it1->isDone() )
            {
                /** The first iterator is not done, we can reset the second to its beginning. */
                _it2->first ();
            }
            else
            {
                /** The first iterator is also done, the product iterator is therefore done. */
                _isDone = true;
            }
        }
        return dp::ITER_UNKNOWN;
    }

    /** \copydoc Iterator::isDone */
    bool isDone() { return _isDone; }

    /** \copydoc Iterator::currentItem */
    std::pair<T1,T2>& currentItem()
    {
        _current.first  = _it1->currentItem();
        _current.second = _it2->currentItem();

        return _current;
    }

private:

    /** First iterator. */
    Iterator<T1>* _it1;

    /** Second iterator. */
    Iterator<T2>* _it2;

    /** Current item in the iteration. */
    std::pair<T1,T2> _current;

    /** Tells whether the iteration is finished or not. */
    bool _isDone;
};

/********************************************************************************/

/** \brief Iterator over several iterators of same type.
 *
 * We define a cartesian iterator for several iterators that iterate the same kind of objects.
 *
 *  It is useful for having only one loop instead of several loops. Note however the constraint:
 *  each provided iterator must iterate the same kind of objects.
 *
 *  This is a generalization of the ProductIterator class with the distinction that the ProductIterator
 *  can iterate the product of two iterators having different kinds of objects.
 *
 *  \code
 *  void foo ()
 *  {
 *      const char* sep = ",";
 *
 *      // We create three iterators
 *      TokenizerIterator it1 ("a,b,c,d", sep);
 *      TokenizerIterator it2 ("1,2,3",   sep);
 *      TokenizerIterator it3 ("X,Y",     sep);
 *
 *      // We build a list holding the three iterators.
 *      list <Iterator<char*>* > itList;
 *      itList.push_back (&it1);
 *      itList.push_back (&it2);
 *      itList.push_back (&it3);
 *
 *      // We define an iterator that will iterate on the product of the three iterators.
 *      // Here, each iterated item will be a list of char*
 *      CartesianIterator <char*> p (itList);
 *
 *      // We loop over the cartesian iterator. Note that we have only one iteration loop
 *      for (p.first(); ! p.isDone(); p.next())
 *      {
 *          // We retrieve the current item as a list of char*
 *          list<char*>& current = p.currentItem();
 *
 *          // We can display the guts of our iteration current item.
 *          for (list<char*>::iterator itStr = current.begin(); itStr != current.end() ; itStr++)
 *          {
 *              printf ("%s ", *itStr);
 *          }
 *          printf ("\n");
 *      }
 * }
 *  \endcode
 */

template <typename T1> class CartesianIterator : public dp::Iterator <std::list<T1>& >
{
public:

    CartesianIterator (std::list<Iterator<T1>*>& itList) : _itList (itList), _isDone(false)
    {
    }

    void first()
    {
        _isDone = false;

        for (typename std::list<Iterator<T1>*>::iterator it=_itList.begin(); it!=_itList.end(); it++)
        {
            (*it)->first ();
        }
    }

    IteratorStatus next()
    {
        typename std::list<Iterator<T1>*>::iterator it;

        for (it=_itList.begin(); it!=_itList.end(); it++)
        {
            (*it)->next ();

            if ((*it)->isDone())  {  (*it)->first ();  }
            else  {  break;  }
        }

        _isDone = (it == _itList.end());

        return ITER_UNKNOWN;
    }

    bool isDone()
    {
        return _isDone;
    }

    std::list<T1>& currentItem()
    {
        _currentItem.clear ();

        for (typename std::list<Iterator<T1>*>::iterator it=_itList.begin(); it!=_itList.end(); it++)
        {
            _currentItem.push_back ((*it)->currentItem() );
        }

        return _currentItem;
    }

private:
   std::list<Iterator<T1>*>& _itList;

    std::list<T1> _currentItem;

    bool _isDone;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _PRODUCT_ITERATOR_HPP_ */
