
#include "stdafx.h"
#include "Plc.h"

#ifdef _WIN64
//#include "ActUtlType64_i.h"
#include "ActUtlType64_i.c"
#else
//#include "ActUtlType_i.h"
#include "ActUtlType_i.c"
#endif

CPlc::CPlc()
{
	Init();
}


CPlc::~CPlc()
{
}

void CPlc::Init()
{
	/***************************************************/
	/*  Initialize OLE Library                         */
	if (!AfxOleInit())
	{
		//�α׷� ����
		//AfxMessageBox(_T("AfxOleInit() failed."));
		//return FALSE;
	}
	/***************************************************/

	/* ACT Compornent Instance Create				   */
	HRESULT hr;


#ifdef _WIN64
	hr = CoCreateInstance(CLSID_ActUtlType64,
		NULL,
		CLSCTX_LOCAL_SERVER,
		IID_IActUtlType64,
		(LPVOID*)&m_plc);
	if (!SUCCEEDED(hr))
	{
		AfxMessageBox(_T("ActUtlType64 CoCreateInstance() failed."));
		//EndDialog(IDOK);
		m_bCreatePlcInstance = false;
	}
#else
	hr = CoCreateInstance(CLSID_ActUtlType,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IActUtlType,
		(LPVOID*)&m_plc);
	if (!SUCCEEDED(hr))
	{
		AfxMessageBox(_T("ActUtlType CoCreateInstance() failed."));
		//EndDialog(IDOK);
		m_bCreatePlcInstance = false;
	}
#endif


}

bool CPlc::Open(int nLogicalNum)
{
	bool bSuccess = false;
	if (m_bCreatePlcInstance == true) {
		HRESULT hr;

		long	lRet;
		m_plc->put_ActLogicalStationNumber(nLogicalNum);
		hr = m_plc->Open(&lRet);

		if (SUCCEEDED(hr)) {
			if (lRet == 0x00) {
				bSuccess = true;
			}
			else {
				bSuccess = false;
			}
		}
		else {
			bSuccess = false;
		}
	}
	return bSuccess;
}

bool CPlc::ReadBlock(CString strAddress, int nSize, short* shValue)
{
	bool bSuccess = false;
	HRESULT hr;
	long	lRet;
	BSTR	sAddress = strAddress.AllocSysString();

	hr = m_plc->ReadDeviceBlock2(sAddress, nSize, shValue, &lRet);

	if (SUCCEEDED(hr)) {
		if (lRet == 0x00) {
			bSuccess = true;
		}
	}
	else {
		bSuccess = false;
	}
	return bSuccess;
}

bool CPlc::WriteBlock(CString strAddress, int nSize, short* shValue)
{
	bool bSuccess = false;
	HRESULT hr;
	long	lRet;
	BSTR	sAddress = strAddress.AllocSysString();

	hr = m_plc->WriteDeviceBlock2(sAddress, nSize, shValue, &lRet);

	if (SUCCEEDED(hr)) {
		if (lRet == 0x00) {
			bSuccess = true;
		}
	}
	else {
		bSuccess = false;
	}
	return bSuccess;
}