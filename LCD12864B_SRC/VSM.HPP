/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****       LABCENTER INTEGRATED SIMULATION ARCHITECTURE       *****/
/*****                                                          *****/
/*****         Virtual System Modeling Interface Header         *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#ifndef VSM_HPP
#define VSM_HPP

// Version 1.00 - Initial release.
// Version 1.01 - Added setvdmhandler() to IINSTANCE, and the ICPU interface
//                Added sethandler() to IDSIMPIN.
//                Added setcallbackex to IDSIMCKT.
// Version 1.02 - Added IWATCHPOPUP interface.
// Version 1.03 - Added getsteptoaddr() function to sourcewin.
// Version 1.04 - COMPONENT::getsymbolarea() now returns BOOL.
//                Added allocvars(), intergrate() and truncerr() to ISPICECKT.
// Version 1.05 - Added createpin and createnet functionality.
// Version 1.06 - Added timestamped authorization function.
// Version 1.07 - Added DSIM systime() function.
//                Added setclockcallback() function.

// Version 1.10 - Added streamlined settiming/setstates/drivexxx methods to IDSIMPIN.
//                Added loadmemory() function to IINSTANCE.
//                Added bus pin API.

#define VSM_API_VERSION  110

// 01/09/2003
#define VSM_BUILD_DATE   0x9ADFBCA5

// VSM uses 4 byte alignment throughout
#pragma pack(push,4)

// Portability typedefs:
#ifndef STDLX_H
#define VOID void
typedef double DOUBLE;
typedef char CHAR;
typedef unsigned char BYTE;
typedef signed short SHORT;
typedef signed int INT;
typedef signed long LONG;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef signed int BOOL;
typedef unsigned int UINT;
typedef unsigned long COLOUR;

#define TRUE  1
#define FALSE 0

#endif

#ifdef __SC__
typedef long long LONGLONG;
#else
typedef __int64 LONGLONG;
#endif

#ifndef _WINDOWS_
typedef VOID *HANDLE;
typedef VOID *HWND;
typedef VOID *HDC;
typedef unsigned WPARAM;
typedef long LPARAM;
typedef long LRESULT;
#endif

typedef unsigned MESSAGE;

#ifndef GM_HPP
#if defined(STDOS_H) || !defined(_WINDOWS_)
struct POINT { LONG x, y; };
struct BOX  { LONG x1, y1, x2, y2; };
#else
typedef RECT BOX;
#endif
#define MIR_X 1
#define MIR_Y 2
#endif


/*********************************************************************
*****  Licence API ****
**********************/


class ILICENCESERVER
 { public:
      virtual BOOL authorize (DWORD product_id, DWORD apiver=VSM_API_VERSION) = 0;
      virtual BOOL authorizeex (DWORD product_id, DWORD apiver=VSM_API_VERSION, DWORD builddate=VSM_BUILD_DATE) = 0;
      virtual DWORD getid() = 0;
      virtual CHAR *getname() = 0;
      virtual CHAR *getcompany() = 0;
      virtual CHAR *getexpirydate() = 0;
 };


/*********************************************************************
*****  Graphical Modelling Interface ****
****************************************/

// Types for active component messaging:
typedef LONG ACTIVESTATE;
struct ACTIVEDATA;

// Return types for aimulator interfaces:
class ISPICEMODEL;
class IDSIMMODEL;

// Pin types:
typedef INT SPICENODE;
typedef VOID *DSIMNODE;
class IDSIMPIN1;
class IDSIMPIN2;
class IBUSPIN;

#define IDSIMPIN IDSIMPIN2

// Flags for controlling property visibility
#define SHOW_ALL      0
#define HIDE_KEYWORD  1
#define HIDE_VALUE    2
#define HIDE_ALL      3
#define HIDE_NOMODIFY 4

