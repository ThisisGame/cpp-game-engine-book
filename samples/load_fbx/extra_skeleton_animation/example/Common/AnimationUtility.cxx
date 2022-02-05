/****************************************************************************************

   Copyright (C) 2015 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

#include "AnimationUtility.h"

FbxAnimLayer * CreateDefaultAnimStackAndLayer(FbxScene * pScene, FbxAnimStack* &pAnimStack)
{
    const char * DEFAULT_STACK_NAME = "Base_Stack";
    const char * DEFAULT_LAYER_NAME = "Base_Layer";

    // Create one animation stack
    pAnimStack = FbxAnimStack::Create(pScene, DEFAULT_STACK_NAME);

    // all animation stacks need, at least, one layer.
    FbxAnimLayer* lAnimLayer = FbxAnimLayer::Create(pScene, DEFAULT_LAYER_NAME);
    pAnimStack->AddMember(lAnimLayer);

    return lAnimLayer;
}
