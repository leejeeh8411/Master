
// MasterDlg.h : ��� ����
//

#pragma once

#include "Plc.h"


// CMasterDlg ��ȭ ����
class CMasterDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CMasterDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

	CPlc m_plc;

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MASTER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
