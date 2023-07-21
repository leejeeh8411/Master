#pragma once


class CInterfacePlc
{
public:
	CInterfacePlc();
	~CInterfacePlc();

	//미쯔비시 랜통신 
	virtual bool Open(int nLogicalNum);
	//미쯔비시 광통신
	virtual bool Open(int nNetworkNo, int nStationNo, int nPlcChannel);
	virtual bool ReadBlock(CString strAddress, int nSize, short* shValue);
	virtual bool WriteBlock(CString strAddress, int nSize, short* shValue);

private:

	

};

