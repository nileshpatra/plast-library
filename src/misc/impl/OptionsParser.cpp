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


#include <stdio.h>
#include <string.h>

#include <misc/impl/OptionsParser.hpp>
#include <designpattern/impl/Property.hpp>

using namespace std;
using namespace dp::impl;

#define DEBUG(a)  //printf a

/********************************************************************************/
namespace misc {
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
OptionsParser::OptionsParser ()  : _properties(0)
{
    _proceed=0;
    setProperties (new Properties());
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
OptionsParser::~OptionsParser ()
{
    setProperties (0);

    for (std::list<Option*>::iterator it = _options.begin(); it != _options.end(); it++)
    {
        (*it)->forget();
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
int OptionsParser::add (Option* option)
{
    if (option != 0)
    {
        option->use ();

        /* We add the option in the list. */
        _options.push_back (option);
    }

    return _options.size();
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
int OptionsParser::parse (int argc, char* argv[])
{
    /* Some initializations. */
    _argc=argc; _argv=argv; _currentArg=1;
    _proceed=1;
    _errors.clear();
    _warnings.clear();
    _seenOptions.clear();
    char* exclude;

    char* txt=0;
    while ( (txt = nextArg ()) != 0)
    {
        /* We look if is matches one of the recognized options. */
        Option* optionMatch = lookForOption (txt);
        DEBUG (("CheckOption::proceed:  txt='%s'  optionMatch=%p \n", txt, optionMatch));

        if (optionMatch != 0)
        {
            /* This is a recognized option. So we try to retrieve the args that should follow. */
            std::list<std::string> optionArgs;
            getOptionArgs (optionMatch, optionArgs);

            DEBUG (("CheckOption::proceed:  optionMatch.size=%ld  optionArgs.size=%ld\n",
                optionMatch->getNbArgs(),
                optionArgs.size()
            ));

            if (optionArgs.size() == optionMatch->getNbArgs())
            {
                /* We first look if this option has not already been met. */
                if (!optionMatch->canBeMultiple () && saw (optionMatch->getLabel()))
                {
                    char buffer[128];
                    snprintf (buffer, sizeof(buffer), "Option %s already seen, ignored...", optionMatch->getLabel().c_str());
                    _warnings.push_back (buffer);
                }
                else if ( (exclude = checkExcludingOptions (optionMatch)) != 0)
                {
                    char buffer[128];
                    snprintf (buffer, sizeof(buffer), "Option %s can't be used with option %s, ignored...",
                        optionMatch->getLabel().c_str(),
                        exclude
                    );
                    _errors.push_back (buffer);
                }
                else
                {
                    optionMatch->proceed (optionArgs);
                    _seenOptions.push_back (optionMatch);
                }
            }
        }
        else
        {
            /* This is an NOT a recognized option. We try to find an No_Option. */
            giveToNoOption (txt);
        }
    }

    /** We check mandatory options. */
    checkMandatoryOptions ();

    /* Now, we check that the accepted options are used with with include options. */
    checkIncludingOptions ();

    /** We may launch an exception if needed. */
    if (!_errors.empty())   {  throw OptionFailure (*this);  }

    /** We fill the properties. */
    buildProperties ();

    /* And we return the errors number. */
    return _errors.size();
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void OptionsParser::buildProperties ()
{
    for (std::list<Option*>::iterator it = _seenOptions.begin();  it != _seenOptions.end();  it++)
    {
        _properties->add (0, (*it)->getLabel(), (*it)->getParam());
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
char* OptionsParser::nextArg ()
{
    return (_currentArg > _argc ? (char*)0 : _argv[_currentArg++]);
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
Option* OptionsParser::lookForOption (char* txt)
{
    Option* result = 0;

    DEBUG (("CheckOption::lookForOption:  txt='%s'  _options.size=%ld  \n", txt, _options.size()));

    for (list<Option*>::iterator it = _options.begin(); !result &&  it != _options.end(); it++)
    {
        if ((*it)->getLabel().compare (txt) == 0)  {  result = *it; }
    }

    DEBUG (("CheckOption::lookForOption:  _options.size=%ld  => found=%d \n", _options.size(), result!=0));

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
void OptionsParser::getOptionArgs (const Option* option, std::list<std::string>& result)
{
    char* txt;
    int i=1;
    int n = option->getNbArgs();
    while ( (i<=n) && (txt=nextArg()) )
    {
        result.push_back (txt);
        i++;
    }

    if (i<=n)
    {
        char buffer [128];
        snprintf (buffer, sizeof(buffer), "To few arguments for the %s option...", option->getLabel().c_str());
        _errors.push_back (buffer);
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
void OptionsParser::displayErrors (FILE* fp)
{
    for (list<string>::iterator it = _errors.begin();  it != _errors.end();  it++)
    {
        fprintf (fp, "Error : %s\n", it->c_str());
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
void OptionsParser::displayWarnings (FILE* fp)
{
    for (list<string>::iterator it = _warnings.begin();  it != _warnings.end();  it++)
    {
        fprintf (fp, "Warning : %s\n", it->c_str());
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
void OptionsParser::displayHelp (FILE* fp)
{
    for (list<Option*>::iterator it = _options.begin();  it != _options.end();  it++)
    {
        if (!(*it)->getLabel().empty())
        {
            fprintf (fp, "\t%s :\t %s\n",  (*it)->getLabel().c_str(),  (*it)->getHelp().c_str());
        }
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
void OptionsParser::giveToNoOption (char* txt)
{
    Option* option = 0;

    for (list<Option*>::iterator it = _options.begin();  option==0  &&  it != _options.end();  it++)
    {
        if ((*it)->getLabel().empty())  {  option = (*it);  }
    }

    if (option != 0)
    {
        list<string> args;
        args.push_back (txt);
        option->proceed (args);
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
bool OptionsParser::saw (const std::string& txt)
{
    bool found = false;

    for (std::list<Option*>::iterator it = _seenOptions.begin();  !found && it != _seenOptions.end();  it++)
    {
        found = (*it)->getLabel().compare(txt) == 0;
    }

    DEBUG (("CheckOption::saw: txt='%s'  _seenOptions.size=%ld  => found=%d \n",
        txt.c_str(), _seenOptions.size(), found
    ));

    return found;
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
char* OptionsParser::checkExcludingOptions (const Option* option)
{
    if (option->getExclude().empty())
    {
        return (char*)0;
    }

    short found=0;

    for (list<Option*>::iterator it = _seenOptions.begin(); it != _seenOptions.end(); it++)
    {
        if (strstr ((*it)->getExclude().c_str(), option->getLabel().c_str()))
        {
            found=1;
            break;
        }
    }
    return (found ? (char*) option->getLabel().c_str() : (char*)0);
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void OptionsParser::checkIncludingOptions ()
{
    for (list<Option*>::iterator it = _seenOptions.begin(); it != _seenOptions.end(); it++)
    {
        string include = (*it)->getInclude ();
        if (! include.empty ())
        {
            short inner_found=0;

            for (list<Option*>::iterator itInner = _seenOptions.begin(); itInner != _seenOptions.end(); itInner++)
            {
                if (strstr (include.c_str(), (*itInner)->getLabel().c_str()))
                {
                    inner_found=1;
                    break;
                }
            }
            if (!inner_found)
            {
                char buffer[128];
                snprintf (buffer, sizeof(buffer),
                    "Option %s must be used with one of the following options : %s",
                    (*it)->getLabel().c_str(),
                    include.c_str()
                );

                _errors.push_back (buffer);
            }
        }
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
void OptionsParser::checkMandatoryOptions ()
{
    for (list<Option*>::iterator it = _options.begin(); it != _options.end(); it++)
    {
        bool found = false;

        for (list<Option*>::iterator itInner = _seenOptions.begin(); !found &&  itInner != _seenOptions.end(); itInner++)
        {
            found = (*it)->getLabel() == (*itInner)->getLabel();
        }

        if ((*it)->isMandatory() && !found)
        {
            char buffer[128];
            snprintf (buffer, sizeof(buffer), "Option %s is mandatory", (*it)->getLabel().c_str());
            _errors.push_back (buffer);
        }
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
const Option* OptionsParser::getSeenOption (const std::string& label)
{
    const Option* result = 0;

    for (list<Option*>::iterator it = _seenOptions.begin();  !result && it != _seenOptions.end(); it++)
    {
        if ((*it)->getLabel().compare (label) == 0)   { result = (*it); }
    }

    return result;
}

/********************************************************************************/
}}  /* end of namespaces. */
/********************************************************************************/