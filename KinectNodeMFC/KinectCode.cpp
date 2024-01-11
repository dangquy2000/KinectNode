#include "stdafx.h"
#include "KinectCode.h"
#define a1_DH 150
#define d1_DH 350
#define a2_DH 250
#define a3_DH 220
#define at_DH 100
#define toollengt 100;
#define PI_S 3.1415926535897932384626433

#define TAU 1000			 // hang so thoi gian 1000ms
#define DMINCRIT 490			 // Dcrit + d0
#define D0_REPULSION 200	 // d0 = 300 mm
#define DCRIT_REPULSION 300	 // dcrit = 100 mm

//Khoang cach giua kinect so voi robot
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

/*
#define DentaX 950 //987 
#define DentaY -490 //490
#define DentaZ 280 //280
*/
#define DentaX 730 //1020
#define DentaY -637
#define DentaZ 280 

const int width = 640;
const int height = 480;
// OpenGL Variables
long depthToRgbMap[width*height * 2];


// We'll be using buffer objects to store the kinect point cloud
GLuint HumanSpace;

GLuint vboId;
GLuint cboId;
GLuint Skeleton;
bool State_Detect = false;
bool HumanSkeletonReady = false;

// Kinect variables
HANDLE depthStream;
HANDLE rgbStream;
INuiSensor* sensor;

// Stores the coordinates of each joint
_Vector4 skeletonPosition[NUI_SKELETON_POSITION_COUNT];
int NumOfPeople = 0;

GLint GlIntSkeleton;

MatrixXd RotSke(3, 3);
MatrixXd TranSke(3, 1);


//Dummy pos 
double k_dummyX = 1000;
double k_dummyY = 2000;
double k_dummyZ = 100;
bool IsUseDummy = false;

#pragma optimize("",off)

enum RepulState
{
	Deactivation,
	Activation
};

bool initKinect()
{
	// Get a working kinect sensor
	if (!IsUseDummy)
	{
		int numSensors;
		if (NuiGetSensorCount(&numSensors) < 0 || numSensors < 1) return false;
		if (NuiCreateSensorByIndex(0, &sensor) < 0) return false;

		// Initialize sensor
		sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_SKELETON);
		sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, // Depth camera or rgb camera?
			NUI_IMAGE_RESOLUTION_640x480,                // Image resolution
			0,        // Image stream flags, e.g. near mode
			2,        // Number of frames to buffer
			NULL,     // Event handle
			&depthStream);
		sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, // Depth camera or rgb camera?
			NUI_IMAGE_RESOLUTION_640x480,                // Image resolution
			0,      // Image stream flags, e.g. near mode
			2,      // Number of frames to buffer
			NULL,   // Event handle
			&rgbStream);
		sensor->NuiSkeletonTrackingEnable(NULL, 0); // NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT for only upper body
	}



	//Tao ma tran tinh tien + xoay
	MatrixXd nice(3, 1);
	nice << 2, 1, 2;
	AngleAxis<float> aax(PI_S / 2, Vector3f(1
		, 0, 0));
	AngleAxis<float> aaz(PI_S / 2, Vector3f(0, 0, 1));
	//auto tttt = nice *aaz.matrix() * aax.matrix();
	//std::cout << "Have a nice day\n\n";
	//
	//std::cout << aaz.matrix() * aax.matrix() << "\n\n";
	auto TT = aaz.matrix() * aax.matrix();
	
	/*
	RotSke << TT(0, 0), TT(0, 1), TT(0, 2)
		, TT(1, 0), TT(1, 1), TT(1, 1)
		, TT(2, 0), TT(2, 1), TT(2, 2);
	*/
	RotSke << -1, 0, 0, 0, 0, 1, 0, 1, 0;
	TranSke << DentaX, DentaY, DentaZ;
	//std::cout << "Have a nice day\n\n";
	//std::cout << RotSke*nice << "\n\n";




	//
	//static bool firsttime = true;
	//if (firsttime)
	//{
	HumanSpace = glGenLists(1);
	glNewList(HumanSpace, GL_COMPILE);
	glDisable(GL_LIGHTING);
	const double r_pu = 3.5 * 1000;
	const double r_so = 1.2 * 1000;
	const double r_pe = 0.45 * 1000;
	const double r_in = 0.45 * 1000;

	glBegin(GL_QUAD_STRIP);

	glColor3f(0.f, 0.75f, 0.f);
	for (int i = 0; i<370; ++i)
	{
		glVertex3f(r_pu*cos(2 * 3.14159*i / 360), r_pu*sin(2 * 3.14159*i / 360), -0.01);
		glVertex3f(r_so*cos(2 * 3.14159*i / 360), r_so*sin(2 * 3.14159*i / 360), -0.01);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);

	glColor3f(0.75f, 0.75f, 0.f);
	for (int i = 0; i<370; ++i)
	{
		glVertex3f(r_so*cos(2 * 3.14159*i / 360), r_so*sin(2 * 3.14159*i / 360), -0.01);
		glVertex3f(r_pe*cos(2 * 3.14159*i / 360), r_pe*sin(2 * 3.14159*i / 360), -0.01);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);

	glColor3f(1.f, 0.1f, 0.f);
	for (int i = 0; i<362; ++i)
	{
		glVertex3f(r_pe*cos(2 * 3.14159*i / 360), r_pe*sin(2 * 3.14159*i / 360), -0.01);
		glVertex3f(0, 0, -0.01);
	}

	glEnd();
	glEnable(GL_LIGHTING);
	glEndList();

	//	firsttime = false;
	//}
	if (IsUseDummy)
	{
		return  true;
	}
	else
	{
		return sensor;
	}

	
}

void getDepthData(GLubyte* dest) 
{
	float* fdest = (float*)dest;
	long* depth2rgb = (long*)depthToRgbMap;
	NUI_IMAGE_FRAME imageFrame;
	NUI_LOCKED_RECT LockedRect;
	if (sensor->NuiImageStreamGetNextFrame(depthStream, 0, &imageFrame) < 0) return;
	INuiFrameTexture* texture = imageFrame.pFrameTexture;
	texture->LockRect(0, &LockedRect, NULL, 0);
	if (LockedRect.Pitch != 0) {
		const USHORT* curr = (const USHORT*)LockedRect.pBits;
		for (int j = 0; j < height; ++j) {
			for (int i = 0; i < width; ++i) {
				// Get depth of pixel in millimeters
				USHORT depth = NuiDepthPixelToDepth(*curr++);
				// Store coordinates of the point corresponding to this pixel
				_Vector4 pos = NuiTransformDepthImageToSkeleton(i, j, depth << 3, NUI_IMAGE_RESOLUTION_640x480);
				*fdest++ = pos.x / pos.w;
				*fdest++ = pos.y / pos.w;
				*fdest++ = pos.z / pos.w;
				// Store the index into the color array corresponding to this pixel
				NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(
					NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_RESOLUTION_640x480, NULL,
					i, j, depth << 3, depth2rgb, depth2rgb + 1);
				depth2rgb += 2;
			}
		}
	}
	texture->UnlockRect(0);
	sensor->NuiImageStreamReleaseFrame(depthStream, &imageFrame);
}

void getRgbData(GLubyte* dest)
{
	float* fdest = (float*)dest;
	long* depth2rgb = (long*)depthToRgbMap;
	NUI_IMAGE_FRAME imageFrame;
	NUI_LOCKED_RECT LockedRect;
	if (sensor->NuiImageStreamGetNextFrame(rgbStream, 0, &imageFrame) < 0) return;
	INuiFrameTexture* texture = imageFrame.pFrameTexture;
	texture->LockRect(0, &LockedRect, NULL, 0);
	if (LockedRect.Pitch != 0) {
		const BYTE* start = (const BYTE*)LockedRect.pBits;
		for (int j = 0; j < height; ++j) {
			for (int i = 0; i < width; ++i) {
				// Determine rgb color for each depth pixel
				long x = *depth2rgb++;
				long y = *depth2rgb++;
				// If out of bounds, then don't color it at all
				if (x < 0 || y < 0 || x > width || y > height) {
					for (int n = 0; n < 3; ++n) *(fdest++) = 0.0f;
				}
				else {
					const BYTE* curr = start + (x + width*y) * 4;
					for (int n = 0; n < 3; ++n) *(fdest++) = curr[2 - n] / 255.0f;
				}

			}
		}
	}
	texture->UnlockRect(0);
	sensor->NuiImageStreamReleaseFrame(rgbStream, &imageFrame);
}