// Useful Colours:
#ifndef COLOURS_H
#define  MAKECOLOUR(r,g,b) ((COLOUR)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define  BLACK           MAKECOLOUR(0x00,0x00,0x00)
#define  BLUE            MAKECOLOUR(0x00,0x00,0xC0)
#define  GREEN           MAKECOLOUR(0x00,0xC0,0x00)
#define  CYAN            MAKECOLOUR(0x00,0xC0,0xC0)
#define  RED             MAKECOLOUR(0xC0,0x00,0x00)
#define  MAGENTA         MAKECOLOUR(0xC0,0x00,0xC0)
#define  YELLOW          MAKECOLOUR(0xC0,0xC0,0x00)
#define  WHITE           MAKECOLOUR(0xC0,0xC0,0xC0)
#define  GREY            MAKECOLOUR(0x80,0x80,0x80)
#define  DARKBLUE        MAKECOLOUR(0x00,0x00,0x40)
#define  DARKGREEN       MAKECOLOUR(0x00,0x40,0x00)
#define  DARKCYAN        MAKECOLOUR(0x00,0x40,0x40)
#define  DARKRED         MAKECOLOUR(0x40,0x00,0x00)
#define  DARKMAGENTA     MAKECOLOUR(0x40,0x00,0x40)
#define  DARKYELLOW      MAKECOLOUR(0x40,0x40,0x00)
#define  DARKGREY        MAKECOLOUR(0x40,0x40,0x40)
#define  MIDBLUE         MAKECOLOUR(0x00,0x00,0x80)
#define  MIDGREEN        MAKECOLOUR(0x00,0x80,0x00)
#define  MIDCYAN         MAKECOLOUR(0x00,0x80,0x80)
#define  MIDRED          MAKECOLOUR(0x80,0x00,0x00)
#define  MIDMAGENTA      MAKECOLOUR(0x80,0x00,0x80)
#define  MIDYELLOW       MAKECOLOUR(0x80,0x80,0x00)
#define  MIDGREY         MAKECOLOUR(0x80,0x80,0x80)
#define  LIGHTBLUE       MAKECOLOUR(0x00,0x00,0xC0)
#define  LIGHTGREEN      MAKECOLOUR(0x00,0xC0,0x00)
#define  LIGHTCYAN       MAKECOLOUR(0x00,0xC0,0xC0)
#define  LIGHTRED        MAKECOLOUR(0xC0,0x00,0x00)
#define  LIGHTMAGENTA    MAKECOLOUR(0xC0,0x00,0xC0)
#define  LIGHTYELLOW     MAKECOLOUR(0xC0,0xC0,0x00)
#define  LIGHTGREY       MAKECOLOUR(0xC0,0xC0,0xC0)
#define  BRIGHTBLUE      MAKECOLOUR(0x00,0x00,0xFF)
#define  BRIGHTGREEN     MAKECOLOUR(0x00,0xFF,0x00)
#define  BRIGHTCYAN      MAKECOLOUR(0x00,0xFF,0xFF)
#define  BRIGHTRED       MAKECOLOUR(0xFF,0x00,0x00)
#define  BRIGHTMAGENTA   MAKECOLOUR(0xFF,0x00,0xFF)
#define  BRIGHTYELLOW    MAKECOLOUR(0xFF,0xFF,0x00)
#define  BRIGHTWHITE     MAKECOLOUR(0xFF,0xFF,0xFF)

#define  INVISIBLE       -1
#define  NOCOLOUR        -1

#endif

// Text justifications:
#define TXJ_LEFT   0
#define TXJ_RIGHT  1
#define TXJ_CENTRE 2
#define TXJ_BOTTOM 0
#define TXJ_TOP    4
#define TXJ_MIDDLE 8

// Handles for graphics and text styles
typedef VOID *HGFXSTYLE;
typedef VOID *HTEXTSTYLE;

// Pop-up window interfaces.
// Handles, types, etc. for pop-up windows:
typedef VOID IPOPUP;
typedef DWORD POPUPID;
typedef DWORD INSTANCEID;

enum POPUPTYPES
 { PWT_USER     = 0,
   PWT_DEBUG    = 1,
   PWT_STATUS   = 2,
   PWT_MEMORY   = 3,
   PWT_WATCH    = 5,
   PWT_SOURCE   = 4,
   PWT_VAR      = 6
 };

// Flags for creating pop-up windows. The bottom 20 bits are reserved for use by VSM,
// whilst the remaining top 12 bits are available for user pop-ups.
enum POPUPFLAGS
 { PWF_VISIBLE       = 0x00000001,
   PWF_SIZEABLE      = 0x00000002,
   PWF_LOCKPOSITION  = 0x00000004,
   PWF_HIDEONANIMATE = 0x00000008,
   PWF_AUTOREFRESH   = 0x00000010,
   PWF_WANTKEYBOARD  = 0x00000020,
   PWF_ACTIVE        = 0x00008000,
   PWF_INTERNAL      = 0x80000000
 };

// Structure used to pass createpopup information:
struct CREATEPOPUPSTRUCT
 { POPUPID    id;
   POPUPTYPES type;
   CHAR *caption;
   INT   width, height;
   DWORD flags;
 };


