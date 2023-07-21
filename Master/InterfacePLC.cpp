
#include "stdafx.h"
#include "InterfacePlc.h"



CInterfacePlc::CInterfacePlc()
{
}


CInterfacePlc::~CInterfacePlc()
{
}


bool CInterfacePlc::Open(int nLogicalNum)
{
	return false;
}

bool CInterfacePlc::Open(int nNetworkNo, int nStationNo, int nPlcChannel)
{
	return false;
}

bool CInterfacePlc::ReadBlock(CString strAddress, int nSize, short* shValue)
{
	return false;
}

bool CInterfacePlc::WriteBlock(CString strAddress, int nSize, short* shValue)
{
	return false;
}