void getSkeletalData()
{
	NUI_SKELETON_FRAME skeletonFrame = { 0 };
	if (sensor->NuiSkeletonGetNextFrame(0, &skeletonFrame) >= 0) {
		sensor->NuiTransformSmooth(&skeletonFrame, NULL);
		// Loop over all sensed skeletons
		NumOfPeople = 0;
		for (int z = 0; z < NUI_SKELETON_COUNT; ++z) {
			const NUI_SKELETON_DATA& skeleton = skeletonFrame.SkeletonData[z];
			// Check the state of the skeleton
			if (skeleton.eTrackingState == NUI_SKELETON_TRACKED) {
				// Copy the joint positions into our array
				NumOfPeople++;
				for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i) {
					skeletonPosition[i] = skeleton.SkeletonPositions[i];
					if (skeleton.eSkeletonPositionTrackingState[i] == NUI_SKELETON_POSITION_NOT_TRACKED) {
						skeletonPosition[i].w = 0;
					}
				}
				return; // Only take the data for one skeleton
			}
		}
	}
}
void getKinectData()
{
	const int dataSize = width*height * 3 * 4;
	GLubyte* ptr;
	/*
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	ptr = (GLubyte*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		getDepthData(ptr);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, cboId);
	ptr = (GLubyte*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		getRgbData(ptr);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	*/
	if (!IsUseDummy)
	{
		getSkeletalData();
	}
	return ;
}
GLuint DrawSkeleton(std::vector<Eigen::MatrixXd>& HumanPos)
{
	//GLuint Skeleton;
	const double pi = 3.14159265359;
	if (!IsUseDummy)
	{

		Eigen::MatrixXd HuPos0(3, 1), HuPos1(3, 1), HuPos2(3, 1), HuPos3(3, 1), HuPos4(3, 1);
		Eigen::MatrixXd HuPos5(3, 1), HuPos6(3, 1), HuPos7(3, 1), HuPos8(3, 1), HuPos9(3, 1);
		Eigen::MatrixXd HuPos10(3, 1), HuPos11(3, 1),HuPos12(3, 1);

		const _Vector4& head = skeletonPosition[NUI_SKELETON_POSITION_HEAD];
		const _Vector4& shoulderCenter = skeletonPosition[NUI_SKELETON_POSITION_SHOULDER_CENTER];
		const _Vector4& spine = skeletonPosition[NUI_SKELETON_POSITION_SPINE];

		HuPos6 << head.x, head.y, head.z;
		HuPos7 << spine.x, spine.y, spine.z;
		HuPos8 << shoulderCenter.x, shoulderCenter.y, shoulderCenter.z;

		const _Vector4& shoulderLeft = skeletonPosition[NUI_SKELETON_POSITION_SHOULDER_LEFT];
		const _Vector4& elbowLeft = skeletonPosition[NUI_SKELETON_POSITION_ELBOW_LEFT];
		const _Vector4& writstLeft = skeletonPosition[NUI_SKELETON_POSITION_WRIST_LEFT];
		HuPos0 << writstLeft.x, writstLeft.y, writstLeft.z;
		HuPos1 << elbowLeft.x, elbowLeft.y, elbowLeft.z;
		HuPos2 << shoulderLeft.x, shoulderLeft.y, shoulderLeft.z;
		const _Vector4& shoulderRight = skeletonPosition[NUI_SKELETON_POSITION_SHOULDER_RIGHT];
		const _Vector4& elbowRight = skeletonPosition[NUI_SKELETON_POSITION_ELBOW_RIGHT];
		const _Vector4& writstRight = skeletonPosition[NUI_SKELETON_POSITION_WRIST_RIGHT];
		HuPos3 << shoulderRight.x, shoulderRight.y, shoulderRight.z;
		HuPos4 << elbowRight.x, elbowRight.y, elbowRight.z;
		HuPos5 << writstRight.x, writstRight.y, writstRight.z;
		const _Vector4& HandLeft = skeletonPosition[NUI_SKELETON_POSITION_HAND_LEFT];
		const _Vector4& HandRight = skeletonPosition[NUI_SKELETON_POSITION_HAND_RIGHT];
		HuPos9 << HandLeft.x, HandLeft.y, HandLeft.z;
		HuPos10 << HandRight.x, HandRight.y, HandRight.z;

		const _Vector4& HipLeft = skeletonPosition[NUI_SKELETON_POSITION_HIP_LEFT];
		const _Vector4& HipRight = skeletonPosition[NUI_SKELETON_POSITION_HIP_RIGHT];
		HuPos11 << HipLeft.x, HipLeft.y, HipLeft.z;
		HuPos12 << HipRight.x, HipRight.y, HipRight.z;
		/*0:WristLeft;1:Elbowleft;2:shoulderLeft*/
		/*3:shoulderRight;4:ElbowRight;5:WristRight*/
		/*6:head;7:spine;8:shoulderCenter*/
		/*9:handLeft;10:Handright*/
		if (head.w != 0 && spine.w
			&& elbowLeft.w != 0 && shoulderLeft.w != 0 && writstLeft.w != 0
			&& shoulderRight.w != 0 && elbowRight.w != 0 && writstRight.w != 0
			&& HandLeft.w!=0 && HandRight.w!=0 
			&& HipLeft.w!=0 && HipRight.w !=0)
		{
			HumanPos.clear();
			HumanPos.push_back(HuPos0 * 1000);
			HumanPos.push_back(HuPos1 * 1000);
			HumanPos.push_back(HuPos2 * 1000);
			HumanPos.push_back(HuPos3 * 1000);
			HumanPos.push_back(HuPos4 * 1000);
			HumanPos.push_back(HuPos5 * 1000);
			HumanPos.push_back(HuPos6 * 1000);
			HumanPos.push_back(HuPos7 * 1000);
			HumanPos.push_back(HuPos8 * 1000);
			HumanPos.push_back(HuPos9 * 1000);
			HumanPos.push_back(HuPos10 * 1000);
			HumanPos.push_back(HuPos11 * 1000);
			HumanPos.push_back(HuPos12 * 1000);
			HumanSkeletonReady = true;
			for (int i = 0; i < HumanPos.size(); ++i)
			{
				HumanPos[i] = RotSke*HumanPos[i];
				HumanPos[i] = HumanPos[i] + TranSke;
				//std::cout << HumanPos[i] << "\n";
			}

			Skeleton = glGenLists(1);
			glNewList(Skeleton, GL_COMPILE);
			glDisable(GL_LIGHTING);

			glLineWidth(7);
			glBegin(GL_LINES);
			glColor3f(1.f, 0.f, 0.f);

			//Code ve khung xuong 
			if (writstLeft.w > 0 && elbowLeft.w > 0 && shoulderLeft.w > 0)
			{
				

				glVertex3f(HumanPos[0](0), HumanPos[0](1), HumanPos[0](2));
				glVertex3f(HumanPos[1](0), HumanPos[1](1), HumanPos[1](2));

				glVertex3f(HumanPos[1](0), HumanPos[1](1), HumanPos[1](2));
				glVertex3f(HumanPos[2](0), HumanPos[2](1), HumanPos[2](2));

				glVertex3f(HumanPos[2](0), HumanPos[2](1), HumanPos[2](2));
				glVertex3f(HumanPos[6](0), HumanPos[6](1), HumanPos[6](2));
			}


			if (shoulderRight.w > 0 && elbowRight.w > 0 && writstRight.w > 0)
			{
				

				glVertex3f(HumanPos[5](0), HumanPos[5](1), HumanPos[5](2));
				glVertex3f(HumanPos[4](0), HumanPos[4](1), HumanPos[4](2));

				glVertex3f(HumanPos[4](0), HumanPos[4](1), HumanPos[4](2));
				glVertex3f(HumanPos[3](0), HumanPos[3](1), HumanPos[3](2));

				glVertex3f(HumanPos[3](0), HumanPos[3](1), HumanPos[3](2));
				glVertex3f(HumanPos[6](0), HumanPos[6](1), HumanPos[6](2));
			}
			if (HipRight.w > 0 && HipLeft.w > 0 && writstRight.w > 0 && writstLeft.w > 0)
			{
				glVertex3f(HumanPos[0](0), HumanPos[0](1), HumanPos[0](2));
				glVertex3f(HumanPos[11](0), HumanPos[11](1), HumanPos[11](2));

				glVertex3f(HumanPos[11](0), HumanPos[11](1), HumanPos[11](2));
				glVertex3f(HumanPos[12](0), HumanPos[12](1), HumanPos[12](2));

				glVertex3f(HumanPos[12](0), HumanPos[12](1), HumanPos[12](2));
				glVertex3f(HumanPos[5](0), HumanPos[5](1), HumanPos[5](2));

			}

		/*	if (shoulderRight.w > 0 && shoulderCenter.w > 0 && shoulderLeft.w > 0)
			{
				glVertex3f(HumanPos[2](0), HumanPos[2](1), HumanPos[2](2));
				glVertex3f(HumanPos[8](0), HumanPos[8](1), HumanPos[8](2));
				glVertex3f(HumanPos[8](0), HumanPos[8](1), HumanPos[8](2));
				glVertex3f(HumanPos[3](0), HumanPos[3](1), HumanPos[3](2));
			}

			if (head.w > 0 && shoulderCenter.w > 0 && spine.w > 0)
			{
				glVertex3f(HumanPos[6](0), HumanPos[6](1), HumanPos[6](2));
				glVertex3f(HumanPos[7](0), HumanPos[7](1), HumanPos[7](2));
			}*/
			/*
			if (shoulderCenter.w > 0 && shoulderLeft.w > 0 && shoulderRight.w > 0) {
			glVertex3f(shoulderLeft.x, shoulderLeft.y, shoulderLeft.z);
			glVertex3f(shoulderCenter.x, shoulderCenter.y, shoulderCenter.z);
			glVertex3f(shoulderCenter.x, shoulderCenter.y, shoulderCenter.z);
			glVertex3f(shoulderRight.x, shoulderRight.y, shoulderRight.z);
			}
			*/
			glLineWidth(10);
			glColor3f(0.f, 1.f, 1.f);
			/*
			if (HandRight.w > 0 && writstRight.w > 0)
			{
			glVertex3f(HandRight.x, HandRight.y, HandRight.z);
			glVertex3f(writstRight.x, writstRight.y, writstRight.z);
			}

			if (HandLeft.w > 0 && writstLeft.w > 0)
			{
			glVertex3f(HandLeft.x, HandLeft.y, HandLeft.z);
			glVertex3f(writstLeft.x, writstLeft.y, writstLeft.z);
			}
			
			*/
			glEnd();
			glEnable(GL_POINT_SMOOTH);

			glPointSize(20);

			glBegin(GL_POINTS);
			glColor3f(0.f, 1.f, 1.f);
			glVertex3f(HumanPos[6](0), HumanPos[6](1), HumanPos[6](2));
			glVertex3f(HumanPos[10](0), HumanPos[10](1), HumanPos[10](2));
			glVertex3f(HumanPos[9](0), HumanPos[9](1), HumanPos[9](2));
			glEnd();

			glLineWidth(1);
			//glPushMatrix();
			if (head.w > 0 && spine.w > 0)
			{
				glTranslatef(HumanPos[7](0), HumanPos[7](1), -10);
				glCallList(HumanSpace);
				glTranslatef(-HumanPos[7](0), -HumanPos[7](1), 10);
			}

			glEndList();
			glEnable(GL_LIGHTING);

		}
		else
		{
			HumanSkeletonReady = false;
		}
	}
	
	if (IsUseDummy)
	{
		Eigen::MatrixXd HuPos0(3, 1), HuPos1(3, 1);

		HuPos0 << k_dummyX, k_dummyY, k_dummyZ;
		HuPos1 << k_dummyX, k_dummyY, 0;
		HumanSkeletonReady = true;
		

		Skeleton = glGenLists(1);
		glNewList(Skeleton, GL_COMPILE);
		glDisable(GL_LIGHTING);

		glLineWidth(7);
		glBegin(GL_LINES);
		glColor3f(2.f, 0.4f, 0.5f);
		glVertex3f(HuPos0(0, 0), HuPos0(1, 0), HuPos0(2, 0));
		glVertex3f(HuPos1(0, 0), HuPos1(1, 0), HuPos1(2, 0));
		glEnd();

		glLineWidth(1);

		glTranslatef(k_dummyX, k_dummyY, -10);
		glCallList(HumanSpace);
		glTranslatef(-k_dummyX, -k_dummyY, 10);

		glEnable(GL_LIGHTING);
		glEndList();

		

	//	return Skeleton;
	}
	return Skeleton;
	//Chuyen he toa do
}
bool GetLmin(GLuint& linemin)
{
	if (NumOfPeople == 1)
	{
		_Vector4& lh = skeletonPosition[NUI_SKELETON_POSITION_HAND_LEFT];
		_Vector4& le = skeletonPosition[NUI_SKELETON_POSITION_ELBOW_LEFT];
		_Vector4& ls = skeletonPosition[NUI_SKELETON_POSITION_SHOULDER_LEFT];

		_Vector4& rh = skeletonPosition[NUI_SKELETON_POSITION_HAND_RIGHT];
		_Vector4& re = skeletonPosition[NUI_SKELETON_POSITION_ELBOW_RIGHT];
		_Vector4& rs = skeletonPosition[NUI_SKELETON_POSITION_SHOULDER_RIGHT];

		// Nhap ket qua vao ma tran
		MatrixXd p1(3, 1), u1(3, 1), p2(3, 1), u2(3, 1), CR1(3, 1), CR2(3, 1);
		p1 << le.x, le.y, le.z;
		u1 << lh.x, lh.y, lh.z;

		p2 << re.x, re.y, re.z;
		u2 << rh.x, rh.y, rh.z;

		double d;

		d = DminCal(p1, u1, p2, u2, CR1, CR2);

		linemin = glGenLists(1);
		glNewList(linemin, GL_COMPILE);
		glBegin(GL_LINES);
		glLineWidth(4);
		glColor3f(0.f, 1.f, 0.f);

		glVertex3f(CR2(0, 0), CR2(1, 0), CR2(2, 0));
		glVertex3f(CR1(0, 0), CR1(1, 0), CR1(2, 0));
		glLineWidth(1);
		glEnd();
		glEndList();

		
		//std::cout << d * 1000 << "\n";
		return true;
	}
	else
	{
		//std::cout << "Ko nhan dien duoc nguoi" << "\n";
		return false;
	}
	
}
double DminCal(Eigen::MatrixXd p1, Eigen::MatrixXd u1, Eigen::MatrixXd p2, Eigen::MatrixXd u2,Eigen::MatrixXd &CR1, Eigen::MatrixXd &CR2)
{
	//Thong so ban dau
	Eigen::MatrixXd s1 = u1 - p1;
	Eigen::MatrixXd s2 = u2 - p2;
	Eigen::MatrixXd A;
	A.setRandom(3, 2);
	A << s2(0, 0), -s1(0, 0), s2(1, 0), -s1(1, 0), s2(2, 0), -s1(2, 0);

	Eigen::MatrixXd u00(2, 1), u01(2, 1), u10(2, 1), u11(2, 1);
	u00 << 0, 0;
	u01 << 0, 1;
	u10 << 1, 0;
	u11 << 1, 1;
	Eigen::MatrixXd y = p2 - p1;

	//Phan tach QR
	Eigen::MatrixXd Q(A.rows(), A.rows());
	Eigen::MatrixXd R(A.rows(), A.cols());
	QRFactorization(A, Q, R);


	auto kqA = R*u00 + Q.transpose()*y;
	auto kqB = R*u01 + Q.transpose()*y;
	auto kqC = R*u10 + Q.transpose()*y;
	auto kqD = R*u11 + Q.transpose()*y;

	//Tinh toan he so cua duong thang
	auto kqAB = kqB - kqA;
	auto kqCD = kqD - kqC;

	double AB_b = (-kqA(0, 0)*kqAB(1, 0) + kqA(1, 0)*kqAB(0, 0)) / kqAB(0, 0);
	double CD_b = (-kqC(0, 0)*kqCD(1, 0) + kqC(1, 0)*kqCD(0, 0)) / kqCD(0, 0);
	//Tim umin
	Eigen::MatrixXd umin(2, 1);
	//Neu diem O nam trong
	if ((kqA(1, 0)*kqB(1, 0) < 0) && (AB_b*CD_b < 0))
	{
		umin << 0, 0;
		auto Matdmin = umin.transpose()*umin + y.transpose()*y - y.transpose()*Q*Q.transpose()*y;
		double dmin = sqrt(Matdmin(0, 0));
		auto x_min = R.inverse()*(umin - Q.transpose()*y);

		CR1 = p1 + x_min(1, 0)*s1;
		CR2 = p2 + x_min(0, 0)*s2;
		if ((x_min(1, 0)>1.0001) || (x_min(0, 0)>1.0001))
		{
			//std::cout << "WTF1!!!!\n\n";
		}
		return dmin;
	}
	else
	{
		const int num = 10; //100
		auto dentaAB = (kqB - kqA) / num;
		auto dentaBC = (kqC - kqB) / num;
		auto dentaCD = (kqD - kqC) / num;
		auto dentaDA = (kqA - kqD) / num;

		double dmin, dmin_tem;
		dmin = 9999;
		Eigen::MatrixXd umin(2, 1), umin_tem(2, 1);
		//check tren AB
		for (int i = 0; i < num; i++)
		{
			umin_tem = kqA + dentaAB*i;
			auto Matdmin = umin_tem.transpose()*umin_tem + y.transpose()*y - y.transpose()*Q*Q.transpose()*y;
			dmin_tem = sqrt(Matdmin(0, 0));
			if (dmin_tem < dmin)
			{
				dmin = dmin_tem;
				umin = umin_tem;
			}
		}
		//check tren BC
		for (int i = 0; i < num; i++)
		{
			umin_tem = kqB + dentaBC*i;
			auto Matdmin = umin_tem.transpose()*umin_tem + y.transpose()*y - y.transpose()*Q*Q.transpose()*y;
			dmin_tem = sqrt(Matdmin(0, 0));
			if (dmin_tem < dmin)
			{
				dmin = dmin_tem;
				umin = umin_tem;
			}
		}
		//Check tren CD
		for (int i = 0; i < num; i++)
		{
			umin_tem = kqC + dentaCD*i;
			auto Matdmin = umin_tem.transpose()*umin_tem + y.transpose()*y - y.transpose()*Q*Q.transpose()*y;
			dmin_tem = sqrt(Matdmin(0, 0));
			if (dmin_tem < dmin)
			{
				dmin = dmin_tem;
				umin = umin_tem;
			}
		}
		//Check tren DA
		for (int i = 0; i < num; i++)
		{
			umin_tem = kqD + dentaDA*i;
			auto Matdmin = umin_tem.transpose()*umin_tem + y.transpose()*y - y.transpose()*Q*Q.transpose()*y;
			dmin_tem = sqrt(Matdmin(0, 0));
			if (dmin_tem < dmin)
			{
				dmin = dmin_tem;
				umin = umin_tem;
			}
		}
		//Return ket qua tim duoc
		auto x_min = R.inverse()*(umin - (Q.transpose()*y));

		CR1 = p1 + x_min(1, 0)*s1;
		CR2 = p2 + x_min(0, 0)*s2;

		if ((x_min(1, 0)>1.0001) || (x_min(0, 0)>1.0001))
		{
			//std::cout << "WTF2!!!!\n\n";
			//std::cout << x_min << "\n";
		}


		return dmin;
	}
}

