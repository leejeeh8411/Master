#include "stdafx.h"
#include "PlcManager.h"


CPlcManager::CPlcManager()
{
	//�׽�Ʈ
	bool bPlcConn = m_plc.Open(1);
	if (bPlcConn == true) {
		short shVal = 0;
		m_plc.ReadBlock("D1000", 2, &shVal);
		int a = 10;
	}

	bool bDBConn = m_db.connect("127.0.0.1", "postgres");

	if (bDBConn == true) {
		DBResult result = m_db.execute("select * from PLCaddress");
		ReadPLC();
		int a = 10;
	}
}


CPlcManager::~CPlcManager()
{

}

vector<st_plc_address> CPlcManager::GetPlcAddressFromDB()
{
	//db�� �ִ� plc ��巹�� ������ �д´�
	vector<st_plc_address> vt_plcAddress;

	string strColumn = "synctype";
	
	string strQuery = "SELECT address, blocksize, synctype FROM plcaddress";
	

	DBResult dbResult = m_db.execute(strQuery);

	if (dbResult.getRowCount() > 0){
		while (dbResult.hasNext()) {
			std::vector<DBRow>::iterator itRow = dbResult.fetchRow();
			int nValue = -1;
			string strValue;

			strValue = itRow->getValue(dbResult.getColumnIndexByName("address"))->getByString();

			nValue = itRow->getValue(dbResult.getColumnIndexByName("blocksize"))->getByInteger();

			strValue = string(itRow->getValue(dbResult.getColumnIndexByName("synctype"))->getByString());
		}
	}

	return vt_plcAddress;
}

vector<st_plc_read_sch> CPlcManager::GetSchFromDB(int nBlockID)
{
	vector<st_plc_read_sch> vt_plcReadSch;

	string strQuery = "SELECT addressid, idxstt, idxbit, size, keyname, datatypedb FROM PLCReadSCH WHERE addressid =? ";
	//strQuery.append(fmt::format(" WHERE {:s} = '{:s}'", strColumn, "read"));

	DBResult dbResult = m_db.execute(strQuery);

	return vt_plcReadSch;

}


//PLC�޾ƿ;��� ��ü read ������ ó���Ѵ�.
void CPlcManager::ReadPLC()
{
	//PLC read�����͸� ���������� ������ ����ü
	//PLC_READ_DATA
	//map<string, string> map_data;

	vector<st_plc_address> vt_address_data = GetPlcAddressFromDB(); //db�� �ִ� Plc ��巹�� ������ �д´�

	int nSize = vt_address_data.size();

	for (int i = 0; i < nSize; i++) {
		std::string strSyncType = vt_address_data[i].cSyncType;
		if (strSyncType != "read") {
			continue;
		}
		int nBlockID = vt_address_data[i].blockID;
		int nBlockSize = vt_address_data[i].blockSize;
		CString strAddress = vt_address_data[i].cAddress;

		//�� �Ҵ�
		short* pPlcData = new short[nBlockSize];
		memset(pPlcData, 0, sizeof(short) * nBlockSize);

		m_plc.ReadBlock(strAddress, nBlockSize, pPlcData); //������ �Ҵ��� Heap�����Ϳ� Size�� �����Ͽ� plc�κ��� data�� �޴´�

		std::vector<st_plc_read_sch> vt_sch_data = GetSchFromDB(nBlockID); //plc block ������ �� ���� ������

		int nSizeSch = vt_sch_data.size();

		for (int j = 0; j < nSizeSch; j++) {
			int nSttIdx = vt_sch_data[j].idxstt;
			int nIdxBit = vt_sch_data[j].idxbit;
			int nSize = vt_sch_data[j].size;
			std::string strKeyName = vt_sch_data[j].keyname;
			CString strType = vt_sch_data[j].cDataTypeDB;

			std::string strVal = ParsePlcData(pPlcData, nSttIdx, nIdxBit, nSize, strType);

			//map_data.insert({ strKeyName, strVal });

			//Ű�� Ȯ���Ͽ� ���� 

		}

		delete[] pPlcData;
	}
}

void CPlcManager::WritePLC()
{
	std::vector<st_plc_address> vt_address_data = GetPlcAddressFromDB(); //db�� �ִ� Plc ��巹�� ������ �д´�

	int nSize = vt_address_data.size();

	for (int i = 0; i < nSize; i++) {
		std::string strSyncType = vt_address_data[i].cSyncType;
		if (strSyncType != "write") {
			continue;
		}

		int nBlockID = vt_address_data[i].blockID;
		int nBlockSize = vt_address_data[i].blockSize;
		CString strAddress = vt_address_data[i].cAddress;

		//�� �Ҵ�
		short* pPlcData = new short[nBlockSize];
		memset(pPlcData, 0, sizeof(short) * nBlockSize);

		std::vector<st_plc_read_sch> vt_sch_data = GetSchFromDB(nBlockID); //plc block ������ �� ���� ������

		int nSizeSch = vt_sch_data.size();

		for (int j = 0; j < nSizeSch; j++) {
			int nSttIdx = vt_sch_data[j].idxstt;
			int nIdxBit = vt_sch_data[j].idxbit;
			int nSize = vt_sch_data[j].size;
			std::string strKeyName = vt_sch_data[j].keyname;
			CString strType = vt_sch_data[j].cDataTypeDB;

			//Ű�� �ش��ϴ� ���� �����´�.
			//std::string strValue = GetValue("Ű��");

			//Ÿ���� ���� �´� �ɷ� ��ȯ
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

		if (nSize == 2) {	//2word ó��
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



std::string  CPlcManager::ParsePlcData(short* pPlcData, int nIdxStt, int nIdxBit, int nSize, CString strType)
{
	std::string strRet;

	if (strType == "string") {
		CString strData = GetStringDataFromShort(&pPlcData[nIdxStt], nSize);
		strRet = strData;
	}
	else if (strType == "int") {
		CString strData;

		if (nSize == 2) {	//2word ó��
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

	char* ptrChar = (char*)pData;	//ù��° �ּ�

	for (int i = 0; i < nStrLength; i++) {
		if (i >= nSize) {
			break;
		}
		char cData = strData.GetAt(i);
		ptrChar[i] = cData;
	}
}

