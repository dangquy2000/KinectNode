#pragma once
#include "GLView2.h"
#include "afxwin.h"
// SecondScreen dialog

class SecondScreen : public CDialogEx
{
	DECLARE_DYNAMIC(SecondScreen)

public:
	SecondScreen(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SecondScreen();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SECOND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	GLView2* m_pclGlView2;
	double m_ptheta[5];
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