// Definition of services provided by an Active Component to an Active Model.
class ICOMPONENT
 { public:
      // Property management:
      virtual CHAR *getprop (CHAR *name) = 0;
      virtual CHAR *getproptext (VOID) = 0;
      virtual VOID addprop (CHAR *propname, CHAR *item, WORD hflags) = 0;
      virtual VOID delprop (CHAR *propname) = 0;
      virtual VOID setproptext(CHAR *text) = 0;

      // Active State processing:
      virtual ACTIVESTATE getstate (INT element, ACTIVEDATA *data) = 0;
      virtual BOOL setstate (ACTIVESTATE state) = 0;

      // Graphics management:
      virtual VOID setdrawscale (INT ppi) = 0;
      virtual HDC  begincache (BOX &area) = 0;
      virtual HDC  begincache (INT symbol) = 0;
      virtual VOID endcache() = 0;

      // Vector drawing services:
      virtual HGFXSTYLE creategfxstyle (CHAR *name=NULL) = 0;
      virtual VOID selectgfxstyle (HGFXSTYLE style) = 0;
      virtual VOID setpenwidth (INT w) = 0;
      virtual VOID setpencolour (COLOUR c) = 0;
      virtual VOID setbrushcolour (COLOUR c) = 0;
      virtual VOID drawline (INT x1, INT y1, INT x2, INT y2) = 0;
      virtual VOID drawbox (INT x1, INT y1, INT x2, INT y2) = 0;
      virtual VOID drawbox (BOX &bx) = 0;
      virtual VOID drawcircle (INT x, INT y, INT radius) = 0;
      virtual VOID drawbezier (POINT *, INT numpoints=4) = 0;
      virtual VOID drawpolyline (POINT *, INT numpoints) = 0;
      virtual VOID drawpolygon (POINT *, INT numpoints) = 0;
      virtual VOID drawsymbol(INT symbol) = 0;
      virtual VOID drawsymbol(INT x, INT y, INT rot, INT mir, INT symbol) = 0;
      virtual VOID drawstate (ACTIVESTATE state) = 0;
      virtual BOOL getsymbolarea (INT symbol, BOX *area) = 0;
      virtual BOOL getmarker (CHAR *name, POINT *pos=NULL, INT *rot=NULL, INT *mir=NULL) = 0;

      // Text output services:
      virtual HTEXTSTYLE createtextstyle (CHAR *name=NULL) = 0;
      virtual VOID selecttextstyle (HTEXTSTYLE style) = 0;
      virtual VOID settextfont (CHAR *name) = 0;
      virtual VOID settextsize (INT h) = 0;
      virtual VOID setbold (BOOL f) = 0;
      virtual VOID setitalic (BOOL f) = 0;
      virtual VOID setunderline (BOOL f) = 0;
      virtual VOID settextcolour (COLOUR c) = 0;
      virtual VOID drawtext (INT x, INT y, INT rot, INT jflags, CHAR *text, ...) = 0;

      // Pop-up window support:
      virtual IPOPUP *createpopup (CREATEPOPUPSTRUCT *cps) = 0;
      virtual VOID deletepopup (POPUPID id) = 0;

      // Miscellaneous:
      virtual VOID settimestep (DOUBLE time) = 0;
      virtual VOID error (CHAR *msg, ...) = 0;
      virtual VOID repaint (BOOL erase) = 0;

 };

// Flags passed to actuate function:
enum ACTUATEFLAGS
 { ACF_LEFT   =  0x00000001,
   ACF_RIGHT  =  0x00000002,
   ACF_MIDDLE =  0x00000004,
   ACF_INC    =  0x00010000,
   ACF_DEC    =  0x00020000,
   ACF_TOGGLE =  0x00040000
 };

// Definition of services that a graphical model must provide to an active component
class IACTIVEMODEL
 { public:
      virtual VOID initialize (ICOMPONENT *cpt) = 0;
      virtual ISPICEMODEL *getspicemodel (CHAR *primitive) = 0;
      virtual IDSIMMODEL  *getdsimmodel (CHAR *primitive) = 0;
      virtual VOID plot (ACTIVESTATE state) = 0;
      virtual VOID animate (INT element, ACTIVEDATA *newstate) = 0;
      virtual BOOL actuate (WORD key, INT x, INT y, DWORD flags) = 0;
 };



/*********************************************************************
*****  Generic Simulator Interface ****
**************************************/

// Time type definitions:
typedef DOUBLE REALTIME;
typedef LONGLONG  ABSTIME;
typedef const ABSTIME &CREFABSTIME;
typedef LONGLONG RELTIME;
typedef const RELTIME &CREFRELTIME;

// Time Conversion Macros
#define DSIMTICK    1e-12

// Maximum bits in bus:
#define MAXBUSBITS     32

//#define dsimtime(t) ABSTIME((t)/DSIMTICK)
//#define realtime(t) REALTIME((t)*DSIMTICK)
inline ABSTIME  dsimtime(DOUBLE t) { return ABSTIME((t)/DSIMTICK); }
inline REALTIME realtime(ABSTIME t) { return REALTIME((t)*DSIMTICK); }


// Simulator Run Control Modes
enum RUNMODES
 { RM_BATCH=-1,
   RM_START,
   RM_STOP,
   RM_SUSPEND,
   RM_ANIMATE,
   RM_STEPTIME,
   RM_STEPOVER,
   RM_STEPINTO,
   RM_STEPOUT,
   RM_STEPTO,
   RM_META,
   RM_DUMP
 };

