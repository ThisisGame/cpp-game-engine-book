/****************************************************************************************

   Copyright (C) 2015 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

#include "GeometryUtility.h"

FbxNode * CreatePyramid(FbxScene * pScene, const char * pName, double pBottomWidth, double pHeight)
{
    FbxMesh * lPyramid = FbxMesh::Create(pScene, pName);

    // Calculate the vertices of the pyramid
    const double lBottomWidthHalf = pBottomWidth / 2;
    const FbxVector4 PyramidControlPointArray[] = 
    {
        FbxVector4(0, pHeight, 0),
        FbxVector4(lBottomWidthHalf, 0, lBottomWidthHalf),
        FbxVector4(lBottomWidthHalf, 0, -lBottomWidthHalf),
        FbxVector4(-lBottomWidthHalf, 0, -lBottomWidthHalf),
        FbxVector4(-lBottomWidthHalf, 0, lBottomWidthHalf)
    };

    // Initialize and set the control points of the mesh
    const int lControlPointCount = sizeof(PyramidControlPointArray) / sizeof(FbxVector4);
    lPyramid->InitControlPoints(lControlPointCount);
    for (int lIndex = 0; lIndex < lControlPointCount; ++lIndex)
    {
        lPyramid->SetControlPointAt(PyramidControlPointArray[lIndex], lIndex);
    }

    // Set the control point indices of the bottom side of the pyramid
    lPyramid->BeginPolygon();
    lPyramid->AddPolygon(1);
    lPyramid->AddPolygon(4);
    lPyramid->AddPolygon(3);
    lPyramid->AddPolygon(2);
    lPyramid->EndPolygon();

    // Set the control point indices of the front side of the pyramid
    lPyramid->BeginPolygon();
    lPyramid->AddPolygon(0);
    lPyramid->AddPolygon(1);
    lPyramid->AddPolygon(2);
    lPyramid->EndPolygon();

    // Set the control point indices of the left side of the pyramid
    lPyramid->BeginPolygon();
    lPyramid->AddPolygon(0);
    lPyramid->AddPolygon(2);
    lPyramid->AddPolygon(3);
    lPyramid->EndPolygon();

    // Set the control point indices of the back side of the pyramid
    lPyramid->BeginPolygon();
    lPyramid->AddPolygon(0);
    lPyramid->AddPolygon(3);
    lPyramid->AddPolygon(4);
    lPyramid->EndPolygon();

    // Set the control point indices of the right side of the pyramid
    lPyramid->BeginPolygon();
    lPyramid->AddPolygon(0);
    lPyramid->AddPolygon(4);
    lPyramid->AddPolygon(1);
    lPyramid->EndPolygon();

    // Attach the mesh to a node
    FbxNode * lPyramidNode = FbxNode::Create(pScene, pName);
    lPyramidNode->SetNodeAttribute(lPyramid);

    // Set this node as a child of the root node
    pScene->GetRootNode()->AddChild(lPyramidNode);

    return lPyramidNode;
}


typedef double Vector4[4];
typedef double Vector2[2];

// Create a cube.
FbxNode* CreateCube(FbxScene* pScene, const char* pName, FbxDouble3& pLclTranslation)
{
    // indices of the vertices per each polygon
    static int vtxId[24] = {
        0,1,2,3, // front  face  (Z+)
        1,5,6,2, // right  side  (X+)
        5,4,7,6, // back   face  (Z-)
        4,0,3,7, // left   side  (X-)
        0,4,5,1, // bottom face  (Y-)
        3,2,6,7  // top    face  (Y+)
    };

    // control points
    static Vector4 lControlPoints[8] = {
        { -5.0,  0.0,  5.0, 1.0}, {  5.0,  0.0,  5.0, 1.0}, {  5.0,10.0,  5.0, 1.0},    { -5.0,10.0,  5.0, 1.0}, 
        { -5.0,  0.0, -5.0, 1.0}, {  5.0,  0.0, -5.0, 1.0}, {  5.0,10.0, -5.0, 1.0},    { -5.0,10.0, -5.0, 1.0} 
    };

    // normals
    static Vector4 lNormals[8] = {
        {-0.577350258827209,-0.577350258827209, 0.577350258827209, 1.0}, 
        { 0.577350258827209,-0.577350258827209, 0.577350258827209, 1.0}, 
        { 0.577350258827209, 0.577350258827209, 0.577350258827209, 1.0},
        {-0.577350258827209, 0.577350258827209, 0.577350258827209, 1.0}, 
        {-0.577350258827209,-0.577350258827209,-0.577350258827209, 1.0}, 
        { 0.577350258827209,-0.577350258827209,-0.577350258827209, 1.0},
        { 0.577350258827209, 0.577350258827209,-0.577350258827209, 1.0},
        {-0.577350258827209, 0.577350258827209,-0.577350258827209, 1.0}
    };

    // uvs
    static Vector2 lUVs[14] = {
        { 0.0, 1.0}, 
        { 1.0, 0.0}, 
        { 0.0, 0.0},
        { 1.0, 1.0}
    };

    // indices of the uvs per each polygon
    static int uvsId[24] = {
        0,1,3,2,2,3,5,4,4,5,7,6,6,7,9,8,1,10,11,3,12,0,2,13
    };

    // create the main structure.
    FbxMesh* lMesh = FbxMesh::Create(pScene,"");

    // Create control points.
    lMesh->InitControlPoints(8);
    FbxVector4* vertex = lMesh->GetControlPoints();
    memcpy((void*)vertex, (void*)lControlPoints, 8*sizeof(FbxVector4));

    // create the materials.
    /* Each polygon face will be assigned a unique material.
    */
    FbxGeometryElementMaterial* lMaterialElement = lMesh->CreateElementMaterial();
    lMaterialElement->SetMappingMode(FbxGeometryElement::eAllSame);
    lMaterialElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

    lMaterialElement->GetIndexArray().Add(0);

    // Create polygons later after FbxGeometryElementMaterial is created. Assign material indices.
    int vId = 0;
    for (int f=0; f<6; f++)
    {
        lMesh->BeginPolygon();
        for (int v=0; v<4; v++)
            lMesh->AddPolygon(vtxId[vId++]);
        lMesh->EndPolygon();
    }

    // specify normals per control point.
    FbxGeometryElementNormal* lNormalElement = lMesh->CreateElementNormal();
	lNormalElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
    lNormalElement->SetReferenceMode(FbxGeometryElement::eDirect);

    for (int n=0; n<8; n++)
        lNormalElement->GetDirectArray().Add(FbxVector4(lNormals[n][0], lNormals[n][1], lNormals[n][2]));


    // Create the node containing the mesh
    FbxNode* lNode = FbxNode::Create(pScene,pName);
    lNode->LclTranslation.Set(pLclTranslation);

    lNode->SetNodeAttribute(lMesh);
    lNode->SetShadingMode(FbxNode::eTextureShading);   
    
    // create UVset
    FbxGeometryElementUV* lUVElement1 = lMesh->CreateElementUV("UVSet1");
	FBX_ASSERT( lUVElement1 != NULL);
    lUVElement1->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
    lUVElement1->SetReferenceMode(FbxGeometryElement::eIndexToDirect);
    for (int i = 0; i <4; i++)
        lUVElement1->GetDirectArray().Add(FbxVector2(lUVs[i][0], lUVs[i][1]));

    for (int i = 0; i<24; i++)
        lUVElement1->GetIndexArray().Add(uvsId[i%4]);

    return lNode;
}

