
/*
필요클래스 : plc, postgreSql
목적 : db에 명세되어 있는 plc정보를 읽어와서
      read/write 연산을 한다.

	  타 클래스가 요청하면 plc에서 받은 정보를 전달한다.
*/

#pragma once

#include <vector>
#include "gPostgreSQL.h"
#include "uniqueue.h"
#include <thread>
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


//plc에서 받은 데이터를 구조체로 관리한다.
//각 공정에 맞는 구조체로 수정하여 사용하세요.
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

	vector<st_plc_read_sch> CPlcManager::GetSchFromDB(int nBlockID);
	vector<st_plc_address> CPlcManager::GetPlcAddressFromDB();

	void CPlcManager::Init(int nPlcType);

	void CPlcManager::InitTestSet();
	void CPlcManager::ReadTest();

	void CPlcManager::ReadPLC();
	void CPlcManager::WritePLC();

	string CPlcManager::ParsePlcData(short* pPlcData, int nIdxStt, int nIdxBit, int nSize, std::string strType);
	void  CPlcManager::DecodingPlcData(short* pPlcData, int nIdxStt, int nSize, CString strType, CString _strValue);

	CString CPlcManager::GetStringDataFromShort(short* pData, int nSize);
	void CPlcManager::GetShortDataFromString(CString strData, short* pData, int nSize);

	st_plc_data_read* CPlcManager::GetPlcDataReadPointer();
	CUniMap <string, string>* CPlcManager::GetPlcDataWritePointer();
	void CPlcManager::SetPlcDataRead(std::string strKeyName, std::string strType, std::string strValue);
	void CPlcManager::InsertPlcWriteData(string keyName, string value);
	bool CPlcManager::GetWriteDataPlcValue(string keyName, string& value);

	void CPlcManager::SetPlcPeriodTime(int nMilleSecond);
	int CPlcManager::GetPlcPeriodTime();

	bool CPlcManager::GetConnPlc();
	bool CPlcManager::GetConnDB();

	void CPlcManager::SetConnPlc(bool bSet);
	void CPlcManager::SetConnDB(bool bSet);

private:

	//instance pointer
	CInterfacePlc* m_plc;

	//instance
	CPlcMel m_plc_lan;
	CPlcMelOptic m_plc_optic;

	bool bConnPLC = false;
	gPostgreSQL m_db;
	bool bConnDB = false;
	st_plc_data_read m_st_plc_data_read;
	CUniMap <string, string> m_map_plc_data_write;

	int m_nTimePlcPeriod = 100;

	
};

