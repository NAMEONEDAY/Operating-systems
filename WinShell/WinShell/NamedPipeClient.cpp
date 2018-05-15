#include "NamedPipeClient.h"


void OpenNamedPipeInClient()
{
	//等待连接命名管道
	if (!WaitNamedPipe(pPipeName1, NMPWAIT_WAIT_FOREVER))
	{
		cout << "命名管道实例不存在 ..." << endl << endl;
		return;
	}

	//打开命名管道
	hNamedPipe1 = CreateFile(pPipeName1, GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hNamedPipe1)
	{
		cout << "打开命名管道失败 ..." << endl << endl;
		return;
	}
}


char *NamedPipeReadInClient()
{
	char *                pReadBuf;
	DWORD                dwRead;

	pReadBuf = new char[1024];
	memset(pReadBuf, 0, 1024);

	//从命名管道中读取数据
	if (!ReadFile(hNamedPipe1, pReadBuf, 1024, &dwRead, NULL))
	{
		cout << "读取数据失败 ..." << endl << endl;
	}
	//cout << "读取数据成功：    " << pReadBuf << endl << endl;
	return pReadBuf;
}


void NamedPipeWriteInClient(char *pStr)
{
	DWORD                dwWrite;

	//向命名管道中写入数据
	if (!WriteFile(hNamedPipe1,pStr, strlen(pStr), &dwWrite, NULL))
	{
		cout << "写入数据失败 ..." << endl << endl;
		return;
	}
	//cout << "写入数据成功：    " << pStr << endl << endl;
}

void ClosePipe() {
	CloseHandle(hNamedPipe1);
}