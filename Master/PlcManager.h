
/*
필요클래스 : 
            CInterfacePlc	: PLC 인터페이스 명세
			CPlcMel			: 미쯔비시 랜통신 PLC DATA
			CPlcMelOptic	: 미쯔비시 광통신 PLC DATA
			postgreSql		: DB에서 PLC read, write 명세를 가져온다

목적 : db에 명세되어 있는 plc정보를 읽어와서
      read/write 연산을 한다.

	  타 클래스가 요청하면 plc에서 받은 정보를 전달한다.
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

	//PLC Type
	//0 : mitsubishi lan type
	//1 : mitsubishi optic type
	//2 : siemens(개발예정)
	void CPlcManager::Init(int nPlcType);

	//PLC에서 연동해야할 주소 정보를 DB에서 가져온다.
	vector<st_plc_address> CPlcManager::GetPlcAddressFromDB();
	//PLC 연동 데이터 Block에 대한 세부 명세를 DB에서 가져온다.
	vector<st_plc_read_sch> CPlcManager::GetSchFromDB(int nBlockID);
	
	//PLC read operation
	void CPlcManager::ReadPLC();
	//PLC write operation
	void CPlcManager::WritePLC();

	//plc 연동 주기를 설정한다
	void CPlcManager::SetPlcPeriodTime(int nMilleSecond);
	//plc 연동 주기를 가져온다
	int CPlcManager::GetPlcPeriodTime();

	//plc 연결상태를 가져온다
	bool CPlcManager::GetConnPlc();
	//db 연결상태를 가져온다
	bool CPlcManager::GetConnDB();
	
	//test-dataSet
	void CPlcManager::InitTestSet();
	//test-read
	void CPlcManager::ReadTest();

	//plc에서 읽어온 데이터를 지정된 데이터형식에 맞춰 파싱한다음 문자열로 리턴한다.
	string CPlcManager::ParsePlcData(short* pPlcData, int nIdxStt, int nIdxBit, int nSize, std::string strType);
	//plc에 써야할 문자열 데이터를 각 데이터형식에 맞게 디코딩 하여 plc 배열블럭(write)에 집어넣는다.
	void  CPlcManager::DecodingPlcData(short* pPlcData, int nIdxStt, int nSize, CString strType, CString _strValue);

	//string data -> short data
	CString CPlcManager::GetStringDataFromShort(short* pData, int nSize);
	//short data -> string data
	void CPlcManager::GetShortDataFromString(CString strData, short* pData, int nSize);


	//아직 fix안된 함수들
	//plc read 데이터를 보관할 구조체를 가져온다
	st_plc_data_read* CPlcManager::GetPlcDataReadPointer();
	//plc에서 읽어온 데이터를 개인(?) 구조체에 집어넣는다.
	void CPlcManager::SetPlcReadData(std::string strKeyName, std::string strType, std::string strValue);
	
	//plc에 쏴야할 데이터를 map<key, value> 형식으로 plc manager가 관리함
	//plc에 쏴야할 데이터를 등록하는 함수
	void CPlcManager::InsertPlcWriteData(string keyName, string value);
	//plc에 쏴야할 데이터를 가지고 있는 map 컨테이너 포인터를 가져온다.
	CUniMap <string, string>* CPlcManager::GetPlcDataWritePointer();
	//plc에 쏴야할 데이터를 키에 해당하는 value를 가져온다.
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

	//plc 연결상태를 설정한다
	void CPlcManager::SetConnPlc(bool bSet);
	//plc 연결상태를 설정한다
	void CPlcManager::SetConnDB(bool bSet);
	
	st_plc_data_read m_st_plc_data_read;
	CUniMap <string, string> m_map_plc_data_write;

	int m_nTimePlcPeriod = 100;

	
};

