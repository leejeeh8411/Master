#include "stdafx.h"
#include "PlcManager.h"

gLogger syslog("syslog", std::string("D:\\log") + std::string("syslog.txt"), true, 1024 * 1024 * 5, 5);



void WorkRead(CPlcManager* pParent)
{
	while (true) {
		pParent->ReadPLC();
		Sleep(pParent->GetReadTime());
	}
}

void WorkWrite(CPlcManager* pParent)
{
	while (true) {
		pParent->WritePLC();
		Sleep(pParent->GetWriteTime());
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

	bool bPlcConn = m_plc.Open(1);
	
	bool bDBConn = m_db.connect("127.0.0.1", "postgres");

	//test
	bool bTestMode = true;
	if (bTestMode == true) {
		InitTestSet();
	}
	else {
		//read,write thread 생성
		thread t1 = thread(WorkRead, this);
		t1.join();

		thread t2 = thread(WorkWrite, this);
		t2.join();
	}

	SetReadTime(100);
	SetWriteTime(100);
}

CPlcManager::~CPlcManager()
{

}

void CPlcManager::InitTestSet()
{
	short shVal = 0;

	shVal = 1;
	m_plc.WriteBlock("D1000", 1, &shVal);

	shVal = 2;
	m_plc.WriteBlock("D1001", 1, &shVal);

	shVal = 3;
	m_plc.WriteBlock("D1002", 1, &shVal);

	shVal = 4;
	m_plc.WriteBlock("D1003", 1, &shVal);

	shVal = 16706;
	m_plc.WriteBlock("D1010", 2, &shVal);

	thread t1 = thread(readTestFunc, this);
	t1.join();

	thread t2 = thread(readTestFunc, this);
	t2.join();

	thread t3 = thread(readTestFunc, this);
	t3.join();

	thread t4 = thread(readTestFunc, this);
	t4.join();
}

void CPlcManager::ReadTest()
{
	int nBlockSize = 4;
	//힙 할당
	short* pPlcData = new short[nBlockSize];
	memset(pPlcData, 0, sizeof(short) * nBlockSize);

	m_plc.ReadBlock("D1000", nBlockSize, pPlcData); //힙영역 할당후 Heap포인터와 Size를 전달하여 plc로부터 data를 받는다

	syslog.info("read:D1000, value 1~4:{},{},{},{}", pPlcData[0], pPlcData[1], pPlcData[2], pPlcData[3]);

	delete[] pPlcData;
}

void CPlcManager::SetReadTime(int nMilleSecond)
{
	m_nTimeRead = nMilleSecond;
}

void CPlcManager::SetWriteTime(int nMilleSecond)
{
	m_nTimeWrite = nMilleSecond;
}

int CPlcManager::GetReadTime()
{
	return m_nTimeRead;
}

int CPlcManager::GetWriteTime()
{
	return m_nTimeWrite;
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
			strncpy(stPlcAddress.cAddress, itRow->getValue(dbResult.getColumnIndexByName("address"))->getByString(), 10);
			stPlcAddress.blockSize = itRow->getValue(dbResult.getColumnIndexByName("blocksize"))->getByInteger();
			strncpy(stPlcAddress.cSyncType, itRow->getValue(dbResult.getColumnIndexByName("synctype"))->getByString(), 10);

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
			strncpy(stPlcReadSch.cDataType, itRow->getValue(dbResult.getColumnIndexByName("datatype"))->getByString(), 10);
			strncpy(stPlcReadSch.keyname, itRow->getValue(dbResult.getColumnIndexByName("keyname"))->getByString(), 10);

			vt_plcReadSch.emplace_back(stPlcReadSch);
		}
	}

	return vt_plcReadSch;

}


//PLC받아와야할 전체 read 정보를 처리한다.
void CPlcManager::ReadPLC()
{

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

		m_plc.ReadBlock(strAddress, nBlockSize, pPlcData); //힙영역 할당후 Heap포인터와 Size를 전달하여 plc로부터 data를 받는다

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
}

//plc read 데이터를 보관할 구조체를 가져온다
st_plc_data_read* CPlcManager::GetPlcDataReadPointer()
{
	return &m_st_plc_data;
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
		strncpy(pStPlcDataRead->cRead1, strValue.c_str(), 20);
	}
}


void CPlcManager::WritePLC()
{
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

		std::vector<st_plc_read_sch> vt_sch_data = GetSchFromDB(nBlockID); //plc block 데이터 명세 정보 가져옴

		int nSizeSch = vt_sch_data.size();

		for (int j = 0; j < nSizeSch; j++) {
			int nSttIdx = vt_sch_data[j].idxstt;
			int nSize = vt_sch_data[j].size;
			std::string strKeyName = vt_sch_data[j].keyname;
			CString strType = vt_sch_data[j].cDataType;

			//키에 해당하는 값을 가져온다.
			//std::string strValue = GetValue("키값");

			//타입을 보고 맞는 걸로 변환
			//DecodingPlcData(pPlcData, nSttIdx, nIdxBit, nSize, strType, strValue);

		}

		delete[] pPlcData;
	}
}

void  CPlcManager::DecodingPlcData(short* pPlcData, int nIdxStt, int nIdxBit, int nSize, CString strType, CString _strValue)
{
	std::string strValue = _strValue;
	if (strType == "string") {
		GetShortDataFromString(_strValue, &pPlcData[nIdxStt], 10);
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
			short sValue = (0x01 << nIdxBit);
			pPlcData[nIdxStt] = sValue;
		}
		else {
			short sValue = (0x00 << nIdxBit);
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


