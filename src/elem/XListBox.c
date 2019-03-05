// =======================================================================
// GUIslice library (extensions)
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// =======================================================================
//
// The MIT License
//
// Copyright 2016-2019 Calvin Hass
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =======================================================================
/// \file GUIslice_ex.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XListBox.h"

#include <stdio.h>

#if (GSLC_USE_PROGMEM)
    #include <avr/pgmspace.h>
#endif

// ----------------------------------------------------------------------------
// Error Messages
// ----------------------------------------------------------------------------

extern const char GSLC_PMEM ERRSTR_NULL[];
extern const char GSLC_PMEM ERRSTR_PXD_NULL[];


// ----------------------------------------------------------------------------
// Extended element definitions
// ----------------------------------------------------------------------------
//
// - This file extends the core GUIslice functionality with
//   additional widget types
// - After adding any widgets to GUIslice_ex, a unique
//   enumeration (GSLC_TYPEX_*) should be added to "GUIslice.h"
//
//   TODO: Consider whether we should remove the need to update
//         these enumerations in "GUIslice.h"; we could instead
//         define a single "GSLC_TYPEX" in GUIslice.h but then
//         allow "GUIslice_ex.h" to create a new set of unique
//         enumerations. This way extended elements could be created
//         in GUIslice_ex and no changes at all would be required
//         in GUIslice.

// ----------------------------------------------------------------------------


// ============================================================================
// Extended Element: ListBox
// - A ListBox control
// ============================================================================

// The following implementation could be greatly simplified
// but left with an explicit search in case we want more
// flexibility in how the item strings are defined.
int16_t gslc_ElemXListBoxCalcItemCnt(const char* pStrItems)
{
  if (pStrItems == NULL) {
    return 0;
  }
  const char* pCh = pStrItems;
  int16_t nItemCnt = 0;
  if (*pCh == 0) {
    return nItemCnt;
  }
  for (nItemCnt=1; *pCh != 0; ++pCh) {
    if (*pCh == '|') {
      nItemCnt++;
    }
  }
  return nItemCnt;
}

bool gslc_ElemXListBoxGetItem(const char* pStrItems,int16_t nItemSel,char* pStrItem,int8_t nStrItemLen)
{
  if (pStrItems == NULL) {
    return false;
  }
  char chCur;
  int8_t nStrPos = 0;
  int16_t nItemCnt = 0;
  bool bDone = false;
  bool bFound = false;
  int8_t nItemLen = 0;
  if (nItemSel < 0) {
    bDone = true;
  }
  if (pStrItems[0] == 0) {
    bDone = true;
  }
  while (!bDone) {
    chCur = pStrItems[nStrPos++];

    if (nItemCnt == nItemSel) {
      bFound = true;
    }

    // FIXME: Handle nStrItemLen vs nItemLen

    if (nItemCnt == nItemSel) {
      if (chCur == 0) {
        // Null terminator. Copy it over
        pStrItem[nItemLen++] = 0;
      } else if (chCur == '|') {
        // Reached end of searched entry, so don't copy separator
        // Instead, just insert terminator
        pStrItem[nItemLen++] = 0;
        bDone = true;
      } else {
        // Inside searched entry, so copy over text
        pStrItem[nItemLen++] = chCur;
      }
    } else {
      // Not on searched entry, so skip
      if (chCur == '|') {
        nItemCnt++;
      }
    }

    if (chCur == 0) {
      bDone = true;
    }

  }
  return bFound;
}


// Create a ListBox element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw and touch
gslc_tsElemRef* gslc_ElemXListBoxCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXListBox* pXData,gslc_tsRect rElem,int16_t nFontId,const char* pStrItems,int16_t nItemDefault)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXListBoxCreate";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_LISTBOX,rElem,NULL,0,nFontId);
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;

  sElem.nGroup            = GSLC_GROUP_ID_NONE;
  pXData->pStrItems       = pStrItems;
  pXData->nItemCnt        = gslc_ElemXListBoxCalcItemCnt(pStrItems);
  pXData->nItemSel        = nItemDefault;
  pXData->pfuncXSel       = NULL;
  sElem.pXData            = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw        = &gslc_ElemXListBoxDraw;
  // Specify the custom touch tracking callback
  sElem.pfuncXTouch       = &gslc_ElemXListBoxTouch;

  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_GRAY;
  sElem.colElemFrameGlow  = GSLC_COL_WHITE;

  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_DEFAULT);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_DEFAULT | GSLC_ELEMREF_REDRAW_FULL;
    return &(pGui->sElemRefTmp);
#endif
  }
  return NULL;
}