// Public Interface to a simulator component instance.
class IINSTANCE
 { public:
      // Basic property access:
      virtual CHAR *id() = 0;
      virtual CHAR *value() = 0;
      virtual CHAR *getstrval(CHAR *name, CHAR *defval=NULL) = 0;
      #ifdef __SC__
      virtual DOUBLE getnumval (CHAR *name, DOUBLE defval=0) = 0;
      #else
      virtual VOID getnumval (DOUBLE *result, CHAR *name, DOUBLE defval=0) = 0;
      #endif
      virtual BOOL getboolval (CHAR *name, BOOL defval=FALSE) = 0;
      virtual DWORD gethexval (CHAR *name, DWORD defval=0) = 0;
      virtual LONG getinitval (CHAR *name, LONG defval=0) = 0;
      virtual RELTIME getdelay (CHAR *name, RELTIME deftime=0) = 0;

      // Special property access
      virtual IACTIVEMODEL *getactivemodel() = 0;
      virtual IINSTANCE *getinterfacemodel() = 0;
      virtual BOOL getmoddata (BYTE **data, DWORD *size) = 0;

      // Access to the nodes and pins:
      virtual SPICENODE getspicenode (CHAR *namelist, BOOL required) = 0;
      virtual IDSIMPIN *getdsimpin (CHAR *namelist, BOOL required) = 0;

      // Logging and messaging:
      virtual VOID log (CHAR *msg, ...) = 0;
      virtual VOID warning (CHAR *msg, ...) = 0;
      virtual VOID error (CHAR *msg, ...) = 0;
      virtual VOID fatal (CHAR *msg, ...) = 0;
      virtual BOOL message (CHAR *msg, ...) = 0;

      // Pop-up window support:
      virtual IPOPUP *createpopup (CREATEPOPUPSTRUCT *cps) = 0;
      virtual VOID deletepopup (POPUPID id) = 0;

      // Virtual debug monitor interface:
      virtual BOOL setvdmhlr (class ICPU *) = 0;

      // Generic memory loader:
      virtual BOOL loadmemory (CHAR *filename, VOID *buffer, UINT size, UINT base=0, UINT shift=0) = 0;

      // Access to bus pins
      virtual IBUSPIN *getbuspin (CHAR *namestem, UINT base, UINT width, BOOL required) = 0;
      virtual IBUSPIN *getbuspin (CHAR *name, IDSIMPIN **pins, UINT width) = 0;
 };


/*********************************************************************
*****  Analogue Modelling Interface ****
***************************************/

typedef DOUBLE SPICEFREQ;

enum SPICEMODES
 { SPICETRAN=0x1,
   SPICEAC=0x2,
   SPICEDC=0x70,
   SPICEDCOP=0x10,
   SPICETRANOP=0x20,
   SPICEDCTRANCURVE=0x40,
   SPICEINITFLOAT=0x100,
   SPICEINITJCT=0x200,
   SPICEINITFIX=0x400,
   SPICEINITSMSIG=0x800,
   SPICEINITTRAN=0x1000,
   SPICEINITPRED=0x2000,
   SPICEUIC=0x10000l
 };

enum SPICEVARS
 { SPICETIME,
   SPICEOMEGA,
   SPICEDELTA,
   SPICEGMIN,
   SPICEDELMIN,
   SPICEMINBREAK,
   SPICESRCFACT,
   SPICEFINALTIME,

   // Conv Tolerances?
 };

class ISPICECKT
 { public:
      virtual BOOL ismode   (SPICEMODES flags) = 0;
      #ifdef __SC__
      virtual DOUBLE sysvar (SPICEVARS var) = 0;
      #else
      virtual VOID sysvar (DOUBLE *result, SPICEVARS var) = 0;
      #endif
      virtual DOUBLE &statevar (INT s, INT n) = 0;
      virtual DOUBLE &rhs (SPICENODE n) = 0;
      virtual DOUBLE &rhsold (SPICENODE n) = 0;
      virtual DOUBLE &irhs (SPICENODE n) = 0;
      virtual DOUBLE &irhsold (SPICENODE n) = 0;
      virtual SPICENODE getnode (CHAR *netname) = 0;
      virtual SPICENODE newvoltnode (CHAR *partid, CHAR *nodename) = 0;
      virtual SPICENODE newcurnode (CHAR *partid, CHAR *nodename) = 0;
      virtual VOID delnode (SPICENODE node) = 0;
      virtual DOUBLE *allocsmp (SPICENODE node1, SPICENODE node2) = 0;
      virtual BOOL setbreak (REALTIME time) = 0;
      virtual VOID suspend (IINSTANCE *instance, CHAR *msg) = 0;
      virtual INT  allocvars (INT n) = 0;
      virtual VOID integrate (DOUBLE *geq, DOUBLE *ceq, DOUBLE cap, INT statevars) =  0;
      virtual VOID truncerr (INT statevars, DOUBLE *timestep) = 0;
 };

class ISPICEMODEL
 { public:
      virtual INT  isanalog (CHAR *pinname) = 0;
      virtual VOID setup (IINSTANCE *, ISPICECKT *) = 0;
      virtual VOID runctrl (RUNMODES mode) = 0;
      virtual VOID actuate (REALTIME time, ACTIVESTATE newstate) = 0;
      virtual BOOL indicate (REALTIME time, ACTIVEDATA *newstate) = 0;
      virtual VOID dcload (REALTIME time, SPICEMODES mode, DOUBLE *oldrhs, DOUBLE *newrhs) = 0;
      virtual VOID acload (SPICEFREQ omega, DOUBLE *rhs, DOUBLE *irhs) = 0;
      virtual VOID trunc  (REALTIME time,  REALTIME *newtimestep) = 0;
      virtual VOID accept (REALTIME time, DOUBLE *rhs) = 0;
 };



/*********************************************************************
*****  Digital Modelling Interface ****
**************************************/

// DSIM Type Definitions:
typedef long EVENTID;
#define EID_BREAKPOINT 0x8000000

// Simulator Modes
enum DSIMMODES
 { DSIMBOOT       = 0x01,
   DSIMSETTLE     = 0x02,
   DSIMNORMAL     = 0x04,
   DSIMEND        = 0x08
 };

