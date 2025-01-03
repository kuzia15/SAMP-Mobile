//
// Created by x1y2z on 01.05.2024.
//

#pragma once

#include <stdint.h>

#define rwPI            ((RwReal)(3.1415926535f))
#define rwPIOVER2       (rwPI / (RwReal)(2.0f))
#define RWRAD2DEG(_x)   ((_x) * (((RwReal)180)/(rwPI)))
#define RWDEG2RAD(_x)   ((_x) * (rwPI/((RwReal)180)))

#define RWFUNCTION(name)       /* No op */
#define RWAPIFUNCTION(name)    /* No op */
#define RWASSERT(condition)    /* No op */
#define RWASSERTM(condition, messageArgs)    /* No op */
#define RWMESSAGE(args)        /* No op */
#define RWRETURN(value) return(value)
#define RWRETURNVOID() return
#define RWERROR(errorcode)      /* No op */
#define RXCHECKFORUSERTRAMPLING(_pipeline) /* No op */

#undef FALSE
#define FALSE 0

#undef TRUE
#define TRUE !FALSE

#undef MACRO_START
#define MACRO_START do

#undef MACRO_STOP
#define MACRO_STOP while(0)

#undef RWFORCEENUMSIZEINT
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))


typedef long        RwFixed;
typedef int32_t     RwInt32;
typedef uint32_t    RwUInt32;
typedef int16_t     RwInt16;
typedef uint16_t    RwUInt16;
typedef uint8_t     RwUInt8;
typedef int8_t      RwInt8;

typedef char        RwChar;
typedef float       RwReal;
typedef RwInt32     RwBool;


/* ------------------------------ RwV2d -------------------------*/
typedef struct RwV2d RwV2d;
/**
 * \ingroup rwv2d
 * \struct RwV2d
 * This type represents points in a 2D space, such as device
 * space, specified by the (x, y) coordinates of the point.
 */
struct RwV2d
{
    RwReal x;   /**< X value*/
    RwReal y;   /**< Y value */
};

/* ------------------------------ RwV3d -------------------------*/
typedef struct RwV3d RwV3d;
/**
 * \ingroup rwv3d
 * \struct RwV3d
 *  This type represents 3D points and vectors specified by
 * the (x, y, z) coordinates of a 3D point or the (x, y, z) components of a
 * 3D vector.
 */
struct RwV3d
{
    RwReal x;   /**< X value */
    RwReal y;   /**< Y value */
    RwReal z;   /**< Z value */
};

/* ------------------------------ RwRect -------------------------*/
typedef struct RwRect RwRect;
/**
 * \ingroup geometricaltypes
 * \struct RwRect
 * This type represents a 2D device space rectangle specified
 * by the position of the top-left corner (the offset x, y) and its width (w)
 * and height (h).
 */
struct RwRect
{
    RwInt32 x;  /**< X value of the top-left corner */
    RwInt32 y;  /**< Y value of the top-left corner */
    RwInt32 w;  /**< Width of the rectangle */
    RwInt32 h;  /**< Height of the rectangle */
};

/* ------------------------------ RwSphere -------------------------*/
typedef struct RwSphere RwSphere;
/**
 * \ingroup geometricaltypes
 * \struct RwSphere
 * This type represents a sphere specified by the position
 * of its center and its radius.
 */
struct RwSphere
{
    RwV3d center;   /**< Sphere center */
    RwReal radius;  /**< Sphere radius */
};
#define RwSphereAssign(_target, _source) ( *(_target) = *(_source) )


/* ------------------------------ RwObject -------------------------*/
typedef struct RwObject RwObject;

/**
 * \ingroup rwobject
 * \struct RwObject
 * This should be considered an opaque type. Use
 * the RwObject API functions to access.
 */

struct RwObject
{
    RwUInt8 type;                /**< Internal Use */
    RwUInt8 subType;             /**< Internal Use */
    RwUInt8 flags;               /**< Internal Use */
    RwUInt8 privateFlags;        /**< Internal Use */
    void   *parent;              /**< Internal Use */
    /* Often a Frame  */
};
static_assert(sizeof(RwObject) == (VER_x32 ? 0x8 : 0x10));