#define XLISTBOX_MARGIN_X 5
#define XLISTBOX_MARGIN_Y 5
#define XLISTBOX_ITEM_H 20
#define XLISTBOX_ITEM_MARGIN_X 5
#define XLISTBOX_ITEM_MARGIN_Y 5
#define XLISTBOX_MAX_STR 20

// Redraw the ListBox
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool gslc_ElemXListBoxDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXListBoxDraw";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*       pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);

  // Fetch the element's extended data structure
  gslc_tsXListBox* pListBox;
  pListBox = (gslc_tsXListBox*)(pElem->pXData);
  if (pListBox == NULL) {
    GSLC_DEBUG_PRINT("ERROR: ElemXListBoxDraw(%s) pXData is NULL\n","");
    return false;
  }

  bool            bGlow     = (pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN) && gslc_ElemGetGlow(pGui,pElemRef);
  int8_t          nItemSel  = pListBox->nItemSel;

  gslc_tsColor colTxt = pElem->colElemText;
  gslc_tsColor colBg = (bGlow) ? pElem->colElemFillGlow : pElem->colElemFill;

  gslc_tsRect rElemRect;
  if (pElem->nFeatures & GSLC_ELEM_FEA_FRAME_EN) {
    rElemRect = gslc_ExpandRect(pElem->rElem, -1, -1);
    gslc_DrawFrameRect(pGui,pElem->rElem,pElem->colElemFrame);
  }
  else {
    rElemRect = pElem->rElem;
  }


  int16_t nX0, nY0, nX1, nY1;
  nX0 = rElemRect.x;
  nY0 = rElemRect.y;
  nX1 = rElemRect.x + rElemRect.w - 1;
  nY1 = rElemRect.y + rElemRect.h - 1;

  gslc_tsRect rItemRect;
  int16_t nItemBaseX, nItemBaseY;
  int16_t nItemX, nItemY;
  int16_t nItemW;
  int16_t nTxtPixX;
  int16_t nTxtPixY;
  bool bItemSel;

  // Loop through the items
  nItemBaseX = nX0 + XLISTBOX_MARGIN_X;
  nItemBaseY = nY0 + XLISTBOX_MARGIN_Y;
  char acStr[XLISTBOX_MAX_STR+1] = "";

  nItemX = nItemBaseX;
  for (int nItemInd = 0; nItemInd < pListBox->nItemCnt; nItemInd++) {

    // FIXME: Replace by search for indexed string
    bool bOk = gslc_ElemXListBoxGetItem(pListBox->pStrItems, nItemInd, acStr, XLISTBOX_MAX_STR);
    // TODO: Handle !bOk
    /*
    if (nItemInd == 0) {
      strncpy(acStr, "Red",XLISTBOX_MAX_STR);
    }
    else if (nItemInd == 1) {
      strncpy(acStr, "Blue",XLISTBOX_MAX_STR);
    }
    else {
      strncpy(acStr, "Green",XLISTBOX_MAX_STR);
    }
    */

    nItemY = nItemBaseY + (nItemInd * (XLISTBOX_ITEM_H + (2*XLISTBOX_ITEM_MARGIN_Y)) );
    nItemW = rElemRect.w - (2 * XLISTBOX_MARGIN_X);
    rItemRect = (gslc_tsRect) { nItemX, nItemY, nItemW, XLISTBOX_ITEM_H + 2 * XLISTBOX_ITEM_MARGIN_Y };
    nTxtPixX = nItemX + XLISTBOX_ITEM_MARGIN_X;
    nTxtPixY = nItemY + XLISTBOX_ITEM_MARGIN_Y;
    bItemSel = (nItemInd == nItemSel) ? true : false;
    //GSLC_DEBUG_PRINT("ListBox: [%d] ItemX=%d ItemY=%d TxtX=%d TxtY=%d Str=[%s]\n", nItemInd,nItemX, nItemY, nTxtPixX, nTxtPixY, acStr);
    gslc_DrawFillRect(pGui,rItemRect,(bItemSel)?pElem->colElemFillGlow:pElem->colElemFill);

    gslc_DrvDrawTxt(pGui, nTxtPixX, nTxtPixY, pElem->pTxtFont, acStr, pElem->eTxtFlags, colTxt, colBg);

  }

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  // Mark page as needing flip
  gslc_PageFlipSet(pGui,true);

  return true;
}

