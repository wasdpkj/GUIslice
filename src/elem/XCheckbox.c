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

#include "elem/XCheckbox.h"

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
// Extended Element: Checkbox
// - Checkbox with custom handler for touch tracking which
//   enables glow to be defined whenever touch is tracked over
//   the element.
// ============================================================================

// Create a checkbox element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw but does not track touch/click
gslc_tsElemRef* gslc_ElemXCheckboxCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXCheckbox* pXData,gslc_tsRect rElem,bool bRadio,gslc_teXCheckboxStyle nStyle,
  gslc_tsColor colCheck,bool bChecked)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXCheckboxCreate";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_CHECKBOX,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;

  // Default group assignment. Can override later with ElemSetGroup()
  sElem.nGroup            = GSLC_GROUP_ID_NONE;

  // Define other extended data
  pXData->bRadio          = bRadio;
  pXData->bChecked        = bChecked;
  pXData->colCheck        = colCheck;
  pXData->nStyle          = nStyle;
  pXData->pfuncXToggle    = NULL;
  sElem.pXData            = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw        = &gslc_ElemXCheckboxDraw;
  // Specify the custom touch tracking callback
  // - NOTE: This is optional (and can be set to NULL).
  //   See the discussion under gslc_ElemXCheckboxTouch()
  sElem.pfuncXTouch       = &gslc_ElemXCheckboxTouch;
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

bool gslc_ElemXCheckboxGetState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  gslc_tsElem*        pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXCheckbox*   pCheckbox  = (gslc_tsXCheckbox*)(pElem->pXData);

  return pCheckbox->bChecked;
}


// Determine which checkbox in the group has been "checked"
gslc_tsElemRef* gslc_ElemXCheckboxFindChecked(gslc_tsGui* pGui,int16_t nGroupId)
{
  int16_t             nCurInd;
  gslc_tsElemRef*     pCurElemRef = NULL;
  gslc_tsElem*        pCurElem = NULL;
  int16_t             nCurType;
  int16_t             nCurGroup;
  bool                bCurChecked;
  gslc_tsElemRef*     pFoundElemRef = NULL;

  // Operate on current page
  // TODO: Support other page layers
  gslc_tsPage* pPage = pGui->apPageStack[GSLC_STACK_CUR];
  if (pPage == NULL) {
    return NULL; // No page added yet
  }

  if (pGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXCheckboxFindChecked";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }

  gslc_tsCollect* pCollect = &pPage->sCollect;
  for (nCurInd=0;nCurInd<pCollect->nElemCnt;nCurInd++) {
    // Fetch extended data
    pCurElemRef   = &(pCollect->asElemRef[nCurInd]);
    pCurElem      = gslc_GetElemFromRef(pGui,pCurElemRef);
    nCurType      = pCurElem->nType;
    // Only want to proceed if it is a checkbox
    if (nCurType != GSLC_TYPEX_CHECKBOX) {
      continue;
    }

    nCurGroup     = pCurElem->nGroup;
    bCurChecked   = gslc_ElemXCheckboxGetState(pGui,pCurElemRef);

    // If this is in a different group, ignore it
    if (nCurGroup != nGroupId) {
      continue;
    }

    // Did we find an element in the group that was checked?
    if (bCurChecked) {
      pFoundElemRef = pCurElemRef;
      break;
    }
  } // nCurInd
  return pFoundElemRef;
}

// Assign the callback function for checkbox/radio state change events
void gslc_ElemXCheckboxSetStateFunc(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, GSLC_CB_XCHECKBOX pfuncCb)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXCheckboxSetStateFunc";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*        pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXCheckbox*   pCheckbox = (gslc_tsXCheckbox*)(pElem->pXData);
  pCheckbox->pfuncXToggle = pfuncCb;
}

// Helper routine for gslc_ElemXCheckboxSetState()
// - Updates the checkbox/radio control's state but does
//   not touch any other controls in the group
void gslc_ElemXCheckboxSetStateHelp(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bChecked)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXCheckboxSetStateHelp";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*        pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXCheckbox*   pCheckbox = (gslc_tsXCheckbox*)(pElem->pXData);


  // Update our data element
  bool  bCheckedOld = pCheckbox->bChecked;
  pCheckbox->bChecked = bChecked;

  // Element needs redraw
  if (bChecked != bCheckedOld) {
    // Only need an incremental redraw
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }

  // If any state callback is defined, call it now
  // - In all cases, return the ElementRef of the element that issued the callback
  //
  // For checkbox:
  // - If not selected:  return ID_NONE     and state=false
  // - If     selected:  return current ID  and state=true
  // For radio button:
  // - If none selected: return ID_NONE     and state=false
  // - If one  selected: return selected ID and state=true
  if (pCheckbox->pfuncXToggle != NULL) {
    gslc_tsElemRef* pRetRef = NULL;
    int16_t nGroup = GSLC_GROUP_ID_NONE;
    int16_t nSelId = GSLC_ID_NONE;
    if (!pCheckbox->bRadio) {
      // Checkbox
      if (bChecked) {
        nSelId = pElem->nId;
      } else {
        nSelId = GSLC_ID_NONE;
      }
    } else {
      // Radio button
      // - Determine the group that the radio button belongs to
      nGroup = pElem->nGroup;
      // Determine if any radio button in the group has been selected
      pRetRef = gslc_ElemXCheckboxFindChecked(pGui, nGroup);
      if (pRetRef != NULL) {
        // One has been selected, return its ID
        bChecked = true;
        nSelId = pRetRef->pElem->nId;
      } else {
        // No radio button selected, return ID NONE
        bChecked = false;
        nSelId = GSLC_ID_NONE;
      }
    }
    // Now send the callback notification
    (*pCheckbox->pfuncXToggle)((void*)(pGui), (void*)(pElemRef), nSelId, bChecked);
  } // pfuncXToggle

}

