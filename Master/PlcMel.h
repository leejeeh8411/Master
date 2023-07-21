#pragma once

#include "InterfacePlc.h"

#ifdef _WIN64
#include "ActUtlType64_i.h"
#else
#include "ActUtlType_i.h"
#endif

class CPlcMel : public CInterfacePlc
{
public:
	
	CPlcMel();
	~CPlcMel();

	void Create();
	bool Close();
	bool Open(int nLogicalNum);
	bool ReadBlock(CString strAddress, int nSize, short* shValue);
	bool WriteBlock(CString strAddress, int nSize, short* shValue);

private:
#ifdef _WIN64
	IActUtlType64* m_plc;
#else
	IActUtlType* m_plc;
#endif

	bool m_bCreatePlcInstance = true;
	
};



