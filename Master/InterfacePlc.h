#pragma once


class CInterfacePlc
{
public:
	CInterfacePlc();
	~CInterfacePlc();

	//������ ����� 
	virtual bool Open(int nLogicalNum);
	//������ �����
	virtual bool Open(int nNetworkNo, int nStationNo, int nPlcChannel);
	virtual bool ReadBlock(CString strAddress, int nSize, short* shValue);
	virtual bool WriteBlock(CString strAddress, int nSize, short* shValue);

private:

	

};