enum DSIMVARS
 { DSIMTIMENOW=-1,
   DSIMTDSCALE
 };

// Pin drive flags
#define DPF_INPUT       0x01
#define DPF_OUTPUT      0x02
#define DPF_BIDIR       0x03

// State polarity bits:
#define SP_UNDEFINED    0x00           // The undefined state.
#define SP_LOW          0x01           // Polarity Low.
#define SP_FLOAT        0x02           // Polarity Float (i.e. half volts).
#define SP_HIGH         0x03           // Polarity High.
#define SP_MASK         0x03           // Use to mask off polarity.

// State strength bits:
#define SS_FLOAT        0x00           // Strength Float.
#define SS_DRIVEN       0x04           // Flag bit for driven state.
#define SS_WEAK         0x04           // Strength Weak.
#define SS_STRONG       0x0C           // Strength Strong.
#define SS_INJECT       0x14           // Strength for generators.
#define SS_POWER        0x1C           // Strength Power rail.
#define SS_MASK         0x1C           // Used to mask off strength.

// State flag bits:
#define SF_CONTENTION   0x20           //
#define SF_MESSAGE      0x40           // Message associated with state (?)
#define SF_MASK         0xE0           // Use to mask off flags.

// State constants:
// Application code should deal only with these values;
// Manipulation of contention  and message flags is a kernel function.
enum STATE
 { UNDEFINED = 0,
   TSTATE    = 1,
   FSTATE    = -1,
   PLO = SS_POWER+SP_LOW,
   ILO = SS_INJECT+SP_LOW,
   SLO = SS_STRONG+SP_LOW,
   WLO = SS_WEAK+SP_LOW,
   FLT = SS_FLOAT+SP_FLOAT,
   WHI = SS_WEAK+SP_HIGH,
   SHI = SS_STRONG+SP_HIGH,
   IHI = SS_INJECT+SP_HIGH,
   PHI = SS_POWER+SP_HIGH,
   WUD = SS_WEAK+SP_UNDEFINED,
   SUD = SS_STRONG+SP_UNDEFINED
 };

// Functions for testing states.
inline BOOL islow (STATE s) { return (s & SP_MASK) == SP_LOW; }
inline BOOL ishigh (STATE s) { return (s & SP_MASK) == SP_HIGH; }
inline BOOL isfloating (STATE s) { return (s & SP_MASK) == SP_FLOAT; }
inline BOOL iscontention (STATE s) { return s & SF_CONTENTION; }
inline BOOL isdefined (STATE s) { return s != SP_UNDEFINED; }
inline BOOL ishighlow (STATE s) { return s & 1; }
inline INT  polarity (STATE s) { return s & SP_MASK; }
inline INT  strength (STATE s) { return s & SS_MASK; }

// Type for memory address
typedef DWORD ADDRESS;

// This is declared so we can use pointers to them, but the implementation is not exposed:
class EVENT;
class IDSIMMODEL;

// This declares a pointer to a function which a model
// can specify to be called for events on a particular pin or group of pins.
typedef VOID (IDSIMMODEL::*PINHANDLERFN)(ABSTIME time, DSIMMODES mode);
typedef VOID (IDSIMMODEL::*CALLBACKHANDLERFN)(ABSTIME time, EVENTID id);


// Interface to a digital component instance.
class IDSIMCKT
 { public:
      #ifdef __SC__
      inline  ABSTIME systime() { DOUBLE t = sysvar(DSIMTIMENOW); return *(ABSTIME *)&t; }
      virtual DOUBLE sysvar (DSIMVARS var) = 0;
      #else
      inline  VOID systime(ABSTIME *at) { sysvar((DOUBLE *)at, DSIMTIMENOW); }
      virtual VOID sysvar (DOUBLE *result, DSIMVARS var) = 0;
      #endif
      virtual EVENT *setcallback (ABSTIME evttime, IDSIMMODEL *model, EVENTID id) = 0;
      virtual BOOL cancelcallback (EVENT *event, IDSIMMODEL *model) = 0;
      virtual VOID setbreak (ABSTIME breaktime) = 0;
      virtual VOID suspend (IINSTANCE *instance, CHAR *msg) = 0;
      virtual EVENT *setcallbackex (ABSTIME evttime, IDSIMMODEL *model, CALLBACKHANDLERFN func, EVENTID id) = 0;
      virtual DSIMNODE newnode (CHAR *partid, CHAR *nodename) = 0;
      virtual IDSIMPIN *newpin (IINSTANCE *, DSIMNODE node, CHAR *name, DWORD flags) = 0;
      virtual EVENT *setclockcallback (ABSTIME starttime, RELTIME period, IDSIMMODEL *model, CALLBACKHANDLERFN func, EVENTID id) = 0;
 };

// Classic Interface to a digital pin object.

