#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "Mesh.h"
#include "Vertex.h"

/******************************************************************************/
/*!
		Class MeshBuilder:
\brief	Provides methods to generate mesh of different shapes
*/
/******************************************************************************/
class MeshBuilder
{
public:
	static Mesh* GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ);
	static Mesh* GenerateQuad(const std::string &meshName, Color color);
	static Mesh* GenerateCube(const std::string &meshName, Color color);
	static Mesh* GenerateCircle(const std::string &meshName, Color color, unsigned numSlices = 36);
	static Mesh* GenerateRing(const std::string &meshName, Color color, float innerRadius, unsigned numSlices = 36);
	static Mesh* GenerateSphere(const std::string &meshName, Color color, unsigned numStacks, unsigned numSlices);
	static Mesh* GenerateHemiSphere(const std::string &meshName, Color color, unsigned numStack, unsigned numSlices);
	static Mesh* GenerateCylinder(const std::string &meshName, Color color, unsigned numStack, unsigned numSlices, unsigned height, unsigned radius);
	static Mesh* GeneratePyramid(const std::string &meshName, Color color, unsigned numStack, unsigned numSlices);
	static Mesh* GenerateTorus(const std::string &meshName, Color color, unsigned numStacks, unsigned numSlices, float outerR, float innerR);
	static Mesh* GenerateOBJ(const std::string &meshName, const std::string &file_path);
	static Mesh* GenerateText(const std::string &meshName, unsigned numRow, unsigned numCol);
};

#endif