/**
 * \ingroup rwobject
 * \ref RwObjectCallBack
 * callback function supplied for object callback functions.
 *
 * \param  object   Pointer to the current object, supplied by
 *                  iterator.
 * \param  data     Pointer to developer-defined data structure.
 *
 * \return Pointer to the current object
 *
 * \see RwFrameForAllObjects
 *
 */
typedef RwObject *(*RwObjectCallBack)(RwObject *object, void *data);

/* Creation/cloning */
#define rwObjectCopy(d,s)                               \
MACRO_START                                             \
{                                                       \
    ((RwObject *)(d))->type =                           \
        ((const RwObject *)(s))->type;                  \
    ((RwObject *)(d))->subType =                        \
        ((const RwObject *)(s))->subType;               \
    ((RwObject *)(d))->flags =                          \
        ((const RwObject *)(s))->flags;                 \
    ((RwObject *)(d))->privateFlags =                   \
        ((const RwObject *)(s))->privateFlags;          \
    ((RwObject *)(d))->parent =                         \
        NULL;                                           \
}                                                       \
MACRO_STOP

#define rwObjectInitialize(o, t, s)                     \
MACRO_START                                             \
{                                                       \
    ((RwObject *)(o))->type = (RwUInt8)(t);             \
    ((RwObject *)(o))->subType = (RwUInt8)(s);          \
    ((RwObject *)(o))->flags = 0;                       \
    ((RwObject *)(o))->privateFlags = 0;                \
    ((RwObject *)(o))->parent = NULL;                   \
}                                                       \
MACRO_STOP

/* Debug */
#define RwObjectGetType(o)                  (((const RwObject *)(o))->type)

#define rwObjectSetType(o, t)               (((RwObject *)(o))->type) = (RwUInt8)(t)

/* Sub type */
#define rwObjectGetSubType(o)               (((const RwObject *)(o))->subType)
#define rwObjectSetSubType(o, t)            (((RwObject *)(o))->subType) = (RwUInt8)(t)

/* Flags */
#define rwObjectGetFlags(o)                 (((const RwObject *)(o))->flags)
#define rwObjectSetFlags(o, f)              (((RwObject *)(o))->flags) = (RwUInt8)(f)
#define rwObjectTestFlags(o, f)             ((((const RwObject *)(o))->flags) & (RwUInt8)(f))

/* Private flags */
#define rwObjectGetPrivateFlags(c)          (((const RwObject *)(c))->privateFlags)
#define rwObjectSetPrivateFlags(c,f)        (((RwObject *)(c))->privateFlags) = (RwUInt8)(f)
#define rwObjectTestPrivateFlags(c,flag)    ((((const RwObject *)(c))->privateFlags) & (RwUInt8)(flag))

/* Hierarchy */
#define rwObjectGetParent(object)           (((const RwObject *)(object))->parent)
#define rwObjectSetParent(c,p)              (((RwObject *)(c))->parent) = (void *)(p)
/* ------------------------------ RwObject end -------------------------*/

/* ------------------------------ RwLLLink -------------------------*/
typedef struct RwLLLink  RwLLLink;                     /*** RwLLLink ***/
/**
 * \ingroup fundtypesdatatypes
 * \struct RwLLLink
 * RwLLLink is an internal two way linked list pointer.  It contains
 * links to the previous and next RwLLLink's.  It is usually used in a
 * ring list fashion.
 */
struct RwLLLink
{
    RwLLLink *next;
    RwLLLink *prev;
};

#define rwLLLinkGetData(linkvar,type,entry)                             \
    ((type *)(((RwUInt8 *)(linkvar))-offsetof(type,entry)))

#define rwLLLinkGetConstData(linkvar,type,entry)                        \
    ((const type *)(((const RwUInt8 *)(linkvar))-offsetof(type,entry)))

