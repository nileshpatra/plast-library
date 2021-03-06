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

/** \file DefaultAlgoConfig.hpp
 *  \brief Default implementation of the IConfiguration interface
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _DEFAULT_ALGO_CONFIG_HPP_
#define _DEFAULT_ALGO_CONFIG_HPP_

/********************************************************************************/

#include <designpattern/api/IProperty.hpp>

#include <algo/core/api/IAlgoConfig.hpp>
#include <algo/core/api/IAlgoEnvironment.hpp>

#include <alignment/core/api/IAlignmentContainer.hpp>
#include <alignment/filter/api/IAlignmentFilter.hpp>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

/** \brief Default implementation of the IConfiguration interface
 *
 * This abstract factory implementation provides default choices for types of created instances.
 *
 * Note that a IProperties instance is given to the constructor. This instance is used for
 * customizing the types of the instances to be created.
 */
class DefaultConfiguration : public IConfiguration
{
public:

    /** Constructor.
     * \param[in] properties : properties for customizing instances creation.
     */
    DefaultConfiguration (IEnvironment* environment, dp::IProperties* properties);

    /** Destructor. */
    virtual ~DefaultConfiguration ();

    /** \copydoc IConfiguration::createDefaultQuickReader */
    database::IDatabaseQuickReader* createDefaultQuickReader (const std::string& uri, bool shouldInferType);

    /** \copydoc IConfiguration::createDefaultParameters */
    IParameters* createDefaultParameters (const std::string& algoName);

    /** \copydoc IConfiguration::createIndexationDispatcher */
    dp::ICommandDispatcher* createIndexationDispatcher ();

    /** \copydoc IConfiguration::createDispatcher */
    dp::ICommandDispatcher* createDispatcher ();

    /** \copydoc IConfiguration::createTimeInfo */
    os::impl::TimeInfo* createTimeInfo ();

    /** \copydoc IConfiguration::createDatabaseProvider */
    algo::core::IDatabasesProvider* createDatabaseProvider ();

    /** \copydoc IConfiguration::createQueryInformation */
    statistics::IQueryInformation* createQueryInformation (
        statistics::IGlobalParameters*  globalStats,
        algo::core::IParameters*        parameters,
        database::ISequenceDatabase*    queryDb,
        size_t                          subjectSize,
        size_t                          subjectNbSequences
    );

    /** \copydoc IConfiguration::createGlobalParameters */
    statistics::IGlobalParameters*  createGlobalParameters (algo::core::IParameters* params, size_t subjectDbLength);

    /** \copydoc IConfiguration::createSeedModel */
    seed::ISeedModel* createSeedModel (
        misc::SeedModelKind_e modelKind,
        size_t span,
        const std::vector<std::string>& subseedStrings
    );

    /** \copydoc IConfiguration::createIndexator */
    IIndexator*  createIndexator (
        seed::ISeedModel*        seedsModel,
        algo::core::IParameters* params,
        bool&                    isRunning
    );

    /** \copydoc IConfiguration::createScoreMatrix */
    IScoreMatrix* createScoreMatrix (
        misc::ScoreMatrixKind_e kind,
        database::Encoding encoding,
        int reward,
        int penalty
    );

    /** \copydoc IConfiguration::createUngapHitIterator */
    algo::hits::IHitIterator* createUngapHitIterator (
        algo::hits::IHitIterator*               source,
        seed::ISeedModel*                       model,
        algo::core::IScoreMatrix*               matrix,
        algo::core::IParameters*                params,
        alignment::core::IAlignmentContainer*   ungapResult,
        bool&                                   isRunning
    );

    /** \copydoc IConfiguration::createUngapExtendHitIterator */
    algo::hits::IHitIterator* createUngapExtendHitIterator (
        algo::hits::IHitIterator*               source,
        seed::ISeedModel*                       model,
        algo::core::IScoreMatrix*               matrix,
        algo::core::IParameters*                params,
        alignment::core::IAlignmentContainer*   ungapResult,
        statistics::IGlobalParameters*          globalStats,
        statistics::IQueryInformation*          queryInfo,
        bool&                                   isRunning
    );

    /** \copydoc IConfiguration::createSmallGapHitIterator */
    algo::hits::IHitIterator* createSmallGapHitIterator (
        algo::hits::IHitIterator*               source,
        seed::ISeedModel*                       model,
        algo::core::IScoreMatrix*               matrix,
        algo::core::IParameters*                params,
        alignment::core::IAlignmentContainer*   ungapResult,
        alignment::core::IAlignmentContainer*   alignmentResult,
        bool&                                   isRunning
    );

    /** \copydoc IConfiguration::createFullGapHitIterator */
    algo::hits::IHitIterator* createFullGapHitIterator  (
        algo::hits::IHitIterator*               source,
        seed::ISeedModel*                       model,
        algo::core::IScoreMatrix*               matrix,
        algo::core::IParameters*                params,
        statistics::IQueryInformation*          queryInfo,
        statistics::IGlobalParameters*          globalStats,
        alignment::core::IAlignmentContainer*   ungapResult,
        alignment::core::IAlignmentContainer*   alignmentResult,
        bool&                                   isRunning
    );

    /** \copydoc IConfiguration::createCompositionHitIterator */
    algo::hits::IHitIterator* createCompositionHitIterator  (
        algo::hits::IHitIterator*               source,
        seed::ISeedModel*                       model,
        algo::core::IScoreMatrix*               matrix,
        algo::core::IParameters*                params,
        statistics::IQueryInformation*          queryInfo,
        statistics::IGlobalParameters*          globalStats,
        alignment::core::IAlignmentContainer*   ungapResult,
        alignment::core::IAlignmentContainer*   alignmentResult,
        bool&                                   isRunning
    );

    /** \copydoc IConfiguration::createGapAlignmentResult */
    alignment::core::IAlignmentContainer* createGapAlignmentResult  ();

    /** \copydoc IConfiguration::createUnapAlignmentResult */
    alignment::core::IAlignmentContainer* createUnapAlignmentResult (size_t querySize);

    /** \copydoc IConfiguration::createAlignmentSplitter */
    alignment::tools::IAlignmentSplitter* createAlignmentSplitter (
        algo::core::IScoreMatrix* scoreMatrix,
        int openGapCost,
        int extendGapCost
    );

    /** \copydoc IConfiguration::createSemiGapAlign */
    alignment::tools::ISemiGapAlign* createSemiGapAlign (
        algo::core::IScoreMatrix* scoreMatrix,
        int openGapCost,
        int extendGapCost,
        int Xdropoff
    );

protected:

    IEnvironment* _environment;

    dp::IProperties* _properties;
    void setProperties (dp::IProperties* properties)  { SP_SETATTR(properties);  }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _DEFAULT_ALGO_CONFIG_HPP_ */
