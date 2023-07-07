
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

	string CPlcManager::ParsePlcData(short* pPlcData, int nIdxStt, int nIdxBit, int nSize, CString strType);
	void  CPlcManager::DecodingPlcData(short* pPlcData, int nIdxStt, int nIdxBit, int nSize, CString strType, CString _strValue);

	CString CPlcManager::GetStringDataFromShort(short* pData, int nSize);
	void CPlcManager::GetShortDataFromString(CString strData, short* pData, int nSize);

private:
	CPlc m_plc;
	gPostgreSQL m_db;
};

