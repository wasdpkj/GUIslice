//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 30 (Arduino): ListBox
//   - Demonstrates push buttons, checkboxes and slider controls
//   - Shows callback notifications for checkboxes and radio buttons
//   - Provide example of additional Adafruit-GFX fonts
//     (see USE_EXTRA_FONTS)
//   - NOTE: This is the simple version of the example without
//     optimizing for memory consumption. Therefore, it may not
//     run on Arduino devices with limited memory. A "minimal"
//     version is located in the "arduino_min" folder which includes
//     FLASH memory optimization for reduced memory devices.
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
//

#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
#include "elem/XListBox.h"

#include <Adafruit_GFX.h>

// To demonstrate additional fonts, uncomment the following line:
//#define USE_EXTRA_FONTS

#ifdef USE_EXTRA_FONTS
  // Note that these files are located within the Adafruit-GFX library folder:
#include "Fonts/FreeSansBold12pt7b.h"
#endif

// Defines for resources


// Enumerations for pages, elements, fonts, images
enum { E_PG_MAIN };
enum {
  E_ELEM_BTN_QUIT, E_ELEM_LISTBOX
};
enum { E_FONT_BTN, E_FONT_TXT };

bool        m_bQuit = false;

// Free-running counter for display
unsigned    m_nCount = 0;


// Instantiate the GUI
#define MAX_PAGE                1
#define MAX_FONT                2

// Define the maximum number of elements per page
#define MAX_ELEM_PG_MAIN          7                                         // # Elems total
#define MAX_ELEM_PG_MAIN_RAM      MAX_ELEM_PG_MAIN                          // # Elems in RAM

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

gslc_tsXListBox             m_sXListBox;


#define MAX_STR     15
char                acTxt[MAX_STR+1];


// Save some element references for quick access
gslc_tsElemRef*  m_pElemCnt = NULL;
gslc_tsElemRef*  m_pElemSel = NULL;
gslc_tsElemRef*  m_pElemListBox = NULL;

// Define debug message function
static int16_t DebugOut(char ch) { Serial.write(ch); return 0; }

// Button callbacks
// - Detect a button press
// - In this particular example, we are looking for the Quit button press
//   which is used to terminate the program.
bool CbBtnQuit(void* pvGui, void *pvElem, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

bool CbListBox(void* pvGui, void* pvElemRef, int16_t nSelId)
{
  gslc_tsGui*     pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui, pElemRef);
  if (pElemRef == NULL) {
    return false;
  }

  // Update the status message with the selection
  if (nSelId == -1) {
    snprintf(acTxt, MAX_STR, "NONE");
  } else {
    snprintf(acTxt, MAX_STR, "%d", nSelId);
  }
  gslc_ElemSetTxtStr(&m_gui, m_pElemSel, acTxt);
  
}


// Create page elements
bool InitOverlays()
{
  gslc_tsElemRef* pElemRef;

  gslc_PageAdd(&m_gui, E_PG_MAIN, m_asPageElem, MAX_ELEM_PG_MAIN_RAM, m_asPageElemRef, MAX_ELEM_PG_MAIN);


  // Background flat color
  gslc_SetBkgndColor(&m_gui, GSLC_COL_GRAY_DK2);

  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 10, 10, 300, 220 });
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);

  // Create Quit button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_QUIT, E_PG_MAIN,
    (gslc_tsRect) {
    240, 20, 60, 30
  }, (char*)"Quit", 0, E_FONT_BTN, &CbBtnQuit);

  // Create counter
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 20, 60, 50, 10 },
    (char*)"Count:", 0, E_FONT_TXT);
  static char mstr1[8] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 80, 60, 50, 10 },
    mstr1, sizeof(mstr1), E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_YELLOW);
  m_pElemCnt = pElemRef; // Save for quick access

  // Create selection status message
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 140, 60, 50, 10 },
    (char*)"Selected:", 0, E_FONT_TXT);
  static char mstr2[20] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 200, 60, 50, 10 },
    mstr2, sizeof(mstr2), E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_ORANGE);
  m_pElemSel = pElemRef; // Save for quick access


  // Create the XListBox
  static const char* m_pStrItems = "Red|Blue|Green";
  pElemRef = gslc_ElemXListBoxCreate(&m_gui, E_ELEM_LISTBOX, E_PG_MAIN, &m_sXListBox,
    (gslc_tsRect) {
    20, 80, 100, 100
  }, E_FONT_TXT, m_pStrItems,0);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_BLUE_DK3, GSLC_COL_GRAY_DK3, GSLC_COL_GREEN_DK1);
  gslc_ElemSetFrameEn(&m_gui, pElemRef, true);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
  gslc_ElemXListBoxSetSelFunc(&m_gui, pElemRef, &CbListBox);
  m_pElemListBox = pElemRef; // Save for quick access

  return true;
}


void setup()
{
  // Initialize debug output
  Serial.begin(9600);
  gslc_InitDebug(&DebugOut);
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  // Initialize
  if (!gslc_Init(&m_gui, &m_drv, m_asPage, MAX_PAGE, m_asFont, MAX_FONT)) { return; }

  // Load Fonts
  #ifdef USE_EXTRA_FONTS
    // Demonstrate the use of additional fonts (must have #include)
    if (!gslc_FontAdd(&m_gui, E_FONT_BTN, GSLC_FONTREF_PTR, &FreeSansBold12pt7b, 1)) { return; }
  #else
    // Use default font
    if (!gslc_FontAdd(&m_gui, E_FONT_BTN, GSLC_FONTREF_PTR, NULL, 1)) { return; }
  #endif
  if (!gslc_FontAdd(&m_gui, E_FONT_TXT, GSLC_FONTREF_PTR, NULL, 1)) { return; }

  // Create graphic elements
  InitOverlays();

  // Start up display on main page
  gslc_SetPageCur(&m_gui, E_PG_MAIN);

  m_bQuit = false;
}

void loop()
{
  // General counter
  m_nCount++;

  // Update elements on active page

  snprintf(acTxt, MAX_STR, "%u", m_nCount / 5);
  gslc_ElemSetTxtStr(&m_gui, m_pElemCnt, acTxt);


  // Periodically call GUIslice update function
  gslc_Update(&m_gui);

  // Slow down updates
  delay(10);

  // In a real program, we would detect the button press and take an action.
  // For this Arduino demo, we will pretend to exit by emulating it with an
  // infinite loop. Note that interrupts are not disabled so that any debug
  // messages via Serial have an opportunity to be transmitted.
  if (m_bQuit) {
    gslc_Quit(&m_gui);
    while (1) {}
  }
}