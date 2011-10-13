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

#include "DefaultAlgoEnvironment.hpp"

#include "DefaultAlgoConfig.hpp"

#include "AbstractAlgorithm.hpp"

#include "ICommand.hpp"

#include "FastaDatabaseQuickReader.hpp"

#include "PlastStrings.hpp"

#include "TokenizerIterator.hpp"

using namespace std;
using namespace dp;
using namespace types;
using namespace database;
using namespace algo;

#include <stdio.h>
#define DEBUG(a)  //printf a

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IConfiguration* DefaultEnvironment::createConfiguration (dp::IProperties* properties)
{
    IConfiguration* result = 0;

    result = new DefaultConfiguration (properties);

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
void DefaultEnvironment::run (dp::IProperties* properties)
{
    /** We create a configuration object for the provided program name (plastp, tplasn...) */
    IConfiguration* config = createConfiguration (properties);
    LOCAL (config);

    /** We retrieve the type of algorithm (may be not set). */
    IProperty* algoProp = properties->getProperty (STR_OPTION_ALGO_TYPE);
    bool inferType = (algoProp == 0);

    /** We create a visitor for visiting the resulting alignments. Note that we use only one visitor even if
     *  we have to run several algorithm; in such a case, the results are 'concatenated' by the same visitor. */
    AlignmentResultVisitor* resultVisitor = config->createResultVisitor ();
    LOCAL (resultVisitor);

    u_int64_t  maxblocksize = 20*1000*1000;
    IProperty* maxBlockProp = properties->getProperty(STR_OPTION_MAX_DATABASE_SIZE);
    if (maxBlockProp != 0)  {  maxblocksize = maxBlockProp->getInt();  }

    /** We need to read the subject database to get its data size and the number of sequences.
     *  This information will be used for computing cutoffs for the query sequences. */
    IDatabaseQuickReader* quickSubjectDbReader = 0;
    IProperty* subjectProp = properties->getProperty (STR_OPTION_SUBJECT_URI);
    if (subjectProp == 0)  {  subjectProp = properties->add (0, STR_OPTION_SUBJECT_URI, "foo"); }
    if (subjectProp != 0)
    {
        quickSubjectDbReader = new FastaDatabaseQuickReader (subjectProp->value, inferType);
        quickSubjectDbReader->read (maxblocksize);
    }
    LOCAL (quickSubjectDbReader);

    /** We need to read the subject database to get its data size and the number of sequences. */
    IDatabaseQuickReader* quickQueryDbReader = 0;
    IProperty* queryProp = properties->getProperty (STR_OPTION_QUERY_URI);
    if (queryProp == 0)  {  queryProp = properties->add (0, STR_OPTION_QUERY_URI, "foo"); }
    if (queryProp != 0)
    {
        quickQueryDbReader = new FastaDatabaseQuickReader (queryProp->value, inferType);
        quickQueryDbReader->read (maxblocksize);
    }
    LOCAL (quickQueryDbReader);

    /** We may have to infer the kind of algorithm (plastp, plastx...) if no one is provided. */
    if (algoProp == 0)
    {
        /** Shortcuts. */
        IDatabaseQuickReader::DatabaseKind_e subjectKind = quickSubjectDbReader->getKind();
        IDatabaseQuickReader::DatabaseKind_e queryKind   = quickQueryDbReader->getKind();

        if (subjectKind == IDatabaseQuickReader::ENUM_AMINO_ACID &&  queryKind == IDatabaseQuickReader::ENUM_AMINO_ACID)
        {
            properties->add (0, STR_OPTION_ALGO_TYPE, "plastp");
        }
        else if (subjectKind == IDatabaseQuickReader::ENUM_AMINO_ACID &&  queryKind == IDatabaseQuickReader::ENUM_NUCLOTID)
        {
            properties->add (0, STR_OPTION_ALGO_TYPE, "plastx");
        }
        else if (subjectKind == IDatabaseQuickReader::ENUM_NUCLOTID &&  queryKind == IDatabaseQuickReader::ENUM_AMINO_ACID)
        {
            properties->add (0, STR_OPTION_ALGO_TYPE, "tplastn");
        }
        else
        {
            /** We should not be there. Should throw an exception ?*/
        }
    }

    /** We build a list of uri for subject/query databases. */
    vector<pair<Range,Range> > uriList = buildUri (quickSubjectDbReader, quickQueryDbReader);

    /** We build a list of Parameters. We will launch the algorithm for each item of this list. */
    vector<IParameters*> parametersList = createParametersList (config, properties, uriList);

    /** We iterate each parameters. */
    for (size_t i=0; i<parametersList.size(); i++)
    {
        list<ICommand*> algorithms;

        /** We send a notification to potential listeners. */
        this->notify (new AlgorithmConfigurationEvent (i, parametersList.size()));

        /** We create an Algorithm instance. */
        IAlgorithm* algo = this->createAlgorithm (config, quickSubjectDbReader, parametersList[i], resultVisitor);
        if (algo == 0)  { continue; }

        /** We can register ourself to be notified by execution events. */
        algo->addObserver (this);

        /** We add this instance to the algorithms list. */
        algorithms.push_back (algo);

        /** We create a commands dispatcher. */
        ICommandDispatcher* dispatcher = config->createDispatcher ();
        LOCAL (dispatcher);

        /** We execute the algorithms through the dispatcher. */
        dispatcher->dispatchCommands (algorithms, 0);
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IAlgorithm* DefaultEnvironment::createAlgorithm (
    IConfiguration*         config,
    IDatabaseQuickReader*   reader,
    IParameters*            params,
    AlignmentResultVisitor* resultVisitor
)
{
    IAlgorithm* result = 0;

    switch (params->algoKind)
    {
        case ENUM_PLASTP:
            result = new AlgorithmPlastp (config, reader, params, resultVisitor);
            break;

        case ENUM_TPLASTN:
            result = new AlgorithmTplastn (config, reader, params, resultVisitor);
            break;

        case ENUM_PLASTX:
            result = new AlgorithmPlastx (config, reader, params, resultVisitor);
            break;

        case ENUM_TPLASTX:
        default:
            break;
    }

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
void DefaultEnvironment::update (dp::EventInfo* evt, dp::ISubject* subject)
{
    /** We just forward the event. */
    if (this != subject) {  this->notify (evt);  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
vector<pair<Range,Range> > DefaultEnvironment::buildUri (
    IDatabaseQuickReader* subjectReader,
    IDatabaseQuickReader* queryReader
)
{
    vector<pair<Range,Range> > result;

    /** Shortcuts. */
    vector<u_int64_t>& subjectOffsets = subjectReader->getOffsets();
    vector<u_int64_t>& queryOffsets   = queryReader->getOffsets();

    for (size_t i=0; i<subjectOffsets.size()-1; i++)
    {
        for (size_t j=0; j<queryOffsets.size()-1; j++)
        {
            Range s (subjectOffsets[i], subjectOffsets[i+1]-1);
            Range q (queryOffsets[j],   queryOffsets  [j+1]-1);

            result.push_back (pair<Range,Range> (s,q));
        }
    }

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
vector<IParameters*> DefaultEnvironment::createParametersList (
    IConfiguration* config,
    dp::IProperties* properties,
    vector <pair <Range,Range> >& uri
)
{
    vector<IParameters*> result;

    /** We look for the program type property. */
    IProperty* progType = properties->getProperty (STR_OPTION_ALGO_TYPE);

    if (progType != 0)
    {
        DEBUG (("DefaultEnvironment::createParametersList: algotype='%s'\n", progType->getString()));

        IProperty* prop = 0;

        for (size_t i=0; i<uri.size(); i++)
        {
            /** We create default parameters. */
            IParameters* params = config->createDefaultParameters (progType->value);

            if ( (prop = properties->getProperty (STR_OPTION_SUBJECT_URI)) != 0)            {  params->subjectUri           = prop->value; }
            if ( (prop = properties->getProperty (STR_OPTION_QUERY_URI)) != 0)              {  params->queryUri             = prop->value; }

            if ( (prop = properties->getProperty (STR_OPTION_OUTPUT_FILE)) != 0)            {  params->outputfile           = prop->value; }
            if ( (prop = properties->getProperty (STR_OPTION_FILTER_QUERY)) != 0)           {  params->filterQuery          = prop->value.compare ("T") == 0;  }

            if ( (prop = properties->getProperty (STR_OPTION_UNGAP_NEIGHBOUR_LENGTH)) != 0) {  params->ungapNeighbourLength = atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_UNGAP_SCORE_THRESHOLD)) != 0)  {  params->ungapScoreThreshold  = atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_SMALLGAP_BAND_WITH)) != 0)     {  params->smallGapBandWidth    = atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_SMALLGAP_THRESHOLD)) != 0)     {  params->smallGapThreshold    = atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_OPEN_GAP_COST)) != 0)          {  params->openGapCost          = atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_EXTEND_GAP_COST)) != 0)        {  params->extendGapCost        = atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_EVALUE)) != 0)                 {  params->evalue               = atof (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_X_DROPOFF_GAPPED)) != 0)       {  params->XdroppofGap          = atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_X_DROPOFF_FINAL)) != 0)        {  params->finalXdroppofGap     = atoi (prop->value.c_str()); }

            if ( (prop = properties->getProperty (STR_OPTION_STRANDS_LIST)) != 0)
            {
                TokenizerIterator it (prop->getString(), ",");
                for (it.first(); !it.isDone(); it.next())
                {
                    ReadingFrame_e val = (ReadingFrame_e) (atoi(it.currentItem()) - 1);
                    params->strands.push_back (val);
                }
            }

            /** We set databases ranges. */
            params->subjectRange = uri[i].first;
            params->queryRange   = uri[i].second;

            result.push_back (params);
        }
    }

    return result;
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/