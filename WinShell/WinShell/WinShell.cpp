#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include<string.h>
#include<fstream>
#include<string>
#include"main.h"
#include <iomanip>
#include<iostream>

int flag1 = 0;
int flag2 = 0;
using namespace std;

//���������
char commandall[30][4096] = { "time" ,"cp","cd","ls","quit",">","<",">>" ,"cat","wc","-help"};

//du����
long getsizef(char *in) {
	LONG i=0;
	WIN32_FIND_DATA lp;
	char source[4096];
	lstrcpy(source, in);
	lstrcat(source, "\\*.*");
	HANDLE hfind = FindFirstFile(source, &lp);
	if (hfind != INVALID_HANDLE_VALUE) {
		while (FindNextFile(hfind, &lp) != 0) {
			//��16���ʾ��Ŀ¼���ݹ����getsizef���м���
			if (lp.dwFileAttributes == 16) {
				if (lstrcmp(lp.cFileName, ".") != 0 && lstrcmp(lp.cFileName, "..") != 0) {
					memset(source, '0', sizeof(source));
					lstrcpy(source, in);
					lstrcat(source, "\\");
					lstrcat(source, lp.cFileName);
					//�ݹ�
					i += getsizef(source);
					//��source���ԭĿ¼
					lstrcpy(source, in);
					lstrcat(source, "\\");
				}
			}
			else {
				//������ļ���ֱ�Ӽ����С�������
				memset(source, 0, sizeof(source));
				//��ó��ļ���
				lstrcpy(source, in);
				lstrcat(source, "\\");
				lstrcat(source, lp.cFileName);
				WIN32_FIND_DATA lp2;
				HANDLE hfind = FindFirstFile(source, &lp2);
				HANDLE hsource = CreateFile(source,
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ,
					NULL,
					OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					NULL);
				//�����ļ���С
				LONG size = lp2.nFileSizeLow - lp2.nFileSizeHigh;
				//���
				i += size;
				//�رվ��
				CloseHandle(hfind);
				CloseHandle(hsource);
				//��·����Ϊԭ·��
				lstrcpy(source, in);
				lstrcat(source, "\\");
			}
		}
	}
	//���ش�С
	return i;
}

void du(char *in) {
	long size;
	size = getsizef(in);
	//���ݴ�С�ж�Ŀ¼�Ƿ���ڻ����Ƿ�Ϊ��
	if (!(size== 0))
		cout << "Ŀ¼" << in << "��СΪ" << size << endl;
	else
		cout << "û�и�Ŀ¼���Ŀ¼Ϊ��" << endl;
}

//gedit����
void gedit(char *in) {
	//Ϊ�˷�ֹû���ļ��ȵ��øú�����û���ļ��ͻ��Զ�����
	HANDLE hsource = CreateFile(in,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	//�رվ��
	CloseHandle(hsource);
	char buffer[4096];
	memset(buffer, 0, sizeof(buffer));
	//���ü��±�
	strcpy(buffer, "notepad.exe");
	strcat(buffer, " ");
	strcat(buffer, in);
	STARTUPINFO si;                           //����������
	PROCESS_INFORMATION pi;                   //������Ϣ
	memset(&si, 0, sizeof(STARTUPINFO));
	int year, month, day, hour, minutes, seconds, milliseconds;
	if (!CreateProcess(NULL, //���ݸ��½��̵��������ַ���
		buffer,
		NULL, //��ȫ��
		NULL, //��ȫ��
		FALSE, //�Ƿ���ý��̵Ĵ򿪾���ĸ���
		0,
		NULL,
		NULL,
		&si,//���������
		&pi)//���
		)//��������
		cout << "�򿪼��±�ʧ��" << endl;
	else
		WaitForSingleObject(pi.hProcess, INFINITE);
}

//search����
//dir��ʾ�����ļ���Ŀ¼
void search(char *in,char *dir) {
	//��ʼ������ǰĿ¼
	char buffer2[4096];
	memset(buffer2, 0, sizeof(buffer2));
	strcpy(buffer2, dir);
	strcat(buffer2, in);
	HANDLE hFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA pNextInfo;
	hFile = FindFirstFile(buffer2, &pNextInfo);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		//cout << "û���������ļ�" << endl;
	}
	else{
		WCHAR infPath[MAX_PATH] = { 0 };
		if (pNextInfo.cFileName[0] != '.')
		{
			printf("Find result = %s%s\n",dir, pNextInfo.cFileName);
		}

		while (FindNextFile(hFile, &pNextInfo))
		{
			if (pNextInfo.cFileName[0] == '.')
			{
				continue;
			}
			printf("Find result = %s%s\n",dir, pNextInfo.cFileName);
		}
	}
	//��ǰĿ¼������ϴӸ�Ŀ¼��ʼ�ݹ�����
	HANDLE hFile2 = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA pNextInfo2;
	char buffer3[4096];
	memset(buffer3, 0, sizeof(buffer3));
	char buffer4[4096];
	memset(buffer4, 0, sizeof(buffer4));
	strcpy(buffer4, dir);
	strcat(buffer4, "\\*.*");
	hFile2 = FindFirstFile(buffer4, &pNextInfo2);
	if (hFile2 != INVALID_HANDLE_VALUE) {
		while (FindNextFile(hFile2, &pNextInfo2) != 0) {
			//��16���ʾ��Ŀ¼���ݹ����mycp���и���
			if (pNextInfo2.dwFileAttributes == 16) {
				if (lstrcmp(pNextInfo2.cFileName, ".") != 0 && lstrcmp(pNextInfo2.cFileName, "..") != 0) {
					strcpy(buffer3, dir);
					strcat(buffer3, "\\");
					strcat(buffer3, pNextInfo2.cFileName);
					strcat(buffer3, "\\");
					search(in, buffer3);
				}
			}
		}
	}
}

