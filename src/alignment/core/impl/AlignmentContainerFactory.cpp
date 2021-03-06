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

#include <alignment/core/impl/AlignmentContainerFactory.hpp>
#include <alignment/core/impl/BasicAlignmentContainer.hpp>
#include <alignment/core/impl/ReaderAlignmentContainer.hpp>

#include <designpattern/impl/FileLineIterator.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>

#include <math.h>

#include <stdio.h>
#define DEBUG(a) //a

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace database;

/********************************************************************************/
namespace alignment {
namespace core      {
namespace impl      {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IAlignmentContainer* AlignmentContainerFactory::createContainer ()
{
    return new BasicAlignmentContainer ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IAlignmentContainer* AlignmentContainerFactory::createContainerFromUri (const std::string& uri, void* context)
{
    return createContainerFromUri (new FileLineIterator (uri.c_str()), context);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IAlignmentContainer* AlignmentContainerFactory::createContainerFromUri (dp::impl::FileLineIterator* it, void* context)
{
    LOCAL(it);

    /** We first create the new container. */
    ReaderAlignmentContainer* result = new ReaderAlignmentContainer ();

    /** We may recover some extra information (not very pretty...) */
    ReaderAlignmentContainer* ref = (ReaderAlignmentContainer*) context;

    /** We need some maps. */
    std::map<std::string,int>& subjectMapComments = (ref ? ref->_subjectMapComments : result->_subjectMapComments);
    std::map<std::string,int>& queryMapComments   = (ref ? ref->_queryMapComments   : result->_queryMapComments);

    int queryIdx   = 0;
    int subjectIdx = 0;

    if (ref != 0)
    {
    	/** In case we parse the comp db, we must not override previous idx of the ref db.
    	 *  So, we ensure that new found sequences in comp (and not present in ref) will have
    	 *  indexes that can't overlap ref indexes. */
    	queryIdx   = queryMapComments.size  ();
    	subjectIdx = subjectMapComments.size ();
    }

    ISequence sbjSequence;
    ISequence qrySequence;

    size_t nbLines = 0;
    for (it->first(); !it->isDone(); it->next(), nbLines++)
    {
        Alignment align;

        align.setSequence (Alignment::SUBJECT, &sbjSequence);
        align.setSequence (Alignment::QUERY,   &qrySequence);

        double identitiesPercent = 0.0;
        u_int16_t nbGaps = 0;
        misc::Range32 qryRange;
        misc::Range32 sbjRange;

        size_t idx = 0;
        TokenizerIterator tokenizer (it->currentItem(), " \t");
        for (tokenizer.first(); !tokenizer.isDone(); tokenizer.next(), idx++)
        {
            char* token = tokenizer.currentItem();

            /** We may have comment lines => just skip them. */
            if (token[0] == '#')  {  continue; }

            if (idx==0)
            {
                map<string,int>::iterator qsearch = queryMapComments.find (token);
                if (qsearch == queryMapComments.end())
                {
                    queryMapComments[token] = queryIdx;
                    qrySequence.index = queryIdx;
                    queryIdx++;
                }
                else
                {
                    qrySequence.index   = qsearch->second;
                }
            }

            else if (idx==1)
            {
                map<string,int>::iterator ssearch = subjectMapComments.find (token);
                if (ssearch == subjectMapComments.end())
                {
                    subjectMapComments[token] = subjectIdx;
                    sbjSequence.index = subjectIdx;
                    subjectIdx++;
                }
                else
                {
                    sbjSequence.index = ssearch->second;
                }
            }

            else if (idx==2)   {  identitiesPercent = misc::atof (token);     }
            else if (idx==3)   {  align.setLength   (misc::atoi (token));     }
            else if (idx==4)   {  align.setNbMisses (misc::atoi (token));     }
            else if (idx==5)   {  nbGaps = misc::atoi (token);                }
            else if (idx==6)   {  qryRange.begin = misc::atoi (token);        }
            else if (idx==7)   {  qryRange.end   = misc::atoi (token);        }
            else if (idx==8)   {  sbjRange.begin = misc::atoi (token);        }
            else if (idx==9)   {  sbjRange.end   = misc::atoi (token);        }
            else if (idx==10)  {  align.setEvalue   (misc::atof (token));     }
            else if (idx==11)  {  align.setBitScore (misc::atof (token));     }

            else if (idx==12)  {  qrySequence.length = misc::atol (token);                    }
            else if (idx==13)  {  align.setNbGaps (Alignment::QUERY, misc::atol (token));     }
            else if (idx==14)  {  align.setFrame  (Alignment::QUERY, misc::atoi (token));     }

            else if (idx==12)  {  sbjSequence.length = misc::atol (token);                    }
            else if (idx==13)  {  align.setNbGaps (Alignment::SUBJECT, misc::atol (token));   }
            else if (idx==14)  {  align.setFrame  (Alignment::SUBJECT, misc::atoi (token));   }

            else if (idx==15)  {  align.setNbPositives (misc::atol (token));   }
        }

        /** We read from a human readable file that counts range with a 1 starting position.
         *  We convert back to a 0 starting position. */
        qryRange.begin  --;
        qryRange.end    --;
        sbjRange.begin  --;
        sbjRange.end    --;

        /** We finalize the alignment. */
        align.setRange     (Alignment::QUERY,   qryRange);
        align.setRange     (Alignment::SUBJECT, sbjRange);
        align.setNbGaps    (nbGaps);
        align.setNbIdentities ((u_int32_t) round (identitiesPercent * align.getLength() / 100.0));

        /** We insert the alignment. */
        result->insert (align, NULL);
    }

    /** Shorcuts. */
    std::vector<std::string>&  subjectComments = result->_subjectComments;
    std::vector<std::string>&  queryComments   = result->_queryComments;

    subjectComments.resize (subjectMapComments.size());
    for (map<string,int>::iterator it = subjectMapComments.begin(); it != subjectMapComments.end(); it++)
    {
        subjectComments [it->second] = it->first;
    }

    queryComments.resize (queryMapComments.size());
    for (map<string,int>::iterator it = queryMapComments.begin(); it != queryMapComments.end(); it++)
    {
        queryComments [it->second] = it->first;
    }

    /** We have now to associate a comment on each ISequence. */
    result->setComments ();

    DEBUG (cout <<  "AlignmentContainerFactory::createContainerFromUri:  "
    		<< "    nbQry="   << queryComments.size()
    		<< "    nbSbj="   << subjectComments.size()
    		<< "    nbAlign=" << nbLines
    		<< endl
	);

    /** We return the result. */
    return result;
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
