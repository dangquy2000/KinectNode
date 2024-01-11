
// GLUT_BUILDING_LIB
// _CRT_SECURE_NO_WARNINGS


#include "stdafx.h"
#include "GLView2.h"
#include "KinectCode.h"
#define PI 3.141592653589793
#define TSAMPLE 50000 //microsecond
using namespace std;
CSTL_File Model_STL2[6];
float thetaX2 = 3 * PI / 2;
int thetaX_do2 = -90;
int q5_do2 = 0;
float q52 = 0;
float g_x2 = 0;
float g_y2 = 700;
float g_z2 = 370;
float lz2 = -10.0;
float lx2 = 0.0;
float angleX2 = 0.0;
float angleZ2 = 0.0;
float dx2 = 0;
int m_upx2 = 0;
int m_upy2 = 0;
int m_upz2 = 1;
float m_scale2 = 3.7;
float m_x12, m_y12, m_x22, m_y22;
int m_count2 = 0;
float x2, y2, z2;





GLuint Part2[6];

GLuint Coordinate2;
GLuint Chessboard2;
GLuint WorkSpace2;



std::vector<double> qdot_containerR2, qdot_containerA2;
std::vector<Eigen::MatrixXd> robotPos2;
//void* font2 = GLUT_BITMAP_8_BY_13;
//
void* font2 = GLUT_BITMAP_HELVETICA_18;


struct Rdata
{
	std::chrono::time_point < std::chrono::steady_clock> time;
	double dmin;
	double humanvel;
	double x, y, z;
	double q1, q2, q3, q4, q5;
};
std::vector<Rdata> RoboData2;
GLView2::GLView2(CWnd* pclWnd)
{
	m_pclWnd2 = pclWnd;
	m_hWnd2 = pclWnd->m_hWnd;
	m_hDC2 = ::GetDC(m_hWnd2);
	m_dmin = 9999;
	m_HumanVel = 0;
	m_Qdot[0] = 0;
	m_Qdot[1] = 0;
	m_Qdot[2] = 0;
	m_Qdot[3] = 0;
	m_Qdot[4] = 0;
	//m_kinectStatus = false;
	qdot_containerR2 = { 0,0,0,0,0 };
	qdot_containerA2 = { 0,0,0,0,0 };


	m_TargetPoint << 370, 0, 600;
	m_LastError << 0, 0, 0;
	m_theta.q1 = 0;
	m_theta.q2 = 0;
	m_theta.q3 = 0;
	m_theta.q4 = 0;
	m_theta.q5 = 0;

	m_GoalSlaveBuff << 370, 0, 600;
	m_J4Point << 370, 0, 600;

	m_qdMax[0] = 0.25 * PI;
	m_qdMax[1] = 0.25 * PI;
	m_qdMax[2] = 0.25 * PI;
	m_qdMax[3] = 0.25 * PI;
	m_qdMax[4] = 0.25 * PI;
	
}

GLView2::~GLView2()
{
	wglMakeCurrent(NULL, NULL);
	::ReleaseDC(m_hWnd2, m_hDC2);
	wglDeleteContext(m_hglRC2);
}