//rm����
void rm(char *in) {
	char parm[50][4096];
	memset(parm, 0, sizeof(parm));
	int ln = strlen(in);
	int i = 0;
	int t = 0;
	int j = 0;
	//���ַ������տո����
	while (i < ln) {
		if (in[i] == ' ') {
			i++;
			j++;
			t = 0;
		}
		else {
			parm[j][t] = in[i];
			t++;
			i++;
		}
	}
	if (in[i - 1] != ' ')
		j++;
	i = 0;
	while (i < j) {
		//����0��ʾɾ��ʧ��
		if (!DeleteFile(parm[i]))
			cout << "ɾ��ʧ��" << endl;
		i++;
	}
}

//mv����
void mv(char *in) {
	char parm[50][4096];
	memset(parm, 0, sizeof(parm));
	int ln = strlen(in);
	int i = 0;
	int t = 0;
	int j = 0;
	//���ַ������տո����
	while (i < ln) {
		if (in[i] == ' ') {
			i++;
			j++;
			t = 0;
		}
		else {
			parm[j][t] = in[i];
			t++;
			i++;
		}
	}
	if (in[i - 1] != ' ')
		j++;
	char buffer[4096];
	char buffer2[4096];
	memset(buffer2, 0, sizeof(buffer2));
	memset(buffer, 0, sizeof(buffer));
	if (j == 2) {
		WIN32_FIND_DATA lp;
		if (FindFirstFile(parm[0], &lp) == INVALID_HANDLE_VALUE)
			cout << "û��" << parm[0] << "����ļ�" << endl;
		else {
			if (FindFirstFile(parm[1], &lp) == INVALID_HANDLE_VALUE) {
				if (rename(parm[0], parm[1]))
					cout << "������ʧ��" << endl;
			}
			else {
				strcpy(buffer2, buffer);
				strcpy(buffer2, parm[1]);
				strcat(buffer2, "\\");
				strcat(buffer2, parm[0]);
				MoveFile(parm[0], buffer2);
			}
		}
	}
	else if (j > 2) {
		WIN32_FIND_DATA lp;
		if (FindFirstFile(parm[j - 1], &lp) == INVALID_HANDLE_VALUE)
			CreateDirectory(parm[j - 1], NULL);
		int i = 0;
		while (i < j - 1) {
			if (FindFirstFile(parm[i], &lp) == INVALID_HANDLE_VALUE)
				cout << "û��" << parm[i] << "����ļ�" << endl;
			else {
				strcpy(buffer2, buffer);
				strcpy(buffer2, parm[j-1]);
				strcat(buffer2, "\\");
				strcat(buffer2, parm[i]);
				MoveFile(parm[i], buffer2);
			}
			i++;
		}
	}
	else
		cout << "��������" << endl;
}

//mkdir����
void mkdir(char *in) {
	char parm[50][4096];
	memset(parm, 0, sizeof(parm));
	int ln = strlen(in);
	int i = 0;
	int t = 0;
	int j = 0;
	//���ַ������տո����
	while (i < ln) {
		if (in[i] == ' ') {
			i++;
			j++;
			t = 0;
		}
		else {
			parm[j][t] = in[i];
			t++;
			i++;
		}
	}
	if (in[i - 1] != ' ')
		j++;
	i = 0;
	while (i < j) {
		if (!CreateDirectory(parm[i], NULL))
			cout << "����ʧ��" << endl;
		i++;
	}
}

