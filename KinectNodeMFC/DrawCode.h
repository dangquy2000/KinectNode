
#pragma once

#include <gl/glut.h>
#include "STL_File.h"
#include "targetver.h"
#include "Vec3D.h"
#include <fstream>
#include <string>
#include <iostream>
#include "stdafx.h"

void DrawChessboard();
void DrawCoordinate();
void DrawLocalCoordinate();
void DrawModel(CSTL_File Model_STL[], float q1, float q2, float q3, float q4, float q5);
void DrawModelV2(CSTL_File Model_STL[], GLuint Part[], float q1, float q2, float q3, float q4, float q5);
struct MyTheta
{
    float q1;
    float q2;
    float q3;
    float q4;
	float q5;
};
struct EyeCamera
{
	double x;
	double y;
	double z;
	double centerx;
	double centery;
	double centerz;
	double upx;
	double upy;
	double upz;
};
struct Vertex
{
    float x, y, z;
};
MyTheta InvKine3(float x, float y, float z, float q5);


std::vector<Vertex>  DocFile(std::string FileName);
std::vector<Vertex>  GhiPoint(std::vector<Vertex> Point, int t);
//void DocFile(std::string filename);