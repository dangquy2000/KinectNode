// SecondScreen.cpp : implementation file
//

//#include "pch.h"

#include "stdafx.h"
#include "KinectNodeMFC.h"
#include "SecondScreen.h"
#include "afxdialogex.h"
#include "KinectCode.h"
#include "GLView2.h"

#define PI 3.141592653589793
// SecondScreen dialog
#define Render 150
#define GUI	   151
#define Kinect 152
IMPLEMENT_DYNAMIC(SecondScreen, CDialogEx)

SecondScreen::SecondScreen(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SECOND, pParent)
{

}

SecondScreen::~SecondScreen()
{
}

void SecondScreen::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SecondScreen, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &SecondScreen::OnBnClickedButton1)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// SecondScreen message handlers


void SecondScreen::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
}


BOOL SecondScreen::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CStatic* pclStatic = (CStatic*)GetDlgItem(IDC_STATIC_Screen);
	m_pclGlView2 = new GLView2(pclStatic);
	CPaintDC dc2(this); //device context for painting
	HDC m_hDC2;
	m_hDC2 = ::GetDC(this->m_hWnd);
	RECT rect;
	GetClientRect(&rect);

	int iWidth = -(rect.right - rect.left);
	int iHeight = rect.top - rect.bottom;
	//Dua con tro dialog cho object opengl

	m_pclGlView2->OnCreate2();
	m_pclGlView2->ReSizeGLScene2(iWidth, iHeight);
	m_pclGlView2->InitGL2();

	//m_KinectStatus = m_pclGlView->GetKinectStatus();
	MyTheta theta0 = { 0,PI / 2,-PI / 2,-PI / 2,0 };
	m_pclGlView2->GetDataDlg2(theta0);

	SetTimer(Render, 30, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SecondScreen::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case Render:
	{
		MyTheta theta;
		theta.q1 = *(m_ptheta + 0);
		theta.q2 = *(m_ptheta + 1);
		theta.q3 = *(m_ptheta + 2);
		theta.q4 = *(m_ptheta + 3);
		theta.q5 = *(m_ptheta + 4);
		//m_pclGlView->GetDataDlg(theta);
		m_pclGlView2->CalDminLine2();
		m_pclGlView2->DrawGLScene2();
		break;
	}
	}
	CDialogEx::OnTimer(nIDEvent);
}