void DeleteD(char *in) {
	WIN32_FIND_DATA lp;
	char source[4096];
	lstrcpy(source, in);
	lstrcat(source, "\\*.*");
	HANDLE hfind = FindFirstFile(source, &lp);
	if (hfind != INVALID_HANDLE_VALUE) {
		while (FindNextFile(hfind, &lp) != 0) {
			//��16���ʾ��Ŀ¼���ݹ����mycp���и���
			if (lp.dwFileAttributes == 16) {
				if (lstrcmp(lp.cFileName, ".") != 0 && lstrcmp(lp.cFileName, "..") != 0) {
					memset(source, '0', sizeof(source));
					lstrcpy(source, in);
					lstrcat(source, "\\");
					lstrcat(source, lp.cFileName);
					DeleteD(source);
					//��source��target���ԭĿ¼
					lstrcpy(source, in);
					lstrcat(source, "\\");
				}
			}
			else {
				memset(source, 0, sizeof(source));
				lstrcpy(source, in);
				lstrcat(source, "\\");
				lstrcat(source, lp.cFileName);
				//���ø��ƺ�����ʼ����
				DeleteFile(source);
				lstrcpy(source, in);
				lstrcat(source, "\\");
			}
		}
		FindClose(hfind);
		RemoveDirectory(in);
	}
	else {
		FindClose(hfind);
		RemoveDirectory(in);
	}
}

//rmdir����
void rmdir(char *in) {
	char parm[50][4096];
	memset(parm, 0, sizeof(parm));
	int ln = strlen(in);
	int i = 0;
	int t = 0;
	int j = 0;
	//���ַ������տո����
	while (i < ln) {
		if (in[i] == ' ') {
			i++;
			j++;
			t = 0;
		}
		else {
			parm[j][t] = in[i];
			t++;
			i++;
		}
	}
	if (in[i - 1] != ' ')
		j++;
	i = 0;
	char buffer[1000];
	while (i < j) {
		DeleteD(parm[i]);
		i++;
	}
}