#define rwLLLinkGetNext(linkvar)                                        \
    ((linkvar)->next)

#define rwLLLinkGetPrevious(linkvar)                                    \
    ((linkvar)->prev)

#define rwLLLinkInitialize(linkvar)                                     \
    ( (linkvar)->prev = (RwLLLink *)NULL,                               \
      (linkvar)->next = (RwLLLink *)NULL )

#define rwLLLinkAttached(linkvar)                                       \
    ((linkvar)->next)
/* ------------------------------ RwLLLink end -------------------------*/


/* ------------------------------ RwLinkList -------------------------*/
typedef struct RwLinkList RwLinkList;
/**
 * \ingroup fundtypesdatatypes
 * \struct RwLLLink
 * Both the next and previous directions of an RwLinkList are linked in a
 * ring list.  That is, to iterate over all the elements of this structure
 * store the starting link and iterate over the elements until
 * returning back to this first element.
 */
struct RwLinkList
{
    RwLLLink link;
};

#define rwLinkListInitialize(list)                                      \
    ( (list)->link.next = ((RwLLLink *)(list)),                         \
      (list)->link.prev = ((RwLLLink *)(list)) )
#define rwLinkListEmpty(list)                                           \
    (((list)->link.next) == (&(list)->link))
#define rwLinkListAddLLLink(list, linkvar)                              \
    ( (linkvar)->next = (list)->link.next,                              \
      (linkvar)->prev = (&(list)->link),                                \
      ((list)->link.next)->prev = (linkvar),                            \
      (list)->link.next = (linkvar) )
#define rwLinkListRemoveLLLink(linkvar)                                 \
    ( ((linkvar)->prev)->next = (linkvar)->next,                        \
      ((linkvar)->next)->prev = (linkvar)->prev )
#define rwLinkListGetFirstLLLink(list)                                  \
    ((list)->link.next)
#define rwLinkListGetLastLLLink(list)                                   \
    ((list)->link.prev)
#define rwLinkListGetTerminator(list)                                   \
    (&((list)->link))

/* ------------------------------ RwLinkList end -------------------------*/


/* ------------------------------ RwSurfaceProperties -------------------------*/
typedef struct RwSurfaceProperties RwSurfaceProperties;

/**
 * \ingroup fundtypesdatatypes
 * \struct RwSurfaceProperties
 *  This type represents the ambient, diffuse and
 * specular reflection coefficients of a particular geometry. Each coefficient
 * is specified in the range 0.0 (no reflection) to 1.0 (maximum reflection).
 * Note that currently the specular element is not used.
 */
struct RwSurfaceProperties
{
    RwReal ambient;   /**< ambient reflection coefficient */
    RwReal specular;  /**< specular reflection coefficient */
    RwReal diffuse;   /**< reflection coefficient */
};

#undef RwSurfacePropertiesAssign
#define RwSurfacePropertiesAssign(_target, _source) ( *(_target) = *(_source) )


/* ------------------------------ RwLine -------------------------*/
typedef struct RwLine RwLine;
/**
 * \ingroup geometricaltypes
 * \struct RwLine
 * This type represents a 3D line specified by the position
 * of its start and end points.
 */
struct RwLine
{
    RwV3d start;    /**< Line start */
    RwV3d end;      /**< Line end */
};

#undef RwLineAssign
#define RwLineAssign(_target, _source) ( *(_target) = *(_source) )


/* ------------------------------ RwPlane -------------------------*/
typedef struct RwPlane RwPlane;

/*
 * This type represents a plane
 */
struct RwPlane
{
    RwV3d normal;    /**< Normal to the plane */
    RwReal distance; /**< Distance to plane from origin in normal direction*/
};


enum RwPlaneType
{
    rwXPLANE = 0, /* These are deliberately multiples of sizeof(RwReal) */
    rwYPLANE = 4,
    rwZPLANE = 8,
    rwPLANETYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwPlaneType RwPlaneType;