void qRDecomposition(Eigen::MatrixXd &A, Eigen::MatrixXd &Q, Eigen::MatrixXd &R)
{
	/*
	A=QR
	Q: is orthogonal matrix-> columns of Q are orthonormal
	R: is upper triangulate matrix

	this is possible when columns of A are linearly indipendent

	*/

	Eigen::MatrixXd thinQ(A.rows(), A.cols()), q(A.rows(), A.rows());

	Eigen::HouseholderQR<Eigen::MatrixXd> householderQR(A);
	q = householderQR.householderQ();
	thinQ.setIdentity();
	Q = householderQR.householderQ() * thinQ;
	R = Q.transpose()*A;
}
void QRFactorization(Eigen::MatrixXd &A, Eigen::MatrixXd &Q, Eigen::MatrixXd &R)
{

	Eigen::MatrixXd thinQ(A.rows(), A.cols()), q(A.rows(), A.rows());

	Eigen::HouseholderQR<Eigen::MatrixXd> householderQR(A);
	q = householderQR.householderQ();
	thinQ.setIdentity();
	Q = householderQR.householderQ() * thinQ;

	//std::cout << "HouseholderQR" << std::endl;

	//std::cout << "Q" << std::endl;
	//std::cout << Q << std::endl;

	//R = householderQR.matrixQR().template  triangularView<Eigen::Upper>();
	R = Q.transpose()*A;

	//std::cout << "R" << std::endl;
	//std::cout << R << std::endl;
	//std::cout << "A-Q*R" << std::endl;
	//std::cout << A - Q*R << std::endl;

}