//ls����
void ls(char *in) {
	SYSTEMTIME ti;
	FILETIME ftCreationTime, ftLastAccessTime, ftLastWriteTime;
	char wenjian[300][1000];
	memset(wenjian, 0, sizeof(wenjian));
	int timeall[300];
	memset(timeall, 0, sizeof(timeall));
	//���ڱ��浱ǰ·��
	char buffer[MAX_PATH];		
	_getcwd(buffer, 4096);
	char parm[6][40];
	memset(parm, 0, sizeof(parm));
	int ln = strlen(in);
	int i = 0;
	int t = 0;
	int j = 0;
	//�����������ã�0Ϊ�޲�����1Ϊ-l,2Ϊ-a,3Ϊ-l��-a��4λ-s��5Ϊ-t
	int fl = 0;
	//���ַ������տո����
	while (i < ln) {
		if (in[i] == ' ') {
			i++;
			j++;
			t = 0;
		}
		else {
			parm[j][t] = in[i];
			t++;
			i++;
		}
	}
	if(strlen(parm[j])!=0)
		j++;
	if (j == 0){
		fl = 0;
	}
	else if (j == 1) {
		if (!strcmp(parm[0], "-l"))
			fl = 1;
		else if (!strcmp(parm[0], "-a"))
			fl = 2;
		else if (!strcmp(parm[0], "-s"))
			fl = 4;
		else if (!strcmp(parm[0], "-t"))
			fl = 5;
		else {
			strcat(buffer, "\\");
			strcat(buffer, parm[0]);
		}
	}
	else if (j == 2) {
		if (!strcmp(parm[0], "-l")) {
			if (!strcmp(parm[1], "-a"))
				fl = 3;
			else {
				fl = 1;
				strcat(buffer, "\\");
				strcat(buffer, parm[1]);
			}
		}
		else if (!strcmp(parm[1], "-a")) {
			if (!strcmp(parm[1], "-l"))
				fl = 3;
			else {
				fl = 2;
				strcat(buffer, "\\");
				strcat(buffer, parm[1]);
			}
		}
	}
	else {
		strcat(buffer, "\\");
		strcat(buffer, parm[2]);
	}
	char buffer1[4096];
	strcpy(buffer1, buffer);
	strcat(buffer, "\\*.*");
	char buffer2[4096];
	char buffer3[4096];
	memset(buffer3, 0, sizeof(buffer3));
	if (fl == 5) {
		i = 0;
		WIN32_FIND_DATA lp;
		HANDLE hfind = FindFirstFile(buffer, &lp);
		if (hfind != INVALID_HANDLE_VALUE) {
			while (FindNextFile(hfind, &lp) != 0) {
				if (lstrcmp(lp.cFileName, ".") != 0 && lstrcmp(lp.cFileName, "..") != 0) {
					strcpy(wenjian[i], lp.cFileName);
					strcpy(buffer2, buffer1);
					strcat(buffer2, "\\");
					strcat(buffer2, lp.cFileName);
					HANDLE hs = CreateFile(buffer2,
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ,
						NULL,
						OPEN_ALWAYS,
						FILE_FLAG_BACKUP_SEMANTICS,
						NULL);
					LONG size = lp.nFileSizeLow - lp.nFileSizeHigh;
					GetFileTime(hs, &ftCreationTime, &ftLastAccessTime, &ftLastWriteTime);
					FileTimeToSystemTime(&ftLastWriteTime, &ti);
					timeall[i] = (((((ti.wYear * 2 + ti.wMonth) * 3 + ti.wDay) * 2 + ti.wHour) * 6 + ti.wMinute) * 6 + ti.wSecond);
					i++;
					CloseHandle(hs);
				}
			}
			CloseHandle(hfind);
			char tmpstr[4096];
			memset(tmpstr, 0, sizeof(tmpstr));
			for (int j = 0; j < i-1; ++j) {
				for (int h = 0; h < i-1; h++) {
					if (timeall[h] < timeall[h+1]) {
						int tmp;
						tmp = timeall[h];
						timeall[h] = timeall[h+1];
						timeall[h+1] = tmp;
						strcpy(tmpstr, wenjian[h]);
						strcpy(wenjian[h], wenjian[h+1]);
						strcpy(wenjian[h+1], tmpstr);
					}
				}
			}
			for (j = 0; j < i; ++j) {
				printf("%-30s", wenjian[j]);
				strcpy(buffer2, buffer1);
				strcat(buffer2, "\\");
				strcat(buffer2, wenjian[j]);
				HANDLE hs = CreateFile(buffer2,
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ,
					NULL,
					OPEN_ALWAYS,
					FILE_FLAG_BACKUP_SEMANTICS,
					NULL);
				LONG size = lp.nFileSizeLow - lp.nFileSizeHigh;
				GetFileTime(hs, &ftCreationTime, &ftLastAccessTime, &ftLastWriteTime);
				FileTimeToSystemTime(&ftLastWriteTime, &ti);
				char Pstr[1000];
				memset(Pstr, 0, sizeof(Pstr));
				sprintf(Pstr, "%4d-%2d-%2d  %2d: %2d : %2d", ti.wYear, ti.wMonth, ti.wDay, ti.wHour, ti.wMinute, ti.wSecond);
				cout << Pstr << endl;
				CloseHandle(hs);
			}
		}
	}
	else {
		WIN32_FIND_DATA lp;
		HANDLE hfind = FindFirstFile(buffer, &lp);
		if (hfind != INVALID_HANDLE_VALUE) {
			while (FindNextFile(hfind, &lp) != 0) {
				if (fl == 0) {
					if (lstrcmp(lp.cFileName, ".") != 0 && lstrcmp(lp.cFileName, "..") != 0) {
						if (flag2) {
							strcat(buffer3, lp.cFileName);
							strcat(buffer3, " ");
						}
						else {
							if (lp.dwFileAttributes == 16) {
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
								cout << lp.cFileName << "\t";
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
							}
							else
								cout << lp.cFileName << "\t";
						}
					}
				}
				else if (fl == 1) {
					if (lstrcmp(lp.cFileName, ".") != 0 && lstrcmp(lp.cFileName, "..") != 0) {
						printf("%-30s", lp.cFileName);
						strcpy(buffer2, buffer1);
						strcat(buffer2, "\\");
						strcat(buffer2, lp.cFileName);
						LONG size = lp.nFileSizeLow - lp.nFileSizeHigh;
						printf("%-10d", size);
						HANDLE hs = CreateFile(buffer2,
							GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ,
							NULL,
							OPEN_ALWAYS,
							FILE_FLAG_BACKUP_SEMANTICS,
							NULL);
						GetFileTime(hs, &ftCreationTime, &ftLastAccessTime, &ftLastWriteTime);
						FileTimeToSystemTime(&ftLastAccessTime, &ti);
						char Pstr[1000];
						memset(Pstr, 0, sizeof(Pstr));
						sprintf(Pstr, "%4d-%2d-%2d  %2d: %2d : %2d", ti.wYear, ti.wMonth, ti.wDay, ti.wHour, ti.wMinute, ti.wSecond);
						cout << Pstr << endl;
						CloseHandle(hs);
					}					}
				else if (fl == 2) {
					if (lp.dwFileAttributes == 16) {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
						cout << lp.cFileName << "\t";
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
					}
					else
						cout << lp.cFileName << "\t";
				}
				else if (fl == 3) {
					printf("%-30s", lp.cFileName);
					strcpy(buffer2, buffer1);
					strcat(buffer2, "\\");
					strcat(buffer2, lp.cFileName);
					HANDLE hs = CreateFile(buffer2,
						GENERIC_READ | GENERIC_WRITE,							
						FILE_SHARE_READ,
						NULL,
						OPEN_ALWAYS,
						FILE_FLAG_BACKUP_SEMANTICS,
						NULL);
					LONG size = lp.nFileSizeLow - lp.nFileSizeHigh;
					printf("%-10d", size);
					GetFileTime(hs, &ftCreationTime, &ftLastAccessTime, &ftLastWriteTime);
					FileTimeToSystemTime(&ftLastAccessTime, &ti);
					char Pstr[1000];
					memset(Pstr, 0, sizeof(Pstr));
					sprintf(Pstr, "%4d-%2d-%2d  %2d: %2d : %2d", ti.wYear, ti.wMonth, ti.wDay, ti.wHour, ti.wMinute, ti.wSecond);
					cout << Pstr << endl;
					CloseHandle(hs);
				}
				else if (fl == 4) {
					printf("%-30s", lp.cFileName);
					strcpy(buffer2, buffer1);
					strcat(buffer2, "\\");
					strcat(buffer2, lp.cFileName);
					HANDLE hs = CreateFile(buffer2,
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ,
						NULL,
						OPEN_ALWAYS,
						FILE_FLAG_BACKUP_SEMANTICS,
						NULL);
					LONG size = lp.nFileSizeLow - lp.nFileSizeHigh;
					printf("%-10d", (int)size / 4096 + 1);
					CloseHandle(hs);
				}
			}
			if (!flag2)
				cout << "\n" << endl;
			else
				NamedPipeWriteInClient(buffer3);
		}
	}
}

