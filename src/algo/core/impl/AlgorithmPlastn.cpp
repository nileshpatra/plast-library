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

#include <algo/core/impl/AlgorithmPlastn.hpp>

#include <designpattern/impl/RangeIterator.hpp>

#include <alignment/core/impl/HspContainer.hpp>
#include <alignment/core/impl/BasicAlignmentContainer.hpp>

#include <algo/hits/hsp/HspGeneratorCmd.hpp>
#include <algo/hits/hsp/HspExtensionCmd.hpp>
#include <algo/hits/hsp/AlignmentGeneratorCmd.hpp>

#include <alignment/tools/impl/SemiGappedAlign.hpp>
#include <alignment/tools/impl/AlignmentSplitter.hpp>

#include <alignment/visitors/impl/FilterContainerVisitor.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace os;
using namespace os::impl;
using namespace misc;
using namespace database;
using namespace database::impl;
using namespace indexation;

using namespace algo::hits;
using namespace algo::hits::hsp;

using namespace alignment::core;
using namespace alignment::core::impl;

using namespace alignment::tools;
using namespace alignment::tools::impl;

using namespace alignment::visitors;
using namespace alignment::visitors::impl;

/********************************************************************************/
namespace algo {
namespace core {
namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlgorithmPlastn::AlgorithmPlastn (
    IConfiguration*                                 config,
    database::IDatabaseQuickReader*                 reader,
    IParameters*                                    params,
    alignment::filter::IAlignmentFilter*            filter,
    alignment::core::IAlignmentContainerVisitor*    resultVisitor,
    algo::core::IDatabasesProvider*                 dbProvider,
    bool&                                           isRunning
)
	: AbstractAlgorithm (config, reader, params, filter, resultVisitor, dbProvider, isRunning),
  	  _hspContainer(0), _nbPasses(4), _currentPass(0)
{
    DEBUG (("AlgorithmPlastn::AlgorithmPlastn\n"));

    /** WARNING !  We first switch to nucleotide alphabet before creating the instance. */
    EncodingManager::singleton().setKind (EncodingManager::ALPHABET_NUCLEOTID);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlgorithmPlastn::~AlgorithmPlastn ()
{
    DEBUG (("AlgorithmPlastn::~AlgorithmPlastn\n"));

    setHspContainer (0);

    /** WARNING !  We switch back to amino acid alphabet. */
    EncodingManager::singleton().setKind (EncodingManager::ALPHABET_AMINO_ACID);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlgorithmPlastn::computeAlignments (
    IAlignmentContainer*   alignmentResult,
    IAlignmentContainer*   ungapAlignmentResult,
    ISequenceDatabase*     subjectDb,
    ISequenceDatabase*     queryDb,
    ICommandDispatcher*    dispatcher,
    TimeInfo*              timeStats
)
{
    DEBUG (("AlgorithmPlastn::computeAlignments BEGIN\n"));

    /** We need a range iterator for getting successive seeds hashcode ranges. */
    u_int32_t maxSeedsNumber = 1 << (2*getSeedsModel()->getSpan());

    misc::Range<u_int32_t> range (0, maxSeedsNumber - 1);
    RangeIterator<u_int32_t> rangeIterator (range, 1000, DefaultFactory::thread().newSynchronizer());

    timesVec.push_back (DefaultFactory::time().gettime());

    /**********************************************************************/
    /***************************   PASS 0  ********************************/
    /**********************************************************************/
    setHspContainer (pass0 (subjectDb, queryDb, dispatcher, rangeIterator));

    timesVec.push_back (DefaultFactory::time().gettime());

    DEBUG (("AlgorithmPlastn::computeAlignments: PASS 0:  %ld HSP generated in %d msec\n",
		_hspContainer->getItemsNumber(), timesVec[timesVec.size()-1] - timesVec[timesVec.size()-2]
    ));

    /**********************************************************************/
    /***************************   PASS 1  ********************************/
    /**********************************************************************/
    _currentPass++;
    setHspContainer (pass1 (subjectDb, queryDb, dispatcher, _hspContainer, _params->XdroppofGap));

    timesVec.push_back (DefaultFactory::time().gettime());

    DEBUG (("AlgorithmPlastn::computeAlignments: PASS 1:  %ld HSP generated in %d msec\n",
		_hspContainer->getItemsNumber(), timesVec[timesVec.size()-1] - timesVec[timesVec.size()-2]
    ));

    /**********************************************************************/
    /***************************   PASS 1b  ********************************/
    /**********************************************************************/
    _currentPass++;
    setHspContainer (pass1 (subjectDb, queryDb, dispatcher, _hspContainer, _params->finalXdroppofGap));

    timesVec.push_back (DefaultFactory::time().gettime());

    DEBUG (("AlgorithmPlastn::computeAlignments: PASS 1b: %ld HSP generated in %d msec\n",
		_hspContainer->getItemsNumber(), timesVec[timesVec.size()-1] - timesVec[timesVec.size()-2]
    ));

    /**********************************************************************/
    /***************************   PASS 2  ********************************/
    /**********************************************************************/
    _currentPass++;
    pass2 (subjectDb, queryDb, dispatcher, _hspContainer, alignmentResult);

    timesVec.push_back (DefaultFactory::time().gettime());

    DEBUG (("AlgorithmPlastn::computeAlignments: PASS 2:  %d alignments generated in %d msec\n",
        alignmentResult->getAlignmentsNumber(),
        timesVec[timesVec.size()-1] - timesVec[timesVec.size()-2]
    ));

    /**********************************************************************/
    /***************************   FINISH  ********************************/
    /**********************************************************************/

    DEBUG (("\n===> FOUND %d alignments in total time %d msec\n\n",
        alignmentResult->getAlignmentsNumber(),
        timesVec[timesVec.size()-1] - timesVec[0]
    ));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlgorithmPlastn::finalizeAlignments (alignment::core::IAlignmentContainer* alignmentResult, os::impl::TimeInfo* timeStats)
{
    timesVec.push_back (DefaultFactory::time().gettime());

    /** We filter the alignments. */
    FilterContainerVisitor filterVisitor (_filter);
    alignmentResult->accept (&filterVisitor);

    /** We create a visitor for dumping the resulting alignments. The used visitor has been provided from a higher layer
     *  but it is likely a 'file dump' visitor that will dump all the alignments into a file. Note by the way that
     *  the actual format of the output file has not to be known here (it could be tabulated columns or xml) and relies
     *  on the actual type of the getResultVisitor. */
    timeStats->addEntry ("output");
    alignmentResult->accept (getResultVisitor());
    timeStats->stopEntry ("output");

    timesVec.push_back (DefaultFactory::time().gettime());
    DEBUG (("AlgorithmPlastn::finalizeAlignments DONE in %d msec\n",
		timesVec[timesVec.size()-1] - timesVec[timesVec.size()-2]
    ));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IHspContainer* AlgorithmPlastn::pass0 (
	ISequenceDatabase*     subjectDb,
	ISequenceDatabase*     queryDb,
	ICommandDispatcher*    dispatcher,
	RangeIterator<u_int32_t>& rangeIterator
)
{
	IHspContainer* result = new HspContainer (queryDb->getSize());

	/** Shortcuts. */
    size_t nbcpu = dispatcher->getExecutionUnitsNumber();

    vector<IHspContainer*>  containers (nbcpu);
    for (size_t i=0; i<nbcpu; i++)  {  (containers[i] = new HspContainer (queryDb->getSize()))->use (); }

    list<ICommand*> commands;
    for (size_t i=0; i<nbcpu; i++)
    {
        commands.push_back (new HSPGenerator(
            getIndexator(),
            containers[i],
            rangeIterator,
            _params->ungapScoreThreshold,
            _params->reward,
            _params->penalty,
            ABS (_params->penalty * 1),
            this
        ));
    }

    /** We dispatch the commands. */
    dispatcher->dispatchCommands (commands, 0);

    /** We merge the collected containers into our result. */
    result->merge (containers);

    /** We release the children containers. */
    for (size_t i=0; i<nbcpu; i++)  {  delete containers[i];  }

    /** We return the result. */
	return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IHspContainer* AlgorithmPlastn::pass1 (
	ISequenceDatabase*  subjectDb,
	ISequenceDatabase*  queryDb,
	ICommandDispatcher* dispatcher,
	IHspContainer*		sourceHsp,
	int					xdrop
)
{
	IHspContainer* result = new HspContainer (queryDb->getSize());

	/** Shortcuts. */
    size_t nbcpu = dispatcher->getExecutionUnitsNumber();

    vector<IHspContainer*>  containers (nbcpu);
    for (size_t i=0; i<nbcpu; i++)  {  (containers[i] = new HspContainer (queryDb->getSize()))->use (); }

    list<ICommand*> commands;
    for (size_t i=0; i<nbcpu; i++)
    {
        commands.push_back (new HspExtensionCmd(
            subjectDb,  queryDb,
            getQueryInfo(),
            sourceHsp, containers[i],
            new SemiGapAlign (getScoreMatrix(), _params->openGapCost, _params->extendGapCost, xdrop),
            _params,
            this
        ));
    }
    dispatcher->dispatchCommands (commands, 0);

    /** We merge the collected containers into our result. */
    result->merge (containers);

    /** We release the children containers. */
    for (size_t i=0; i<nbcpu; i++)  {  delete containers[i];  }

    /** We return the result. */
	return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlgorithmPlastn::pass2 (
	ISequenceDatabase*    subjectDb,
	ISequenceDatabase*    queryDb,
	ICommandDispatcher*   dispatcher,
	IHspContainer*		  sourceHsp,
	IAlignmentContainer*  alignContainer
)
{
	/** Shortcuts. */
    size_t nbcpu = dispatcher->getExecutionUnitsNumber();

    vector<IAlignmentContainer*>  containers (nbcpu);
    for (size_t i=0; i<nbcpu; i++)  {  (containers[i] = _config->createGapAlignmentResult())->use (); }

    list<ICommand*> commands;
    for (size_t i=0; i<nbcpu; i++)
    {
        commands.push_back (new AlignmentGeneratorCmd (
            subjectDb,  queryDb,
            getQueryInfo(),
            getGlobalStatistics(),
            _hspContainer,
            containers[i],
            getScoreMatrix(),
            _params,
            this
        ));
    }
    dispatcher->dispatchCommands (commands, 0);

    /** We merge the collected containers into our result. */
    alignContainer->merge (containers);

    /** We release the children containers. */
    for (size_t i=0; i<nbcpu; i++)  {  delete containers[i];  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlgorithmPlastn::update (dp::EventInfo* evt, dp::ISubject* subject)
{
	if (subject != this)
	{
        IterationStatusEvent* e1 = dynamic_cast<IterationStatusEvent*> (evt);
        if (e1 != 0)
        {
        	/** Since we have several successive passes, we rationalize the incoming notifications
        	 *  in order to look like we have only one phase.
        	 */
        	float currentPercent = (float)(e1->getCurrentIndex()) / (float)(e1->getTotalNumber());

            u_int64_t currentIndex = 100 * (currentPercent + _currentPass);
            u_int64_t totalNumber  = 100 * (_nbPasses);

        	notify (new IterationStatusEvent (ITER_ON_GOING, currentIndex, totalNumber, e1->getMessage(), currentIndex, totalNumber));
        }
	}
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