Eigen::Matrix4d DHMatrixCal(double theta, double lengt, double offset, double twist)
{
	/*
	Matrix4 dh (cos(theta), -sin(theta)*cos(twist), sin(theta)*sin(twist), lengt * cos(theta),
	sin(theta),  cos(theta)*cos(twist), -cos(theta)*sin(twist), lengt * sin(theta),
	0,           sin(twist),             cos(twist),            offset,
	0, 0, 0, 1);
	*/
	Eigen::Matrix4d dh;
	dh << cos(theta), -sin(theta)*cos(twist), sin(theta)*sin(twist), lengt * cos(theta),
		sin(theta), cos(theta)*cos(twist), -cos(theta)*sin(twist), lengt * sin(theta),
		0, sin(twist), cos(twist), offset,
		0, 0, 0, 1;

	//std::cout << "T" << "\n";
	//std::cout << dh << "\n\n\n";
	return dh;
}
std::vector<Eigen::MatrixXd> RobotSkeleton(double q1, double q2, double q3, double q4, double q5)
{
	std::vector<Eigen::MatrixXd> vec;
	Eigen::MatrixXd p0(4, 1), p1(4, 1), p2(4, 1), p3(4, 1), p4(4, 1), p5(4,1);
	//Tinh toa do cac diem va tra ve
	const double pi = 3.14159265359;
	float a1, a2, a3, a4, a5;
	a1 = a1_DH;
	a2 = a2_DH;
	a3 = a3_DH;
	a4 = 0;
	a5 = 0;
	float d1, d2, d3, d4, d5;
	d1 = d1_DH;
	d2 = 0;
	d3 = 0;
	d4 = 0;
	d5 = toollengt;
	float alpha1, alpha2, alpha3, alpha4, alpha5;
	alpha1 = pi / 2;
	alpha2 = 0;
	alpha3 = 0;
	alpha4 = -pi / 2;
	alpha5 = 0;

	double q1_s, q2_s, q3_s, q4_s, q5_s;
	q1_s = q1;
	q2_s = q2 ;
	q3_s = q3 ;
	q4_s = q4;
	q5_s = q5;

	Eigen::Matrix4d T01, T12, T23, T34, T45;
	T01 = DHMatrixCal(q1_s, a1, d1, alpha1);
	T12 = DHMatrixCal(q2_s, a2, d2, alpha2);
	T23 = DHMatrixCal(q3_s, a3, d3, alpha3);
	T34 = DHMatrixCal(q4_s, a4, d4, alpha4);
	T45 = DHMatrixCal(q5_s, a5, d5, alpha5);

	Eigen::Matrix4d T02, T03, T04, T05;
	T02 = T01*T12;
	T03 = T02*T23;
	T04 = T03*T34;
	T05 = T04*T45;
	p0 << 0, 0, 0, 1;
	p1 = T01.block(0, 3, 3, 1);
	p2 = T02.block(0, 3, 3, 1);
	p3 = T03.block(0, 3, 3, 1);
	p4 = T04.block(0, 3, 3, 1);
	p5 = T05.block(0, 3, 3, 1);
	vec.push_back(p0);
	vec.push_back(p1);
	vec.push_back(p2);
	vec.push_back(p3);
	vec.push_back(p4);
	vec.push_back(p5);
	return vec;
}

