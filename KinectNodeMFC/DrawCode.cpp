 
//#include "DrawCode.h"
#include "stdafx.h"

#include <gl/glut.h>
#include "STL_File.h"
#include "targetver.h"

#include <fstream>
#include <string>
#include <iostream>


using namespace std;

const float a1 = 150;
const float d1 = 350;

struct Vertex
{
    float x, y, z;
   
};


struct MyTheta
{
    float q1;
    float q2;
    float q3;
    float q4;
};




void DrawChessboard()
{
    //chess board
    glDisable(GL_LIGHTING);
    float latticeLen = 50;
    for (int y = -40; y < 40; y++) {
        for (int x = -40; x < 40; x++) {
            if ((x + y) % 2) {
                glColor3ub(100, 100, 100);
            }
            else {
                glColor3ub(100, 100, 100);
            }
            glBegin(GL_LINE_STRIP);
            glVertex3f(x * latticeLen, y * latticeLen, 0);
            glVertex3f((x + 1) * latticeLen, y * latticeLen, 0);
            glVertex3f((x + 1) * latticeLen, (y + 1) * latticeLen, 0);
            glVertex3f(x * latticeLen, (y + 1) * latticeLen, 0);
            glEnd();
        }
    }
    glEnable(GL_LIGHTING);
}

void DrawCoordinate()
{
	glLineWidth(4);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0, 0.0, 0.0);
    glVertex3f(1000.0, 0.0, 0.0);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1000.0, 0.0);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1000.0);
    glEnd();
    glEnable(GL_LIGHTING);
	glLineWidth(1);
}

void DrawLocalCoordinate()
{
   // glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 1.0, 0.0);
    glutSolidSphere(10, 10, 10);
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0, 0.0, 0.0);
    glVertex3f(150, 0.0, 0.0);
    glEnd();
    glTranslatef(150, 0, 0);
    glRotatef(90, 0, 1, 0);
    glutSolidCone(20, 40, 10, 10);
    glRotatef(-90, 0, 1, 0);
    glTranslatef(-150, 0, 0);
    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 150, 0.0);
    glEnd();
    glTranslatef(0, 150, 0);
    glRotatef(90, -1, 0, 0);
    glutSolidCone(20, 40, 10, 10);
    glRotatef(-90, -1, 0, 0);
    glTranslatef(0, -150, 0);
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 150);
    glEnd();
    glTranslatef(0, 0, 150);
    glutSolidCone(20, 40, 10, 10);
    glTranslatef(0, 0, -150);
    glEnable(GL_LIGHTING);
   // glPopMatrix();
}


