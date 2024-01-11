#pragma once
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

class GLView2 :
	public CWnd
{
	// Constrcstion
public:
	GLView2(CWnd* pclWnd);
	~GLView2();
	// Attibutes
public:
	HDC m_hDC2; // GDI Device Context
	HGLRC m_hglRC2; // Rendering Context
	CWnd* m_pclWnd2;
	HWND m_hWnd2;
	std::thread loopthread;
	bool m_useloop = false;
private:

	GLuint m_HumanSkeleton2;
	GLuint m_RobotSkeleton2;
	GLuint m_DminLine2;

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
	BOOL SetPixelformat2(HDC hdc);
	GLvoid ReSizeGLScene2(GLsizei width, GLsizei height);
	int InitGL2(GLvoid);
	int DrawGLScene2();
	int OnCreate2();
	void GetDataDlg2(MyTheta Slider_theta);

	void MyLeftMouse2(int diffX, int diffY);
	void MyWheelMouse2(short zDelta);
	void MyKeyBoard2(int X);

	std::vector<double> GetQdot2();
	double GetDmin2();

	void CalDminLine2();
	bool GetKinectStatus2();
	bool InitKinect2();
	void SetDummyIsUse2(bool buse);
	std::thread StartSimuLoop2();
	void GhiFile2();
private:
	void SimuLoop2();
	void GLView2::ControllerATTSimu2(double& qd1, double& qd2, double& qd3, double& qd4, double& qd5);
	double* GetQdotJacobiJ4Ver2_2(Eigen::MatrixXd vel);
};

