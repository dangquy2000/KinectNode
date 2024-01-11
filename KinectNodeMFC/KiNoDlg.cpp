
// KinNoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KinectNodeMFC.h"
#include "KiNoDlg.h"
#include "SecondScreen.h"
#include "afxdialogex.h"
#include "KinectCode.h"
#include "GLView.h"

double dem = 0;
double x, y, z, thx, thy;
// C3DSim dialog
bool f_usingEditBox;
bool f_usingSlider;
bool f_MouseDown = 0;
bool f_inWindow = 0;
int lastX = 0;
int lastY = 0;
int currentX = 0;
int currentY = 0;
int mouseMoveX = 0;
int mouseMoveY = 0;

#define PI 3.141592653589793
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define RenderTimer 101
#define GuiTimer 102
#define KinectTimer 103
#define SecondTimer 104
#define SendTimer 105
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CKiNoDlg dialog



CKiNoDlg::CKiNoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_KINECTNODEMFC_DIALOG, pParent)
	, m_chkFaker(FALSE)
	, m_sttDmin(_T(""))
	, m_sttTCPStatus(_T(""))
	, m_IsOnSimu(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Qdot[0] = 0.0;
	m_Qdot[1] = 0.0;
	m_Qdot[2] = 0.0;
	m_Qdot[3] = 0.0;
	m_Qdot[4] = 0.0;

	m_ptheta[0] = 0.0;
	m_ptheta[1] = +PI/2;
	m_ptheta[2] = -PI/2;
	m_ptheta[3] = -PI/2;
	m_ptheta[4] = 0.0;
	m_KinectStatus = FALSE;
	m_bIsSend = FALSE;
	m_REfps = m_SEfps = 0;
}

void CKiNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPENGL, OPENGLWIN);
	DDX_Text(pDX, IDC_STT_KINECTSTATUS, m_kinect_Status);
	DDX_Control(pDX, IDC_CBO_CPORT, m_cboPort);
	DDX_Control(pDX, IDC_CBO_BAUD, m_cboBaud);
	DDX_Control(pDX, IDC_LST_SEND, m_lstSend);
	DDX_Control(pDX, IDC_LST_RECEIVE, m_lstReceive);

	DDX_Text(pDX, IDC_STT_CDEBUG, m_sttDebug);
	DDX_Text(pDX, IDC_STT_CRECEIVESTT, m_sttReStatus);
	DDX_Text(pDX, IDC_STT_CREFPS, m_sttRefps);
	DDX_Text(pDX, IDC_STT_CSEFPS, m_sttSefps);
	DDX_Text(pDX, IDC_STT_CSENDSTT, m_sttSeStatus);

	DDX_Check(pDX, IDC_CHK_FAKER, m_chkFaker);
	DDX_Text(pDX, IDC_STT_DMIN, m_sttDmin);
	DDX_Control(pDX, IDC_EDT_TCPPORT, m_edtTCPPort);
	DDX_Text(pDX, IDC_STT_TCPSTATUS, m_sttTCPStatus);
	DDX_Check(pDX, IDC_CHK_SIMU, m_IsOnSimu);
}

