#include "stdafx.h"
#include "PlcManager.h"

gLogger syslog("syslog", std::string("D:\\log\\") + std::string("syslog.txt"), true, 1024 * 1024 * 5, 5);



void WorkReadWrite(CPlcManager* pParent)
{
	while (true) {
		pParent->ReadPLC();
		pParent->WritePLC();
		Sleep(pParent->GetPlcPeriodTime());
	}
}


//test thread
void readTestFunc(CPlcManager* pParent)
{
	while (true) {
		pParent->ReadTest();
		Sleep(10);
	}
}


CPlcManager::CPlcManager()
{

}

CPlcManager::~CPlcManager()
{
	
}

void CPlcManager::Init(int nPlcType)
{

	if (nPlcType == PLCTYPE_MEL_LAN) {
		m_plc = &m_plc_lan;
		bool bPlcConn = m_plc->Open(1);
		SetConnPlc(bPlcConn);
	}
	else if (nPlcType == PLCTYPE_MEL_OPTIC) {
		m_plc = &m_plc_optic;
		bool bPlcConn = m_plc->Open(1, 1, 141);
		//SetConnPlc(bPlcConn);
		SetConnPlc(true);
	}
	else if (nPlcType == PLCTYPE_SIEMENS) {

	}

	bool bDBConn = m_db.connect("127.0.0.1", "postgres");
	SetConnDB(bDBConn);

	SetPlcPeriodTime(100);

	//test
	bool bTestMode = false;
	if (bTestMode == true) {
		InitTestSet();
	}
	else {
		//read,write thread 생성
		thread t1 = thread(WorkReadWrite, this);
		t1.detach();
	}

}

bool CPlcManager::GetConnPlc()
{
	return bConnPLC;
}

bool CPlcManager::GetConnDB()
{
	return bConnDB;
}

void CPlcManager::SetConnPlc(bool bSet)
{
	bConnPLC = bSet;
}

void CPlcManager::SetConnDB(bool bSet)
{
	bConnDB = bSet;
}

void CPlcManager::InitTestSet()
{
	short shVal = 0;

	shVal = 1;
	m_plc->WriteBlock("D1000", 1, &shVal);

	shVal = 2;
	m_plc->WriteBlock("D1001", 1, &shVal);

	shVal = 3;
	m_plc->WriteBlock("D1002", 1, &shVal);

	shVal = 4;
	m_plc->WriteBlock("D1003", 1, &shVal);

	shVal = 16706;
	m_plc->WriteBlock("D1010", 2, &shVal);

	thread t1 = thread(readTestFunc, this);
	t1.detach();

	thread t2 = thread(readTestFunc, this);
	t2.detach();

	thread t3 = thread(readTestFunc, this);
	t3.detach();

	thread t4 = thread(readTestFunc, this);
	t4.detach();

	InsertPlcWriteData("w-data1", "10.3");
	InsertPlcWriteData("w-data2", "8.3");
	InsertPlcWriteData("w-data1", "9.3");
}

void CPlcManager::ReadTest()
{
	int nBlockSize = 4;
	//힙 할당
	short* pPlcData = new short[nBlockSize];
	memset(pPlcData, 0, sizeof(short) * nBlockSize);

	m_plc->ReadBlock("D1000", nBlockSize, pPlcData); //힙영역 할당후 Heap포인터와 Size를 전달하여 plc로부터 data를 받는다

	syslog.info("read:D1000, value 1~4:{},{},{},{}", pPlcData[0], pPlcData[1], pPlcData[2], pPlcData[3]);

	delete[] pPlcData;
}

void CPlcManager::SetPlcPeriodTime(int nMilleSecond)
{
	m_nTimePlcPeriod = nMilleSecond;
}


int CPlcManager::GetPlcPeriodTime()
{
	return m_nTimePlcPeriod;
}


vector<st_plc_address> CPlcManager::GetPlcAddressFromDB()
{
	//db에 있는 plc 어드레스 정보를 읽는다
	vector<st_plc_address> vt_plcAddress;

	string strColumn = "synctype";
	
	string strQuery = "SELECT addressid, address, blocksize, synctype FROM plcaddress";
	

	DBResult dbResult = m_db.execute(strQuery);

	if (dbResult.getRowCount() > 0){
		while (dbResult.hasNext()) {
			std::vector<DBRow>::iterator itRow = dbResult.fetchRow();
			
			st_plc_address stPlcAddress;

			stPlcAddress.blockID = itRow->getValue(dbResult.getColumnIndexByName("addressid"))->getByInteger();
			strncpy(stPlcAddress.cAddress, itRow->getValue(dbResult.getColumnIndexByName("address"))->getByString(), STRING_LENGTH);
			stPlcAddress.blockSize = itRow->getValue(dbResult.getColumnIndexByName("blocksize"))->getByInteger();
			strncpy(stPlcAddress.cSyncType, itRow->getValue(dbResult.getColumnIndexByName("synctype"))->getByString(), STRING_LENGTH);

			vt_plcAddress.emplace_back(stPlcAddress);
		}
	}

	return vt_plcAddress;
}

