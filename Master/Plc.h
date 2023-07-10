#pragma once

#ifdef _WIN64
#include "ActUtlType64_i.h"
#else
#include "ActUtlType_i.h"
#endif

class CPlc
{
public:
	CPlc();
	~CPlc();


	bool CPlc::Open(int nLogicalNum);
	bool CPlc::ReadBlock(CString strAddress, int nSize, short* shValue);
	bool CPlc::WriteBlock(CString strAddress, int nSize, short* shValue);

private:
#ifdef _WIN64
	IActUtlType64* m_plc;
#else
	IActUtlType* m_plc;
#endif

	bool m_bCreatePlcInstance = true;
	void CPlc::Init();

};