class IDSIMPIN1
 { public:
      virtual BOOL invert() = 0;
      virtual STATE istate() = 0;
      virtual BOOL issteady () = 0;         // Will false for return *any* change of activity.
      virtual INT  activity () = 0;
      virtual BOOL isactive () = 0;
      virtual BOOL isinactive () = 0;
      virtual BOOL isposedge () = 0;
      virtual BOOL isnegedge () = 0;
      virtual BOOL isedge () = 0;           // Will return true only for a full edge transition.
      virtual EVENT *setstate (ABSTIME time, RELTIME tlh, RELTIME thl, RELTIME tgq, STATE state) = 0;
      virtual EVENT *setstate (ABSTIME time, RELTIME tgq, STATE state) = 0;
      virtual VOID setstate (STATE state) = 0;
      virtual VOID sethandler (IDSIMMODEL *model, PINHANDLERFN phf) = 0;
      virtual DSIMNODE getnode() = 0;
      virtual STATE getstate() = 0;
 };

// Alternate drive interface interface - APIVER 1.10

class IDSIMPIN2 : public IDSIMPIN1
 { public:
      virtual VOID settiming (RELTIME tlh, RELTIME thl, RELTIME tgq) = 0;
      virtual VOID setstates (STATE tstate, STATE fstate, STATE zstate) = 0;
      virtual EVENT *drivebool (ABSTIME time, BOOL flag) = 0;
      virtual EVENT *drivestate (ABSTIME time, STATE state) = 0;
      virtual EVENT *drivetristate (ABSTIME time) = 0;
 };


// Interface to bus pin object - new in APIVER 1.10
class IBUSPIN
 { public:
      virtual VOID  settiming (RELTIME tlh, RELTIME thl, RELTIME tz) = 0;
      virtual VOID  setstates (STATE tstate, STATE fstate, STATE zstate) = 0;
      virtual VOID  sethandler (IDSIMMODEL *model, PINHANDLERFN phf) = 0;
      virtual VOID  drivebusvalue (ABSTIME time, DWORD value) = 0;
      virtual VOID  drivetristate (ABSTIME time) = 0;
      virtual VOID  drivebitstate (ABSTIME time, UINT bit, STATE state) = 0;
      virtual DWORD getbusvalue() = 0;
      virtual DWORD getbusdrive() = 0;
      virtual STATE getbitstate(UINT bit) = 0;
 };

// Interface to a digital model class.
class IDSIMMODEL
 { public:
      virtual INT  isdigital (CHAR *pinname) = 0;
      virtual VOID setup (IINSTANCE *instance, IDSIMCKT *dsim) = 0;
      virtual VOID runctrl (RUNMODES mode) = 0;
      virtual VOID actuate (REALTIME time, ACTIVESTATE newstate) = 0;
      virtual BOOL indicate (REALTIME time, ACTIVEDATA *newstate) = 0;
      virtual VOID simulate (ABSTIME time, DSIMMODES mode) = 0;
      virtual VOID callback (ABSTIME time, EVENTID eventid) = 0;
 };


/*********************************************************************
*****  Mixed Model Interface ****
********************************/

// Interface to a mixed mode model class:
// This is actually just a combination of an ISPICEMODEL and an IDSIMMODEL.
class IMIXEDMODEL : public ISPICEMODEL, public IDSIMMODEL
 { public:
      // Pin typing and connection functions:
      virtual INT  isanalog (CHAR *pinname) = 0;
      virtual INT  isdigital (CHAR *pinname) = 0;

      // Common Functions:
      virtual VOID runctrl (RUNMODES mode) = 0;
      virtual VOID actuate (REALTIME time, ACTIVESTATE newstate) = 0;
      virtual BOOL indicate (REALTIME time, ACTIVEDATA *newstate) = 0;

      // Functions called by SPICE:
      virtual VOID setup (IINSTANCE *, ISPICECKT *spice) = 0;
      virtual VOID dcload (REALTIME time, SPICEMODES mode, DOUBLE *oldrhs, DOUBLE *newrhs) = 0;
      virtual VOID acload (SPICEFREQ omega, DOUBLE *rhs, DOUBLE *irhs) = 0;
      virtual VOID trunc  (REALTIME time,  REALTIME *newtimestep) = 0;
      virtual VOID accept (REALTIME time, DOUBLE *rhs) = 0;

      // Functions called by DSIM:
      virtual VOID setup (IINSTANCE *, IDSIMCKT *dsim) = 0;
      virtual VOID simulate (ABSTIME time, DSIMMODES mode) = 0;
      virtual VOID callback (ABSTIME time, EVENTID eventid) = 0;
 };

/*********************************************************************
*****  Active Event Messaging ****
*********************************/

// Struct for active component animation event:
enum ACTIVEDATATYPES
 { ADT_VOID=-1,
   ADT_REAL,
   ADT_BOOLEAN,
   ADT_INTEGER,
   ADT_STATE,
   ADT_PINVOLTAGE,
   ADT_PINSTATE,
   ADT_WIREINFO,
   ADT_SPICEDATA,
   ADT_DSIMDATA,
   ADT_USER=100
 };

struct SPICEDATA
 { DWORD numtimepoints;
   DWORD numpins;
   REALTIME *timepoints;
   DOUBLE *nodedata;
 };

struct DSIMDATA
 { DWORD numtimepoints;
   DWORD numpins;
   ABSTIME *timepoints;
   STATE *nodedata;
 };

