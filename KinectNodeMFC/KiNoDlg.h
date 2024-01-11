
// KiNoDlg.h : header file
//
#pragma once
#include "GLView.h"
#include "afxwin.h"
#include "SerialCtrl.h"
// CKiNoDlg dialog

#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

class CKiNoDlg : public CDialogEx, public CSerialIO
{
// Construction
public:
	CKiNoDlg(CWnd* pParent = NULL);	// standard constructor
	~CKiNoDlg();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KINECTNODEMFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
//------------------------------------------------------------------------------------------------
private:
	std::vector<double> m_qdot;
	std::vector<double> m_qs;
	bool m_bIsSend;
	DCB m_dcb;
	int m_SEfps;
	int m_REfps;
	bool m_bPortOpened;
	double m_dmin;
	std::string m_UARTBuff;
	CString m_kinect_Status;
	CString m_sttSefps;
	CString m_sttRefps;
	CString m_sttDebug;
	CString m_sttSeStatus;
	CString m_sttReStatus;
	double m_Qdot[5];
	double m_ptheta[5];
	bool m_KinectStatus;
	CComboBox m_cboPort;
	CComboBox m_cboBaud;
	CListBox m_lstSend;
	CListBox m_lstReceive;

	//TCP/IP
	SOCKET m_in;
	char m_tcpbuf[1024];
	char m_tcpmsg[1000];
	std::thread tw1;
	bool m_bIsUseTCP = false;
	CString m_RecvBuf;
	CString m_SendBuf;


public:
	sockaddr_in client; // Use to hold the client information (port / ip address)
	int clientLength = sizeof(client); // The size of the client information
	CStatic OPENGLWIN;
	MyTheta theta_exchange;
	GLView* m_pclGlView;

//------------------------------------------------------------------------------------------------
private:
	void OnClose();
public:
	afx_msg void OnBnClickedBtnCstart();
	afx_msg void OnBnClickedBtnCstop();
	afx_msg void OnBnClickedCsend();
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void DeCode(std::string str);
	afx_msg void OnBnClickedBtnKinectstart();
//Serial com
	virtual void OnEventOpen(BOOL bSuccess);
	virtual void OnEventClose(BOOL bSuccess);
	virtual void OnEventRead(char *inPacket, int inLength);
	virtual void OnEventWrite(int nWritten);
private:
	void SendUARTQdot();
	std::thread StartServer();
	void ServerMain();
public:
	BOOL m_chkFaker;
	afx_msg void OnBnClickedChkFaker();
	CString m_sttDmin;
	CEdit m_edtTCPPort;
	CString m_sttTCPStatus;
	afx_msg void OnBnClickedBtnCstartserver();
	afx_msg void OnBnClickedBtnCstopserver();
	BOOL m_IsOnSimu;
	afx_msg void OnBnClickedChkSimu();
	afx_msg void OnBnClickedButtonScreen2();
	afx_msg void OnBnClickedButtonload();
};