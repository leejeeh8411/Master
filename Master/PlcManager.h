
/*
�ʿ�Ŭ���� : 
            CInterfacePlc	: PLC �������̽� ��
			CPlcMel			: ������ ����� PLC DATA
			CPlcMelOptic	: ������ ����� PLC DATA
			postgreSql		: DB���� PLC read, write ���� �����´�

���� : db�� ���Ǿ� �ִ� plc������ �о�ͼ�
      read/write ������ �Ѵ�.

	  Ÿ Ŭ������ ��û�ϸ� plc���� ���� ������ �����Ѵ�.
*/

#pragma once

#include <vector>
#include <thread>
#include "gPostgreSQL.h"
#include "uniqueue.h"
#include "gLogger.h"
#include "InterfacePlc.h"
#include "PlcMel.h"
#include "PlcMelOptic.h"

enum PLCTYPE
{
	//PLC Type
	//0 : mitsubishi lan type
	//1 : mitsubishi optic type
	//2 : siemens
	PLCTYPE_MEL_LAN,
	PLCTYPE_MEL_OPTIC,
	PLCTYPE_SIEMENS
};

extern gLogger syslog;


const static int STRING_LENGTH = 20;

struct st_plc_address
{
	int blockID;
	char cAddress[STRING_LENGTH];
	int blockSize;
	char cSyncType[STRING_LENGTH];
	char cComment[50];

	st_plc_address() {
		blockID = 0;
		memset(cAddress, NULL, sizeof(char) * STRING_LENGTH);
		blockSize = 0;
		memset(cSyncType, NULL, sizeof(char) * STRING_LENGTH);
		memset(cComment, NULL, sizeof(char) * 50);
	}
};

struct st_plc_read_sch
{
	int addressid;
	int idxstt;
	int size;
	char keyname[STRING_LENGTH];
	char cDataType[STRING_LENGTH];

	st_plc_read_sch() {
		addressid = 0;
		idxstt = 0;
		size = 0;
		memset(keyname, NULL, sizeof(char) * STRING_LENGTH);
		memset(cDataType, NULL, sizeof(char) * STRING_LENGTH);
	}
};


//plc���� ���� �����͸� ����ü�� �����Ѵ�.
//�� ������ �´� ����ü�� �����Ͽ� ����ϼ���.
struct st_plc_data_read
{
	int nRead1;
	int nRead2;
	int nRead3;
	int nRead4;
	char cRead1[STRING_LENGTH];

	st_plc_data_read() {
		nRead1 = 0;
		nRead2 = 0;
		nRead3 = 0;
		nRead4 = 0;
		memset(cRead1, NULL, sizeof(char) * STRING_LENGTH);
	}
};


using namespace std;

class CPlcManager
{
public:
	CPlcManager();
	~CPlcManager();

	//PLC Type
	//0 : mitsubishi lan type
	//1 : mitsubishi optic type
	//2 : siemens(���߿���)
	void CPlcManager::Init(int nPlcType);

	//PLC���� �����ؾ��� �ּ� ������ DB���� �����´�.
	vector<st_plc_address> CPlcManager::GetPlcAddressFromDB();
	//PLC ���� ������ Block�� ���� ���� ���� DB���� �����´�.
	vector<st_plc_read_sch> CPlcManager::GetSchFromDB(int nBlockID);
	
	//PLC read operation
	void CPlcManager::ReadPLC();
	//PLC write operation
	void CPlcManager::WritePLC();

	//plc ���� �ֱ⸦ �����Ѵ�
	void CPlcManager::SetPlcPeriodTime(int nMilleSecond);
	//plc ���� �ֱ⸦ �����´�
	int CPlcManager::GetPlcPeriodTime();

	//plc ������¸� �����´�
	bool CPlcManager::GetConnPlc();
	//db ������¸� �����´�
	bool CPlcManager::GetConnDB();
	
	//test-dataSet
	void CPlcManager::InitTestSet();
	//test-read
	void CPlcManager::ReadTest();

	//plc���� �о�� �����͸� ������ ���������Ŀ� ���� �Ľ��Ѵ��� ���ڿ��� �����Ѵ�.
	string CPlcManager::ParsePlcData(short* pPlcData, int nIdxStt, int nIdxBit, int nSize, std::string strType);
	//plc�� ����� ���ڿ� �����͸� �� ���������Ŀ� �°� ���ڵ� �Ͽ� plc �迭��(write)�� ����ִ´�.
	void  CPlcManager::DecodingPlcData(short* pPlcData, int nIdxStt, int nSize, CString strType, CString _strValue);

	//string data -> short data
	CString CPlcManager::GetStringDataFromShort(short* pData, int nSize);
	//short data -> string data
	void CPlcManager::GetShortDataFromString(CString strData, short* pData, int nSize);


	//���� fix�ȵ� �Լ���
	//plc read �����͸� ������ ����ü�� �����´�
	st_plc_data_read* CPlcManager::GetPlcDataReadPointer();
	//plc���� �о�� �����͸� ����(?) ����ü�� ����ִ´�.
	void CPlcManager::SetPlcReadData(std::string strKeyName, std::string strType, std::string strValue);
	
	//plc�� ������ �����͸� map<key, value> �������� plc manager�� ������
	//plc�� ������ �����͸� ����ϴ� �Լ�
	void CPlcManager::InsertPlcWriteData(string keyName, string value);
	//plc�� ������ �����͸� ������ �ִ� map �����̳� �����͸� �����´�.
	CUniMap <string, string>* CPlcManager::GetPlcDataWritePointer();
	//plc�� ������ �����͸� Ű�� �ش��ϴ� value�� �����´�.
	bool CPlcManager::GetWriteDataPlcValue(string keyName, string& value);


private:

	//plc instance pointer
	CInterfacePlc* m_plc;

	bool bConnPLC = false;
	//plc instance
	CPlcMel m_plc_lan;
	CPlcMelOptic m_plc_optic;
	
	bool bConnDB = false;
	//db
	gPostgreSQL m_db;

	//plc ������¸� �����Ѵ�
	void CPlcManager::SetConnPlc(bool bSet);
	//plc ������¸� �����Ѵ�
	void CPlcManager::SetConnDB(bool bSet);
	
	st_plc_data_read m_st_plc_data_read;
	CUniMap <string, string> m_map_plc_data_write;

	int m_nTimePlcPeriod = 100;

	
};