GLuint DrawRobotSkeleton(const std::vector<Eigen::MatrixXd> robotvec)
{
	GLuint RobotSkeleton;
	RobotSkeleton = glGenLists(1);

	glNewList(RobotSkeleton, GL_COMPILE);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3f(0.f, 1.f, 1.f);
	//link 1
	glVertex3f(robotvec[0](0, 0), robotvec[0](1, 0), robotvec[0](2, 0));
	glVertex3f(robotvec[1](0, 0), robotvec[1](1, 0), robotvec[1](2, 0));
	//link 2
	glVertex3f(robotvec[1](0, 0), robotvec[1](1, 0), robotvec[1](2, 0));
	glVertex3f(robotvec[2](0, 0), robotvec[2](1, 0), robotvec[2](2, 0));
	//link 3
	glVertex3f(robotvec[2](0, 0), robotvec[2](1, 0), robotvec[2](2, 0));
	glVertex3f(robotvec[3](0, 0), robotvec[3](1, 0), robotvec[3](2, 0));
	//link 4
	//
	glVertex3f(robotvec[3](0, 0), robotvec[3](1, 0), robotvec[3](2, 0));
	glVertex3f(robotvec[4](0, 0), robotvec[4](1, 0), robotvec[4](2, 0));
	//link 5
	glColor3f(1.f, 0.f, 0.5f);
	glVertex3f(robotvec[4](0, 0), robotvec[4](1, 0), robotvec[4](2, 0));
	glVertex3f(robotvec[5](0, 0), robotvec[5](1, 0), robotvec[5](2, 0));

	glEnd();
	glEndList();

	return RobotSkeleton;
}
GLuint DrawRobotSkeleton2(const std::vector<Eigen::MatrixXd> robotvec)
{
	GLuint RobotSkeleton2;
	RobotSkeleton2 = glGenLists(1);

	glNewList(RobotSkeleton2, GL_COMPILE);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3f(0.f, 1.f, 1.f);
	//link 1
	glVertex3f(robotvec[0](0, 0), robotvec[0](1, 0), robotvec[0](2, 0));
	glVertex3f(robotvec[1](0, 0), robotvec[1](1, 0), robotvec[1](2, 0));
	//link 2
	glVertex3f(robotvec[1](0, 0), robotvec[1](1, 0), robotvec[1](2, 0));
	glVertex3f(robotvec[2](0, 0), robotvec[2](1, 0), robotvec[2](2, 0));
	//link 3
	glVertex3f(robotvec[2](0, 0), robotvec[2](1, 0), robotvec[2](2, 0));
	glVertex3f(robotvec[3](0, 0), robotvec[3](1, 0), robotvec[3](2, 0));
	//link 4
	//
	glVertex3f(robotvec[3](0, 0), robotvec[3](1, 0), robotvec[3](2, 0));
	glVertex3f(robotvec[4](0, 0), robotvec[4](1, 0), robotvec[4](2, 0));
	//link 5
	glColor3f(1.f, 0.f, 0.5f);
	glVertex3f(robotvec[4](0, 0), robotvec[4](1, 0), robotvec[4](2, 0));
	glVertex3f(robotvec[5](0, 0), robotvec[5](1, 0), robotvec[5](2, 0));

	glEnd();
	glEndList();

	return RobotSkeleton2;
}
double GetOffset(const std::vector<Eigen::MatrixXd> robotpos, Eigen::MatrixXd pos, int linknum)
{
	double offset = 0;
	switch (linknum)
	{
	case 1:
	{
		Eigen::MatrixXd line;
		line = pos - robotpos[1];
		offset = 150 - sqrt(line(0, 0)*line(0, 0) + line(1, 0)*line(1, 0) + line(2, 0)*line(2, 0));
		break;
	}
	case 2:
	{
		Eigen::MatrixXd line;
		line = pos - robotpos[1];
		double line1, line2, line3;
		line1 = line(0, 0);
		line2 = line(1, 0);
		line3 = line(2, 0);
		offset = sqrt(line(0, 0)*line(0, 0) + line(1, 0)*line(1, 0) + line(2, 0)*line(2, 0));
		break;
	}
	case 3:
	{
		Eigen::MatrixXd line;
		line = pos - robotpos[2];
		double line1, line2, line3;
		line1 = line(0, 0);
		line2 = line(1, 0);
		line3 = line(2, 0);
		offset = sqrt(line(0, 0)*line(0, 0) + line(1, 0)*line(1, 0) + line(2, 0)*line(2, 0));
		break;
	}
	case 4:
	{
		Eigen::MatrixXd line;
		line = pos - robotpos[3];
		offset = sqrt(line(0, 0)*line(0, 0) + line(1, 0)*line(1, 0) + line(2, 0)*line(2, 0));
		break;
	}
	case 5:
	{
		Eigen::MatrixXd line;
		line = pos - robotpos[3];
		offset = sqrt(line(0, 0)*line(0, 0) + line(1, 0)*line(1, 0) + line(2, 0)*line(2, 0));
		break;
	}

	default:
		break;
	}
	return offset;
}
double GetDminLine(const std::vector<Eigen::MatrixXd> robotpos, const std::vector<Eigen::MatrixXd> humanpos,
				std::vector<Eigen::MatrixXd>& out, int& linknumrobotnum, int& linkhumannum)
{
	double dmin, dmin_tem;
	linknumrobotnum = 1;
	dmin = 9999;
	if (!IsUseDummy)
	{
		std::vector<Eigen::MatrixXd> robot = robotpos;
		std::vector<Eigen::MatrixXd> human = humanpos;
		MatrixXd p1(3, 1), u1(3, 1), p2(3, 1), u2(3, 1), CR1(3, 1), CR2(3, 1);
		MatrixXd S_CR1(3, 1), S_CR2(3, 1);
		for (int i = 2; i < robotpos.size(); i++)
		{

			for (int j = 1; j < 6; j++)
			{
				dmin_tem = DminCal(robot[i - 1], robot[i], human[j - 1], human[j], CR1, CR2);
				if (dmin_tem < dmin)
				{
					dmin = dmin_tem;
					S_CR1 = CR1;
					S_CR2 = CR2;
					linknumrobotnum = i;
					linkhumannum = j;
				}
			}

			dmin_tem = DminCal(robot[i - 1], robot[i], human[6], human[7], CR1, CR2);
			if (dmin_tem < dmin)
			{
				dmin = dmin_tem;
				S_CR1 = CR1;
				S_CR2 = CR2;
				linknumrobotnum = i;
				linkhumannum = 6;
			}
		}

		if (linknumrobotnum == 4) linknumrobotnum = 3;

		out.push_back(S_CR1);
		out.push_back(S_CR2);
	}
	

	if (IsUseDummy)
	{
		Eigen::MatrixXd HuPos0(3, 1), HuPos1(3, 1);
		HuPos0 << k_dummyX, k_dummyY, k_dummyZ;
		HuPos1 << k_dummyX, k_dummyY, 0;

		//humanpos.clear();
		//humanpos.push_back(HuPos0 * 1000);
		//humanpos.push_back(HuPos1 * 1000);

		double dmin, dmin_tem;
		linknumrobotnum = 1;
		dmin = 9999;
		std::vector<Eigen::MatrixXd> robot = robotpos;
		std::vector<Eigen::MatrixXd> human = humanpos;
		MatrixXd p1(3, 1), u1(3, 1), p2(3, 1), u2(3, 1), CR1(3, 1), CR2(3, 1);
		MatrixXd S_CR1(3, 1), S_CR2(3, 1);

		for (int i = 2; i < robotpos.size(); i++)
		{
			dmin_tem = DminCal(robot[i - 1], robot[i], HuPos0, HuPos1, CR1, CR2);
			if (dmin_tem < dmin)
			{
				dmin = dmin_tem;
				S_CR1 = CR1;
				S_CR2 = CR2;
				linknumrobotnum = i;
			}
		}

		if (linknumrobotnum == 4) linknumrobotnum = 3;
		out.push_back(S_CR1);
		out.push_back(S_CR2);
		return dmin;
	}


	//dmin = dmin - 200 - 175; //200 cho capsule nguoi va 175 cho capsule robot
	dmin = dmin - 100 - 75;
	return dmin;
	//std::cout << dmin << "\n";
}
GLuint DrawDminLine(MatrixXd S_CR1, MatrixXd S_CR2)
{
	GLuint DminLine;
	DminLine = glGenLists(1);
	glNewList(DminLine, GL_COMPILE);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
		glColor3f(0.7f, 0.f, 0.7f);
		glVertex3f(S_CR2(0, 0), S_CR2(1, 0), S_CR2(2, 0));		
		glVertex3f(S_CR1(0, 0), S_CR1(1, 0), S_CR1(2, 0));
	glEnd();
	glEnable(GL_LIGHTING);
	glEndList();
	return DminLine;
}