BEGIN_MESSAGE_MAP(CKiNoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()

	ON_BN_CLICKED(IDC_BTN_KINECTSTART, &CKiNoDlg::OnBnClickedBtnKinectstart)
	ON_BN_CLICKED(IDC_BTN_CSTART, &CKiNoDlg::OnBnClickedBtnCstart)
	ON_BN_CLICKED(IDC_BTN_CSTOP, &CKiNoDlg::OnBnClickedBtnCstop)
	ON_BN_CLICKED(IDC_CSEND, &CKiNoDlg::OnBnClickedCsend)
	ON_BN_CLICKED(IDC_CHK_FAKER, &CKiNoDlg::OnBnClickedChkFaker)
	ON_BN_CLICKED(IDC_BTN_CSTARTSERVER, &CKiNoDlg::OnBnClickedBtnCstartserver)

	ON_BN_CLICKED(IDC_BTN_CSTOPSERVER, &CKiNoDlg::OnBnClickedBtnCstopserver)
	ON_BN_CLICKED(IDC_CHK_SIMU, &CKiNoDlg::OnBnClickedChkSimu)
	ON_BN_CLICKED(IDC_BUTTON_Screen2, &CKiNoDlg::OnBnClickedButtonScreen2)
	ON_BN_CLICKED(IDC_BUTTON_load, &CKiNoDlg::OnBnClickedButtonload)
END_MESSAGE_MAP()



// CKiNoDlg message handlers

BOOL CKiNoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	// Thiet lap cho dialog

	// Thiet lap cho opengl
	CStatic* pclStatic = (CStatic*)GetDlgItem(IDC_OPENGL);
	m_pclGlView = new GLView(pclStatic);
	CPaintDC dc(this); //device context for painting
	HDC m_hDC;
	m_hDC = ::GetDC(this->m_hWnd);
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);
	RECT rect;
	GetClientRect(&rect);

	int iWidth = -(rect.right - rect.left);
	int iHeight = rect.top - rect.bottom;
	//Dua con tro dialog cho object opengl

	m_pclGlView->OnCreate();
	m_pclGlView->ReSizeGLScene(iWidth, iHeight);
	m_pclGlView->InitGL();

	//m_KinectStatus = m_pclGlView->GetKinectStatus();
	MyTheta theta0 = { 0,PI/2,-PI / 2,-PI / 2,0 };
	m_pclGlView->GetDataDlg(theta0);
	m_pclGlView->CalDminLine();
	//Init kinect
	

	//CWnd::SetTimer(2, 1000, NULL);
	m_cboPort.AddString(_T("COM1"));
	m_cboPort.AddString(_T("COM2"));
	m_cboPort.AddString(_T("COM3"));
	m_cboPort.AddString(_T("COM4"));
	m_cboPort.AddString(_T("COM5"));
	m_cboPort.AddString(_T("COM6"));
	m_cboPort.AddString(_T("COM7"));
	m_cboPort.AddString(_T("COM8"));
	m_cboPort.AddString(_T("COM9"));
	m_cboPort.AddString(_T("COM10"));
	m_cboPort.SetCurSel(4);
	m_cboBaud.AddString(_T("9600"));
	m_cboBaud.AddString(_T("115200"));
	m_cboBaud.SetCurSel(1);

	m_edtTCPPort.SetWindowTextW(_T("54000"));

	CWnd::SetTimer(RenderTimer, 50, NULL);//16.6667
	CWnd::SetTimer(GuiTimer, 30, NULL);
	CWnd::SetTimer(KinectTimer, 34, NULL);
	CWnd::SetTimer(SecondTimer, 1000, NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKiNoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKiNoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}

	RECT rect;
	GetClientRect(&rect);
	int iWidth = -(rect.right - rect.left);
	int iHeight = rect.top - rect.bottom;

	m_pclGlView->OnCreate();
	m_pclGlView->ReSizeGLScene(iWidth, iHeight);

	MyTheta theta0;
	theta0.q1 = *(m_ptheta + 0);
	theta0.q2 = *(m_ptheta + 1);
	theta0.q3 = *(m_ptheta + 2);
	theta0.q4 = *(m_ptheta + 3);
	theta0.q5 = *(m_ptheta + 4);
	m_pclGlView->GetDataDlg(theta0);
	m_pclGlView->DrawGLScene();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKiNoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CKiNoDlg::~CKiNoDlg()
{
	delete m_pclGlView;
}
void CKiNoDlg::OnClose()
{
	CDialog::OnClose();
	m_bIsUseTCP = false;
	DestroyWindow();
}

void CKiNoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case RenderTimer:
	{
		MyTheta theta;
		theta.q1 = PI / 2;// *(m_ptheta + 0);
		theta.q2 = *(m_ptheta + 1);
		theta.q3 = *(m_ptheta + 2);
		theta.q4 = *(m_ptheta + 3);
		theta.q5 = *(m_ptheta + 4);
		//m_pclGlView->GetDataDlg(theta);
		m_pclGlView->DrawGLScene();
		break;
	}
	case GuiTimer:
	{
		UpdateData(FALSE);

		if (m_bIsUseTCP)
		{
			m_lstReceive.AddString(m_RecvBuf);
			m_lstSend.AddString(m_SendBuf);
		}
		int count1 = m_lstReceive.GetCount();
		m_lstReceive.SetCurSel(count1 - 1);
		int count2 = m_lstSend.GetCount();
		m_lstSend.SetCurSel(count2 - 1);
		UpdateData(false);



		double dmin;
		dmin = m_pclGlView->GetDmin();
		CString tem;
		tem.Format(_T("dmin = %.01f"), dmin);
		m_sttDmin = tem;
		break;
	}
	case KinectTimer:
	{
		if (m_KinectStatus)
		{
			getKinectData();
			m_pclGlView->CalDminLine();
			m_dmin = m_pclGlView->GetDmin();
		}
		break;
	}
	case SecondTimer:
	{
		CString temp, temp2;
		temp.Format(_T("%d"), m_SEfps);
		m_sttSefps = temp;
		temp2.Format(_T("%d"), m_REfps);
		m_sttRefps = temp2;
		UpdateData(false);
		m_SEfps = 0;
		m_REfps = 0;
		break;
	}
	case SendTimer:
	{
		SendUARTQdot();
		break;
	}
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}
BOOL CKiNoDlg::PreTranslateMessage(MSG* pMsg)
{
	int X = (int)pMsg->wParam;

	if (pMsg->message == WM_KEYDOWN)
	{
		m_pclGlView->MyKeyBoard(X);
	}

	if (pMsg->message == WM_LBUTTONDOWN)
	{
		f_MouseDown = 1;

		GetDlgItem(IDC_OPENGL)->GetSafeHwnd() == pMsg->hwnd;

		OnMouseMove(0, pMsg->pt);
		pMsg->wParam;

		lastX = pMsg->pt.x;
		lastY = pMsg->pt.y;
		RECT rec;
		OPENGLWIN.GetWindowRect(&rec);

		if ((lastX < rec.right) && (lastX > rec.left) && (lastY < rec.bottom) && (lastY > rec.top))
		{
			f_inWindow = 1;
		}
		else
		{
			f_inWindow = 0;
		}
		/*
		CClientDC dc(this);
		CString str;
		str.Format(_T("X: %d,  Y: %d    "), lastX, lastY);
		dc.TextOutW(200, 10, str);
		*/
	}
	if (pMsg->message == WM_MOUSEMOVE)
	{
		GetDlgItem(IDC_OPENGL)->GetSafeHwnd() == pMsg->hwnd;

		//OnMouseMove(0, pMsg->pt);
		pMsg->wParam;
		currentX = pMsg->pt.x;
		currentY = pMsg->pt.y;

		if (f_MouseDown && f_inWindow)
		{

			int diffX = currentX - lastX;
			int diffY = currentY - lastY;
			m_pclGlView->MyLeftMouse(diffX, diffY);
			m_pclGlView->DrawGLScene();
		}
		/*
		CClientDC dc(this);
		CString str;
		str.Format(_T("X: %d,  Y: %d    "), currentX, currentY);
		dc.TextOutW(500, 10, str);
		*/
	}
	if (pMsg->message == WM_LBUTTONUP) f_MouseDown = 0;

	return 0;
}

void CKiNoDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	mouseMoveX = point.x;
	mouseMoveY = point.y;
	CDialog::OnMouseMove(nFlags, point);
}