void cd(char *in) {
	//����Ŀ¼ʧ�ܷ���0
	if (!SetCurrentDirectory(in))
		cout << "����Ŀ¼ʧ�ܣ������Ƿ����" << endl;
}

//wc����
//��ȡ����
void getlineNum(char *in) {
	string str;
	int i = 0;
	ifstream OpenFile(in);
	if (OpenFile) {
		while (!OpenFile.eof()) {
			//getlineһ�����Զ�����
			//��i��¼������
			getline(OpenFile, str);
			i++;
		}
		cout << "������" << i << endl;
	}
	else
		cout << "û��" << in << "����ļ�" << endl;
	OpenFile.close();
}

//��ȡ������
void getwordNum(char *in) {
	char str;
	int i = 0;
	ifstream OpenFile(in);
	if (OpenFile) {
		while (!OpenFile.eof()) {
			//get������ȡ�ַ��������ո��еȵ�����+1
			OpenFile.get(str);
			if (str == ' ' || str == '.' || str == ',' || str == '?'||str=='\n')
				i++;
		}
		cout << "��������" << i << endl;
	}
	else
		cout << "û��" << in << "����ļ�" << endl;
	OpenFile.close();
}

//��ȡ�ַ���
void getbitNum(char *in) {
	char str;
	int i = 0;
	ifstream OpenFile(in);
	if (OpenFile) {
		while (!OpenFile.eof()) {
			OpenFile.get(str);
				i++;
		}
		cout << "�ַ�����" << i << endl;
	}
	else
		cout << "û��" << in << "����ļ�" << endl;
	OpenFile.close();
}

void wc(char *in) {
	char parm[50][4096];
	memset(parm, 0, sizeof(parm));
	int ln = strlen(in);
	int i = 0;
	int t = 0;
	int j = 0;
	//���ַ������տո����
	while (i < ln) {
		if (in[i] == ' ') {
			i++;
			j++;
			t = 0;
		}
		else {
			parm[j][t] = in[i];
			t++;
			i++;
		}
	}
	j++;
	if (in[i - 1] == ' ')
		j--;
	if (!strcmp(parm[0], "-l")) {
		i = 1;
		while (i < j) {
			if ((!strcmp(parm[i], ">>")) || (!strcmp(parm[i], ">")))
				break;
			ifstream OpenFile(parm[i]);
			if(OpenFile){
				getlineNum(parm[i]);
				cout << "�ļ���" << parm[i] << endl;
			}
			else
				cout << "û��"<<parm[i]<<"���߸ò���ΪĿ¼" << endl;
			i++;
		}
	}
	else if (!strcmp(parm[0], "-w")) {
		i = 1;
		while (i < j) {
			if ((!strcmp(parm[i], ">>")) || (!strcmp(parm[i], ">")))
				break;
			ifstream OpenFile(parm[i]);
			if (OpenFile) {
				getwordNum(parm[i]);
				cout << "�ļ���" << parm[i] << endl;
			}
			else
				cout << "û��" << parm[i] << "���߸ò���ΪĿ¼" << endl;
			i++;
		}
	}
	else if (!strcmp(parm[0], "-c")) {
		i = 1;
		while (i < j) {
			if ((!strcmp(parm[i], ">>")) || (!strcmp(parm[i], ">")))
				break;
			ifstream OpenFile(parm[i]);
			if (OpenFile) {
				getbitNum(parm[i]);
				cout << "�ļ���" << parm[i] << endl;
			}
			else
				cout << "û��" << parm[i] << "���߸ò���ΪĿ¼" << endl;
			i++;
		}
	}
	else {
		i = 0;
		while (i < j) {
			if ((!strcmp(parm[i], ">>")) || (!strcmp(parm[i], ">")))
				break;
			ifstream OpenFile(parm[i]);
			if (OpenFile) {
				getlineNum(parm[i]);
				getwordNum(parm[i]);
				getbitNum(parm[i]);
				cout << "�ļ���" << parm[i] << endl;
			}
			else
				cout << "û��" << parm[i] << "���߸ò���ΪĿ¼" << endl;
			i++;
		}
	}
}