int GLView2::OnCreate2()
{
	m_hDC2 = ::GetDC(this->m_hWnd2);
	if (!SetPixelformat2(m_hDC2))
	{
		::MessageBox(::GetFocus(), _T("SetPixelformat Failed!"), _T("Error"), MB_OK);
		return -1;
	}
	m_hglRC2 = wglCreateContext(m_hDC2);
	int i = wglMakeCurrent(m_hDC2, m_hglRC2);
	InitGL2();
	return 0;
}
BOOL GLView2::SetPixelformat2(HDC hdc)
{
	PIXELFORMATDESCRIPTOR* ppfd;
	int pixelformat;
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR), // size of this pfd
		1, // version number
		PFD_DRAW_TO_WINDOW | // support window
		PFD_SUPPORT_OPENGL | // support OpenGL
		PFD_GENERIC_FORMAT |
		PFD_DOUBLEBUFFER, // double buffered
		PFD_TYPE_RGBA, // RGBA type
		24, // 24-bit color depth
		0, 0, 0, 0, 0, 0, // color bits ignored
		8, // no alpha buffer
		0, // shift bit ignored
		8, // no accumulation buffer
		0, 0, 0, 0, // accum bits ignored
		32, // 32-bit z-buffer
		8, // no stencil buffer
		8, // no auxiliary buffer
		PFD_MAIN_PLANE, // main layer
		0, // reserved
		0, 0, 0 // layer masks ignored
	};
	ppfd = &pfd;
	if ((pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0)
	{
		::MessageBox(NULL, _T("ChoosePixelFormat failed"), _T("Error"), MB_OK);
		return FALSE;
	}
	if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE)
	{
		::MessageBox(NULL, _T("SetPixelFormat failed"), _T("Error"), MB_OK);
		return FALSE;
	}
	return TRUE;
}
GLvoid GLView2::ReSizeGLScene2(GLsizei width, GLsizei height)
{

	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, width, height); // Reset The Current Viewport
	glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
	glLoadIdentity(); // Reset The Projection Matrix
					  // Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 10000.0f);//0.1 100
	glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
	glLoadIdentity(); // Reset The Modelview Matrix

	m_height = height;
	m_width = width;


	GLuint HumanSpace = glGenLists(1);
	glNewList(HumanSpace, GL_COMPILE);
	glDisable(GL_LIGHTING);
	const double r_pu = 3.5 * 1000;
	const double r_so = 1.2 * 1000;
	const double r_pe = 0.45 * 1000;
	const double r_in = 0.45 * 1000;

	glBegin(GL_QUAD_STRIP);

	glColor3f(0.f, 0.75f, 0.f);
	for (int i = 0; i < 370; ++i)
	{
		glVertex3f(r_pu * cos(2 * 3.14159 * i / 360), r_pu * sin(2 * 3.14159 * i / 360), -0.01);
		glVertex3f(r_so * cos(2 * 3.14159 * i / 360), r_so * sin(2 * 3.14159 * i / 360), -0.01);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);

	glColor3f(0.75f, 0.75f, 0.f);
	for (int i = 0; i < 370; ++i)
	{
		glVertex3f(r_so * cos(2 * 3.14159 * i / 360), r_so * sin(2 * 3.14159 * i / 360), -0.01);
		glVertex3f(r_pe * cos(2 * 3.14159 * i / 360), r_pe * sin(2 * 3.14159 * i / 360), -0.01);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);

	glColor3f(1.f, 0.1f, 0.f);
	for (int i = 0; i < 362; ++i)
	{
		glVertex3f(r_pe * cos(2 * 3.14159 * i / 360), r_pe * sin(2 * 3.14159 * i / 360), -0.01);
		glVertex3f(0, 0, -0.01);
	}

	glEnd();
	glEnable(GL_LIGHTING);
	glEndList();



	ReDrawHumanSpace(HumanSpace);
	return GLvoid();

}
int GLView2::InitGL2(GLvoid)
{
	//

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Blue Background
	glClearDepth(1.0f); // Depth Buffer Setup
	glShadeModel(GL_SMOOTH); // Enable Smooth Shading
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST); // Enables Depth Testing
	glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice
	glEnable(GL_COLOR_MATERIAL);
	GLfloat light_pos[] = { 0, 0, 1, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	GLfloat ambient[] = { 1.0, 0.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);

	GLfloat diff_use[] = { 0.7, 0.7, 0.7, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff_use);

	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);

	GLfloat shininess = 50.0f;
	glMateriali(GL_FRONT, GL_SHININESS, shininess);

	Model_STL2[0].Load("3D Model/part1_base.STL");
	Model_STL2[1].Load("3D Model/part2_shouder.STL");
	Model_STL2[2].Load("3D Model/part3_upperarm.STL");
	Model_STL2[3].Load("3D Model/part4_lowerarm.STL");
	Model_STL2[4].Load("3D Model/part5_wrist1.STL");
	Model_STL2[5].Load("3D Model/part6_wrist2.STL");

	//Perspective calculations

	//
	//Ve he truc toa do
	Coordinate2 = glGenLists(1);
	glNewList(Coordinate2, GL_COMPILE);
	DrawCoordinate();
	glEndList();
	//Ve Chessboard2

	Chessboard2 = glGenLists(1);
	glNewList(Chessboard2, GL_COMPILE);
	DrawChessboard();
	glEndList();

	for (int i = 0; i < 6; i++)
	{
		Part2[i] = glGenLists(1);
		glNewList(Part2[i], GL_COMPILE);
		Model_STL2[i].Draw(false, true);
		glEndList();
	}


	WorkSpace2 = glGenLists(1);
	glNewList(WorkSpace2, GL_COMPILE);
	glDisable(GL_LIGHTING);
	const double rws = 700;
	glBegin(GL_QUAD_STRIP);
	glColor3f(0.75f, 0.25f, 0.f);
	for (int i = 0; i < 362; ++i)
	{
		glVertex3f(rws * cos(2 * 3.14159 * i / 360), rws * sin(2 * 3.14159 * i / 360), -5);
		glVertex3f(0, 0, -0.01);
	}

	glEnd();
	glEnable(GL_LIGHTING);
	glEndList();

	return TRUE;
	//return 0;
}