BOOL CKiNoDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	
	RECT rec;
	OPENGLWIN.GetWindowRect(&rec);
	int x = pt.x;
	int y = pt.y;
	if ((x < rec.right) && (x > rec.left) && (y < rec.bottom) && (y > rec.top))
	{
		m_pclGlView->MyWheelMouse(zDelta);
	}
	CClientDC dc(this);
	CString str;
	//str.Format(_T( "zDenta: %d    "),zDelta );
	//dc.TextOutW(700, 10, str);
	//m_pclGlView->DrawGLScene();
	return 0;
}

void CKiNoDlg::OnBnClickedBtnKinectstart()
{
	// TODO: Add your control notification handler code here
	 m_KinectStatus = m_pclGlView->InitKinect();

	 if (m_KinectStatus)
		 m_kinect_Status = "Ok";
	 else
	 {
		 m_kinect_Status = "False";
		// m_pclGlView->GhiFile();
	 }
	 UpdateData(FALSE);
}

// -----------------------------------------------------------------------------------
// Phan Serial comunication code

void CKiNoDlg::OnBnClickedBtnCstart()
{
	int port, baudrate;
	port = m_cboPort.GetCurSel();
	baudrate = m_cboBaud.GetCurSel();
	CString name;
	m_cboPort.GetWindowTextW(name);
	switch (baudrate)
	{
	case 0:
	{
		m_dcb.BaudRate = CBR_9600;
		break;
	}
	case 1:
	{
		m_dcb.BaudRate = CBR_115200;
		break;
	}
	default:
		break;
	}
	m_dcb.ByteSize = 8;
	m_dcb.Parity = NOPARITY;
	m_dcb.StopBits = ONESTOPBIT;
	if (m_bPortOpened == FALSE)
	{
		OpenPortV2(name, m_dcb);
	}

}
void CKiNoDlg::OnBnClickedCsend()
{
	m_bIsSend = true;

	//Delay 30ms
	CWnd::SetTimer(SendTimer, 30, NULL);

}
void CKiNoDlg::OnBnClickedBtnCstop()
{
	// TODO: Add your control notification handler code here
	m_bIsSend = false;
	CWnd::KillTimer(SendTimer);
	ClosePort();

}
void CKiNoDlg::OnEventOpen(BOOL bSuccess)
{
	CString str;
	if (bSuccess)
	{
		str = m_strPortName + _T(" open successfully");
		m_bPortOpened = TRUE;
		//m_btnOpen.SetWindowText("Close");

	}
	else
	{
		str = m_strPortName + _T(" open failed");

		m_bPortOpened = FALSE;
	}

	m_sttDebug = str;
}
void CKiNoDlg::OnEventClose(BOOL bSuccess)
{
	CString str;
	if (bSuccess)
	{
		str = m_strPortName + _T(" close successfully");
		m_bPortOpened = FALSE;
		//m_edtDebug.SetWindowText(_T("Open"));

	}
	else
	{
		str = m_strPortName + _T(" close failed");
	}
	m_sttDebug = str;
}
void CKiNoDlg::OnEventRead(char *inPacket, int inLength)
{
	CString str;
	str.Format(_T("%d bytes read"), inLength);
	m_sttReStatus = str;
	//Xu ly chuoi
	m_UARTBuff.append(inPacket);
	std::size_t stop;
	std::size_t start;
	stop = m_UARTBuff.find("ST");
	while (stop != std::string::npos)
	{
		start = m_UARTBuff.find("BEGIN");
		if (start == std::string::npos) break;
		std::string strlenh;
		strlenh.assign(m_UARTBuff, start, stop - (start));
		DeCode(strlenh);
		m_UARTBuff.erase(0, stop + 2);
		stop = m_UARTBuff.find("ST");
		//std::cout << UARTBuff << '\n';
	}
	CString test;
	test = inPacket;
	m_lstReceive.AddString(test);
	int count = m_lstReceive.GetCount();
	m_lstReceive.SetCurSel(count - 1);
	m_REfps++;

}
void CKiNoDlg::OnEventWrite(int nWritten)
{
	m_SEfps++;

}

