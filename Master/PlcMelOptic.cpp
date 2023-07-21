#include "stdafx.h"
#include "PlcMelOptic.h"




CPlcMelOptic::CPlcMelOptic()
{
	Create();
}


CPlcMelOptic::~CPlcMelOptic()
{
	Close();
}

void CPlcMelOptic::Create()
{

}

bool CPlcMelOptic::Open(int nNetworkNo, int nStationNo, int nChannelNo)
{
	bool bSuccess = false;
	long longPath;

	if (mdOpen(nChannelNo, PLC_CONN_MODE, &longPath) == 0){
		m_nNetworkNo = nNetworkNo;
		m_nStationNo = nStationNo;
		m_nChannelNo = nChannelNo;

		bSuccess = true;
	}

	return bSuccess;
}

bool CPlcMelOptic::Close()
{
	bool bSuccess = false;

	if (mdClose(m_nChannelNo) == 0) {
		bSuccess = true;
	}
	
	return bSuccess;
}

//ex:W200 
bool CPlcMelOptic::ReadBlock(CString strAddress, int nSize, short* shValue)
{
	bool bSuccess = false;

	short ret;
	LONG size = nSize;

	//test code
	CString strHexExam = "200";
	strAddress = strHexExam;
	long lExam = 0x0200;

	long lAddress = 0;

	// String to Hex
	int nAddress = 0;
	char* ch;
	ch = strAddress.GetBuffer(strAddress.GetLength());
	std::stringstream convert(ch);
	convert >> std::hex >> lAddress;

	ret = mdReceiveEx(m_nChannelNo, m_nNetworkNo, m_nStationNo, DevW, lAddress, &size, shValue);

	return bSuccess;
}

bool CPlcMelOptic::WriteBlock(CString strAddress, int nSize, short* shValue)
{
	bool bSuccess = false;

	short ret;
	LONG size = nSize;

	long lAddress = 0;

	// String to Hex
	int nAddress = 0;
	char* ch;
	ch = strAddress.GetBuffer(strAddress.GetLength());
	std::stringstream convert(ch);
	convert >> std::hex >> lAddress;

	//ret = mdSendEx(m_nChannelNo, m_nNetworkNo, m_nStationNo, DevW, lAddress, &size, pdata);

	return bSuccess;
}