int GLView2::DrawGLScene2()
{
	static Eigen::MatrixXd goal(3, 1);

	goal << 470, 0, 600;


	wglMakeCurrent(m_hDC2, m_hglRC2);

	//m_scale2 = zoom/100;
	// TODO: Add your implementation code here.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	//DrawModel(Model_STL2, theta.q1, theta.q2, theta.q3, theta.q4, q5);

	gluLookAt((g_x2 * m_scale2), (g_y2 * m_scale2), (g_z2 * m_scale2), 0, 0, 0, m_upx2, m_upy2, m_upz2);
	glRotatef(angleZ2, 0, 0, 1);
	glRotatef(angleX2, 1, 0, 0);

	glPushMatrix();
	glLineWidth(1);
	glCallList(Coordinate2);
	glCallList(Chessboard2);
	glLineWidth(12);
	glCallList(m_RobotSkeleton2);
	//work space robot
	glCallList(WorkSpace2);
	glLineWidth(1);
	glPopMatrix();

	/*if (CheckStateHuman())
	{
		//Phep doi hinh skeleton chi tac dong len opengl
		//glTranslatef(-500, 0, 0);
		//glRotatef(90, 0, 0, 1);
		//glRotatef(90, 1, 0, 0);

		//glScalef(1000.0, 1000.0, 1000.0);
		glPushMatrix();
		glCallList(m_HumanSkeleton2);

		//glScalef(1 / 1000.0, 1 / 1000.0, 1 / 1000.0);

		glLineWidth(10);
		if (CheckHumanSkeletonReady())
		{
			glPopMatrix();
			glCallList(m_DminLine2);
		}
		else
		{
			glPopMatrix();
		}
	}*/
	SwapBuffers(m_hDC2);
	return TRUE;
}

void GLView2::MyKeyBoard2(int X)
{
	// TODO: Add your implementation code here.
	if (X == 84) g_z2 = g_z2 + 10;	// a
	if (X == 71) g_z2 = g_z2 - 10;	// d
	if (X == 90)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_POLYGON_SMOOTH);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	}
	if (X == 88)
	{
		glDisable(GL_BLEND);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_POINT_SMOOTH);
		glDisable(GL_POLYGON_SMOOTH);
	}

	if (X == 73) m_dummyX += -100;		//w//87  //i
	if (X == 75) m_dummyX += +100;	//s:83

	if (X == 74) m_dummyY += -100;		//a:65
	if (X == 76) m_dummyY += +100;	//d:68 

	if (X == 81) m_dummyZ += 100;		//q
	if (X == 69) m_dummyZ += -100;	//e


	SetDummyPos(m_dummyX, m_dummyY, m_dummyZ);

}
void GLView2::MyLeftMouse2(int diffX, int diffY)
{

	g_z2 = g_z2 + diffY;
	angleZ2 = angleZ2 + float(diffX) / 100;
	//angleX2 = angleX2 - float(diffY) / 100;

}