void CKiNoDlg::DeCode(std::string str)
{
	str.erase(0, 5);
	int code = (str[0] - 0x30) * 1000 + (str[1] - 0x30) * 100 + (str[2] - 0x30) * 10 + (str[3] - 0x30) * 1;
	switch (code)
	{
	case 2010:
	{
		std::string tem = &str[4];
		std::size_t start1 = tem.find("q1=");
		std::size_t start2 = tem.find("q2=");
		std::size_t start3 = tem.find("q3=");
		std::size_t start4 = tem.find("q4=");
		std::size_t start5 = tem.find("q5=");
		std::size_t stop = tem.find("ST");
		std::string numstr1, numstr2, numstr3, numstr4, numstr5;
		numstr1.assign(tem, start1 + 3, start2 - 1 - (start2 + 3));
		numstr2.assign(tem, start2 + 3, start3 - 1 - (start2 + 3));
		numstr3.assign(tem, start3 + 3, start4 - 1 - (start3 + 3));
		numstr4.assign(tem, start4 + 3, start5 - 1 - (start4 + 3));
		numstr5.assign(tem, start5 + 3, stop - (start5 + 3));
		*(m_ptheta + 0) = std::stod(numstr1);
		*(m_ptheta + 1) = std::stod(numstr2);
		*(m_ptheta + 2) = std::stod(numstr3);
		*(m_ptheta + 3) = std::stod(numstr4);
		*(m_ptheta + 4) = std::stod(numstr5);

		m_sttDebug = "Ma lenh 2010";
		break;
	}
	case 2011:
	{
		std::string tem = &str[4];
		std::size_t start = tem.find("--");
		std::size_t stop = tem.find("ST");
		std::string numstring;
		numstring.assign(tem, start + 2, stop - (start + 2));
		*(m_ptheta + 0) = std::stod(numstring);
		//std::cout <<"Day la q1: " <<q1 << '\n';
		break;
	}
	case 2012:
	{
		std::string tem = &str[4];
		std::size_t start = tem.find("--");
		std::size_t stop = tem.find("ST");
		std::string numstring;
		numstring.assign(tem, start + 2, stop - (start + 2));
		*(m_ptheta + 1) = std::stod(numstring);
		//std::cout << "Day la q2: " << q2 << '\n';
		break;
	}
	case 2013:
	{
		std::string tem = &str[4];
		std::size_t start = tem.find("--");
		std::size_t stop = tem.find("ST");
		std::string numstring;
		numstring.assign(tem, start + 2, stop - (start + 2));
		*(m_ptheta + 2) = std::stod(numstring);
		//std::cout << "Day la q3: " << q3 << '\n';
		break;
	}
	case 2014:
	{
		std::string tem = &str[4];
		std::size_t start = tem.find("--");
		std::size_t stop = tem.find("ST");
		std::string numstring;
		numstring.assign(tem, start + 2, stop - (start + 2));
		*(m_ptheta + 3) = std::stod(numstring);
		//std::cout << "Day la q4: " << q4 << '\n';
		break;
	}
	case 2015:
	{
		std::string tem = &str[4];
		std::size_t start = tem.find("--");
		std::size_t stop = tem.find("ST");
		std::string numstring;
		numstring.assign(tem, start + 2, stop - (start + 2));
		*(m_ptheta + 4) = std::stod(numstring);
		//std::cout << "Day la q5: " << q5 << '\n';
		break;
	}
	case 1002:
	{
		break;
	}
	default:
		break;
	}
}

