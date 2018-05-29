// CppChainConstruction.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include "MerkelTree.h"




typedef struct BitBlock 
{
	char transaction[256];
	char parent_hash[256];
	char myHash[256];
	SYSTEMTIME TimeStamp;
	BitBlock* pLastBlock;
	BitBlock* pNextBlock;
}BITBLOCK, *PBITBLOCK;


class BlockChain
{
	BitBlock *pHead;
	BitBlock *pTail;
	BitBlock *pTemp;

	string LastHash;

public:
	BlockChain()
	{
		pHead = NULL;
		pTail = NULL;
		pTemp = NULL;
		LastHash = "";

		Initialize();
	}

	void Initialize( )
	{
		if (pHead == NULL)
		{
			int szblock = sizeof(BitBlock);
			pHead = (BitBlock*)malloc(szblock);
			pHead->myHash[0] = '\0';
			pHead->parent_hash[0] = '\0';
			pHead->transaction[0] = '\0';
			
			GetSystemTime(&pHead->TimeStamp);
			pTail = pHead;
		
			LastHash = sha256("");
			
			pHead->pLastBlock = NULL;
			pHead->pNextBlock = NULL;

			memcpy(pHead->myHash, LastHash.c_str(), 256);
			
		}

	}



	BitBlock* AddBlock( string transaction)
	{
		int szblock = sizeof(BitBlock);
		pTemp = (BitBlock*)malloc(szblock);
	
	//TimeStamp
		GetSystemTime(&pTemp->TimeStamp);
	
	//Parent Hast
		int lenph = strlen(pTail->myHash);
		memcpy(pTemp->parent_hash, pTail->myHash, lenph);
		pTemp->parent_hash[lenph] = '\0';
		
	//Transaction
		int lentran = transaction.length();
		memcpy(pTemp->transaction, transaction.c_str(), lentran);
		pTemp->transaction[lentran] = '\0';
	
	//Current block hash
		string input = transaction;
		string hashstr = input.append(LastHash);

		LastHash = sha256(hashstr);
		int lencb = LastHash.length();

		memcpy(pTemp->myHash, LastHash.c_str(), lencb );
		pTemp->myHash[lencb] = '\0';

		pTemp->pLastBlock = pTail;
		pTail->pNextBlock = pTemp;
		pTail = pTemp;
		
		pTemp->pNextBlock = NULL;

		//cout << "\n\nBlock :: " << transaction << "\nParent Hash:" << pTemp->parent_hash << "\nMyHash :" << pTemp->myHash;
		return pTail;
	}



	void PrintChainInFile(string fname)
	{

		ofstream ofile(fname.c_str(), ios::app | ios::out);
		pTemp = pHead;
		if (ofile.is_open())
		{
			while (pTemp != NULL)
			{
				SYSTEMTIME st = pTemp->TimeStamp;
				std::ostringstream ossMessage;

				ossMessage << st.wYear << "-"
					<< std::setw(2) << std::setfill('0') << st.wMonth << "-"
					<< std::setw(2) << std::setfill('0') << st.wDay << " "
					<< std::setw(2) << std::setfill('0') << st.wHour << ":"
					<< std::setw(2) << std::setfill('0') << st.wMinute << ":"
					<< std::setw(2) << std::setfill('0') << st.wSecond << "."
					<< std::setw(3) << std::setfill('0') << st.wMilliseconds;

				string strMessage = ossMessage.str();
				//std::cout << "System time = " << strMessage << std::endl;

				ofile << "\n"
					<< "\nTransaction  : " << pTemp->transaction
					<< "\nTime stamp   : " << strMessage
					<< "\nParent Hash  : " << pTemp->parent_hash
					<< "\nMyHash ......: " << pTemp->myHash;
				pTemp = pTemp->pNextBlock;
			}
			ofile.close();
		}
		else cout << "Unable to open file";

		return ;

	}


	void PrintOldestFirst()
	{
		pTemp = pHead;
		cout << "\n\n\nBlock Chain ----- Oldest Transaction First\n"; 
		
		while (pTemp != NULL)
		{
			cout << "\n" 
				 << "\nTransaction  : " << pTemp->transaction 
				 << "\nParent Hash  : " << pTemp->parent_hash 
				 << "\nMyHash ......: " << pTemp->myHash;
			pTemp = pTemp->pNextBlock;
		}
	
	}

	void PrintLatestFirst()
	{
		pTemp = pTail;
		cout << "\nBlock Chain ----- Recent Transaction First\n";
		while (pTemp != NULL)
		{
			cout << "\n"
				<< "\nTransaction  : " << pTemp->transaction
				<< "\nParent Hash  : " << pTemp->parent_hash
				<< "\nMyHash ......: " << pTemp->myHash;
			pTemp = pTemp->pLastBlock;
		}

	}

};



void ReadDataFile(const char* pFilename, STRING_COLLECTION* vec )
{
	
	string line;
	ifstream myfile(pFilename);
	
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			//cout << line << '\n';
			(*vec).push_back(line);
		}
		myfile.close();
	}

	else cout << "Unable to open file";

	return ;
}

int main( int argc, char** argv )
{
	if (argc < 2)
	{
		cout << "\nUsage : application.exe <transaction_input_file_path>\n";
		return 0;
	}

	string inputfile = argv[1];
	STRING_COLLECTION inputCollection;
	
	ReadDataFile(inputfile.c_str(), &inputCollection );


	BlockChain *pChain = new BlockChain();
	

	int count = inputCollection.size();
	
	for (int i = 1; i< count; ++i )
	{
		string pTransaction = inputCollection.at(i);

		pChain->AddBlock(pTransaction);

	}

	string strfname = "OutPut.chain";
	pChain->PrintChainInFile(strfname);
		
	//pChain->PrintOldestFirst();
	//pChain->PrintLatestFirst();


	cout << "\n\n**TEST-1 PROCESSED TRANSACTIONS - BitChain Length generated -: " << count;
	cout << endl;


	cout << "\n\n**TEST-2 Sample Merkel Tree Processing for MERKEL ROOT -: " ;

	//Passing NULL means we dont have a collection of Transaction Hashes
	STRING_COLLECTION vst;
	
	string stRoodHash = GetMerkelRootHash(vst);
	
	cout << "\nProcessed MERKEL ROOT -:: ";
	cout << stRoodHash <<endl ;

    return 0;
}