struct ACTIVEDATA
 { ACTIVEDATATYPES type;
   union
    { INT intval;
      DOUBLE realval;
      STATE stateval;
      DOUBLE wireinfo[2];     // Voltage and current
      SPICEDATA spicedata;
      DSIMDATA dsimdata;
      VOID *userinfo;         // Pointer to user data
    };
 };

struct ACTIVEEVENT
 { DWORD instance;            // Design Global Instance value for target indicator
   DWORD callback;            // Callback Pointer to the active component object
   DWORD element;             // Sub-element within parent.
   ACTIVEDATA data;           // Data structure passed to models:
 };


/*********************************************************************
***** Popup Window Support ****
******************************/

// Data types and formats used by various popup window types
enum DATATYPES    { DT_VOID=-1, DT_STRING, DT_TEXT,  DT_BYTE, DT_WORD, DT_DWORD, DT_QWORD, DT_IEEE_FLOAT, DT_IEEE_DOUBLE, DT_HTEC_FLOAT, DT_MCHP_FLOAT, DT_BIGENDIAN=0x8000 };
enum DISPFORMATS  { DF_VOID=-1, DF_BINARY, DF_OCTAL, DF_HEXADECIMAL, DF_SIGNED, DF_UNSIGNED, DF_FLOAT, DF_TEXT };

// Backward compatibility
#define DT_LSF_WORD  (DT_WORD)
#define DT_MSF_WORD  (DT_WORD|DT_BIGENDIAN)
#define DT_LSF_DWORD (DT_DWORD)
#define DT_MSF_DWORD (DT_DWORD|DT_BIGENDIAN)
#define DT_LSF_QWORD (DT_QWORD)
#define DT_MSF_QWORD (DT_QWORD|DT_BIGENDIAN)

// Limits that can be used for arrays, menus IDs etc.
#define MAXDATATYPES    10
#define MAXDISPFORMATS  10


// ID's for system-global  popups:
#define PID_LOGWINDOW   0
#define PID_WATCHWINDOW 1

// Interface through which windows messages can be passed to a USERPOPUP's owner:
class IMSGHLR
 { public:
      virtual LRESULT msghlr (HWND, MESSAGE, WPARAM, LPARAM) = 0;
 };

// User defined popup window:
class IUSERPOPUP
 { public:
      virtual CHAR *getprop (CHAR *key) = 0;
      virtual VOID setprop (CHAR *key, CHAR *value) = 0;
      virtual VOID setmsghlr (IMSGHLR *handler) = 0;
      virtual LRESULT callwindowproc (MESSAGE msg, WPARAM warg, LPARAM larg) = 0;
 };

// Debug/logging window:
class IDEBUGPOPUP
 { public:
      virtual VOID print (CHAR *msg, ...) = 0;
      virtual VOID dump (const BYTE *ptr, UINT nbytes, UINT base=0) = 0;
 };

// Status display window:
class ISTATUSPOPUP
 { public:
      virtual VOID setarea (UINT columns, UINT rows, UINT border, BOOL copy) = 0;
      virtual VOID setcursorto (UINT pixelx, UINT pixely) = 0;
      virtual UINT getcharwidth (VOID) = 0;
      virtual UINT getcharheight (VOID) = 0;
      virtual VOID print (CHAR *message, ...) = 0;
      virtual VOID print (INT col, INT row, COLOUR textcolour, CHAR *msg, ...) = 0;
      virtual VOID setctabstops (const INT ts[], INT n) = 0;
      virtual VOID setptabstops (const INT ts[], INT n) = 0;
      virtual VOID clear (COLOUR bkcolour=NOCOLOUR) = 0;
      virtual BOOL setredraw (BOOL on_off, BOOL redraw_now) = 0;
      virtual VOID repaint (VOID) = 0;
 };

// Memory dump window:
class IMEMORYPOPUP
 { public:
      virtual VOID setmemory (ADDRESS baseaddr, BYTE *data, UINT nbytes) = 0;
      virtual VOID repaint (VOID) = 0;
 };


// Watch window interface
#define WATCHITEM_NAME_SIZE 128
#define WATCHITEM_ADDR_SIZE 32

class IWATCHPOPUP
 { public:
      // Offset is the byte offset within the memory block independent of any 'base' associated with the memory
      // window. For example, if offset is '4' we are refering to the fourth byte in the memory block; if the memory
      // block as an implied base of, say, 12, this would be address '16'.
      virtual VOID addnameditem (CHAR *name, IMEMORYPOPUP *, DWORD offset, DATATYPES type, DISPFORMATS format) = 0;

      // This interfaces are used by PROSPICE itself to poll for watchpoints
      virtual BOOL initwatchpoints () = 0;
      virtual BOOL testwatchpoints () = 0;

 };



/*********************************************************************
**** Definitions for VSM Debug Support ****
******************************************/

// See VDM.HPP
struct VDM_COMMAND;

// Variable Descriptor
struct VARITEM
 { CHAR name[WATCHITEM_NAME_SIZE];
   DWORD loader, seg;
   ADDRESS address;
   DATATYPES type;
   DISPFORMATS format;
   DWORD size;
   ADDRESS scope_begin;
   ADDRESS scope_end;
 };

