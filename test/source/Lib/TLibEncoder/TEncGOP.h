/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.  
 *
 * Copyright (c) 2010-2011, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     TEncGOP.h
    \brief    GOP encoder class (header)
*/

#ifndef __TENCGOP__
#define __TENCGOP__

#include <list>

#include <stdlib.h>

#include "TLibCommon/TComList.h"
#include "TLibCommon/TComPic.h"
#include "TLibCommon/TComBitCounter.h"
#include "TLibCommon/TComLoopFilter.h"
#include "TLibCommon/AccessUnit.h"
#include "TEncAdaptiveLoopFilter.h"
#include "TEncSampleAdaptiveOffset.h"
#include "TEncSlice.h"
#include "TEncEntropy.h"
#include "TEncCavlc.h"
#include "TEncSbac.h"

#include "TEncAnalyze.h"

#if RVM_VCEGAM10
#include <vector>
#endif

//! \ingroup TLibEncoder
//! \{

class TEncTop;

// ====================================================================================================================
// Class definition
// ====================================================================================================================

/// GOP encoder class
class TEncGOP
{
private:
  //  Data
  Int                     m_iHrchDepth;
  Int                     m_iGopSize;
  Int                     m_iRateGopSize;
  Int                     m_iNumPicCoded;
  Bool                    m_bFirst;
  
  //  Access channel
  TEncTop*                m_pcEncTop;
  TEncCfg*                m_pcCfg;
  TEncSlice*              m_pcSliceEncoder;
  TComList<TComPic*>*     m_pcListPic;
  
  TEncEntropy*            m_pcEntropyCoder;
  TEncCavlc*              m_pcCavlcCoder;
  TEncSbac*               m_pcSbacCoder;
  TEncBinCABAC*           m_pcBinCABAC;
  TComLoopFilter*         m_pcLoopFilter;
  
  // Adaptive Loop filter
  TEncAdaptiveLoopFilter* m_pcAdaptiveLoopFilter;
  //--Adaptive Loop filter
#if SAO
  TEncSampleAdaptiveOffset*  m_pcSAO;
#endif
  TComBitCounter*         m_pcBitCounter;
  
  // indicate sequence first
  Bool                    m_bSeqFirst;
  
  // clean decoding refresh
  Bool                    m_bRefreshPending;
  UInt                    m_uiPOCCDR;
  UInt*                   m_uiStoredStartCUAddrForEncodingSlice;
  UInt*                   m_uiStoredStartCUAddrForEncodingEntropySlice;

// #if MTK_NONCROSS_INLOOP_FILTER
//   UInt                    m_uiILSliceCount;
//   UInt*                   m_puiILSliceStartLCU;
//   UInt*                   m_puiILSliceEndLCU;
// #endif

#if RVM_VCEGAM10
  std::vector<Int> m_vRVM_RP;
#endif

public:
  TEncGOP();
  virtual ~TEncGOP();
  
  Void  create      ( Int iWidth, Int iHeight, UInt iMaxCUWidth, UInt iMaxCUHeight );
  Void  destroy     ();
  
  Void  init        ( TEncTop* pcTEncTop );
  Void  compressGOP ( Int iPOCLast, Int iNumPicRcvd, TComList<TComPic*>& rcListPic, TComList<TComPicYuv*>& rcListPicYuvRec, std::list<AccessUnit>& accessUnitsInGOP );
#if TILES_DECODER
  Void xWriteTileLocationToSliceHeader (OutputNALUnit& rNalu, TComOutputBitstream*& rpcBitstreamRedirect, TComSlice*& rpcSlice);
#endif

  
  Int   getGOPSize()          { return  m_iGopSize;  }
  Int   getRateGOPSize()      { return  m_iRateGopSize;  }
  Int   isHierarchicalB()     { return  m_pcCfg->getHierarchicalCoding();  }
  Int   getHrchDepth()        { return  m_iHrchDepth; }
  
  TComList<TComPic*>*   getListPic()      { return m_pcListPic; }
  
  Void  printOutSummary      ( UInt uiNumAllPicCoded );
  Void  preLoopFilterPicAll  ( TComPic* pcPic, UInt64& ruiDist, UInt64& ruiBits );
  
  TEncSlice*  getSliceEncoder()   { return m_pcSliceEncoder; }

#if F747_APS
  Void freeAPS     (TComAPS* pAPS, TComSPS* pSPS);
  Void allocAPS    (TComAPS* pAPS, TComSPS* pSPS);
protected:
  Void encodeAPS   (TComAPS* pcAPS, TComOutputBitstream& APSbs, TComSlice* pcSlice);            //!< encode APS syntax elements
  Void assignNewAPS(TComAPS& cAPS, Int apsID, std::vector<TComAPS>& vAPS, TComSlice* pcSlice);  //!< Assign APS object into APS container
#endif
  

protected:
  Void  xInitGOP          ( Int iPOC, Int iNumPicRcvd, TComList<TComPic*>& rcListPic, TComList<TComPicYuv*>& rcListPicYuvRecOut );
  Void  xGetBuffer        ( TComList<TComPic*>& rcListPic, TComList<TComPicYuv*>& rcListPicYuvRecOut, Int iNumPicRcvd, Int iTimeOffset, TComPic*& rpcPic, TComPicYuv*& rpcPicYuvRecOut, UInt uiPOCCurr );
  
  NalUnitType getNalUnitType( UInt uiPOCCurr );

  Void  xCalculateAddPSNR ( TComPic* pcPic, TComPicYuv* pcPicD, const AccessUnit&, Double dEncTime );
  
  UInt64 xFindDistortionFrame (TComPicYuv* pcPic0, TComPicYuv* pcPic1);

#if RVM_VCEGAM10
  Double xCalculateRVM();
#endif
};// END CLASS DEFINITION TEncGOP

// ====================================================================================================================
// Enumeration
// ====================================================================================================================
#if E045_SLICE_COMMON_INFO_SHARING
enum PROCESSING_STATE
{
  EXECUTE_INLOOPFILTER,
#if F747_APS
  ENCODE_APS,
#else
  ENCODE_PPS,
#endif
  ENCODE_SLICE
};
#endif

//! \}

#endif // __TENCGOP__

