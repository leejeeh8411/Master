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

//W200  W:Word Data Area, Address:200
//B200  B:Bit Data Area, Address:200
bool CPlcMelOptic::ReadBlock(CString strAddress, int nSize, short* shValue)
{
	bool bSuccess = false;

	short ret;
	LONG size = nSize;

	CString dataArea = strAddress.Left(1);
	CString address = strAddress.Mid(1, strAddress.GetLength()-1);

	long lAddress = 0;

	// String to Hex
	char* ch;
	ch = address.GetBuffer(address.GetLength());
	std::stringstream convert(ch);
	convert >> std::hex >> lAddress;

	long lArea = 0;
	if (dataArea == "W") {
		lArea = DevW;
	}
	else if (dataArea == "B") {
		lArea = DevB;
	}
	else {
		lArea = DevW;
	}

	ret = mdReceiveEx(m_nChannelNo, m_nNetworkNo, m_nStationNo, lArea, lAddress, &size, shValue);

	return bSuccess;
}

bool CPlcMelOptic::WriteBlock(CString strAddress, int nSize, short* shValue)
{
	bool bSuccess = false;

	short ret;
	LONG size = nSize;

	CString dataArea = strAddress.Left(1);
	CString address = strAddress.Mid(1, strAddress.GetLength() - 1);

	long lAddress = 0;

	// String to Hex
	char* ch;
	ch = address.GetBuffer(address.GetLength());
	std::stringstream convert(ch);
	convert >> std::hex >> lAddress;

	long lArea = 0;
	if (dataArea == "W") {
		lArea = DevW;
	}
	else if (dataArea == "B") {
		lArea = DevB;
	}
	else {
		lArea = DevW;
	}

	ret = mdSendEx(m_nChannelNo, m_nNetworkNo, m_nStationNo, lArea, lAddress, &size, shValue);

	return bSuccess;
}