void GLView2::MyWheelMouse2(short zDelta)
{
	m_scale2 = m_scale2 + float(zDelta) / 1200;
	if (m_scale2 < 0.01) m_scale2 = 0.01;
	// TODO: Add your implementation code here.
}

void GLView2::GetDataDlg2(MyTheta Slider_theta)
{
	// TODO: Add your implementation code here.
	m_theta.q1 = Slider_theta.q1;// *PI / 180;
	m_theta.q2 = Slider_theta.q2;// *PI / 180;
	m_theta.q3 = Slider_theta.q3;// *PI / 180;
	m_theta.q4 = Slider_theta.q4;// *PI / 180;
	m_theta.q5 = Slider_theta.q5;// *PI / 180;
}
void GLView2::CalDminLine2()
{
	static int linkrobotnum = 0;
	static int linkhumannum = 0;

	std::vector<Eigen::MatrixXd> HumanPos;
	m_HumanSkeleton2 = DrawSkeleton(HumanPos);

	robotPos2 = RobotSkeleton(m_theta.q1, m_theta.q2, m_theta.q3, m_theta.q4, m_theta.q5);
	m_RobotSkeleton2 = DrawRobotSkeleton2(robotPos2);

	m_j4x = robotPos2[4](0, 0);
	m_j4y = robotPos2[4](1, 0);
	m_j4z = robotPos2[4](2, 0);


	if (CheckHumanSkeletonReady())// checkStateHuman
	{
		std::vector<MatrixXd> point;
		m_dmin = GetDminLine(robotPos2, HumanPos, point, linkrobotnum, linkhumannum);
		m_HumanVel = HumanVel(m_dmin);
		m_DminLine2 = DrawDminLine(point[0], point[1]);
		double offset = GetOffset(robotPos2, point[0], linkrobotnum);
		//Them bo dieu khien
		Eigen::MatrixXd updatevel;
		updatevel = ControllerREP(point[0] - point[1], m_dmin);
		std::vector<double> q_container;

		q_container.push_back(m_theta.q1);
		q_container.push_back(m_theta.q2);
		q_container.push_back(m_theta.q3);
		q_container.push_back(m_theta.q4);
		q_container.push_back(m_theta.q5);

		qdot_containerR2 = GetQdotJacobi(q_container, updatevel, offset, linkrobotnum);
		m_Qdot[0] = qdot_containerR2[0];
		m_Qdot[1] = qdot_containerR2[1];
		m_Qdot[2] = qdot_containerR2[2];
		m_Qdot[3] = qdot_containerR2[3];
		m_Qdot[4] = qdot_containerR2[4];

	}
}
std::vector<double> GLView2::GetQdot2()
{
	std::vector<double> qdot(5);
	qdot[0] = *m_Qdot;
	qdot[1] = *(m_Qdot + 1);
	qdot[2] = *(m_Qdot + 2);
	qdot[3] = *(m_Qdot + 3);
	qdot[4] = *(m_Qdot + 4);
	return qdot;
}
double GLView2::GetDmin2()
{
	return m_dmin;
}

bool GLView2::GetKinectStatus2()
{
	return m_kinectStatus;
}
bool GLView2::InitKinect2()
{
	m_kinectStatus = initKinect();

	if (!m_kinectStatus)
	{
		AfxMessageBox(_T("Khong the khoi dong Kinect camera !"));
	}
	return m_kinectStatus;

}

void  GLView2::SetDummyIsUse2(bool buse)
{
	SetUseDummy(buse);
}