// Update the checkbox/radio control's state. If it is a radio button
// then also update the state of all other buttons in the group.
void gslc_ElemXCheckboxSetState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bChecked)
{
  // Operate on current page
  // TODO: Support other page layers
  gslc_tsPage* pPage = pGui->apPageStack[GSLC_STACK_CUR];
  if (pPage == NULL) {
    return; // No page added yet
  }

  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXCheckboxSetState";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }

  gslc_tsElem*        pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXCheckbox*   pCheckbox = (gslc_tsXCheckbox*)(pElem->pXData);
  bool                bRadio    = pCheckbox->bRadio;
  int16_t             nGroup    = pElem->nGroup;
  int16_t             nElemId   = pElem->nId;

  // Special handling when we select a radio button
  if (bRadio && bChecked) {

    // If we are selecting a radio button that is already
    // selected, then skip further update events.
    // NOTE: This check is not very efficient, but it avoids
    // the creation of extra events.
    gslc_tsElemRef* pTmpRef = gslc_ElemXCheckboxFindChecked(pGui, nGroup);
    if (pTmpRef == pElemRef) {
      // Same element, so skip
      return;
    }

    // Proceed to deselect any other selected items in the group.
    // Note that SetState calls itself to deselect other items so it
    // is important to qualify this logic with bChecked=true
    int16_t           nCurInd;
    int16_t           nCurId;
    gslc_tsElem*      pCurElem = NULL;
    gslc_tsElemRef*   pCurElemRef = NULL;
    int16_t           nCurType;
    int16_t           nCurGroup;

    // We use the GUI pointer for access to other elements
    // NOTE: There is an assumption that we are calling ElemXCheckboxSetState
    //       on a checkbox on the current page.

    gslc_tsCollect* pCollect = &pPage->sCollect;
    for (nCurInd=0;nCurInd<pCollect->nElemRefCnt;nCurInd++) {
      // Fetch extended data
      pCurElemRef   = &pCollect->asElemRef[nCurInd];
      pCurElem      = gslc_GetElemFromRef(pGui,pCurElemRef);

      // FIXME: Handle pCurElemRef->eElemFlags
      nCurId        = pCurElem->nId;
      nCurType      = pCurElem->nType;

      // Only want to proceed if it is a checkbox
      if (nCurType != GSLC_TYPEX_CHECKBOX) {
        continue;
      }

      nCurGroup     = pCurElem->nGroup;

      // If this is in a different group, ignore it
      if (nCurGroup != nGroup) {
        continue;
      }

      // Is this our element? If so, ignore the deselect operation
      if (nCurId == nElemId) {
        continue;
      }

      // Deselect all other elements
      gslc_ElemXCheckboxSetStateHelp(pGui,pCurElemRef,false);

    } // nInd

  } // bRadio

  // Set the state of the current element
  gslc_ElemXCheckboxSetStateHelp(pGui,pElemRef,bChecked);
}

// Toggle the checkbox control's state
void gslc_ElemXCheckboxToggleState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXCheckboxToggleState";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  // Update the data element
  bool bCheckNew = (gslc_ElemXCheckboxGetState(pGui,pElemRef))? false : true;
  gslc_ElemXCheckboxSetState(pGui,pElemRef,bCheckNew);

  // Element needs redraw
  // - Only incremental is needed
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
}