//cat����
void cat(char *in){
	char parm[50][4096];
	memset(parm, 0, sizeof(parm));
	int ln = strlen(in);
	int i = 0;
	int t = 0;
	int j = 0;
	//���ַ������տո����
	while (i < ln) {
		if (in[i] == ' ') {
			i++;
			j++;
			t = 0;
		}
		else {
			parm[j][t] = in[i];
			t++;
			i++;
		}
	}
	if (in[i] != ' ')
		j++;
	//�༭һ�����ļ�
	if (!strcmp(parm[0], ">")) {
		HANDLE hsource = CreateFile(parm[1],
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		char str[4096];
		memset(str, 0, sizeof(4096));
		gets_s(str);
		if (!strcmp(str, "EOFY"))
			;
		else {
			cout << str ;
			while (1) {
				gets_s(str);
				//��������EOFY
				if (!strcmp(str, "EOFY"))
					break;
				else
					printf("\n%s", str);
			}
		}
	}
	//�г��ļ�����
	else {
		i = 0;
		while (i < j) {
			//�ض��������>
			if (!strcmp(parm[i], ">>")||!strcmp(parm[i],">"))
				break;
			else {
				ifstream OpenFile(parm[i]);
				if (OpenFile) {
					string str;
					while (!OpenFile.eof()) {
						getline(OpenFile, str);
						cout << str <<endl;
					}
				}
				else
					cout << "û��" << parm[i] << "����ļ�" << endl;
				OpenFile.close();
			}
			i++;
		}
	}
}

//��ִ���ļ�
void pro(char *e, char *parm) {
	{
		STARTUPINFO si;                           //����������
		PROCESS_INFORMATION pi;                   //������Ϣ
		memset(&si, 0, sizeof(STARTUPINFO));
		strcat(e, " ");
		strcat(e, parm);
		if (!CreateProcess(NULL, //���ݸ��½��̵��������ַ���
			e,
			NULL, //��ȫ��
			NULL, //��ȫ��
			FALSE, //�Ƿ���ý��̵Ĵ򿪾���ĸ���
			0,
			NULL,
			NULL,
			&si,//���������
			&pi)//���
			)//��������
			cout << "û�и�����Ҳû�иÿ�ִ�г���" << endl;
		else
			WaitForSingleObject(pi.hProcess, INFINITE);
	}
}

//�������Ϊʵ��������
void copyfile(char* source, char* target) {
	WIN32_FIND_DATA lp;
	HANDLE hfind = FindFirstFile(source, &lp);
	HANDLE hsource = CreateFile(source,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	HANDLE htarget = CreateFile(target,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	FILETIME ct, at, wt;
	GetFileTime(hsource, &ct, &at, &wt);
	LONG size = lp.nFileSizeLow - lp.nFileSizeHigh;
	DWORD wordbit;
	int *BUFFER = new int[size];
	//��Դ�ļ����ݵ�buffer
	ReadFile(hsource,
		BUFFER,
		size,
		&wordbit,
		NULL);
	//дbuffer���ݵ�Ŀ���ļ�
	WriteFile(htarget,
		BUFFER,
		size,
		&wordbit,
		NULL);
	//����ʱ��
	SetFileTime(htarget, &ct, &at, &wt);
	CloseHandle(hfind);
	CloseHandle(hsource);
	CloseHandle(htarget);
}

void mycp(char* fsource, char* ftarget) {
	WIN32_FIND_DATA lp;
	FILETIME ct, at, wt;
	char source[4096];
	char target[4096];
	lstrcpy(source, fsource);
	lstrcpy(target, ftarget);
	lstrcat(source, "\\*.*");
	lstrcat(target, "\\");
	HANDLE hfind = FindFirstFile(source, &lp);
	if (hfind != INVALID_HANDLE_VALUE) {
		while (FindNextFile(hfind, &lp) != 0) {
			//��16���ʾ��Ŀ¼���ݹ����mycp���и���
			if (lp.dwFileAttributes == 16) {
				if (lstrcmp(lp.cFileName, ".") != 0 && lstrcmp(lp.cFileName, "..") != 0) {
					memset(source, '0', sizeof(source));
					lstrcpy(source, fsource);
					lstrcat(source, "\\");
					lstrcat(source, lp.cFileName);
					lstrcat(target, lp.cFileName);
					CreateDirectory(target, NULL);
					mycp(source, target);
					//��source��target���ԭĿ¼
					lstrcpy(source, fsource);
					lstrcpy(target, ftarget);
					lstrcat(source, "\\");
					lstrcat(target, "\\");
				}
			}
			else {
				memset(source, 0, sizeof(source));
				lstrcpy(source, fsource);
				lstrcat(source, "\\");
				lstrcat(source, lp.cFileName);
				lstrcat(target, lp.cFileName);
				//���ø��ƺ�����ʼ����
				copyfile(source, target);
				lstrcpy(source, fsource);
				lstrcpy(target, ftarget);
				lstrcat(source, "\\");
				lstrcat(target, "\\");
			}
		}
	}
	HANDLE hsource = CreateFile(fsource,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL);
	HANDLE htarget = CreateFile(ftarget,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL);
	//���Դ�ļ�ʱ��
	GetFileTime(hsource, &ct, &at, &wt);
	//�޸�Ŀ���ļ�ʱ��
	SetFileTime(htarget, &ct, &at, &wt);
	CloseHandle(hsource);
	CloseHandle(htarget);
}

void cp(char *in) {
	//���������Ĳ����ֳ�����Ŀ¼����
	char parm[3][4096];
	memset(parm, 0, sizeof(parm));
	int ln = strlen(in);
	int i = 0;
	int j = 0;
	int t = 0;
	while (i < ln) {
		if (in[i] == ' ') {
			i++;
			j++;
			t = 0;
		}
		else {
			parm[j][t] = in[i];
			t++;
			i++;
		}
	}
	WIN32_FIND_DATA lp;
	if (FindFirstFile(parm[0], &lp) == INVALID_HANDLE_VALUE)
		cout << "����Դ�ļ�·��ʧ��" << endl;
	else {
		if (FindFirstFile(parm[1], &lp) == INVALID_HANDLE_VALUE)
			CreateDirectory(parm[1], NULL);
		mycp(parm[0], parm[1]);
	}
}

//��������ʱ�����Ϊʵ�������
void time(char in[4096]) {
	char parm[4096];
	strcpy_s(parm, in);
	SYSTEMTIME start, end;                    //ʱ��
	STARTUPINFO si;                           //����������
	PROCESS_INFORMATION pi;                   //������Ϣ
	memset(&si, 0, sizeof(STARTUPINFO));
	int year, month, day, hour, minutes, seconds, milliseconds;
	if (!CreateProcess(NULL, //���ݸ��½��̵��������ַ���
		parm,
		NULL, //��ȫ��
		NULL, //��ȫ��
		FALSE, //�Ƿ���ý��̵Ĵ򿪾���ĸ���
		0,
		NULL,
		NULL,
		&si,//���������
		&pi)//���
		)//��������
		cout << "��������ʧ��,������������Ƿ���ȷ" << endl;
	else {
		GetSystemTime(&start);
		//�ȴ����̽���
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetSystemTime(&end);
		//����ʱ��
		milliseconds = end.wMilliseconds - start.wMilliseconds;
		seconds = end.wSecond - start.wSecond;
		minutes = end.wMinute - start.wMinute;
		hour = end.wHour - start.wHour;
		day = end.wDay - start.wDay;
		month = end.wMonth - start.wMonth;
		year = end.wYear - start.wYear;
		if (milliseconds < 0) {            //�����λ
			seconds--;
			milliseconds += 1000;
		}
		if (seconds < 0) {
			minutes--;
			seconds += 60;
		}
		if (minutes < 0) {
			hour--;
			minutes += 60;
		}
		if (hour < 0) {
			day--;
			hour += 24;
		}
		if (day < 0) {
			month--;
			day += 30;
		}
		if (month < 0) {
			year--;
			month += 12;
		}
		cout <<"����ʱ��"<< year << "��" << month << "��" << day << "��" << hour << "ʱ" << minutes << "��" << seconds << "��" << milliseconds << "����" << endl;
	}
}

//vim
void vim(char *in) {
	char a[4093];
	memset(a, 0, sizeof(a));
	strcpy(a, "�ı��༭��.exe");
	pro(a,in);
}

//������������ж��Ƿ��ض���
void command(char *in) {
	flag1 = 0;
	char command[50][4096];//����
	memset(command, 0, sizeof(command));
	int ln = strlen(in);
	int i = 0;
	int t = 0;
	int j = 0;
	while (i < ln) {//���ַ������տո����
		if (in[i] == ' ') {
			i++;
			j++;
			t = 0;
		}
		else {
			command[j][t] = in[i];
			t++;
			i++;
		}
	}
	j++;
	char parm[4096];//���ݸ�����Ĳ���
	memset(parm, 0, sizeof(parm));
	int m = 1;
	//�ж��Ƿ������ض���
	if (!strcmp(command[1], commandall[6])) {
		if (freopen(command[2], "r", stdin) == NULL)
			cout << "�������ļ�ʧ��(�ض���ʧ��)" << endl;
		else
			gets_s(parm);
		flag1 = 1;
	}
	else
		if (j == 1)
			memset(parm, 0, sizeof(parm));
		else
			strncpy(parm, in + strlen(command[0]) + 1, ln - strlen(command[0]) + 1);
	//�ж��Ƿ�����ض���
	while (m < j) {
		if (!strcmp(command[m], commandall[5])) {
			//����ض���ʧ�ܣ�����Ŀ���ļ�
			if (freopen(command[m + 1], "w", stdout) == NULL) {
				HANDLE file = CreateFile(command[m + 1],
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ,
					NULL,
					OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					NULL);
				freopen(command[m + 1], "w", stdout);
			}
			if (flag1)
				strncpy(parm, in + strlen(command[0]) + 1, ln - strlen(command[0]) +
					3 + strlen(command[m]) + strlen(command[m + 1]));
			break;
		}
		else if (!strcmp(command[m], commandall[7])) {
			if (freopen(command[m + 1], "a", stdout) == NULL) {
				HANDLE file = CreateFile(command[m + 1],
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ,
					NULL,
					OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					NULL);
				freopen(command[m + 1], "a", stdout);
			}
			if (flag1)
				strncpy(parm, in + strlen(command[0]) + 1, ln - strlen(command[0]) +
					3 + strlen(command[m]) + strlen(command[m + 1]));
			break;
		}
		m++;
	}
	//�ж������ʼ����
	if (!strcmp(command[0], commandall[0]))//����ʱ��
		time(parm);
	else if (!strcmp(command[0], commandall[1]))//�����ļ�
		cp(parm);
	else if (!strcmp(command[0], commandall[4]))//�˳�
		exit(0);
	else if (!strcmp(command[0], commandall[8]))//�鿴�ļ�����
		cat(parm);
	else if (!strcmp(command[0], commandall[9]))//�������������������ַ���
		wc(parm);
	else if (!strcmp(command[0], commandall[2]))//�����ļ���
		cd(parm);
	else if (!strcmp(command[0], "dirwalk"))//;��ʾĿ¼��Ϣ
		DirWalk(parm, true);
	else if (!strcmp(command[0], "system"))//��ʾϵͳ��Ϣ
		GetSysInfo();
	else if (!strcmp(command[0], "windows")) //��ʾϵͳ�汾
		windows();
	else if (!strcmp(command[0], "back"))//�����ϼ�����Ŀ¼
		SetCurrentDirectory(Stringsub());
	else if (!strcmp(command[0], "root"))//���ظ�Ŀ¼
		SetCurrentDirectory(WindowsPath());
	else if (!strcmp(command[0], "cpu"))//�鿴cpuռ����
		cpul();
	else if (!strcmp(command[0], "memory")) {//�鿴�ڴ�ռ����
		cout << "��ǰ�ڴ�ʹ����";
		memorylook();
	}
	else if (!strcmp(command[0], "search")) //�����ļ�
		search(parm, "");
	else if (!strcmp(command[0], "ls"))//�鿴Ŀ¼���ļ�
		ls(parm);
	else if (!strcmp(command[0], "mkdir"))//����Ŀ¼
		mkdir(parm);
	else if (!strcmp(command[0], "rmdir"))//ɾ��Ŀ¼
		rmdir(parm);
	else if (!strcmp(command[0], "mv"))//�ƶ������������ļ�
		mv(parm);
	else if (!strcmp(command[0], "rm"))//ɾ���ļ�
		rm(parm);
	else if (!strcmp(command[0], "gedit"))//ʹ�ü��±��༭�ļ�
		gedit(parm);
	else if (!strcmp(command[0], "vim"))//ʹ�ü��±��༭�ļ�
		vim(parm);
	else if (!strcmp(command[0], "du"))//����Ŀ¼��С
		du(parm);
	//���������Ƿ�Ϊ��ִ���ļ�
	else
		pro(command[0], parm);
	//�ض���cmd
	freopen("CON", "w", stdout);
	freopen("CON", "r", stdin);
}

//�������������յ����ַ����ֿ��ж��Ƿ�ʹ�ùܵ�
int main() {
	//���յ��ַ���
	//���ӹܵ�
	OpenNamedPipeInClient();
	char in[4096];
	while (1) {
		flag2 = 0;
		flag1 = 0;
		//���ڱ��浱ǰ·��
		char buffer[MAX_PATH];		
		//����������ɫ
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
		_getcwd(buffer, 4096);
		cout << buffer << ":";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
		memset(in, 0, sizeof(in));
		gets_s(in);
		int ln = strlen(in);
		int i = 0;
		int t = 0;
		int j = 0;
		while (i < ln) {//���ַ������տո����
						//��Ҫ�ܵ�
			if (in[i] == '|') {
				flag2 = 1;
			}
			i++;
		}
		//����Ҫ�ܵ�
		if (!flag2)
			command(in);
		//��Ҫ�ܵ��������������ִ��
		else {
			char parm[4096];
			memset(parm, 0, sizeof(parm));
			char *str;
			i = 0;
			j = 0;
			while (i < ln) {//��ÿ���������
				if (in[i] == '|') {
					strncpy(parm, in + j, i - j - 1);
					if (j != 0) {
						str = NamedPipeReadInClient();
						strcat(parm, str);
					}
					command(parm);
					j = i + 2;
				}
				else if (i == (ln - 1)) {
					strncpy(parm, in + j, i - j + 1);
					str = NamedPipeReadInClient();
					strcat(parm, " ");
					strcat(parm, str);
					command(parm);
				}
				i++;
			}
		}
	}
	ClosePipe();
}