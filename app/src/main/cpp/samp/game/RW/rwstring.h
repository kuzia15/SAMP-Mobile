#pragma once

/* For size_t defn */
#include <stdlib.h>
#include <stdarg.h>

#ifdef __ELATE__
#include <machine/ansi.h>
#include <alloca.h>
#include <stdarg.h>
#endif

/* for RwStream */
#include "stream.h"

/*************************************************** Defines */

#define rwsprintf   RWSRCGLOBAL(stringFuncs).vecSprintf
#define rwvsprintf  RWSRCGLOBAL(stringFuncs).vecVsprintf
#define rwstrcpy    RWSRCGLOBAL(stringFuncs).vecStrcpy
#define rwstrncpy   RWSRCGLOBAL(stringFuncs).vecStrncpy
#define rwstrcat    RWSRCGLOBAL(stringFuncs).vecStrcat
#define rwstrncat   RWSRCGLOBAL(stringFuncs).vecStrncat
#define rwstrrchr   RWSRCGLOBAL(stringFuncs).vecStrrchr
#define rwstrchr    RWSRCGLOBAL(stringFuncs).vecStrchr
#define rwstrstr    RWSRCGLOBAL(stringFuncs).vecStrstr
#define rwstrcmp    RWSRCGLOBAL(stringFuncs).vecStrcmp
#define rwstrncmp   RWSRCGLOBAL(stringFuncs).vecStrncmp
#define rwstricmp   RWSRCGLOBAL(stringFuncs).vecStricmp
#define rwstrlen    RWSRCGLOBAL(stringFuncs).vecStrlen
#define rwstrupr    RWSRCGLOBAL(stringFuncs).vecStrupr
#define rwstrlwr    RWSRCGLOBAL(stringFuncs).vecStrlwr
#define rwstrtok    RWSRCGLOBAL(stringFuncs).vecStrtok
#define rwsscanf    RWSRCGLOBAL(stringFuncs).vecSscanf

#define rwstrdup(_result, _string)                \
do                                                \
{                                                 \
    _result = ((RwChar*)NULL);                    \
                                                  \
    if (((RwChar*)NULL) != (_string))             \
    {                                             \
        _result = (RwChar *)                      \
            RwMalloc( (rwstrlen(_string) + 1) *   \
                      sizeof (RwChar),            \
                      rwID_STRING |               \
                      rwMEMHINTDUR_EVENT);        \
                                                  \
        if (((RwChar*)NULL) != (_result))         \
        {                                         \
            rwstrcpy(_result, _string);           \
        }                                         \
    }                                             \
}                                                 \
while (0)


/****************************************************************************
 Global Types
 */

typedef int (*vecSprintfFunc)(RwChar *buffer,
                              const RwChar *format,
                              ...) /* __RWFORMAT__(printf, 2, 3) */; 
typedef int (*vecVsprintfFunc)(RwChar *buffer,
                               const RwChar *format,
                               va_list argptr); 
typedef RwChar *(*vecStrcpyFunc)(RwChar *dest,
                                 const RwChar *srce);
typedef RwChar *(*vecStrncpyFunc)(RwChar *dest,
                                  const RwChar *srce,
                                  size_t size);
typedef RwChar *(*vecStrcatFunc)(RwChar *dest,
                                 const RwChar *srce);
typedef RwChar *(*vecStrncatFunc)(RwChar *dest,
                                  const RwChar *srce,
                                  size_t size);
typedef RwChar *(*vecStrrchrFunc)(const RwChar *string,
                                  int findThis);
typedef RwChar *(*vecStrchrFunc)(const RwChar *string,
                                 int findThis);
typedef RwChar *(*vecStrstrFunc)(const RwChar *string,
                                 const RwChar *findThis);
typedef int (*vecStrcmpFunc)(const RwChar *string1,
                             const RwChar *string2);
typedef int (*vecStrncmpFunc)(const RwChar *string1,
                              const RwChar *string2,
                              size_t max_size);
typedef int (*vecStricmpFunc)(const RwChar *string1,
                              const RwChar *string2);
typedef size_t (*vecStrlenFunc)(const RwChar *string);
typedef RwChar *(*vecStruprFunc)(RwChar *string);
typedef RwChar *(*vecStrlwrFunc)(RwChar *string);
typedef RwChar *(*vecStrtokFunc)(RwChar *string, const RwChar *delimit);
typedef int (*vecSscanfFunc)(const RwChar *buffer,
                             const RwChar *format,
                             ...) /* __RWFORMAT__(scanf, 2, 3) */;

typedef struct RwStringFunctions RwStringFunctions;

struct RwStringFunctions
{
    vecSprintfFunc  vecSprintf ;
    vecVsprintfFunc vecVsprintf;
    vecStrcpyFunc   vecStrcpy;
    vecStrncpyFunc  vecStrncpy;
    vecStrcatFunc   vecStrcat;
    vecStrncatFunc  vecStrncat;
    vecStrrchrFunc  vecStrrchr;
    vecStrchrFunc   vecStrchr;
  //  vecStrstrFunc   vecStrstr;
    vecStrcmpFunc   vecStrcmp;
 //   vecStrncmpFunc  vecStrncmp;
    vecStricmpFunc  vecStricmp;
    vecStrlenFunc   vecStrlen;
    vecStruprFunc   vecStrupr;
    vecStrlwrFunc   vecStrlwr;
    vecStrtokFunc   vecStrtok;
    vecSscanfFunc   vecSscanf;
};
static_assert(sizeof(RwStringFunctions) == (VER_x32 ? 0x3C : 0x78));

/****************************************************************************
 Function prototypes
 */

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

extern RwBool
_rwStringOpen(void);

extern void
_rwStringClose(void);

extern RwBool
_rwStringDestroy(RwChar * string);

extern RwUInt32
_rwStringStreamGetSize(const RwChar * string);

extern const RwChar *
_rwStringStreamWrite(const RwChar * string, RwStream * stream);

extern RwChar *
_rwStringStreamFindAndRead(RwChar *string, RwStream * stream);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */
