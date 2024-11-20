#pragma once


#include "tkreg.h"
//#include "badevice.h"
#include "memory.h"
#include "badevice.h"
#include "rwstring.h"
//#include "rwstring.h"
//#include "drvmodel.h"

/****************************************************************************
 Macros
 */

/* RWPUBLIC */
/* Finding MSBs */

#define RWBYTEFINDMSB(a) \
   (_rwMsbBit[(a)]-1)

#define RWWORDFINDMSB(a) \
   (((a)&0xff00)?RWBYTEFINDMSB((a)>>8)+8: RWBYTEFINDMSB(a))

#define RWLONGFINDMSB(a) \
   (((a)&0xffff0000UL)?RWWORDFINDMSB((a)>>16)+16: RWWORDFINDMSB(a))

/****************************************************************************
 Defines
 */

/* macros used to access plugin data in objects */
#define RWPLUGINOFFSET(_type, _base, _offset)                   \
   ((_type *)((RwUInt8 *)(_base) + (_offset)))

#define RWPLUGINOFFSETCONST(_type, _base, _offset)              \
   ((const _type *)((const RwUInt8 *)(_base) + (_offset)))

/* macro used to access global data structure (the root type is RwGlobals) */
#define RWSRCGLOBAL(variable) \
   (((RwGlobals *)RwEngineInstance)->variable)

#define RWASSERTISTYPE(_f, _t) \
   RWASSERT((((const RwObject *)(_f))->type)==(_t))

/****************************************************************************
 Global Types
 */

enum RwEngineStatus
{
    rwENGINESTATUSIDLE = 0,                     /* This needs to be zero */
    rwENGINESTATUSINITED = 1,
    rwENGINESTATUSOPENED = 2,
    rwENGINESTATUSSTARTED = 3,
    rwENGINESTATUSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwEngineStatus RwEngineStatus;

typedef struct RwGlobals RwGlobals;

struct RwGlobals
{
#ifdef RWDEBUG
        RwDebugHandler      debugFunction;          /* debug string handler */
        RwInt32             debugStackDepth;        /* current depth of function stack */
        RwBool              debugTrace;             /* is function tracing enabled */
//@{ 20050511 DDonSS : RWMESSAGE ��¿��� ������
        RwBool				debugMessage;

//@} DDonSS
//@{ 20050513 DDonSS : Threadsafe �׽�Ʈ��
        RwUInt32			debugMainThreadId;		// ���� Thread�� Id
//@} DDonSS

#endif

		//@{ Jaewon 20050603
		// For debug-logging in a crash
		//DebugLogFunc		debugLogFunc;
		//@} Jaewon

        /* Current entities */
        void                *curCamera;             /* Current camera */
        void                *curWorld;              /* Current World */

        /* Checking counters */
        RwUInt16            renderFrame;            /* Camera display count */
        RwUInt16            lightFrame;             /* Used to ensure each light is applied only once. */
        RwUInt16            pad[2];                 /* Longword align it again */

        /* For the currently accessed device */
        RwDevice            dOpenDevice;

        /* Standard renderers and functions */
        RwStandardFunc      stdFunc[rwSTANDARDNUMOFSTANDARD];

        /* All of the frames which have been updated */
        RwLinkList          dirtyFrameList;

        uint8_t fileFuncs[(VER_x32 ? 0x2C : 0x58)];

        /* The string functions */
        RwStringFunctions   stringFuncs;

        /* The memory allocation functions */
        RwMemoryFunctions   memoryFuncs;
#ifdef RWDEBUG
        RwBool              freeListExtraDebug;
#endif /* RWDEBUG */

        /* virtual memory alloc/free functions */
        RwMemoryAllocFn         memoryAlloc;
        RwMemoryFreeFn          memoryFree;

        RwMetrics           *metrics;

        /* Current engine status */
        RwEngineStatus      engineStatus;

        /* Resource arena init size. */
        RwUInt32            resArenaInitSize;

		/* 2005.4.1 gemani */
		//RwBool				bLockSpecial1;					// For Radeon 7000~7500

//@{ 2005414 DDonSS : Threadsafe
#if defined WIN32
#if ( defined USE_EXTERNAL_LOCK_FUNCTION ) || ( defined USE_EXTERNAL_LOCK_FUNCTION_WITH_FILEINFO )
		CsLockFunc			CsInitLock;
		CsLockFunc			CsLock;
		CsLockFunc			CsUnlock;
		CsLockFunc			CsDelLock;
#endif // ( defined USE_EXTERNAL_LOCK_FUNCTION ) || ( defined USE_EXTERNAL_LOCK_FUNCTION_WITH_FILEINFO )

#if defined USE_THREADSAFE_FRAME
		// Critical Section for dirtyFrameList
		CRITICAL_SECTION	csFrame;
#endif // defined USE_THREADSAFE_FRAME

#if defined USE_THREADSAFE_RESARENA
		// Critical Section for Resource Arena
		CRITICAL_SECTION	csResArena;
#endif // defined USE_THREADSAFE_RESARENA

#if defined USE_THREADSAFE_ATOMIC
		// Critical Section for Atomic Sync & remove
		CRITICAL_SECTION	csAtomic;
#endif // defined USE_THREADSAFE_ATOMIC

#if defined USE_THREADSAFE_GEOMETRY
		// Critical Section for global geometry access
		CRITICAL_SECTION	csGeometry;
#endif // defined USE_THREADSAFE_GEOMETRY


#else // defined WIN32

#if defined USE_THREADSAFE_FRAME
		void*				csFrame;
#endif // defined USE_THREADSAFE_FRAME

#if defined USE_THREADSAFE_RESARENA
		void*				csResArena;
#endif // defined USE_THREADSAFE_RESARENA

#endif // defined WIN32
//@} DDonSS
};
static_assert(sizeof(RwGlobals) == (VER_x32 ? 0x150 : 0x288));


typedef struct RwModuleInfo RwModuleInfo;
struct RwModuleInfo
{
        RwInt32     globalsOffset;
        RwInt32     numInstances;
};
static_assert(sizeof(RwModuleInfo) == 0x8);

/****************************************************************************
 Program wide globals
 */

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

#ifdef RWGLOBALSIZE
extern RwUInt32     ourGlobals[RWGLOBALSIZE / sizeof(RwUInt32)];
#define RwEngineInstance ourGlobals
#else /* RWGLOBALSIZE */

#ifdef _RWDLL
__declspec(dllimport) extern void         *RwEngineInstance;
#else
//extern void         *RwEngineInstance;
#endif

#endif /* RWGLOBALSIZE */

#if defined(_RWDLL)
__declspec(dllimport) extern RwInt8 _rwMsbBit[];
#else /* defined(_RWDLL) */
extern RwInt8 _rwMsbBit[];
#endif /* defined(_RWDLL) */

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */
