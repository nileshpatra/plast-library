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

#include <os/impl/DefaultOsFactory.hpp>

#include <misc/api/macros.hpp>

#include <alignment/tools/impl/AlignmentSplitter.hpp>

#include <iostream>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace database;
using namespace dp;
using namespace os;
using namespace os::impl;
using namespace algo::core;
using namespace alignment::core;

//#define VERBOSE

/********************************************************************************/
namespace alignment {
namespace tools     {
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
AlignmentSplitter::AlignmentSplitter (IScoreMatrix* scoreMatrix, int openGapCost, int extendGapCost)
 : _scoreMatrix(0),
   _openGapCost(openGapCost), _extendGapCost(extendGapCost),
   _matrix_H (0), _matrix_E(0), _matrix_F(0),
   _DefaultAlignSize(1000), _MaxAlignSize(6000)
{
    setScoreMatrix (scoreMatrix);

    _matrix_H = newMatrix (_DefaultAlignSize, _DefaultAlignSize);
    _matrix_E = newMatrix (_DefaultAlignSize, _DefaultAlignSize);
    _matrix_F = newMatrix (_DefaultAlignSize, _DefaultAlignSize);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlignmentSplitter::~AlignmentSplitter ()
{
    setScoreMatrix (0);

    freeMatrix (&_matrix_H);
    freeMatrix (&_matrix_E);
    freeMatrix (&_matrix_F);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
size_t AlignmentSplitter::splitAlign (
    const database::LETTER* sbjSeq,
    const database::LETTER* qrySeq,
    const misc::Range32&    sbjRange,
    const misc::Range32&    qryRange,
    SplitOutput& output
)
{
    int s,x,y,i,j,d,lg,nbg,nbi,qryLen,subLen;
    int  j1,j2;

    int jj = qryRange.begin - sbjRange.begin;
    jj = jj - (qryRange.end - sbjRange.end);

    int delta = (jj>=0 ? (jj + 2) : (-jj + 2));

    qryLen = qryRange.getLength();
    subLen = sbjRange.getLength();

    /** A little check. */
    if (qryLen >= _MaxAlignSize  ||  subLen >= _MaxAlignSize)
    {
        /** Alignments are too big for computing information. */
        return 0;
    }

    /** Shortcuts. */
    int8_t** MATRIX = _scoreMatrix->getMatrix();

    const char* qryStr = qrySeq + qryRange.begin;
    const char* subStr = sbjSeq + sbjRange.begin;

#if 0
    const LETTER* convert = EncodingManager::singleton().getEncodingConversion(SUBSEED, ASCII);
    for (int ii=0; ii<qryLen; ii++)  { printf ("%c", convert[qryStr[ii]]);  }  printf("\n");
    for (int ii=0; ii<subLen; ii++)  { printf ("%c", convert[subStr[ii]]);  }  printf("\n");
#endif

    int opengap   = _openGapCost;
    int extendgap = _extendGapCost;

    /** We configure the matrix for the dynamic programming. */
    int16_t** H = 0;
    int16_t** E = 0;
    int16_t** F = 0;

    if (qryLen >= _DefaultAlignSize ||  subLen >= _DefaultAlignSize)
    {
        /** We release the currently allocated matrixes. */
        freeMatrix (&_matrix_H);
        freeMatrix (&_matrix_E);
        freeMatrix (&_matrix_F);

        /** We set the default to the max size. */
        _DefaultAlignSize = _MaxAlignSize;

        /** We reallocate our matrixes. */
        _matrix_H = newMatrix (_DefaultAlignSize, _DefaultAlignSize);
        _matrix_E = newMatrix (_DefaultAlignSize, _DefaultAlignSize);
        _matrix_F = newMatrix (_DefaultAlignSize, _DefaultAlignSize);
    }

    /** We use a shortcut. */
    H = _matrix_H;
    E = _matrix_E;
    F = _matrix_F;

    for (i=0; i<=delta+2; i++)  {    H[0][i]=0; F[0][i]=0;  }

    for (i=1; i<=qryLen; i++)
    {
        j1 = MAX (1,      i-delta);
        j2 = MIN (subLen, i+delta);

        /** Shortcuts (and optimization) */
        int16_t* H0 = H[i];
        int16_t* H1 = H[i-1];
        int16_t* E0 = E[i];
        int16_t* F0 = F[i];
        int16_t* F1 = F[i-1];

        int8_t* matrixrow = MATRIX [(int)qryStr[i-1]];

        H0 [j1-1] = -1000;
        E0 [j1-1] = -1000;
        H1 [j2]   = -1000;
        F1 [j2]   = -1000;

        /** We use some temporary variables in order to avoid some memory access
         *  (which is time costly) and can't be easily optimized by compiler.
         */
        int16_t e0 = E0 [j1-1];
        int16_t h0 = H0 [j1-1];
        int16_t f0 = 0;

        for (j=j1; j<=j2; j++)
        {
            E0[j] = e0 = MAX (h0    - opengap, e0    - extendgap);
            F0[j] = f0 = MAX (H1[j] - opengap, F1[j] - extendgap);

            d = H1[j-1] + matrixrow [(int)subStr[j-1]];

            H0[j] = h0 = MAX (MAX (e0,f0), d);
        }
    }

    i=qryLen;  j=subLen;  x=0;  y=0;
    lg=0; nbg=0; nbi=0;

    if (output.splittab)  {  (output.splittab)[y++] = qryLen - 1;  }
    if (output.splittab)  {  (output.splittab)[y++] = subLen - 1;  }

    char qryLocal [10000];
    char subLocal [10000];
    int status = 0;

    /** We reset some alignments fields to be completed. */
    output.identity = 0;
    output.positive = 0;
    output.nbGapQry = 0;
    output.nbGapSbj = 0;
    output.nbMis    = 0;

    while ( (i>0) && (j>0) )
    {
        s = MATRIX [(int)qryStr[i-1]] [(int)subStr[j-1]];

        if (s > 0)  { output.positive++; }

        if ((H[i-1][j-1] + s) >= MAX (E[i][j],F[i][j]))
        {
            qryLocal[x] = qryStr[i-1];
            subLocal[x] = subStr[j-1];

            i--;
            j--;
            x++;

            status = 0;

            if (lg == 1)
            {
                if (output.splittab)  {  (output.splittab)[y++]=i;  }
                if (output.splittab)  {  (output.splittab)[y++]=j;  }
            }

            lg=0;
        }
        else
        {
            if (E[i][j] > F[i][j])
            {
                qryLocal[x] = CODE_DASH;
                subLocal[x] = subStr[j-1];
                x++;

                if (lg==0)
                {
                    if (output.splittab)  {  (output.splittab)[y++]=i;  }
                    if (output.splittab)  {  (output.splittab)[y++]=j;  }
                    nbg++;
                    lg=1;
                }
                j--;

                if  (status ==0)  {  output.nbGapQry ++;  }
            }

            else
            {
                qryLocal[x] = qryStr[i-1];
                subLocal[x] = CODE_DASH;
                x++;

                if (lg==0)
                {
                    if (output.splittab)  {  (output.splittab)[y++]=i;  }
                    if (output.splittab)  {  (output.splittab)[y++]=j;  }
                    nbg++;
                    lg=1;
                }
                i--;

                if  (status ==0)  {  output.nbGapSbj ++;  }
            }

            status = 1;
        }

    } /* end of while ( (i>0) && (j>0) ) */

    for (i=x-1; i>=0; i--)
    {
        /** Shortcuts (and optimization). */
        char l1 = qryLocal[i];
        char l2 = subLocal[i];

#if 1
             if (l1==l2         || (l1==CODE_X || l2==CODE_X))       {  output.identity ++;  }
        else if (l1!=CODE_DASH  &&  l2!=CODE_DASH)    {  output.nbMis++;      }
#else
             if (l1==l2         &&  l1!=CODE_X)       {  output.identity ++;  }
        else if (l1!=CODE_DASH  &&  l2!=CODE_DASH)    {  output.nbMis++;      }
#endif
    }

    if (output.splittab)  {  (output.splittab)[y++]=0;  }
    if (output.splittab)  {  (output.splittab)[y++]=0;  }

    output.alignSize = x;

    if (output.subjectAlign != 0)  { memcpy (output.subjectAlign, subLocal, output.alignSize); }
    if (output.queryAlign   != 0)  { memcpy (output.queryAlign,   qryLocal, output.alignSize); }

#if 0
    printf ("\nALIGN: nbGapQry=%d   nbGapSbj=%d   \n", output.nbGapQry, output.nbGapSbj);
    const LETTER* convert = EncodingManager::singleton().getEncodingConversion(SUBSEED, ASCII);
    for (u_int32_t ii=0; ii<x; ii++)  { printf ("%c", convert[(int)qryLocal[ii]]);  }  printf("\n");
    for (u_int32_t ii=0; ii<x; ii++)  { printf ("%c", (qryLocal[ii]==subLocal[ii] && qryLocal[ii] != CODE_X ? '|' : ' '));  }  printf("\n");
    for (u_int32_t ii=0; ii<x; ii++)  { printf ("%c", convert[(int)subLocal[ii]]);  }  printf("\n");
#endif

    /** We return the result. */
    return y;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
size_t AlignmentSplitter::splitAlign (Alignment& align,  SplitOutput& output)
{
    size_t result = 0;

    result = splitAlign (
        align.getSequence(Alignment::SUBJECT)->data.letters.data,
        align.getSequence(Alignment::QUERY)->data.letters.data,
        align.getRange(Alignment::SUBJECT),
        align.getRange(Alignment::QUERY),
        output
    );

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
int16_t** AlignmentSplitter::newMatrix (int nrows, int ncols)
{
    int i;             /* iteration index */
    int16_t ** mat;      /* the new matrix */

    mat = (int16_t **) DefaultFactory::memory().calloc(nrows, sizeof(int16_t *));
    if (mat != NULL)
    {
        mat[0] = (int16_t *) DefaultFactory::memory().malloc((size_t) nrows * (size_t) ncols * sizeof(int16_t));
        if (mat[0] != NULL)
        {
            for (i = 1;  i < nrows;  i++) {  mat[i] = &mat[0][i * ncols];  }
        }
        else
        {
            DefaultFactory::memory().free(mat);
            mat = NULL;
        }
    }

    return mat;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlignmentSplitter::freeMatrix (int16_t *** mat)
{
    if(*mat != NULL)
    {
        DefaultFactory::memory().free((*mat)[0]);
        DefaultFactory::memory().free(*mat);
    }
    *mat = NULL;
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