std::thread GLView2::StartSimuLoop2()
{
	m_useloop = true;
	return std::thread([=] {SimuLoop2(); });
}
void GLView2::SimuLoop2()
{
	auto t_start = std::chrono::high_resolution_clock::now();
	std::chrono::microseconds delay(TSAMPLE);
	double z = 600;
	double dau = 1;
	double myvel = 0;
	while (m_useloop)
	{
		auto t_now = std::chrono::high_resolution_clock::now();
		std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t_now - t_start);
		if (delay <= elapsed)
		{
			// Timer
			t_start = t_now;

			// Calculate q dot
			double qdot1, qdot2, qdot3, qdot4, qdot5;
			qdot1 = qdot2 = qdot3 = qdot4 = qdot5 = 0;
			double qda1, qda2, qda3, qda4, qda5;
			qda1 = qda2 = qda3 = qda4 = qda5 = 0;

			const double vel = 30;
			const double dy = vel * TSAMPLE / 1000000;

			if ((int)m_j4z == 600) z = 300;
			if ((int)m_j4z == 300) z = 600;


			//y += (dau*dy);
			m_GoalSlaveBuff << 370, 0, z;

			m_J4Point << m_j4x, m_j4y, m_j4z;
			//debug
			double a, b, c;
			//a = robotPos2[4](0, 0);

			//b = robotPos2[4](1, 0);

			//c = robotPos2[4](2, 0);


			ControllerATTSimu2(qda1, qda2, qda3, qda4, qda5);


			qdot1 = qdot_containerR2[0] + qda1;
			qdot2 = qdot_containerR2[1] + qda2;
			qdot3 = qdot_containerR2[2] + qda3;
			qdot4 = qda4;
			qdot5 = qda5;
			// Gioi han toc do

			if (abs(qdot1) > m_qdMax[0])
				qdot1 = (qdot1 / abs(qdot1)) * m_qdMax[0];
			if (abs(qdot2) > m_qdMax[1])
				qdot2 = (qdot2 / abs(qdot2)) * m_qdMax[1];
			if (abs(qdot3) > m_qdMax[2])
				qdot3 = (qdot3 / abs(qdot3)) * m_qdMax[2];
			if (abs(qdot4) > m_qdMax[2])
				qdot4 = (qdot4 / abs(qdot4)) * m_qdMax[3];
			if (abs(qdot5) > m_qdMax[2])
				qdot5 = (qdot5 / abs(qdot5)) * m_qdMax[4];

			// q = q + qdot*Tsampe
			m_theta.q1 = m_theta.q1 + qdot1 * TSAMPLE / 1000000;
			m_theta.q2 = m_theta.q2 + qdot2 * TSAMPLE / 1000000;
			m_theta.q3 = m_theta.q3 + qdot3 * TSAMPLE / 1000000;
			m_theta.q4 = m_theta.q4 + qdot4 * TSAMPLE / 1000000;
			m_theta.q5 = m_theta.q5 + qdot5 * TSAMPLE / 1000000;

			myvel = m_HumanVel;
			// luu tru du lieu
			Rdata tem;
			tem.time = t_now;
			tem.x = m_j4x;// Get from skeleton 
			tem.y = m_j4y;
			tem.z = m_j4z;

			//get Human State
			tem.dmin = m_dmin;
			tem.humanvel = myvel;

			tem.q1 = m_theta.q1;
			tem.q2 = m_theta.q2;
			tem.q3 = m_theta.q3;
			tem.q4 = m_theta.q4;
			tem.q5 = m_theta.q5;
			RoboData2.push_back(tem);
		}
	}
}

void GLView2::GhiFile2()
{
	std::fstream file;
	file.open("DataSimu.txt", std::ofstream::out);
	for (int i = 0; i < RoboData2.size() - 1; i++)
	{
		std::chrono::duration<double> duration = RoboData2[i].time - RoboData2[0].time;
		double  dduration = duration.count() * 1000;
		file << i << "\t";
		file << dduration << "\t";

		file << RoboData2[i].dmin << "\t";
		file << RoboData2[i].humanvel << "\t";

		file << RoboData2[i].x << "\t";
		file << RoboData2[i].y << "\t";
		file << RoboData2[i].z << "\t";

		file << RoboData2[i].q1 << "\t";
		file << RoboData2[i].q2 << "\t";
		file << RoboData2[i].q3 << "\t";
		file << RoboData2[i].q4 << "\t";
		file << RoboData2[i].q5 << "\t";

		file << "\n";
	}
}

