#pragma once


#include "InterfacePlc.h"
#include <iostream>
#include <sstream>
#include	"mdfunc.h"

#define PLC_CONN_MODE				-1

class CPlcMelOptic : public CInterfacePlc
{
public:
	CPlcMelOptic();
	~CPlcMelOptic();

	void Create();

	bool Open(int nNetworkNo, int nStationNo, int nPlcChannel);
	bool Close();
	bool ReadBlock(CString strAddress, int nSize, short* shValue);
	bool WriteBlock(CString strAddress, int nSize, short* shValue);

private:
	bool m_bCreatePlcInstance = true;
	int m_nNetworkNo;
	int m_nStationNo;
	int m_nChannelNo;
};