void CKiNoDlg::SendUARTQdot()
{

	if (m_bPortOpened)
	{
		char msg1[100];
		char msg2[100];
		char msg3[200];
		std::vector<double> qdot;
		double dmin;
		//Rdata temp;
		dmin = m_pclGlView->GetDmin();

		qdot = m_pclGlView->GetQdot();

		//Gioi han qdot tranh tinh trang toc do qua lon
		//toc do ne <20 do
		const double minvel = 45 * PI / 180;
		for (int i = 0; i < 4; i++)
		{
			if (abs(qdot[i])>minvel)
			{
				qdot[i] = minvel*(qdot[0] / abs(qdot[0]));
			}
		}
		sprintf(msg1, "BEGIN2020--q1d=%.05fq2d=%.05fq3d=%.05fq4d=%.05fq5d=%.05fdmn=%0.1fST",
			qdot[0], qdot[1], qdot[2], qdot[3], qdot[4], dmin);

		//sprintf(msg2, "BEGIN2021--dmin=%0.5fST", dmin);
		//sprintf(msg3, "BEGIN2020--q1d=%.05fq2d=%.05fq3d=%.05fq4d=%.05fq5d=%.05fSTBEGIN2021--dmin=%0.5fST", qdot[0], qdot[1], qdot[2], qdot[3], qdot[4],dmin);

		Write(msg1, strlen(msg1));

		//std::this_thread::sleep_for(std::chrono::microseconds(5000));
		//Write(msg2, strlen(msg2));
		//Write(msg3, strlen(msg2));

		CString tem1, tem2;
		tem1 = msg1;
		m_lstSend.AddString(tem1);	
		int count = m_lstSend.GetCount();
		m_lstSend.SetCurSel(count - 1);

	}
}

void CKiNoDlg::OnBnClickedChkFaker()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_pclGlView->SetDummyIsUse(m_chkFaker);

}



void CKiNoDlg::OnBnClickedBtnCstartserver()
{
	// TODO: Add your control notification handler code here
	m_bIsUseTCP = true;
	// update trang thai
	m_sttTCPStatus.Format(_T("Server is on now "));
	// khoi dong server
	tw1 = StartServer();
}
std::thread CKiNoDlg::StartServer()
{
	return std::thread([=] {ServerMain(); });
	
}

void CKiNoDlg::OnBnClickedBtnCstopserver()
{
	// TODO: Add your control notification handler code here
	m_bIsUseTCP = false;
	m_sttTCPStatus.Format(_T("Shutdown server"));
	if (tw1.joinable())
	{
		tw1.join();
	//	m_pclGlView->GhiFile();
	}
}


