//
// Created by x1y2z on 20.04.2023.
//

#pragma once

/*===========================================================================*
 *--- Global Types ----------------------------------------------------------*
 *===========================================================================*/
typedef struct RwMatrixWeights RwMatrixWeights;

/**
 * \ingroup rpskin
 * \struct RwMatrixWeights
 * A structure for defining up to four matrix weights per vertex.
 * Not all entries need to be used.
 *
 * \note
 * Values should be sorted, such that any zero 0.0f entries appear
 * after the valid weights. Any weights that appear after a zero
 * entry will be ignored.
 *
 * \see RpSkinCreate
 */
struct RwMatrixWeights
{
    RwReal w0; /**< The first matrix weight.  */
    RwReal w1; /**< The second matrix weight. */
    RwReal w2; /**< The third matrix weight.  */
    RwReal w3; /**< The fourth matrix weight. */
};

/**
 * \ingroup rpskin
 * \struct RpSkin
 *
 * Skin object. This should be considered an opaque type.
 * Use the RpSkin API functions to access.
 *
 * \see RpSkinCreate
 * \see RpSkinDestroy
 */
typedef struct RpSkin RpSkin;

/*---------------------------------------------------------------------------*
 *-   Skin pipeline                                                         -*
 *---------------------------------------------------------------------------*/

/**
 * \ingroup rpskin
 * \ref RpSkinType defines the different ways a skinned atomic can
 * be rendered. Once a skinned \ref RpGeometry has been attached to
 * an \ref RpAtomic the atomic must be setup with the correct skin
 * rendering pipeline with \ref RpSkinAtomicSetType.
 */
enum RpSkinType
{
    rpNASKINTYPE        = 0, /**<Invalid skin pipeline.           */
    rpSKINTYPEGENERIC   = 1, /**<Generic skin rendering.          */
    rpSKINTYPEMATFX     = 2, /**<Material effects skin rendering. */
    rpSKINTYPETOON      = 3, /**<Toon skin rendering.   To use this
                               * flag the \ref rptoon plugin (part
                               * of the FX Pack) needs to be
                               * installed.          */
    rpSKINTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpSkinType RpSkinType;


RpSkin* RpSkinGeometryGetSkin(RpGeometry* geometry);
RpHAnimHierarchy* RpSkinAtomicGetHAnimHierarchy(const RpAtomic* atomic);