#define DMINCRIT 490			 // tinh tu cong thuc beta = 0.98
#define D0_REPULSION 200	 // d0 = 200 mm
#define DCRIT_REPULSION 300	 // dcrit = 300 mm
#define LAMDA 0.1 //0.01
#define VMAX 50 //mm/s
void GLView2::ControllerATTSimu2(double& qd1, double& qd2, double& qd3, double& qd4, double& qd5)
{
	double kp = 15;
	double ki = 0;
	double beta;
	double tem;

	double temDmin = m_dmin;
	if (temDmin < 310) temDmin = 310;

	tem = (temDmin - DCRIT_REPULSION) / D0_REPULSION;

	beta = (2 / (1 + exp(-(tem) * (tem)))) - 1;

	if (beta < 0) beta = 0;
	if (beta > 1) beta = 1;

	Eigen::Vector3d error;

	error = m_GoalSlaveBuff - m_J4Point;// ? sao lai sai m_J4Point 

	double ex = error(0);
	double ey = error(1);
	double ez = error(2);

	//Check in position
	double denta_e = sqrt(ex * ex + ey * ey + ez * ez);

	/*
	if (denta_e < TOLERANCE)
	{
		m_bInpos = true;
		m_bInMotion = false;
	}
	else
	{
		m_bInpos = false;
		m_bInMotion = true;
	}
	*/

	double vx, vy, vz, omegax, omegar, omegaz;

	vx = error(0) * kp + (ki * TSAMPLE / 1000000) * (error(0) + m_LastError(0));
	vy = error(1) * kp + (ki * TSAMPLE / 1000000) * (error(1) + m_LastError(1));
	vz = error(2) * kp + (ki * TSAMPLE / 1000000) * (error(2) + m_LastError(2));

	//Gioi han toc do

	double velc = sqrt(vx * vx + vy * vy + vz * vz);

	double ix = vx / velc;
	double iy = vy / velc;
	double iz = vz / velc;

	//
	m_velmaxXYZ = 25;
	if (velc >= m_velmaxXYZ)
	{
		vx = ix * m_velmaxXYZ;
		vy = iy * m_velmaxXYZ;
		vz = iz * m_velmaxXYZ;
	}
	else
	{
		vx = vx;
		vy = vy;
		vz = vz;
	}


	omegax = (m_ThetaXGoal - m_ThetaX) * kp / (8 * PI);

	if (m_LockThetaR)
	{
		omegar = (m_ThetaRGoal - m_ThetaR) * kp / (2 * PI);
	}
	else
		omegar = 0;


	omegaz = 0;
	Eigen::MatrixXd velkp(6, 1);
	velkp << vx, vy, vz, omegax, omegar, omegaz;

	m_LastError = error;
	m_vel[0] = velkp(0);
	m_vel[1] = velkp(1);
	m_vel[2] = velkp(2);
	m_vel[3] = velkp(3);
	m_vel[4] = velkp(4);
	m_vel[5] = velkp(5);
	//std::vector<double> qdot_container = GetQdotJacobiEEF(velkp);
	double* qdot_container;

	qdot_container = GetQdotJacobiJ4Ver2_2(velkp);

	//double qdot_container_a[5];
	qd1 = qdot_container[0] * beta;
	qd2 = qdot_container[1] * beta;
	qd3 = qdot_container[2] * beta;
	qd4 = qdot_container[3] * beta;
	qd5 = qdot_container[4] * beta;



	return;
}