bool CheckStateHuman()
{
	//getKinectData();
	if (NumOfPeople == 1)
	{
		State_Detect = true;
	}
	else
	{
		State_Detect = false;
		//std::cout << "Ko nhan dien duoc nguoi" << "\n";
	}
	if (IsUseDummy) State_Detect = true;
	return State_Detect;
}
bool CheckHumanSkeletonReady()
{
	return HumanSkeletonReady;
}

//Ham tinh jacobian tai moi diem thuoc robot
std::vector<double> GetQdotJacobi(std::vector<double> q_container, Eigen::MatrixXd vel, double offset, int linknum)
{
	//Ham duoc tinh toan cho robot fanuc 100i
	//Bien cuc bo
	//if (linknum == 5) linknum = 4;
	std::vector<double> qdot_container;
	double q1_dot, q2_dot, q3_dot, q4_dot, q5_dot;
	double q1, q2, q3, q4, q5;
	q1 = q_container[0];
	q2 = q_container[1];
	q3 = q_container[2];
	q4 = q_container[3];
	q5 = q_container[4];
	//Tinh toan du lieu
	switch (linknum)
	{
	case 1:
	{
		//Khi diem thuoc link 1
		Eigen::MatrixXd Jadot1(1, 3);
		Jadot1 << -(1000 * offset*sin(q1)) / (1000 * offset*offset + 1001), (1000 * offset*cos(q1)) / (1000 * offset*offset + 1001), 0;
		auto mvel = Jadot1*vel;
		q1_dot = mvel(0, 0);
		q2_dot = q3_dot = q4_dot = q5_dot = 0;
		break;
	}
	case 2:
	{
		//Khi diem thuoc link 2
		Eigen::MatrixXd Jadot2(2, 3);
		Jadot2 << -(100 * sin(q1)*(offset*cos(q2) + 150)) / (100 * offset*offset*cos(q2)*cos(q2) + 30000 * offset*cos(q2) + 2250101),
			(100 * cos(q1)*(offset*cos(q2) + 150)) / (100 * offset*offset * cos(q2)*cos(q2) + 30000 * offset*cos(q2) + 2250101),
			0,
			-(100 * offset*cos(q1)*sin(q2)) / (100 * offset*offset + 101),
			-(100 * offset*sin(q1)*sin(q2)) / (100 * offset *offset + 101),
			(100 * offset*cos(q2)) / (100 * offset*offset + 101);
		auto mvel = Jadot2*vel;
		q1_dot = mvel(0, 0);
		q2_dot = mvel(1, 0);
		q3_dot = q4_dot = q5_dot = 0;
		break;
	}
	case 3:
	{
		//Khi diem thuoc link 3
		Eigen::MatrixXd Jadot3(3, 3);
		double a3 = offset;
		Jadot3(0, 0) = -(100 * sin(q1)*(250 * cos(q2) + a3*cos(q2 + q3) + 150)) / (3125000 * cos(2 * q2) + 7500000 * cos(q2) + 30000 * a3*cos(q2 + q3) + 50 * a3*a3 * cos(2 * q2 + 2 * q3) + 25000 * a3*cos(q3) + 50 * a3*a3 + 25000 * a3*cos(2 * q2 + q3) + 5375101);
		Jadot3(0, 1) = (100 * cos(q1)*(250 * cos(q2) + a3*cos(q2 + q3) + 150)) / (3125000 * cos(2 * q2) + 7500000 * cos(q2) + 30000 * a3*cos(q2 + q3) + 50 * a3*a3 * cos(2 * q2 + 2 * q3) + 25000 * a3*cos(q3) + 50 * a3*a3 + 25000 * a3*cos(2 * q2 + q3) + 5375101);
		Jadot3(0, 2) = 0;
		Jadot3(1, 0) = -(100 * cos(q1)*(25250 * sin(q2) + 12500 * a3*a3* sin(q2) - 12500 * a3 *a3 * sin(q2 + 2 * q3) + a3*sin(q2 + q3))) / (50000 * a3*cos(q3) - 625000000 * a3*a3 * cos(q3)*cos(q3) + 625000200 * a3 *a3 + 631250201);
		Jadot3(1, 1) = -(100 * sin(q1)*(25250 * sin(q2) + 12500 * a3*a3 * sin(q2) - 12500 * a3 *a3 * sin(q2 + 2 * q3) + a3*sin(q2 + q3))) / (50000 * a3*cos(q3) - 625000000 * a3 *a3* cos(q3)*cos(q3) + 625000200 * a3*a3 + 631250201);
		Jadot3(1, 2) = (2525000 * cos(q2) - 1250000 * a3*a3 * cos(q2 + 2 * q3) + 100 * a3*cos(q2 + q3) + 1250000 * a3 *a3 * cos(q2)) / (50000 * a3*cos(q3) - 625000000 * a3 *a3* cos(q3) *cos(q3) + 625000200 * a3 *a3 + 631250201);
		Jadot3(2, 0) = (100 * cos(q1)*(25000 * sin(q2) + 12500 * a3 *a3 * sin(q2) - 12500 * a3*a3 * sin(q2 + 2 * q3) - 3125001 * a3*sin(q2 + q3) + 3125000 * a3*sin(q2 - q3))) / (50000 * a3*cos(q3) - 625000000 * a3*a3 * cos(q3)*cos(q3) + 625000200 * a3*a3 + 631250201);
		Jadot3(2, 1) = (100 * sin(q1)*(25000 * sin(q2) + 12500 * a3 *a3 * sin(q2) - 12500 * a3*a3 * sin(q2 + 2 * q3) - 3125001 * a3*sin(q2 + q3) + 3125000 * a3*sin(q2 - q3))) / (50000 * a3*cos(q3) - 625000000 * a3 *a3 * cos(q3) *cos(q3) + 625000200 * a3 *a3 + 631250201);
		Jadot3(2, 2) = -(2500000 * cos(q2) - 1250000 * a3 *a3 * cos(q2 + 2 * q3) - 312500100 * a3*cos(q2 + q3) + 312500000 * a3*cos(q2 - q3) + 1250000 * a3*a3 * cos(q2)) / (50000 * a3*cos(q3) - 625000000 * a3 *a3* cos(q3)*cos(q3) + 625000200 * a3 *a3 + 631250201);
		auto mvel = Jadot3*vel;
		q1_dot = mvel(0, 0);
		q2_dot = mvel(1, 0);
		q3_dot = mvel(2, 0);
		q4_dot = q5_dot = 0;
		break;
	}
	case 4:
	{
		//Khi diem thuoc link 3
		Eigen::MatrixXd Jadot3(3, 3);
		double a3 = offset;
		Jadot3(0, 0) = -(100 * sin(q1)*(250 * cos(q2) + a3*cos(q2 + q3) + 150)) / (3125000 * cos(2 * q2) + 7500000 * cos(q2) + 30000 * a3*cos(q2 + q3) + 50 * a3*a3 * cos(2 * q2 + 2 * q3) + 25000 * a3*cos(q3) + 50 * a3*a3 + 25000 * a3*cos(2 * q2 + q3) + 5375101);
		Jadot3(0, 1) = (100 * cos(q1)*(250 * cos(q2) + a3*cos(q2 + q3) + 150)) / (3125000 * cos(2 * q2) + 7500000 * cos(q2) + 30000 * a3*cos(q2 + q3) + 50 * a3*a3 * cos(2 * q2 + 2 * q3) + 25000 * a3*cos(q3) + 50 * a3*a3 + 25000 * a3*cos(2 * q2 + q3) + 5375101);
		Jadot3(0, 2) = 0;
		Jadot3(1, 0) = -(100 * cos(q1)*(25250 * sin(q2) + 12500 * a3*a3* sin(q2) - 12500 * a3 *a3 * sin(q2 + 2 * q3) + a3*sin(q2 + q3))) / (50000 * a3*cos(q3) - 625000000 * a3*a3 * cos(q3)*cos(q3) + 625000200 * a3 *a3 + 631250201);
		Jadot3(1, 1) = -(100 * sin(q1)*(25250 * sin(q2) + 12500 * a3*a3 * sin(q2) - 12500 * a3 *a3 * sin(q2 + 2 * q3) + a3*sin(q2 + q3))) / (50000 * a3*cos(q3) - 625000000 * a3 *a3* cos(q3)*cos(q3) + 625000200 * a3*a3 + 631250201);
		Jadot3(1, 2) = (2525000 * cos(q2) - 1250000 * a3*a3 * cos(q2 + 2 * q3) + 100 * a3*cos(q2 + q3) + 1250000 * a3 *a3 * cos(q2)) / (50000 * a3*cos(q3) - 625000000 * a3 *a3* cos(q3) *cos(q3) + 625000200 * a3 *a3 + 631250201);
		Jadot3(2, 0) = (100 * cos(q1)*(25000 * sin(q2) + 12500 * a3 *a3 * sin(q2) - 12500 * a3*a3 * sin(q2 + 2 * q3) - 3125001 * a3*sin(q2 + q3) + 3125000 * a3*sin(q2 - q3))) / (50000 * a3*cos(q3) - 625000000 * a3*a3 * cos(q3)*cos(q3) + 625000200 * a3*a3 + 631250201);
		Jadot3(2, 1) = (100 * sin(q1)*(25000 * sin(q2) + 12500 * a3 *a3 * sin(q2) - 12500 * a3*a3 * sin(q2 + 2 * q3) - 3125001 * a3*sin(q2 + q3) + 3125000 * a3*sin(q2 - q3))) / (50000 * a3*cos(q3) - 625000000 * a3 *a3 * cos(q3) *cos(q3) + 625000200 * a3 *a3 + 631250201);
		Jadot3(2, 2) = -(2500000 * cos(q2) - 1250000 * a3 *a3 * cos(q2 + 2 * q3) - 312500100 * a3*cos(q2 + q3) + 312500000 * a3*cos(q2 - q3) + 1250000 * a3*a3 * cos(q2)) / (50000 * a3*cos(q3) - 625000000 * a3 *a3* cos(q3)*cos(q3) + 625000200 * a3 *a3 + 631250201);
		auto mvel = Jadot3*vel;
		q1_dot = mvel(0, 0);
		q2_dot = mvel(1, 0);
		q3_dot = mvel(2, 0);
		q4_dot = q5_dot = 0;
		break;

	}
	case 5:
	{
		const double lamda = 0.1;
		double d5 = offset;
		//Khi diem thuoc link 4
		Eigen::MatrixXd Ja3(6, 5);
		Ja3(0, 0) = -sin(q1)*(220 * cos(q2 + q3) + 250 * cos(q2) - d5*sin(q2 + q3 + q4) + 150);
		Ja3(0, 1) = -cos(q1)*(220 * sin(q2 + q3) + 250 * sin(q2) + d5*cos(q2 + q3 + q4));
		Ja3(0, 2) = -cos(q1)*(220 * sin(q2 + q3) + d5*cos(q2 + q3 + q4));
		Ja3(0, 3) = -d5*cos(q2 + q3 + q4)*cos(q1);
		Ja3(0, 4) = 0;

		Ja3(1, 0) = cos(q1)*(220 * cos(q2 + q3) + 250 * cos(q2) - d5*sin(q2 + q3 + q4) + 150);
		Ja3(1, 1) = -sin(q1)*(220 * sin(q2 + q3) + 250 * sin(q2) + d5*cos(q2 + q3 + q4));
		Ja3(1, 2) = -sin(q1)*(220 * sin(q2 + q3) + d5*cos(q2 + q3 + q4));
		Ja3(1, 3) = -d5*cos(q2 + q3 + q4)*sin(q1);
		Ja3(1, 4) = 0;

		Ja3(2, 0) = 0;
		Ja3(2, 1) = 220 * cos(q2 + q3) + 250 * cos(q2) - d5*sin(q2 + q3 + q4);
		Ja3(2, 2) = 220 * cos(q2 + q3) - d5*sin(q2 + q3 + q4);
		Ja3(2, 3) = -d5*sin(q2 + q3 + q4);
		Ja3(2, 4) = 0;

		Ja3(3, 0) = 0;
		Ja3(3, 1) = sin(q1);
		Ja3(3, 2) = sin(q1);
		Ja3(3, 3) = sin(q1);
		Ja3(3, 4) = -sin(q2 + q3 + q4)*cos(q1);

		Ja3(4, 0) = 0;
		Ja3(4, 1) = -cos(q1);
		Ja3(4, 2) = -cos(q1);
		Ja3(4, 3) = -cos(q1);
		Ja3(4, 4) = -sin(q2 + q3 + q4)*sin(q1);

		Ja3(5, 0) = 1;
		Ja3(5, 1) = 0;
		Ja3(5, 2) = 0;
		Ja3(5, 3) = 0;
		Ja3(5, 4) = cos(q2 + q3 + q4);

		Eigen::MatrixXd Jadot3(6, 6);
		Eigen::MatrixXd v(6, 1);
		v << vel(0, 0), vel(1, 0), vel(2, 0), 0, 0, 0;
		auto i = Eigen::Matrix<double, 6, 6>::Identity();
		auto tem1 = Ja3*Ja3.transpose();
		auto tem2 = (tem1 + lamda*lamda*i);
		Jadot3 = Ja3.transpose()*tem2.inverse();
		auto mvel = Jadot3*v;
		q1_dot = mvel(0, 0);
		q2_dot = mvel(1, 0);
		q3_dot = mvel(2, 0);
		//q4_dot = mvel(3, 0);
		//q5_dot = mvel(4, 0);

		q4_dot = q5_dot = 0;
		//std::cout << "qdot link 5\n";
		//std::cout << mvel << "\n";
		break;

	}
	default:
		break;
	}


	//Dong goi du lieu
	qdot_container.push_back(q1_dot);
	qdot_container.push_back(q2_dot);
	qdot_container.push_back(q3_dot);
	qdot_container.push_back(q4_dot);
	qdot_container.push_back(q5_dot);

	return qdot_container;
}
void VelRepulsion(double dmin, double vel,
	std::chrono::time_point<std::chrono::steady_clock> t0, double &vrepmod)
{
	const double maxvel = 20;
	//Khai bao cac he so tinh chinh
	const double k1 = 20;
	const double k2 = 0;

	double velrep, velrep1, velrep2;
	//Thanh phan khoan cach (cong thuc 3.25)
	if ((dmin - DCRIT_REPULSION) < D0_REPULSION)
	{
		double ddmin = dmin;
		if (ddmin < 310) ddmin = 310;
		velrep1 = k1 * ((D0_REPULSION / (ddmin - DCRIT_REPULSION)) - 1);
	}
	else
		velrep1 = 0;
	//Thanh phan van toc (cong thuc 3.27) 
	if (vel < 0)
		velrep2 = -k2*vel;
	else
		velrep2 = 0;

	//Cong 2 thanh phan
	velrep = velrep1 + velrep2;
	if (velrep > maxvel)
	{
		velrep = maxvel;
	}
	static std::chrono::duration<float> duration;
	std::chrono::time_point<std::chrono::steady_clock> now;
	now = std::chrono::high_resolution_clock::now();
	duration = now - t0;

	double duration_ms = duration.count() * 1000;
	double garma = 1 - exp(-duration_ms / TAU);
	
	//Set garma = 1
	//garma = 1;
	vrepmod = garma * velrep;
}

