#pragma once
#include "afxwin.h"

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <thread>
#include <chrono>
#include <math.h>
#include "DrawCode.h"
#include <Eigen\Dense>

class GLView :
	public CWnd
{
// Constrcstion
public:
	GLView(CWnd *pclWnd);
	~GLView();
// Attibutes
public:
	HDC m_hDC; // GDI Device Context
	HGLRC m_hglRC; // Rendering Context
	CWnd *m_pclWnd;
	HWND m_hWnd;
	std::thread loopthread;
	std::thread loadloopthread;
	bool m_useloop = false;
	bool m_useload = false;
private:
	
	GLuint m_HumanSkeleton;
	GLuint m_RobotSkeleton;
	GLuint m_DminLine;

	bool m_kinectStatus;
	double m_dmin;
	double m_HumanVel;
	double m_Qdot[5];
	float m_height;
	float m_width;
	MyTheta m_theta;
	double m_dummyX = 1000;
	double m_dummyY = 2000;
	double m_dummyZ = 100;

	double m_j4x = 370;
	double m_j4y = 0;
	double m_j4z = 600;

	Eigen::Vector3d m_EEFPoint;
	Eigen::Vector3d m_J4Point;
	Eigen::Vector3d m_TargetPoint;
	Eigen::Vector3d m_GoalSlaveBuff;
	Eigen::Vector3d m_LastError;
	double m_vel[6] = { 0,0,0,0,0,0 };
	double m_qdASlaveBuff[5] = { 0,0,0,0,0 };
	double m_velmaxXYZ = 100;
	bool m_LockThetaR;
	double m_ThetaXGoal = 0;
	double m_ThetaX = 0;
	double m_ThetaRGoal = 0;
	double m_ThetaR = 0;
	double m_qdMax[5];
//Operations
public:
	BOOL SetPixelformat(HDC hdc);
	GLvoid ReSizeGLScene(GLsizei width, GLsizei height);
	int InitGL(GLvoid);
	int DrawGLScene();
	int OnCreate();
	void GetDataDlg(MyTheta Slider_theta);

	void MyLeftMouse(int diffX, int diffY);
	void MyWheelMouse(short zDelta);
	void MyKeyBoard(int X);

	std::vector<double> GetQdot();
	double GetDmin();
	
	void CalDminLine();
	bool GetKinectStatus();
	bool InitKinect();
	void SetDummyIsUse(bool buse);
	std::thread StartSimuLoop();
	std::thread StartLoad();
	void GhiFile();
private:
	void SimuLoop();
	void LoadLoop();
	void GLView::ControllerATTSimu(double &qd1, double &qd2, double &qd3, double &qd4, double &qd5);
	double* GetQdotJacobiJ4Ver2(Eigen::MatrixXd vel);
};