void DrawModel(CSTL_File Model_STL[], float q1, float q2, float q3, float q4, float q5)
{
    glPushMatrix();
    // Ve khau 4
    glRotatef(90.0, 1, 0, 0);
    glTranslatef(-190.79f, 0, -95);
    glColor3f(1.0, 0.0, 0.0);
    Model_STL[0].Draw(false, true);
    glPopMatrix();
    GLfloat* m_01 = new GLfloat[16];
    for (int i = 0; i < 16; i++) {
        m_01[i] = 0;
    }
    m_01[0] = cos(q1);
    m_01[1] = sin(q1);
    m_01[4] = -sin(q1);
    m_01[5] = cos(q1);
    m_01[10] = 1;
    m_01[12] = a1 * cos(q1);
    m_01[13] = a1 * sin(q1);
    m_01[14] = d1;
    m_01[15] = 1;
    //glTranslatef(150,0, 350);
    glPushMatrix();

    glMultMatrixf(m_01);
    glTranslatef(0, 123.83, 0);
    glRotatef(90.0, 1, 0, 0);
    glTranslatef(-290.52, -134.62, 0);


    glColor3f(1.0, 1.0, 0.0);
    Model_STL[1].Draw(false, true);


    // Ve khau 2
    GLfloat* m_12 = new GLfloat[16];
    for (int i = 0; i < 16; i++) {
        m_12[i] = 0;
    }
    m_12[0] = cos(q2);
    m_12[1] = sin(q2);
    m_12[4] = -sin(q2);
    m_12[5] = cos(q2);
    m_12[10] = 1;
    m_12[12] = 1 * cos(q2);
    m_12[13] = 1 * sin(q2);
    m_12[14] = 0;
    m_12[15] = 1;

    //glTranslatef(150,0, 350);

    glPushMatrix();

    glTranslatef(290.52, 134.62, -30);
    glMultMatrixf(m_12);
    glTranslatef(-69, -69, 0);

    glColor3f(1.0, 0.0, 1.0);
    Model_STL[2].Draw(false, true);

    // Ve khau 3
    GLfloat* m_23 = new GLfloat[16];
    for (int i = 0; i < 16; i++) {
        m_23[i] = 0;
    }
    m_23[0] = cos(q3);
    m_23[1] = sin(q3);
    m_23[4] = -sin(q3);
    m_23[5] = cos(q3);
    m_23[10] = 1;
    m_23[12] = 1 * cos(q3);
    m_23[13] = 1 * sin(q3);
    m_23[14] = 0;
    m_23[15] = 1;
    //glPopMatrix();
    //glPopMatrix();
    glTranslatef(69, 69, 0);
    glTranslatef(0, 250, 53);
    glTranslatef(0, 0, 0);
    glMultMatrixf(m_23);

    glTranslatef(-140.5, -61.5, 0);

    glColor3f(0.5, 0.5, 0.0);
    Model_STL[3].Draw(false, true);

    // Ve khau 4
    GLfloat* m_43 = new GLfloat[16];
    for (int i = 0; i < 16; i++) {
        m_43[i] = 0;
    }
    m_43[0] = cos(q4);
    m_43[1] = sin(q4);
    m_43[4] = -sin(q4);
    m_43[5] = cos(q4);
    m_43[10] = 1;
    m_43[12] = 1 * cos(q4);
    m_43[13] = 1 * sin(q4);
    m_43[14] = 0;
    m_43[15] = 1;

    glTranslatef(357.5, 61.5, 100.85); //370 80 100 // chinh 0, 357 61.5 100.85
    glMultMatrixf(m_43);
    glTranslatef(-83.58, -58.5, -42.56);//-83.58, -58.5, -42.56

    glColor3f(0.0, 1.0, 0.0);
    Model_STL[4].Draw(false, true);

    glTranslatef(83.58, 58.5, 42.56);// 80.06 83.58 85


    // Ve khau 5
    GLfloat* m_54 = new GLfloat[16];
    for (int i = 0; i < 16; i++) {
        m_54[i] = 0;
    }
    m_54[0] = 1;
    m_54[5] = cos(q5);
    m_54[6] = sin(q5);
    m_54[9] = -sin(q5);
    m_54[10] = cos(q5);
    m_54[12] = 0;// 1 * cos(q5);
    m_54[13] = 0;//  1 * sin(q5);
    m_54[15] = 1;
    //glRotatef(q5, 1, 0, 0);
    glMultMatrixf(m_54);
    glTranslatef(37.5, 0, 0);
    glRotatef(0, 1, 0, 0);
    glTranslatef(-37.04, -35, -35);
    Model_STL[5].Draw(false, true);
    glTranslatef(37.04, 35, 35);
    glTranslatef(-37.5, 0, 0);



    glRotatef(-90, 1, 0, 0);
    glTranslatef(100, 0, 0); // tinh toi mut khay 5 la 80, cong them 20 la 100
    DrawLocalCoordinate();




    glPopMatrix();
}

MyTheta InvKine3(float x, float y, float z, float q5)
{
    float PI = 3.141592653589793;
    int a1, d1, a2, a3, a4;
    float q1, q2, q3, q4;
    d1 = 350; a1 = 150; a2 = 250; a3 = 220; a4 = 100;

    float R = sqrtf(x * x + y * y);

    R = R - a4 * cos(q5);
    z = z - a4 * sin(q5);
    q1 = atan2f(y, x);
    float R_a = R - a1;
    float Z_d = z - d1;

    float cos_beta = (powf(R_a, 2) + powf(Z_d, 2) - a2 * a2 - a3 * a3) / (-2 * a2 * a3);
    float beta = acosf(cos_beta);
    q3 = beta + PI + PI / 2;

    float cos_phi = (powf(R_a, 2) + powf(Z_d, 2) + a2 * a2 - a3 * a3) / (2 * a2 * sqrtf(powf(R_a, 2) + powf(Z_d, 2)));
    float phi = acosf(cos_phi);
    float anpha = atan2f(Z_d, R_a);
    q2 = phi + anpha - PI / 2;

    q4 = atan2f(sinf(q5), cosf(q5)) - (q2 + q3);
    
    MyTheta theta;
    theta.q1 = q1;
    theta.q2 = q2;
    theta.q3 = q3;
    theta.q4 = q4;
    return theta;
}

