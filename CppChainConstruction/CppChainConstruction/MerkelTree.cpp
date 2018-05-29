// MerkelTreeCpp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "MerkelTree.h"


static STRING_COLLECTION vHashCollection;


void FillFakeTransactionHashes(ifstream* ifile)
{
	UINT64 iCount = 0;
	string RandomStr = "The actual value of a company or an asset based on an underlying perception of its true value including all aspects of the business.";
	if (ifile->bad())
	{
		cout << "NUMBER OF TRANSACTIONS - ";
		cin >> iCount;
		UINT64 i = -1;
		char buffer[65] = "\0";
		string hash = "";
		string hasStr = RandomStr;
		while (++i < iCount)
		{
			sprintf(buffer, "%d", i);
			hasStr = hasStr.append(buffer);
			string hash = sha256(hasStr);

			vHashCollection.push_back(hash);
		}
	}
	else // read input file
	{
		//cout << "Reading - ";
		string st;
		ifstream &fin = *ifile;
		while (ifile->good())
		{
			st = "";
			getline(fin, st);
			vHashCollection.push_back(st);
		}
		ifile->close();
	}

	return;
}

STRING_COLLECTION MoveToNextLevel(STRING_COLLECTION vInput)
{
	STRING_COLLECTION vTemp;
	STRING_COLLECTION::iterator itr = vInput.begin();
	string newStr;

	UINT32 index = 0;
	while (index < vInput.size())
	{
		if (index + 2 > vInput.size())
		{//special case
			newStr = vInput[index];
			newStr = newStr.append(vInput[index]);

			string newHash = sha256(newStr);
			vTemp.push_back(newHash);
		}
		else
		{//append and fill new hash on top
			newStr = vInput[index];
			newStr = newStr.append(vInput[index + 1]);

			string newHash = sha256(newStr);
			vTemp.push_back(newHash);
		}

		index += 2;
		newStr = "";
	}
	return vTemp;

}


string GetMerkelRootHash(STRING_COLLECTION vTempInputHash)
{
	if (!vTempInputHash.empty())
		vHashCollection = vTempInputHash;

	else
	{

		ifstream ifile("HashInput.hash");

		FillFakeTransactionHashes(&ifile);
	}
	

	STRING_COLLECTION vtemp = vHashCollection;
	UINT64 ts = GetTickCount64();
	CollectionVector vvCollection;

	do
	{
		STRING_COLLECTION vtemp1 = MoveToNextLevel(vtemp);
		vtemp.clear();
		vtemp = vtemp1;

		vvCollection.push_back(vtemp1);

		cout << endl << "Level [" << vvCollection.size() << "] has Merkel Leaves  = " << vtemp.size() ;

	} while (vtemp.size()  > 1);

	//cout << "\n****Merkel Root = " << *vtemp.begin() << endl;

	UINT64 te = GetTickCount64();

	cout << "\nTime Taken = " << te - ts << endl;

	return  *vtemp.begin();
}