// Variable Data structure
struct VARDATA
 { CHAR addr[WATCHITEM_ADDR_SIZE];
   DATATYPES type;
   BYTE *memory;
   DWORD memsize;
   DWORD offset;  // from start of memory block.
 };


// CPU Interface - CPU models derive off this to implement debug support.
class ICPU
 { public:
      virtual LRESULT vdmhlr (VDM_COMMAND *cmd, BYTE *data) = 0;
      virtual VOID loaddata (INT format, INT seg, ADDRESS address, BYTE *data, INT numbytes) = 0;
      virtual VOID disassemble (ADDRESS address, INT numbytes) = 0;
      virtual BOOL getvardata (VARITEM *vip, VARDATA *vdp) = 0;
 };

// Source debugging window:
class ISOURCEPOPUP
 { public:
      virtual BOOL addsdifile (CHAR *file) = 0;
      virtual BOOL setpcaddr (ADDRESS addr) = 0;
      virtual BOOL isbreakpoint (ADDRESS addr) = 0;
      virtual BOOL iscurrentline (ADDRESS addr) = 0;
      virtual BOOL findfirstbpt (ADDRESS *addr) = 0;
      virtual BOOL findnextbpt (ADDRESS *addr) = 0;

      virtual BOOL addsrcfile (CHAR *file, BOOL lowlevel) = 0;
      virtual VOID addcodeline (INT srclinenum, ADDRESS address) = 0;
      virtual VOID addcodelabel (CHAR *label, ADDRESS address) = 0;
      virtual VOID update () = 0;

      virtual BOOL getsteptoaddr (ADDRESS *addr) = 0;                            // Added in release 5.20 SP1 (APIVER 1.03)
      virtual VOID setinsertpos (INT fileid, INT linenum, BOOL newblock) = 0;
      virtual VOID insertline (ADDRESS addr, CHAR *opcodes, CHAR *srctext) = 0;  // Insert a new line at the current position. (APIVER 1.05)
      virtual BOOL findfirstsrcline (ADDRESS *addr) = 0;
      virtual BOOL findnextsrcline (ADDRESS *addr) = 0;
      virtual CHAR *findlabel (ADDRESS addr) = 0;                                // Return label for specified address (APIVER 1.05)
 };

// Variable Window - new in APIVER 1.05
class IVARPOPUP
 { public:
      virtual VOID setcpu (ICPU *cpu) = 0;
      virtual VOID additem (VARITEM *vip) = 0;
 };



// Loader formats
#define FORMAT_BIN    0
#define FORMAT_HEX    1
#define FORMAT_S19    2
#define FORMAT_OMF51  3
#define FORMAT_UBROF  4
#define FORMAT_COD    5
#define FORMAT_BASIC  6
#define FORMAT_COFF   7

// Extern loader functions - implemented in LOADER.DLL
extern "C"
{ BOOL load_auto (CHAR *file, IINSTANCE *, ISOURCEPOPUP *, IVARPOPUP *, ICPU *);
  BOOL load_bin (CHAR *file, IINSTANCE *, ISOURCEPOPUP *, IVARPOPUP *, ICPU *);
  BOOL load_hex (CHAR *file, IINSTANCE *, ISOURCEPOPUP *, IVARPOPUP *, ICPU *);
  BOOL load_s19 (CHAR *file, IINSTANCE *, ISOURCEPOPUP *, IVARPOPUP *, ICPU *);
  BOOL load_omf51 (CHAR *file, IINSTANCE *, ISOURCEPOPUP *, IVARPOPUP *, ICPU *);
  BOOL load_ubrof (CHAR *file, IINSTANCE *, ISOURCEPOPUP *, IVARPOPUP *, ICPU *);
  BOOL load_cod (CHAR *file, IINSTANCE *, ISOURCEPOPUP *, IVARPOPUP *, ICPU *);
  BOOL load_basic (CHAR *file, IINSTANCE *, ISOURCEPOPUP *, IVARPOPUP *, ICPU *);
  BOOL load_coff (CHAR *file, IINSTANCE *, ISOURCEPOPUP *, IVARPOPUP *, ICPU *);
};

// Extern VSM Tools functions - implemented in VSMTOOLS.LIB
extern "C"
 { BOOL licence_model (ILICENCESERVER *ils, BOOL proonly=FALSE);
 };


/*********************************************************************
*****  COM Instance Creation/Destruction ****
********************************************/

extern "C"
 { typedef IACTIVEMODEL *CREATEACTIVEMODELFN (CHAR *device, ILICENCESERVER *ils);
   typedef VOID DELETEACTIVEMODELFN (IACTIVEMODEL *model);
   typedef ISPICEMODEL *CREATESPICEMODELFN (CHAR *device, ILICENCESERVER *ils);
   typedef VOID DELETESPICEMODELFN (ISPICEMODEL *);
   typedef IDSIMMODEL  *CREATEDSIMMODELFN (CHAR *device, ILICENCESERVER *ils);
   typedef VOID DELETEDSIMMODELFN (IDSIMMODEL *);
   typedef IMIXEDMODEL *CREATEMIXEDMODELFN (CHAR *device, ILICENCESERVER *ils);
   typedef VOID DELETEMIXEDMODELFN (IMIXEDMODEL *);
 }

#pragma pack(pop)

#endif