vector<st_plc_read_sch> CPlcManager::GetSchFromDB(int nBlockID)
{
	vector<st_plc_read_sch> vt_plcReadSch;

	string strColumn = "addressid";

	string strQuery = "SELECT addressid, idxstt, size, datatype, keyname FROM plcaddressdetail";
	strQuery.append(fmt::format(" WHERE {:s} = {:d}", strColumn, nBlockID));

	DBResult dbResult = m_db.execute(strQuery);

	if (dbResult.getRowCount() > 0) {
		while (dbResult.hasNext()) {
			std::vector<DBRow>::iterator itRow = dbResult.fetchRow();
			
			st_plc_read_sch stPlcReadSch;

			stPlcReadSch.addressid = itRow->getValue(dbResult.getColumnIndexByName("addressid"))->getByInteger();
			stPlcReadSch.idxstt = itRow->getValue(dbResult.getColumnIndexByName("idxstt"))->getByInteger();
			stPlcReadSch.size = itRow->getValue(dbResult.getColumnIndexByName("size"))->getByInteger();
			strncpy(stPlcReadSch.cDataType, itRow->getValue(dbResult.getColumnIndexByName("datatype"))->getByString(), STRING_LENGTH);
			strncpy(stPlcReadSch.keyname, itRow->getValue(dbResult.getColumnIndexByName("keyname"))->getByString(), STRING_LENGTH);

			vt_plcReadSch.emplace_back(stPlcReadSch);
		}
	}

	return vt_plcReadSch;

}


//PLC받아와야할 전체 read 정보를 처리한다.
void CPlcManager::ReadPLC()
{
	if (GetConnPlc() == false) {
		syslog.info("plc read fail");
		return;
	}
	vector<st_plc_address> vt_address_data = GetPlcAddressFromDB(); //db에 있는 Plc 어드레스 정보를 읽는다

	int nSize = vt_address_data.size();

	for (int i = 0; i < nSize; i++) {
		std::string strSyncType = vt_address_data[i].cSyncType;
		if (strSyncType != "read") {
			continue;
		}
		int nBlockID = vt_address_data[i].blockID;
		int nBlockSize = vt_address_data[i].blockSize;
		CString strAddress = vt_address_data[i].cAddress;

		//힙 할당
		short* pPlcData = new short[nBlockSize];
		memset(pPlcData, 0, sizeof(short) * nBlockSize);

		m_plc->ReadBlock(strAddress, nBlockSize, pPlcData); //힙영역 할당후 Heap포인터와 Size를 전달하여 plc로부터 data를 받는다

		std::vector<st_plc_read_sch> vt_sch_data = GetSchFromDB(nBlockID); //plc block 데이터 명세 정보 가져옴

		int nSizeSch = vt_sch_data.size();

		for (int j = 0; j < nSizeSch; j++) {
			int nSttIdx = vt_sch_data[j].idxstt;
			int nSize = vt_sch_data[j].size;
			std::string strKeyName = vt_sch_data[j].keyname;
			std::string strType = vt_sch_data[j].cDataType;

			std::string strVal = ParsePlcData(pPlcData, nSttIdx, 0, nSize, strType);

			SetPlcDataRead(strKeyName, strType, strVal);

		}
		delete[] pPlcData;
	}
	syslog.info("plc read");
}

//plc read 데이터를 보관할 구조체를 가져온다
st_plc_data_read* CPlcManager::GetPlcDataReadPointer()
{
	return &m_st_plc_data_read;
}

CUniMap <string, string>* CPlcManager::GetPlcDataWritePointer()
{
	return &m_map_plc_data_write;
}


void CPlcManager::SetPlcDataRead(std::string strKeyName, std::string strType, std::string strValue)
{
	st_plc_data_read* pStPlcDataRead = GetPlcDataReadPointer();

	//아래는 db 키에 대해 변수 어디에다 넣을지
	//edit하면 됩니다
	if (strKeyName == "read int 1") {
		pStPlcDataRead->nRead1 = std::stoi(strValue);
	}
	else if (strKeyName == "read int 2") {
		pStPlcDataRead->nRead2 = std::stoi(strValue);
	}
	else if (strKeyName == "read int 3") {
		pStPlcDataRead->nRead3 = std::stoi(strValue);
	}
	else if (strKeyName == "read int 4") {
		pStPlcDataRead->nRead4 = std::stoi(strValue);
	}
	else if (strKeyName == "read str 1") {
		strncpy(pStPlcDataRead->cRead1, strValue.c_str(), STRING_LENGTH);
	}
}


void CPlcManager::InsertPlcWriteData(string keyName, string value)
{
	CUniMap <string, string>* m_map = GetPlcDataWritePointer();

	string val;
	bool bFind = m_map->GetData(keyName, &val);

	if (bFind == true) {
		//같은 키값이 존재하면 값만 바꾸기
		m_map->SetData(keyName, value);
	}
	else {
		m_map->Push(keyName, value);
	}
}