double* GLView2::GetQdotJacobiJ4Ver2_2(Eigen::MatrixXd vel)
{
	const double lamda = LAMDA;
	double d5 = 0;
	double qdot_container[5];
	double* Q;

	//Q = GetQsState();

	double q1_dot, q2_dot, q3_dot, q4_dot, q5_dot;
	double q1, q2, q3, q4, q5;
	//q1 = m_q_container[0];
	//q2 = m_q_container[1];
	//q3 = m_q_container[2];
	//q4 = m_q_container[3];
	//q5 = m_q_container[4];
	q1 = m_theta.q1;
	q2 = m_theta.q2;// -PI / 2;
	q3 = m_theta.q3;// +PI / 2;
	q4 = m_theta.q4;// +PI / 2;
	q5 = m_theta.q5;
	//Khi diem thuoc link 4
	Eigen::MatrixXd Ja3(6, 5);

	Ja3(0, 0) = -sin(q1) * (220 * cos(q2 + q3) + 250 * cos(q2) - d5 * sin(q2 + q3 + q4) + 150);
	Ja3(0, 1) = -cos(q1) * (220 * sin(q2 + q3) + 250 * sin(q2) + d5 * cos(q2 + q3 + q4));
	Ja3(0, 2) = -cos(q1) * (220 * sin(q2 + q3) + d5 * cos(q2 + q3 + q4));
	Ja3(0, 3) = -d5 * cos(q2 + q3 + q4) * cos(q1);
	Ja3(0, 4) = 0;

	Ja3(1, 0) = cos(q1) * (220 * cos(q2 + q3) + 250 * cos(q2) - d5 * sin(q2 + q3 + q4) + 150);
	Ja3(1, 1) = -sin(q1) * (220 * sin(q2 + q3) + 250 * sin(q2) + d5 * cos(q2 + q3 + q4));
	Ja3(1, 2) = -sin(q1) * (220 * sin(q2 + q3) + d5 * cos(q2 + q3 + q4));
	Ja3(1, 3) = -d5 * cos(q2 + q3 + q4) * sin(q1);
	Ja3(1, 4) = 0;

	Ja3(2, 0) = 0;
	Ja3(2, 1) = 220 * cos(q2 + q3) + 250 * cos(q2) - d5 * sin(q2 + q3 + q4);
	Ja3(2, 2) = 220 * cos(q2 + q3) - d5 * sin(q2 + q3 + q4);
	Ja3(2, 3) = -d5 * sin(q2 + q3 + q4);
	Ja3(2, 4) = 0;

	Ja3(3, 0) = 0;
	Ja3(3, 1) = 1;
	Ja3(3, 2) = 1;
	Ja3(3, 3) = 1;
	Ja3(3, 4) = 0;

	Ja3(4, 0) = -1;
	Ja3(4, 1) = 0;
	Ja3(4, 2) = 0;
	Ja3(4, 3) = 0;
	Ja3(4, 4) = 1;

	Ja3(5, 0) = 0;
	Ja3(5, 1) = 0;
	Ja3(5, 2) = 0;
	Ja3(5, 3) = 0;
	Ja3(5, 4) = 0;

	Eigen::MatrixXd Jadot3(6, 6);
	Eigen::MatrixXd v(6, 1);

	v << vel(0, 0), vel(1, 0), vel(2, 0), vel(3, 0), vel(4, 0), vel(5, 0);
	auto i = Eigen::Matrix<double, 6, 6>::Identity();
	auto tem1 = Ja3 * Ja3.transpose();
	auto tem2 = (tem1 + lamda * lamda * i);
	Jadot3 = Ja3.transpose() * tem2.inverse();


	auto mvel = Jadot3 * v;
	q1_dot = mvel(0, 0);
	q2_dot = mvel(1, 0);
	q3_dot = mvel(2, 0);
	q4_dot = mvel(3, 0);
	q5_dot = mvel(4, 0);
	//Tinh q4 doy


	qdot_container[0] = q1_dot;
	qdot_container[1] = q2_dot;
	qdot_container[2] = q3_dot;
	qdot_container[3] = q4_dot;
	qdot_container[4] = q5_dot;

	return qdot_container;
}