bool gslc_ElemXListBoxTouch(void* pvGui, void* pvElemRef, gslc_teTouch eTouch, int16_t nRelX, int16_t nRelY)
{
  #if defined(DRV_TOUCH_NONE)
  return false;
  #else
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXListBoxTouch";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL, FUNCSTR);
    return false;
  }
  gslc_tsGui*           pGui = NULL;
  gslc_tsElemRef*       pElemRef = NULL;
  gslc_tsElem*          pElem = NULL;
  gslc_tsXListBox*      pListBox = NULL;

  // Typecast the parameters to match the GUI
  pGui = (gslc_tsGui*)(pvGui);
  pElemRef = (gslc_tsElemRef*)(pvElemRef);
  pElem = gslc_GetElemFromRef(pGui, pElemRef);
  pListBox = (gslc_tsXListBox*)(pElem->pXData);

  bool    bGlowingOld = gslc_ElemGetGlow(pGui, pElemRef);
  bool    bTouchInside = false;
  bool    bUpdateSel = false;
  bool    bIndexed = false;

  switch (eTouch) {

  case GSLC_TOUCH_DOWN_IN:
    // Start glowing as must be over it
    gslc_ElemSetGlow(pGui, pElemRef, true);
    bUpdateSel = true;
    bTouchInside = true;
    break;

  case GSLC_TOUCH_MOVE_IN:
    gslc_ElemSetGlow(pGui, pElemRef, true);
    bUpdateSel = true;
    bTouchInside = true;
    break;
  case GSLC_TOUCH_MOVE_OUT:
    gslc_ElemSetGlow(pGui, pElemRef, false);
    bUpdateSel = true;
    break;

  case GSLC_TOUCH_UP_IN:
    // End glow
    gslc_ElemSetGlow(pGui, pElemRef, false);
    bTouchInside = true;
    break;
  case GSLC_TOUCH_UP_OUT:
    // End glow
    gslc_ElemSetGlow(pGui, pElemRef, false);
    bUpdateSel = true; //???
    break;

  case GSLC_TOUCH_SET_REL:
  case GSLC_TOUCH_SET_ABS:
    //bIndexed = true;
    //gslc_ElemSetGlow(pGui,pElemRef,true);
    //bUpdateSel = true;
    break;

  default:
    return false;
    break;
  }

  // If we need to update the ListBox selection, calculate the value
  // and perform the update
  if (bUpdateSel) {
    int16_t nItemSelOld = pListBox->nItemSel;
    int16_t nItemSel = -1;

    if (bIndexed) {
      // The selection is changed by direct control (eg. keyboard)
      // instead of touch coordinates


      // FIXME: Change the following to be either absolute or relative
      // value assignment instead of inc/dec. Then the user code can
      // define what the magnitude and direction should be.

      if (eTouch == GSLC_TOUCH_SET_REL) {
        // Overload the "nRelY" parameter
        nItemSel = nItemSelOld + nRelY;
      } else if (eTouch == GSLC_TOUCH_SET_ABS) {
        // Overload the "nRelY" parameter
        nItemSel = nRelY;
      }
      // Clamp the value to the allowed range
      nItemSel = (nItemSel >= pListBox->nItemCnt) ? pListBox->nItemCnt - 1 : nItemSel;
      nItemSel = (nItemSel < 0) ? 0 : nItemSel;
    }
    else {
      // Determine which item we are tracking
      int16_t nItemY;
      if (bTouchInside) {
        nItemY = (nRelY - XLISTBOX_MARGIN_Y) / (XLISTBOX_ITEM_MARGIN_Y*2 + XLISTBOX_ITEM_H);

        if ((nItemY >= 0) && (nItemY < pListBox->nItemCnt)) {
          nItemSel = nItemY;
        }
      }

      // Update the ListBox selection
      //gslc_ElemXListBoxSetSel(pGui,pElemRef,nItemSel);
      if (nItemSel != nItemSelOld) {
        // Update the selection
        pListBox->nItemSel = nItemSel;
        // If any selection callback is defined, call it now
        if (pListBox->pfuncXSel != NULL) {
          (*pListBox->pfuncXSel)((void*)(pGui),(void*)(pElemRef),nItemSel);
        }
        // Redraw the element
        gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_INC);
      }
    }

    // If the ListBox changed state, redraw
    //if (gslc_ElemGetGlow(pGui,pElemRef) != bGlowingOld) {
      //gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
    //}
  }

  return true;
  
  #endif // DRV_TOUCH_NONE
}

int16_t gslc_ElemXListBoxGetSel(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXListBoxGetSel";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return 0;
  }
  gslc_tsElem*     pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXListBox* pListBox = (gslc_tsXListBox*)(pElem->pXData);

  return pListBox->nItemSel;
}

// Assign the selection callback function for a ListBox
void gslc_ElemXListBoxSetSelFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_XLISTBOX_SEL funcCb)
{
  if ((pElemRef == NULL) || (funcCb == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXListBoxSetSelFunc";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXListBox*   pListBox = (gslc_tsXListBox*)(pElem->pXData);
  pListBox->pfuncXSel = funcCb;
}

// ============================================================================