// Redraw the checkbox
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool gslc_ElemXCheckboxDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXCheckboxDraw";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);

  gslc_tsRect   rInner;

  // Fetch the element's extended data structure
  gslc_tsXCheckbox* pCheckbox;
  pCheckbox = (gslc_tsXCheckbox*)(pElem->pXData);
  if (pCheckbox == NULL) {
    GSLC_DEBUG_PRINT("ERROR: ElemXCheckboxDraw(%s) pXData is NULL\n","");
    return false;
  }

  bool bChecked  = pCheckbox->bChecked;
  gslc_teXCheckboxStyle   nStyle = pCheckbox->nStyle;
  bool bGlow     = (pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN) && gslc_ElemGetGlow(pGui,pElemRef);

  // Draw the background
  gslc_DrawFillRect(pGui,pElem->rElem,pElem->colElemFill);

  // Generic coordinate calcs
  int16_t nX0,nY0,nX1,nY1,nMidX,nMidY;
  nX0 = pElem->rElem.x;
  nY0 = pElem->rElem.y;
  nX1 = pElem->rElem.x + pElem->rElem.w - 1;
  nY1 = pElem->rElem.y + pElem->rElem.h - 1;
  nMidX = (nX0+nX1)/2;
  nMidY = (nY0+nY1)/2;
  if (nStyle == GSLCX_CHECKBOX_STYLE_BOX) {
    // Draw the center indicator if checked
    rInner = gslc_ExpandRect(pElem->rElem,-5,-5);
    if (bChecked) {
      // If checked, fill in the inner region
      gslc_DrawFillRect(pGui,rInner,pCheckbox->colCheck);
    } else {
      // Assume the background fill has already been done so
      // we don't need to do anything more in the unchecked case
    }
    // Draw a frame around the checkbox
    gslc_DrawFrameRect(pGui,pElem->rElem,(bGlow)?pElem->colElemFrameGlow:pElem->colElemFrame);

  } else if (nStyle == GSLCX_CHECKBOX_STYLE_X) {
    // Draw an X through center if checked
    if (bChecked) {
      gslc_DrawLine(pGui,nX0,nY0,nX1,nY1,pCheckbox->colCheck);
      gslc_DrawLine(pGui,nX0,nY1,nX1,nY0,pCheckbox->colCheck);
    }
    // Draw a frame around the checkbox
    gslc_DrawFrameRect(pGui,pElem->rElem,(bGlow)?pElem->colElemFrameGlow:pElem->colElemFrame);

  } else if (nStyle == GSLCX_CHECKBOX_STYLE_ROUND) {
    // Draw inner circle if checked
    if (bChecked) {
      gslc_DrawFillCircle(pGui,nMidX,nMidY,5,pCheckbox->colCheck);
    }
    // Draw a frame around the checkbox
    gslc_DrawFrameCircle(pGui,nMidX,nMidY,(pElem->rElem.w/2),(bGlow)?pElem->colElemFrameGlow:pElem->colElemFrame);

  }

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  // Mark page as needing flip
  gslc_PageFlipSet(pGui,true);

  return true;
}

// This callback function is called by gslc_ElemSendEventTouch()
// after any touch event
// - NOTE: Adding this touch callback is optional. Without it, we
//   can still have a functional checkbox, but doing the touch
//   tracking allows us to change the glow state of the element
//   dynamically, as well as updating the checkbox state if the
//   user releases over it (ie. a click event).
//
bool gslc_ElemXCheckboxTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY)
{
#if defined(DRV_TOUCH_NONE)
  return false;
#else

  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXCheckboxTouch";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  gslc_tsGui*           pGui = NULL;
  gslc_tsElemRef*       pElemRef = NULL;
  gslc_tsElem*          pElem = NULL;
  gslc_tsXCheckbox*     pCheckbox = NULL;

  // Typecast the parameters to match the GUI
  pGui  = (gslc_tsGui*)(pvGui);
  pElemRef = (gslc_tsElemRef*)(pvElemRef);
  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pCheckbox = (gslc_tsXCheckbox*)(pElem->pXData);

  bool  bRadio      = pCheckbox->bRadio;
  bool  bCheckedOld = pCheckbox->bChecked;
  bool  bGlowingOld = gslc_ElemGetGlow(pGui,pElemRef);

  switch(eTouch) {

    case GSLC_TOUCH_DOWN_IN:
      gslc_ElemSetGlow(pGui,pElemRef,true);
      break;

    case GSLC_TOUCH_MOVE_IN:
      gslc_ElemSetGlow(pGui,pElemRef,true);
      break;
    case GSLC_TOUCH_MOVE_OUT:
      gslc_ElemSetGlow(pGui,pElemRef,false);
      break;

    case GSLC_TOUCH_UP_IN:
      gslc_ElemSetGlow(pGui,pElemRef,false);
      // Now that we released on element, update the state
      bool  bCheckNew;
      if (bRadio) {
        // Radio button action: set
        bCheckNew = true;
      } else {
        // Checkbox button action: toggle
        bCheckNew = (pCheckbox->bChecked)?false:true;
      }
      gslc_ElemXCheckboxSetState(pGui,pElemRef,bCheckNew);
      break;
    case GSLC_TOUCH_UP_OUT:
      gslc_ElemSetGlow(pGui,pElemRef,false);
      break;

    default:
      return false;
      break;
  }

  // If the checkbox changed state, redraw
  bool  bChanged = false;
  if (gslc_ElemGetGlow(pGui,pElemRef) != bGlowingOld) { bChanged = true; }
  if (pCheckbox->bChecked != bCheckedOld) { bChanged = true; }
  if (bChanged) {
    // Incremental redraw
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }

  return true;
  #endif // !DRV_TOUCH_NONE
}

// ============================================================================
