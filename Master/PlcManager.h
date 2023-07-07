
/*
필요클래스 : plc, postgreSql
목적 : db에 명세되어 있는 plc정보를 읽어와서
      read/write 연산을 한다.

	  타 클래스가 요청하면 plc에서 받은 정보를 전달한다.
*/

#pragma once

#include <vector>
#include "Plc.h"
#include "gPostgreSQL.h"
#include "uniqueue.h"

struct st_plc_address
{
	int blockID;
	char cAddress[10];
	int blockSize;
	char cSyncType[10];
	char cComment[50];

	st_plc_address() {
		blockID = 0;
		memset(cAddress, NULL, sizeof(char) * 10);
		blockSize = 0;
		memset(cSyncType, NULL, sizeof(char) * 10);
		memset(cComment, NULL, sizeof(char) * 50);
	}
};

struct st_plc_read_sch
{
	int idx;
	int addressid;
	int idxstt;
	int idxbit;
	int size;
	char keyname[20];
	char cDataTypeDB[10];

	st_plc_read_sch() {
		idx = 0;
		addressid = 0;
		idxstt = 0;
		idxbit = 0;
		size = 0;
		memset(keyname, NULL, sizeof(char) * 20);
		memset(cDataTypeDB, NULL, sizeof(char) * 10);
	}
};

struct st_plc_data_read
{
	int nRead1;
	int nRead2;
	int nRead3;
	int nRead4;
	char cRead1[20];

	st_plc_data_read() {
		nRead1 = 0;
		nRead2 = 0;
		nRead3 = 0;
		nRead4 = 0;
		memset(cRead1, NULL, sizeof(char) * 20);
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

	void CPlcManager::ReadPLC();
	void CPlcManager::WritePLC();

	string CPlcManager::ParsePlcData(short* pPlcData, int nIdxStt, int nIdxBit, int nSize, std::string strType);
	void  CPlcManager::DecodingPlcData(short* pPlcData, int nIdxStt, int nIdxBit, int nSize, CString strType, CString _strValue);

	CString CPlcManager::GetStringDataFromShort(short* pData, int nSize);
	void CPlcManager::GetShortDataFromString(CString strData, short* pData, int nSize);

	st_plc_data_read* CPlcManager::GetPlcDataReadPointer();
	void CPlcManager::SetPlcDataRead(std::string strKeyName, std::string strType, std::string strValue);

private:
	CPlc m_plc;
	gPostgreSQL m_db;
	st_plc_data_read m_st_plc_data;
};

