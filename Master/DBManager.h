#pragma once

#include <string>
#include "gPostgreSQL.h"

using namespace std;

class CDBManager
{
public:
	CDBManager();
	~CDBManager();

	bool CDBManager::Open(string strDBAddress, string strDBName);
	DBResult CDBManager::execQuery(string strQuery);

private:
	gPostgreSQL m_db;
};

