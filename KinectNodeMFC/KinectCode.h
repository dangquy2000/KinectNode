#pragma once
//Phan cua camera

#include <Windows.h>
#include <Ole2.h>

#include <NuiApi.h>
#include <NuiImageCamera.h>
#include <NuiSensor.h>

#include <iostream>
#include <Eigen/Dense>
#include <vector>
#include <chrono>


#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>


using namespace Eigen;

/*
	Cac ham lien quan den kinect sensor
*/
//Ham khoi dong kinect
bool initKinect();
//Ham lay du lieu depth data tu kinect 
void getDepthData(GLubyte* dest);
//Ham lay du lieu rgb data tu kinect 
void getRgbData(GLubyte* dest);
//Ham lay du lieu skeleton tu kinect 
void getSkeletalData();
//Ham lay du lieu tat ca du lieu tu kinect 
void getKinectData();
//Ham kiem tra xem co nguoi hay khong
bool CheckStateHuman();
//Ham kiem tra xem co xac dinh duoc du skeleton hay khong
bool CheckHumanSkeletonReady();
//--------------------------------------------------------------------------------------------------
/*

Cac ham tinh toan

*/
//Ham phan tach qr loai 0
void qRDecomposition(Eigen::MatrixXd &A, Eigen::MatrixXd &Q, Eigen::MatrixXd &R);
//Ham phan tach qr loai 1 , su dung loai nay
void QRFactorization(Eigen::MatrixXd &A, Eigen::MatrixXd &Q, Eigen::MatrixXd &R);
//Ham tinh ma tran Denavit–Hartenberg
Eigen::Matrix4d DHMatrixCal(double theta, double lengt, double offset, double twist);
//Ham tinh doan thang ngan nhat giua 2 diem
double DminCal(Eigen::MatrixXd p1, Eigen::MatrixXd u1, Eigen::MatrixXd p2, Eigen::MatrixXd u2,
	Eigen::MatrixXd &CR1, Eigen::MatrixXd &CR2);
//Ham tinh van toc goc tu moi diem thuoc robot su dung ma tran jacobi
std::vector<double> GetQdotJacobi(std::vector<double> q_container, Eigen::MatrixXd vel, double offset, int linknum);
//Ham tinh toa do cac khop robot 
std::vector<Eigen::MatrixXd> RobotSkeleton(double q1, double q2, double q3, double q4, double q5);
//Ham tinh duong thang ngan nhat giua nguoi va robot 
double GetDminLine(const std::vector<Eigen::MatrixXd> robotpos, std::vector<Eigen::MatrixXd> humanpos,
	std::vector<Eigen::MatrixXd>& out, int& linkrobotnum, int& linkhumannum);
//Tinh khoan cach tu diem den khop gan nhat
double GetOffset(const std::vector<Eigen::MatrixXd> robotpos, Eigen::MatrixXd pos, int linknum);
//Tinh qdot cho end effector
//--------------------------------------------------------------------------------------------------
/*
Cac ham dieu khien robot
*/
//Ham tinh do lon van toc day robot 
void VelRepulsion(double dmin, double vel,
	std::chrono::time_point<std::chrono::steady_clock> t0, double &vrepmod);
Eigen::MatrixXd ControllerREP(Eigen::MatrixXd vectordmin, double dmin);
double HumanVel(double dmin);
std::vector<double> ControllerATT(Eigen::MatrixXd goal, Eigen::MatrixXd eef, std::vector<double> q_container, double dmin);
//Eigen::MatrixXd ControllerATT(Eigen::MatrixXd goal, double dmin);
//--------------------------------------------------------------------------------------------------
/*

	Cac ham ve hinh tu du lieu camera va robot

*/
//Ham ve khung xuong cua nguoi
GLuint DrawSkeleton(std::vector<Eigen::MatrixXd>& HumanPos);
//Ham ve khung xuong cua robot
GLuint DrawRobotSkeleton(const std::vector<Eigen::MatrixXd> robotvec);
GLuint DrawRobotSkeleton2(const std::vector<Eigen::MatrixXd> robotvec);
//Ham ve duong thang ngan nhat giua nguoi va robot
GLuint DrawDminLine(MatrixXd S_CR1, MatrixXd S_CR2);

//--------------------------------------------------------------------------------------------------

bool GetLmin(GLuint& linemin);// khong su dung ham nay


void SetDummyPos(double dummyX, double dummyY, double dummyZ);
void SetUseDummy(bool isuse);
void ReDrawHumanSpace(GLuint input);