bool CPlcManager::GetWriteDataPlcValue(string keyName, string& value)
{
	CUniMap <string, string>* m_map = GetPlcDataWritePointer();

	bool bFind = m_map->GetData(keyName, &value);

	return bFind;
}

void CPlcManager::WritePLC()
{
	if (GetConnPlc() == false) {
		syslog.info("plc write fail");
		return;
	}

	std::vector<st_plc_address> vt_address_data = GetPlcAddressFromDB(); //db에 있는 Plc 어드레스 정보를 읽는다

	int nSize = vt_address_data.size();

	for (int i = 0; i < nSize; i++) {
		std::string strSyncType = vt_address_data[i].cSyncType;
		if (strSyncType != "write") {
			continue;
		}

		int nBlockID = vt_address_data[i].blockID;
		int nBlockSize = vt_address_data[i].blockSize;
		CString strAddress = vt_address_data[i].cAddress;

		//힙 할당
		short* pPlcData = new short[nBlockSize];
		memset(pPlcData, 0, sizeof(short) * nBlockSize);

		vector<st_plc_read_sch> vt_sch_data = GetSchFromDB(nBlockID); //plc block 데이터 명세 정보 가져옴

		int nSizeSch = vt_sch_data.size();

		for (int j = 0; j < nSizeSch; j++) {
			int nSttIdx = vt_sch_data[j].idxstt;
			int nSize = vt_sch_data[j].size;
			string strKeyName = vt_sch_data[j].keyname;
			CString strType = vt_sch_data[j].cDataType;

			//키에 해당하는 값을 가져온다.
			string strValue;
			bool bFind = GetWriteDataPlcValue(strKeyName, strValue);
			CString strValueConv = strValue.c_str();
				
			//타입을 보고 맞는 걸로 변환
			if (bFind == true) {
				DecodingPlcData(pPlcData, nSttIdx, nSize, strType, strValueConv);
			}
		}

		m_plc->WriteBlock(strAddress, nBlockSize, pPlcData);


		delete[] pPlcData;
	}
	syslog.info("plc write");
}

void  CPlcManager::DecodingPlcData(short* pPlcData, int nIdxStt, int nSize, CString strType, CString _strValue)
{
	std::string strValue = _strValue;
	if (strType == "string") {
		GetShortDataFromString(_strValue, &pPlcData[nIdxStt], nSize);
	}
	else if (strType == "int") {
		CString strData;

		if (nSize == 2) {	//2word 처리
			long lValueSrc = std::stoi(strValue);
			short* pShort = (short*)&lValueSrc;

			pPlcData[nIdxStt] = pShort[0] & 0xFFFF;
			pPlcData[nIdxStt + 1] = pShort[1] & 0xFFFF;
		}
		else {
			pPlcData[nIdxStt] = std::stoi(strValue);
		}
	}
	else if (strType == "bit") {
		if (strValue == "True") {
			short sValue = (0x01 << nIdxStt);
			pPlcData[nIdxStt] = sValue;
		}
		else {
			short sValue = (0x00 << nIdxStt);
			pPlcData[nIdxStt] = sValue;
		}
	}
}



std::string  CPlcManager::ParsePlcData(short* pPlcData, int nIdxStt, int nIdxBit, int nSize, std::string strType)
{
	std::string strRet;

	if (strType == "string") {
		CString strData = GetStringDataFromShort(&pPlcData[nIdxStt], nSize);
		strRet = strData;
	}
	else if (strType == "int") {
		CString strData;

		if (nSize == 2) {	//2word 처리
			long lValue = 0;
			short* pShort = (short*)&lValue;
			pShort[0] = pPlcData[nIdxStt] & 0xFFFF;
			pShort[1] = pPlcData[nIdxStt + 1] & 0xFFFF;
			strData.Format("%d", lValue);
		}
		else {
			strData.Format("%d", pPlcData[nIdxStt]);
		}

		strRet = strData;
	}
	else if (strType == "bit") {
		bool bValue = pPlcData[nIdxStt] & (0x01 << nIdxBit);

		if (bValue == true) {
			strRet = "TRUE";
		}
		else {
			strRet = "FALSE";
		}
	}

	return strRet;

}



CString CPlcManager::GetStringDataFromShort(short* pData, int nSize)
{
	int nSizeWithNull = (nSize * 2) + 1;
	char* cData = new char[nSizeWithNull];
	memset(cData, NULL, sizeof(char) * nSizeWithNull);

	for (int i = 0; i < nSize; i++) {
		cData[i * 2 + 0] = pData[i] >> 8;
		cData[i * 2 + 1] = pData[i] & 0xFF;
	}

	CString retString;
	retString.Format("%s", cData);

	delete[] cData;

	return retString;
}

void CPlcManager::GetShortDataFromString(CString strData, short* pData, int nSize)
{
	int nStrLength = strData.GetLength();

	char* ptrChar = (char*)pData;	//첫번째 주소

	for (int i = 0; i < nStrLength; i++) {
		if (i >= nSize) {
			break;
		}
		char cData = strData.GetAt(i);
		ptrChar[i] = cData;
	}
}