Eigen::MatrixXd ControllerREP(Eigen::MatrixXd vectordmin, double dmin)
{
	//dmin = dmin - 300;

	static int State = Deactivation;
	static int LastState = State;
	static std::chrono::time_point<std::chrono::steady_clock> now, t0, lasttime, BaseTime;
	static std::chrono::duration<float> duration, time, dentat;

	now = std::chrono::high_resolution_clock::now();
	static double lastdmin = DMINCRIT;
	static double vel;
	static double vrepmod = 0;

	dentat = now - lasttime;
	vel = (dmin - lastdmin) / (dentat.count());
	if (dmin < DMINCRIT)
	{
		State = Activation;
		if (State != LastState)
			t0 = now;
	}
	else
	{
		State = Deactivation;
	}

	LastState = State;

	if (State == Activation)
	{

		VelRepulsion(dmin, vel, t0, vrepmod);
		//std::cout << "velrepmod = " << vrepmod << "\n";

		MatrixXd velrep;
		velrep = (vrepmod / dmin)*vectordmin;

		lastdmin = dmin;
		time = now - BaseTime;
		return velrep;
	}
	else
	{
		MatrixXd velrep = 0 * vectordmin;
		lastdmin = dmin;
		time = now - BaseTime;
		return velrep;
	}


	//std::cout << "Thoi diem: " << time.count() << "\n";
	//std::cout << "State " << State << "\n";
	//std::cout << "Dmin = " << dmin << "\n";



}

