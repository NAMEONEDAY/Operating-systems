#include "NamedPipeClient.h"


void OpenNamedPipeInClient()
{
	//�ȴ����������ܵ�
	if (!WaitNamedPipe(pPipeName1, NMPWAIT_WAIT_FOREVER))
	{
		cout << "�����ܵ�ʵ�������� ..." << endl << endl;
		return;
	}

	//�������ܵ�
	hNamedPipe1 = CreateFile(pPipeName1, GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hNamedPipe1)
	{
		cout << "�������ܵ�ʧ�� ..." << endl << endl;
		return;
	}
}


char *NamedPipeReadInClient()
{
	char *                pReadBuf;
	DWORD                dwRead;

	pReadBuf = new char[1024];
	memset(pReadBuf, 0, 1024);

	//�������ܵ��ж�ȡ����
	if (!ReadFile(hNamedPipe1, pReadBuf, 1024, &dwRead, NULL))
	{
		cout << "��ȡ����ʧ�� ..." << endl << endl;
	}
	//cout << "��ȡ���ݳɹ���    " << pReadBuf << endl << endl;
	return pReadBuf;
}


void NamedPipeWriteInClient(char *pStr)
{
	DWORD                dwWrite;

	//�������ܵ���д������
	if (!WriteFile(hNamedPipe1,pStr, strlen(pStr), &dwWrite, NULL))
	{
		cout << "д������ʧ�� ..." << endl << endl;
		return;
	}
	//cout << "д�����ݳɹ���    " << pStr << endl << endl;
}

void ClosePipe() {
	CloseHandle(hNamedPipe1);
}