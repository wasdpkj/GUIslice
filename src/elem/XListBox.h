#ifndef _GUISLICE_EX_XLISTBOX_H_
#define _GUISLICE_EX_XLISTBOX_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library extension: Slider control
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

/// Callback function for slider feedback
//typedef bool (*GSLC_CB_XSLIDER_POS)(void* pvGui,void* pvElem,int16_t nPos);

// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer

/// Extended data for Slider element
typedef struct {
  // Config
  const char*     pStrItems;      ///< String containing items (newline-limited)
  /*
  bool            bVert;          ///< Orientation: true if vertical, else horizontal
  int16_t         nThumbSz;       ///< Size of the thumb control
  int16_t         nPosMin;        ///< Minimum position value of the slider
  int16_t         nPosMax;        ///< Maximum position value of the slider
  // Style config
  uint16_t        nTickDiv;       ///< Style: number of tickmark divisions (0 for none)
  int16_t         nTickLen;       ///< Style: length of tickmarks
  gslc_tsColor    colTick;        ///< Style: color of ticks
  bool            bTrim;          ///< Style: show a trim color
  gslc_tsColor    colTrim;        ///< Style: color of trim
  */
  // State
  int8_t          nItemSel;       ///< Currently selected item (-1 for none)
  /*
  int16_t         nPos;           ///< Current position value of the slider
  // Callbacks
  GSLC_CB_XSLIDER_POS pfuncXPos;  ///< Callback func ptr for position update
  */
} gslc_tsXListBox;


///
/// Create a Slider Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining checkbox size
/// \param[in]  nPosMin:     Minimum position value
/// \param[in]  nPosMax:     Maximum position value
/// \param[in]  nPos:        Starting position value
/// \param[in]  nThumbSz:    Size of the thumb control
/// \param[in]  bVert:       Orientation (true for vertical)
///
/// \return Pointer to Element reference or NULL if failure
///
//gslc_tsElemRef* gslc_ElemXSliderCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
//  gslc_tsXSlider* pXData,gslc_tsRect rElem,int16_t nPosMin,int16_t nPosMax,int16_t nPos,
//  uint16_t nThumbSz,bool bVert);

gslc_tsElemRef* gslc_ElemXListBoxCreate(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
  gslc_tsXListBox* pXData, gslc_tsRect rElem, int16_t nFontId, const char* pStrItems, int16_t nSelDefault);


/*
///
/// Set a Slider element's current position
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  bTrim:       Show a colored trim?
/// \param[in]  colTrim:     Color of trim
/// \param[in]  nTickDiv:    Number of tick divisions to show (0 for none)
/// \param[in]  nTickLen:    Length of tickmarks
/// \param[in]  colTick:     Color of ticks
///
/// \return none
///
void gslc_ElemXSliderSetStyle(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,
        bool bTrim,gslc_tsColor colTrim,uint16_t nTickDiv,
        int16_t nTickLen,gslc_tsColor colTick);


///
/// Get a Slider element's current position
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return Current slider position
///
int gslc_ElemXSliderGetPos(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);


///
/// Set a Slider element's current position
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nPos:        New position value
///
/// \return none
///
void gslc_ElemXSliderSetPos(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nPos);

///
/// Assign the position callback function for a slider
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  funcCb:      Function pointer to position routine (or NULL for none)
///
/// \return none
///
void gslc_ElemXSliderSetPosFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_XSLIDER_POS funcCb);
*/

///
/// Draw a Slider element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
//xxx bool gslc_ElemXSliderDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);
bool gslc_ElemXListBoxDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);

///
/// Handle touch events to Slider element
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
//xxx bool gslc_ElemXSliderTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY);
bool gslc_ElemXListBoxTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY);

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