vector<Vertex>  DocFile(string FileName)
{
    float a, b, c, d;
    float x_temp, y_temp, z_temp;
    vector<Vertex>  vertices_3;
    string line;
        fstream file;
        file.open(FileName);
        while (!file.eof())
        {
            getline(file, line);
            while (line[0] == ' ')  line = line.substr(1);
            if (line[0] == 0) break;

            a = line.length();
            b = line.find(" ");
            string subline1, subline2, subline3;
            subline1 = line.substr(0, b);
            subline2 = line.substr(b);
            while (subline2[0] == ' ')  subline2 = subline2.substr(1);
            c = subline2.find(' ');
            subline3 = subline2.substr(c);
            subline2 = subline2.substr(0, c);
            while (subline3[0] == ' ')  subline3 = subline3.substr(1);

           // cout << subline1 << endl;
          //  cout << subline2 << endl;
          //  cout << subline3 << endl;

            x_temp = stof(subline1);
            y_temp = stof(subline2);
            z_temp = stof(subline3);
            
            vertices_3.push_back({ x_temp, y_temp, z_temp });
        }
        //string line = "6.500e+02    -5.000e+02    3.400e+02    ";
        cout << vertices_3.size() << endl;
        return vertices_3;
}

vector<Vertex>  GhiPoint(vector<Vertex> Point, int t)
{
    int count=0; 
    float a, b, c;
    vector<Vertex> SubPoint;
    SubPoint.resize(t * (Point.size()-1)+1);
    while (count < Point.size()-1)
    { 
        SubPoint[(int)count*t] = { Point[count].x, Point[count].y, Point[count].z };
        a = (Point[count + 1].x - Point[count].x) / t;
        b = (Point[count + 1].y - Point[count].y) / t;
        c = (Point[count + 1].z - Point[count].z) / t;
        for (int i = 1; i < t; i++)
        {
            SubPoint[count * t + i] = { Point[count].x + a * i, Point[count].y + b * i,Point[count].z + c * i };
        }
        count ++;
    }
    SubPoint[count * t]={ Point[count].x, Point[count].y, Point[count].z };
    return SubPoint;
}

