#include <algo/core/impl/IterativePlastnConfig.hpp>
#include <algo/core/api/IAlgoEnvironment.hpp>
#include <algo/core/api/IAlgoParameters.hpp>
#include <alignment/visitors/impl/FoundQuerySequencesGeneratingVisitor.hpp>

#include <set>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {

/********************************************************************************/

IterativePlastnConfig::IterativePlastnConfig(IEnvironment* environment,
        dp::IProperties* properties,
        std::set<u_int64_t>& blacklist)
    : PlastnConfiguration(environment, properties), _blacklist(blacklist)
{
}

IterativePlastnConfig::~IterativePlastnConfig() { }

IParameters* IterativePlastnConfig::createDefaultParameters (const std::string& algoName)
{
    IParameters* params = PlastnConfiguration::createDefaultParameters(algoName);

    updateKmersBitsetPath(params);

    params->querySequencesBlacklist = &_blacklist;

    return params;
}

void IterativePlastnConfig::updateKmersBitsetPath(IParameters* params)
{
    dp::IProperty* kmersToSelect= _properties->getProperty(STR_OPTION_KMERS_TO_SELECT);

    if (kmersToSelect == NULL) {
        return;
    }

    long kmersToSelectCount = kmersToSelect->getInt();
    if (kmersToSelectCount == 0) {
        params->kmersBitsetPath = "";

        return;
    }

    const size_t bufferMaxSize = 128;
    char buffer[bufferMaxSize];
    snprintf(buffer, bufferMaxSize, "/tmp/seed_bacteria_test_d_%ld.bin", kmersToSelect->getInt());

    params->kmersBitsetPath = buffer;
}

} // namespace impl
} // namespace core
} // namespace algo
