#ifndef _GUISLICE_EX_XLISTBOX_H_
#define _GUISLICE_EX_XLISTBOX_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library extension: ListBox control
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
/// \file XSlider.h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


// ============================================================================
// Extended Element: Slider
// ============================================================================

// Define unique identifier for extended element type
// - Select any number above GSLC_TYPE_BASE_EXTEND
#define  GSLC_TYPEX_LISTBOX GSLC_TYPE_BASE_EXTEND + 10

/// Callback function for ListBox feedback
typedef bool (*GSLC_CB_XLISTBOX_SEL)(void* pvGui,void* pvElem,int16_t nSel);

// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer

/// Extended data for ListBox element
typedef struct {

  // Config
  const char*     pStrItems;      ///< String containing items (newline-limited)
  int16_t         nItemCnt;       ///< Number of items in the list

  // Style config

  // State
  int16_t         nItemSel;       ///< Currently selected item (-1 for none)

  // Callbacks
  GSLC_CB_XLISTBOX_SEL pfuncXSel; ///< Callback func ptr for selection update

} gslc_tsXListBox;


///
/// Create a Slider Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining checkbox size
/// \param[in]  nFontId:     Font ID for item display
/// \param[in]  pStrItems:   String containing list of items (delimited by '|')
/// \param[in]  nSelDefault: Default item to select
///
/// \return Pointer to Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemXListBoxCreate(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
  gslc_tsXListBox* pXData, gslc_tsRect rElem, int16_t nFontId, const char* pStrItems, int16_t nSelDefault);



///
/// Draw a ListBox element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXListBoxDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);

///
/// Handle touch events to ListBox element
/// - Called from gslc_ElemSendEventTouch()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element ref (typecast to gslc_tsElemRef*)
/// \param[in]  eTouch:      Touch event type
/// \param[in]  nRelX:       Touch X coord relative to element
/// \param[in]  nRelY:       Touch Y coord relative to element
///
/// \return true if success, false otherwise
///
bool gslc_ElemXListBoxTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY);


///
/// Get a ListBox element's current selection
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return Current ListBox selection (or -1 if none)
///
int16_t gslc_ElemXListBoxGetSel(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

///
/// Assign the selection callback function for a ListBox
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  funcCb:      Function pointer to selection routine (or NULL for none)
///
/// \return none
///
void gslc_ElemXListBoxSetSelFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_XLISTBOX_SEL funcCb);

// ============================================================================

// ------------------------------------------------------------------------
// Read-only element macros
// ------------------------------------------------------------------------

// Macro initializers for Read-Only Elements in Flash/PROGMEM
//



#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XLISTBOX_H_

