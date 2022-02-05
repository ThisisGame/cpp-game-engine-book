/****************************************************************************************

   Copyright (C) 2015 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

#ifndef INCLUDE_GEOMETRY_UTILITY_H_
#define INCLUDE_GEOMETRY_UTILITY_H_

#include <fbxsdk.h>

/** Create a pyramid mesh with specific bottom width and height, and attach it to a node.
  * /param pScene The scene in which the pyramid mesh is created.
  * /param pName The name of the pyramid mesh and the node to which the pyramid is attached.
  * /param pBottomWidht The width of the bottom side of the pyramid.
  * /param pHeight The height of the pyramid.
  * /return Return the node to which the pyramid mesh is attached.
  */
FbxNode * CreatePyramid(FbxScene * pScene, const char * pName, double pBottomWidth, double pHeight);

FbxNode* CreateCube(FbxScene* pScene, const char* pName, FbxDouble3& pLclTranslation);

#endif // INCLUDE_GEOMETRY_UTILITY_H_