void DrawModelV2(CSTL_File Model_STL[], GLuint Part[], float q1, float q2, float q3, float q4, float q5)
{
	glPushMatrix();
	// Ve khau 4
	glRotatef(90.0, 1, 0, 0);
	glTranslatef(-190.79f, 0, -95);
	glColor3f(1.0, 0.0, 0.0);


	//Model_STL[0].Draw(false, true);
	glCallList(Part[0]);

	glPopMatrix();
	GLfloat* m_01 = new GLfloat[16];
	for (int i = 0; i < 16; i++) {
		m_01[i] = 0;
	}
	m_01[0] = cos(q1);
	m_01[1] = sin(q1);
	m_01[4] = -sin(q1);
	m_01[5] = cos(q1);
	m_01[10] = 1;
	m_01[12] = a1 * cos(q1);
	m_01[13] = a1 * sin(q1);
	m_01[14] = d1;
	m_01[15] = 1;
	//glTranslatef(150,0, 350);
	glPushMatrix();

	glMultMatrixf(m_01);
	glTranslatef(0, 123.83, 0);
	glRotatef(90.0, 1, 0, 0);
	glTranslatef(-290.52, -134.62, 0);


	glColor3f(1.0, 1.0, 0.0);


	//Model_STL[1].Draw(false, true);
	glCallList(Part[1]);

	// Ve khau 2
	GLfloat* m_12 = new GLfloat[16];
	for (int i = 0; i < 16; i++) {
		m_12[i] = 0;
	}
	m_12[0] = cos(q2);
	m_12[1] = sin(q2);
	m_12[4] = -sin(q2);
	m_12[5] = cos(q2);
	m_12[10] = 1;
	m_12[12] = 1 * cos(q2);
	m_12[13] = 1 * sin(q2);
	m_12[14] = 0;
	m_12[15] = 1;

	//glTranslatef(150,0, 350);

	glPushMatrix();

	glTranslatef(290.52, 134.62, -30);
	glMultMatrixf(m_12);
	glTranslatef(-69, -69, 0);

	glColor3f(1.0, 0.0, 1.0);


	//Model_STL[2].Draw(false, true);
	glCallList(Part[2]);

	// Ve khau 3
	GLfloat* m_23 = new GLfloat[16];
	for (int i = 0; i < 16; i++) {
		m_23[i] = 0;
	}
	m_23[0] = cos(q3);
	m_23[1] = sin(q3);
	m_23[4] = -sin(q3);
	m_23[5] = cos(q3);
	m_23[10] = 1;
	m_23[12] = 1 * cos(q3);
	m_23[13] = 1 * sin(q3);
	m_23[14] = 0;
	m_23[15] = 1;
	//glPopMatrix();
	//glPopMatrix();
	glTranslatef(69, 69, 0);
	glTranslatef(0, 250, 53);
	glTranslatef(0, 0, 0);
	glMultMatrixf(m_23);

	glTranslatef(-140.5, -61.5, 0);

	glColor3f(0.5, 0.5, 0.0);

	//Model_STL[3].Draw(false, true);
	glCallList(Part[3]);
	// Ve khau 4
	//q4 = q4 + PI / 2;       //sau khi chinh bang DH ngay 27/12/2020
	GLfloat* m_43 = new GLfloat[16];
	for (int i = 0; i < 16; i++) {
		m_43[i] = 0;
	}
	m_43[0] = cos(q4);
	m_43[1] = sin(q4);
	m_43[4] = -sin(q4);
	m_43[5] = cos(q4);
	m_43[10] = 1;
	m_43[12] = 1 * cos(q4);
	m_43[13] = 1 * sin(q4);
	m_43[14] = 0;
	m_43[15] = 1;

	glTranslatef(357.5, 61.5, 100.85); //370 80 100 // chinh 0, 357 61.5 100.85
	glMultMatrixf(m_43);
	glTranslatef(-83.58, -58.5, -42.56);//-83.58, -58.5, -42.56

	glColor3f(0.0, 1.0, 0.0);
	//Model_STL[4].Draw(false, true);
	glCallList(Part[4]);
	glTranslatef(83.58, 58.5, 42.56);// 80.06 83.58 85


									 // Ve khau 5
	GLfloat* m_54 = new GLfloat[16];
	for (int i = 0; i < 16; i++) {
		m_54[i] = 0;
	}
	m_54[0] = 1;
	m_54[5] = cos(q5);
	m_54[6] = sin(q5);
	m_54[9] = -sin(q5);
	m_54[10] = cos(q5);
	m_54[12] = 0;// 1 * cos(q5);
	m_54[13] = 0;//  1 * sin(q5);
	m_54[15] = 1;
	//glRotatef(q5, 1, 0, 0);
	glMultMatrixf(m_54);
	glTranslatef(37.5, 0, 0);
	glRotatef(0, 1, 0, 0);
	glTranslatef(-37.04, -35, -35);

	//Model_STL[5].Draw(false, true);
	glCallList(Part[5]);
	glTranslatef(37.04, 35, 35);
	glTranslatef(-37.5, 0, 0);



	//glRotatef(-90, 1, 0, 0);
	//glRotatef(90, 0, 1,0);
	glTranslatef(100, 0, 0); // tinh toi mut khay 5 la 80, cong them 20 la 100
	glRotatef(90, 0, 1, 0);
	glRotatef(-90, 0, 0, 1);
	DrawLocalCoordinate();




	glPopMatrix();
}