double HumanVel(double dmin)
{
	static std::chrono::time_point<std::chrono::steady_clock> now, t0, lasttime, BaseTime;
	static std::chrono::duration<float> duration, time, dentat;

	now = std::chrono::high_resolution_clock::now();
	static double lastdmin = DMINCRIT;
	static double vel;

	dentat = now - lasttime;
	vel = (dmin - lastdmin) / (dentat.count());
	return vel;
}

std::vector<double> GetQdotJacobiEEF(std::vector<double> q_container, Eigen::MatrixXd vel)
{
	const double lamda = 0.1;
	double d5 = 100;

	std::vector<double> qdot_container;
	double q1_dot, q2_dot, q3_dot, q4_dot, q5_dot;
	double q1, q2, q3, q4, q5;
	q1 = q_container[0];
	q2 = q_container[1];
	q3 = q_container[2];
	q4 = q_container[3];
	q5 = q_container[4];
	//Khi diem thuoc link 4
	Eigen::MatrixXd Ja3(6, 5);

	Ja3(0, 0) = -sin(q1)*(220 * cos(q2 + q3) + 250 * cos(q2) - d5*sin(q2 + q3 + q4) + 150);
	Ja3(0, 1) = -cos(q1)*(220 * sin(q2 + q3) + 250 * sin(q2) + d5*cos(q2 + q3 + q4));
	Ja3(0, 2) = -cos(q1)*(220 * sin(q2 + q3) + d5*cos(q2 + q3 + q4));
	Ja3(0, 3) = -d5*cos(q2 + q3 + q4)*cos(q1);
	Ja3(0, 4) = 0;

	Ja3(1, 0) = cos(q1)*(220 * cos(q2 + q3) + 250 * cos(q2) - d5*sin(q2 + q3 + q4) + 150);
	Ja3(1, 1) = -sin(q1)*(220 * sin(q2 + q3) + 250 * sin(q2) + d5*cos(q2 + q3 + q4));
	Ja3(1, 2) = -sin(q1)*(220 * sin(q2 + q3) + d5*cos(q2 + q3 + q4));
	Ja3(1, 3) = -d5*cos(q2 + q3 + q4)*sin(q1);
	Ja3(1, 4) = 0;

	Ja3(2, 0) = 0;
	Ja3(2, 1) = 220 * cos(q2 + q3) + 250 * cos(q2) - d5*sin(q2 + q3 + q4);
	Ja3(2, 2) = 220 * cos(q2 + q3) - d5*sin(q2 + q3 + q4);
	Ja3(2, 3) = -d5*sin(q2 + q3 + q4);
	Ja3(2, 4) = 0;

	Ja3(3, 0) = 0;
	Ja3(3, 1) = sin(q1);
	Ja3(3, 2) = sin(q1);
	Ja3(3, 3) = sin(q1);
	Ja3(3, 4) = -sin(q2 + q3 + q4)*cos(q1);

	Ja3(4, 0) = 0;
	Ja3(4, 1) = -cos(q1);
	Ja3(4, 2) = -cos(q1);
	Ja3(4, 3) = -cos(q1);
	Ja3(4, 4) = -sin(q2 + q3 + q4)*sin(q1);

	Ja3(5, 0) = 1;
	Ja3(5, 1) = 0;
	Ja3(5, 2) = 0;
	Ja3(5, 3) = 0;
	Ja3(5, 4) = cos(q2 + q3 + q4);

	Eigen::MatrixXd Jadot3(6, 6);
	Eigen::MatrixXd v(6, 1);
	v << vel(0, 0), vel(1, 0), vel(2, 0), 0, 0, 0;
	auto i = Eigen::Matrix<double, 6, 6>::Identity();
	auto tem1 = Ja3*Ja3.transpose();
	auto tem2 = (tem1 + lamda*lamda*i);
	Jadot3 = Ja3.transpose()*tem2.inverse();


	auto mvel = Jadot3*v;
	q1_dot = mvel(0, 0);
	q2_dot = mvel(1, 0);
	q3_dot = mvel(2, 0);
	q4_dot = mvel(3, 0);
	q5_dot = mvel(4, 0);

	qdot_container.push_back(q1_dot);
	qdot_container.push_back(q2_dot);
	qdot_container.push_back(q3_dot);
	qdot_container.push_back(q4_dot);
	qdot_container.push_back(q5_dot);

	return qdot_container;
}


std::vector<double> ControllerATT(Eigen::MatrixXd goal, Eigen::MatrixXd eef, std::vector<double> q_container, double dmin)
{

	const double kp = 0.1;
	double beta;
	double tem;
	dmin = dmin;

	tem = (dmin - DCRIT_REPULSION) / D0_REPULSION;
	 
	beta = (2 / (1 + exp(-(tem)*(tem)))) - 1;
	Eigen::MatrixXd vel, velkp;
	vel = goal - eef;
	std::cout << "vel\n" << vel << "\n";
	velkp = vel*kp;
	std::vector<double> qdot_container = GetQdotJacobiEEF(q_container, velkp);

	for (int i = 0; i < qdot_container.size(); ++i)
	{
		qdot_container[i] = qdot_container[i] * beta;
	}
	return qdot_container;

}

void SetDummyPos(double dummyX, double dummyY, double dummyZ) 
{
	k_dummyX = dummyX;
	k_dummyY = dummyY;
	k_dummyZ = dummyZ;
}

void SetUseDummy(bool isuse)
{
	IsUseDummy = isuse;
}

void ReDrawHumanSpace(GLuint input)
{
	HumanSpace = input;

}

#pragma optimize("", off)