void  CKiNoDlg::ServerMain()
{
	WSADATA data;

	// To start WinSock, the required version must be passed to
	// WSAStartup(). This server is going to use WinSock version
	// 2 so I create a word that will store 2 and 2 in hex i.e.
	// 0x0202
	WORD version = MAKEWORD(2, 2);

	// Start WinSock
	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0)
	{
		// Not ok! Get out quickly
		//cout << "Can't start Winsock! " << wsOk;
		return;
	}

	////////////////////////////////////////////////////////////
	// SOCKET CREATION AND BINDING
	////////////////////////////////////////////////////////////

	// Create a socket, notice that it is a user datagram socket (UDP)

	m_in = socket(AF_INET, SOCK_DGRAM, 0);

	// Create a server hint structure for the server
	sockaddr_in serverHint;
	serverHint.sin_addr.S_un.S_addr = ADDR_ANY; // Us any IP address available on the machine
	serverHint.sin_family = AF_INET; // Address format is IPv4
	serverHint.sin_port = htons(54000); // Convert from little to big endian


										//cout << "Dang doi ket noi... \n";
										// Try and bind the socket to the IP and port

	m_sttTCPStatus.Format(_T("Dang doi ket noi"));
	std::this_thread::sleep_for(std::chrono::microseconds(20000));
	int tan = bind(m_in, (sockaddr*)&serverHint, sizeof(serverHint));
	if ( tan == SOCKET_ERROR)
	{
		//cout << "Can't bind socket! " << WSAGetLastError() << endl;
		return;
	}
	m_sttTCPStatus.Format(_T("Da ket noi"));
	////////////////////////////////////////////////////////////
	// MAIN LOOP SETUP AND ENTRY
	////////////////////////////////////////////////////////////

	//sockaddr_in client; // Use to hold the client information (port / ip address)
	//int clientLength = sizeof(client); // The size of the client information


	sprintf(m_tcpbuf, "hello");
	std::string v = "hi";
	// Enter a loop
	int i = 0;

	sprintf(m_tcpmsg, "BEGIN");

	int bytesIn = 0;
	std::this_thread::sleep_for(std::chrono::microseconds(20000));
	while (m_bIsUseTCP)
	{		
		// Enter a loop

		std::this_thread::sleep_for(std::chrono::microseconds(10000));
		ZeroMemory(&client, clientLength); // Clear the client structure
		ZeroMemory(m_tcpbuf, 1024); // Clear the receive buffer

							   // Wait for message
		bytesIn = recvfrom(m_in, m_tcpbuf, 1024, 0, (sockaddr*)&client, &clientLength);

		//bytesIn = recv(m_in, m_tcpbuf, 1024, 0);
		if (bytesIn == SOCKET_ERROR)
		{
			//cout << "Error receiving from client " << WSAGetLastError() << endl;
			continue;
		}
		else
		{
			m_REfps++;
			m_RecvBuf = m_tcpbuf;
			DeCode(m_tcpbuf);
		}

		// Display message and client info
		char clientIp[256]; // Create enough space to convert the address byte array
		ZeroMemory(clientIp, 256); // to string of characters

								   // Convert from byte array to chars
		inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);

		// Display the message / who sent it
		//cout << "Message recv from " << clientIp << " : " << buf << endl;

		//01234567890123456789012345678901234567890123456789012345678901234567890123456789
		std::vector<double> qdot = { 0,0,0,0,0 };
		double dmin;

		dmin = m_pclGlView->GetDmin();

		qdot = m_pclGlView->GetQdot();

		//Gioi han qdot tranh tinh trang toc do qua lon
		//toc do ne <20 do
		const double minvel = 45 * PI / 180;
		for (int i = 0; i < 4; i++)
		{
			if (abs(qdot[i])>minvel)
			{
				qdot[i] = minvel*(qdot[0] / abs(qdot[0]));
			}
		}
		sprintf(m_tcpmsg, "BEGIN2020--q1d=%.05fq2d=%.05fq3d=%.05fq4d=%.05fq5d=%.05fdmn=%0.1fST",
			qdot[0], qdot[1], qdot[2], qdot[3], qdot[4], dmin);
		//sprintf(msg, "Hello lan thu %d 901234567890123456789012345678901234567890123456789012345678901234567890123456789 ", i);
		
		//i++;
		v = m_tcpmsg;
		int sendOk = sendto(m_in, v.c_str(), v.size() + 1, 0, (sockaddr*)&client, sizeof(client));

		if (sendOk == SOCKET_ERROR)
		{
			//cout << "That didn't work! " << WSAGetLastError() << endl;
		}
		else
		{
			m_SEfps++;
			//CString tem2;
			//tem2 = msg;
			m_SendBuf = m_tcpmsg;

			//m_lstSend.AddString(tem2);
		}
	}

	// Close socket
	closesocket(m_in);

	// Shutdown winsock
	WSACleanup();
}


void CKiNoDlg::OnBnClickedChkSimu()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if (m_IsOnSimu)
	{
		m_pclGlView->loopthread = m_pclGlView->StartSimuLoop();
	}
	if (!m_IsOnSimu)
	{
		m_pclGlView->m_useloop = false;
		//if (m_pclGlView->loopthread.joinable())
		//	m_pclGlView->loopthread.join();
		m_pclGlView->GhiFile();
	}



}


void CKiNoDlg::OnBnClickedButtonScreen2()
{
	SecondScreen* m_SecondScreen = new SecondScreen(this);
	m_SecondScreen->Create(IDD_DIALOG_SECOND);
	m_SecondScreen->ShowWindow(SW_SHOW);
}


void CKiNoDlg::OnBnClickedButtonload()
{
	UpdateData();
	m_pclGlView->loadloopthread = m_pclGlView->StartLoad();
}
