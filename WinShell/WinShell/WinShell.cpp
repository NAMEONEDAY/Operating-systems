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

//各种命令集合
char commandall[30][4096] = { "time" ,"cp","cd","ls","quit",">","<",">>" ,"cat","wc","-help"};

//du函数
long getsizef(char *in) {
	LONG i=0;
	WIN32_FIND_DATA lp;
	char source[4096];
	lstrcpy(source, in);
	lstrcat(source, "\\*.*");
	HANDLE hfind = FindFirstFile(source, &lp);
	if (hfind != INVALID_HANDLE_VALUE) {
		while (FindNextFile(hfind, &lp) != 0) {
			//是16则表示是目录，递归调用getsizef进行计算
			if (lp.dwFileAttributes == 16) {
				if (lstrcmp(lp.cFileName, ".") != 0 && lstrcmp(lp.cFileName, "..") != 0) {
					memset(source, '0', sizeof(source));
					lstrcpy(source, in);
					lstrcat(source, "\\");
					lstrcat(source, lp.cFileName);
					//递归
					i += getsizef(source);
					//把source变回原目录
					lstrcpy(source, in);
					lstrcat(source, "\\");
				}
			}
			else {
				//如果是文件则直接计算大小并且相加
				memset(source, 0, sizeof(source));
				//获得长文件名
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
				//计算文件大小
				LONG size = lp2.nFileSizeLow - lp2.nFileSizeHigh;
				//相加
				i += size;
				//关闭句柄
				CloseHandle(hfind);
				CloseHandle(hsource);
				//把路径变为原路径
				lstrcpy(source, in);
				lstrcat(source, "\\");
			}
		}
	}
	//返回大小
	return i;
}

void du(char *in) {
	long size;
	size = getsizef(in);
	//根据大小判断目录是否存在或者是否为空
	if (!(size== 0))
		cout << "目录" << in << "大小为" << size << endl;
	else
		cout << "没有该目录或该目录为空" << endl;
}

//gedit函数
void gedit(char *in) {
	//为了防止没有文件先调用该函数，没有文件就会自动创建
	HANDLE hsource = CreateFile(in,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	//关闭句柄
	CloseHandle(hsource);
	char buffer[4096];
	memset(buffer, 0, sizeof(buffer));
	//调用记事本
	strcpy(buffer, "notepad.exe");
	strcat(buffer, " ");
	strcat(buffer, in);
	STARTUPINFO si;                           //主窗口特性
	PROCESS_INFORMATION pi;                   //进程信息
	memset(&si, 0, sizeof(STARTUPINFO));
	int year, month, day, hour, minutes, seconds, milliseconds;
	if (!CreateProcess(NULL, //传递给新进程的命令行字符串
		buffer,
		NULL, //安全性
		NULL, //安全性
		FALSE, //是否调用进程的打开句柄的副本
		0,
		NULL,
		NULL,
		&si,//主窗口外观
		&pi)//句柄
		)//创建进程
		cout << "打开记事本失败" << endl;
	else
		WaitForSingleObject(pi.hProcess, INFINITE);
}

//search函数
//dir表示搜索文件的目录
void search(char *in,char *dir) {
	//初始搜索当前目录
	char buffer2[4096];
	memset(buffer2, 0, sizeof(buffer2));
	strcpy(buffer2, dir);
	strcat(buffer2, in);
	HANDLE hFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA pNextInfo;
	hFile = FindFirstFile(buffer2, &pNextInfo);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		//cout << "没有这样的文件" << endl;
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
	//当前目录搜索完毕从根目录开始递归搜索
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
			//是16则表示是目录，递归调用mycp进行复制
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

//rm函数
void rm(char *in) {
	char parm[50][4096];
	memset(parm, 0, sizeof(parm));
	int ln = strlen(in);
	int i = 0;
	int t = 0;
	int j = 0;
	//将字符串按照空格隔开
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
		//返回0表示删除失败
		if (!DeleteFile(parm[i]))
			cout << "删除失败" << endl;
		i++;
	}
}

//mv函数
void mv(char *in) {
	char parm[50][4096];
	memset(parm, 0, sizeof(parm));
	int ln = strlen(in);
	int i = 0;
	int t = 0;
	int j = 0;
	//将字符串按照空格隔开
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
			cout << "没有" << parm[0] << "这个文件" << endl;
		else {
			if (FindFirstFile(parm[1], &lp) == INVALID_HANDLE_VALUE) {
				if (rename(parm[0], parm[1]))
					cout << "重命名失败" << endl;
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
				cout << "没有" << parm[i] << "这个文件" << endl;
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
		cout << "参数错误" << endl;
}

//mkdir函数
void mkdir(char *in) {
	char parm[50][4096];
	memset(parm, 0, sizeof(parm));
	int ln = strlen(in);
	int i = 0;
	int t = 0;
	int j = 0;
	//将字符串按照空格隔开
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
			cout << "创建失败" << endl;
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
			//是16则表示是目录，递归调用mycp进行复制
			if (lp.dwFileAttributes == 16) {
				if (lstrcmp(lp.cFileName, ".") != 0 && lstrcmp(lp.cFileName, "..") != 0) {
					memset(source, '0', sizeof(source));
					lstrcpy(source, in);
					lstrcat(source, "\\");
					lstrcat(source, lp.cFileName);
					DeleteD(source);
					//把source和target变回原目录
					lstrcpy(source, in);
					lstrcat(source, "\\");
				}
			}
			else {
				memset(source, 0, sizeof(source));
				lstrcpy(source, in);
				lstrcat(source, "\\");
				lstrcat(source, lp.cFileName);
				//调用复制函数开始复制
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

//rmdir函数
void rmdir(char *in) {
	char parm[50][4096];
	memset(parm, 0, sizeof(parm));
	int ln = strlen(in);
	int i = 0;
	int t = 0;
	int j = 0;
	//将字符串按照空格隔开
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

//ls函数
void ls(char *in) {
	SYSTEMTIME ti;
	FILETIME ftCreationTime, ftLastAccessTime, ftLastWriteTime;
	char wenjian[300][1000];
	memset(wenjian, 0, sizeof(wenjian));
	int timeall[300];
	memset(timeall, 0, sizeof(timeall));
	//用于保存当前路径
	char buffer[MAX_PATH];		
	_getcwd(buffer, 4096);
	char parm[6][40];
	memset(parm, 0, sizeof(parm));
	int ln = strlen(in);
	int i = 0;
	int t = 0;
	int j = 0;
	//参数类型设置，0为无参数，1为-l,2为-a,3为-l、-a、4位-s、5为-t
	int fl = 0;
	//将字符串按照空格隔开
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
	//设置目录失败返回0
	if (!SetCurrentDirectory(in))
		cout << "进入目录失败，请检查是否存在" << endl;
}

//wc函数
//读取行数
void getlineNum(char *in) {
	string str;
	int i = 0;
	ifstream OpenFile(in);
	if (OpenFile) {
		while (!OpenFile.eof()) {
			//getline一行且自动换行
			//用i记录总行数
			getline(OpenFile, str);
			i++;
		}
		cout << "行数：" << i << endl;
	}
	else
		cout << "没有" << in << "这个文件" << endl;
	OpenFile.close();
}

//读取单词数
void getwordNum(char *in) {
	char str;
	int i = 0;
	ifstream OpenFile(in);
	if (OpenFile) {
		while (!OpenFile.eof()) {
			//get函数读取字符，遇到空格换行等单词数+1
			OpenFile.get(str);
			if (str == ' ' || str == '.' || str == ',' || str == '?'||str=='\n')
				i++;
		}
		cout << "单词数：" << i << endl;
	}
	else
		cout << "没有" << in << "这个文件" << endl;
	OpenFile.close();
}

//读取字符数
void getbitNum(char *in) {
	char str;
	int i = 0;
	ifstream OpenFile(in);
	if (OpenFile) {
		while (!OpenFile.eof()) {
			OpenFile.get(str);
				i++;
		}
		cout << "字符数：" << i << endl;
	}
	else
		cout << "没有" << in << "这个文件" << endl;
	OpenFile.close();
}

void wc(char *in) {
	char parm[50][4096];
	memset(parm, 0, sizeof(parm));
	int ln = strlen(in);
	int i = 0;
	int t = 0;
	int j = 0;
	//将字符串按照空格隔开
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
				cout << "文件名" << parm[i] << endl;
			}
			else
				cout << "没有"<<parm[i]<<"或者该参数为目录" << endl;
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
				cout << "文件名" << parm[i] << endl;
			}
			else
				cout << "没有" << parm[i] << "或者该参数为目录" << endl;
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
				cout << "文件名" << parm[i] << endl;
			}
			else
				cout << "没有" << parm[i] << "或者该参数为目录" << endl;
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
				cout << "文件名" << parm[i] << endl;
			}
			else
				cout << "没有" << parm[i] << "或者该参数为目录" << endl;
			i++;
		}
	}
}

//cat函数
void cat(char *in){
	char parm[50][4096];
	memset(parm, 0, sizeof(parm));
	int ln = strlen(in);
	int i = 0;
	int t = 0;
	int j = 0;
	//将字符串按照空格隔开
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
	//编辑一个短文件
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
				//结束符号EOFY
				if (!strcmp(str, "EOFY"))
					break;
				else
					printf("\n%s", str);
			}
		}
	}
	//列出文件内容
	else {
		i = 0;
		while (i < j) {
			//重定向结束符>
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
					cout << "没有" << parm[i] << "这个文件" << endl;
				OpenFile.close();
			}
			i++;
		}
	}
}

//可执行文件
void pro(char *e, char *parm) {
	{
		STARTUPINFO si;                           //主窗口特性
		PROCESS_INFORMATION pi;                   //进程信息
		memset(&si, 0, sizeof(STARTUPINFO));
		strcat(e, " ");
		strcat(e, parm);
		if (!CreateProcess(NULL, //传递给新进程的命令行字符串
			e,
			NULL, //安全性
			NULL, //安全性
			FALSE, //是否调用进程的打开句柄的副本
			0,
			NULL,
			NULL,
			&si,//主窗口外观
			&pi)//句柄
			)//创建进程
			cout << "没有该命令也没有该可执行程序" << endl;
		else
			WaitForSingleObject(pi.hProcess, INFINITE);
	}
}

//复制命令，为实验五内容
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
	//读源文件内容到buffer
	ReadFile(hsource,
		BUFFER,
		size,
		&wordbit,
		NULL);
	//写buffer内容到目标文件
	WriteFile(htarget,
		BUFFER,
		size,
		&wordbit,
		NULL);
	//更改时间
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
			//是16则表示是目录，递归调用mycp进行复制
			if (lp.dwFileAttributes == 16) {
				if (lstrcmp(lp.cFileName, ".") != 0 && lstrcmp(lp.cFileName, "..") != 0) {
					memset(source, '0', sizeof(source));
					lstrcpy(source, fsource);
					lstrcat(source, "\\");
					lstrcat(source, lp.cFileName);
					lstrcat(target, lp.cFileName);
					CreateDirectory(target, NULL);
					mycp(source, target);
					//把source和target变回原目录
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
				//调用复制函数开始复制
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
	//获得源文件时间
	GetFileTime(hsource, &ct, &at, &wt);
	//修改目标文件时间
	SetFileTime(htarget, &ct, &at, &wt);
	CloseHandle(hsource);
	CloseHandle(htarget);
}

void cp(char *in) {
	//将传过来的参数分成两个目录名称
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
		cout << "查找源文件路径失败" << endl;
	else {
		if (FindFirstFile(parm[1], &lp) == INVALID_HANDLE_VALUE)
			CreateDirectory(parm[1], NULL);
		mycp(parm[0], parm[1]);
	}
}

//计算运行时间命令，为实验二内容
void time(char in[4096]) {
	char parm[4096];
	strcpy_s(parm, in);
	SYSTEMTIME start, end;                    //时间
	STARTUPINFO si;                           //主窗口特性
	PROCESS_INFORMATION pi;                   //进程信息
	memset(&si, 0, sizeof(STARTUPINFO));
	int year, month, day, hour, minutes, seconds, milliseconds;
	if (!CreateProcess(NULL, //传递给新进程的命令行字符串
		parm,
		NULL, //安全性
		NULL, //安全性
		FALSE, //是否调用进程的打开句柄的副本
		0,
		NULL,
		NULL,
		&si,//主窗口外观
		&pi)//句柄
		)//创建进程
		cout << "创建进程失败,请检查输入参数是否正确" << endl;
	else {
		GetSystemTime(&start);
		//等待进程结束
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetSystemTime(&end);
		//计算时间
		milliseconds = end.wMilliseconds - start.wMilliseconds;
		seconds = end.wSecond - start.wSecond;
		minutes = end.wMinute - start.wMinute;
		hour = end.wHour - start.wHour;
		day = end.wDay - start.wDay;
		month = end.wMonth - start.wMonth;
		year = end.wYear - start.wYear;
		if (milliseconds < 0) {            //处理借位
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
		cout <<"运行时间"<< year << "年" << month << "月" << day << "日" << hour << "时" << minutes << "分" << seconds << "秒" << milliseconds << "毫秒" << endl;
	}
}

//vim
void vim(char *in) {
	char a[4093];
	memset(a, 0, sizeof(a));
	strcpy(a, "文本编辑器.exe");
	pro(a,in);
}

//调用命令函数，判断是否重定向
void command(char *in) {
	flag1 = 0;
	char command[50][4096];//命令
	memset(command, 0, sizeof(command));
	int ln = strlen(in);
	int i = 0;
	int t = 0;
	int j = 0;
	while (i < ln) {//将字符串按照空格隔开
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
	char parm[4096];//传递给命令的参数
	memset(parm, 0, sizeof(parm));
	int m = 1;
	//判断是否输入重定向
	if (!strcmp(command[1], commandall[6])) {
		if (freopen(command[2], "r", stdin) == NULL)
			cout << "打开输入文件失败(重定向失败)" << endl;
		else
			gets_s(parm);
		flag1 = 1;
	}
	else
		if (j == 1)
			memset(parm, 0, sizeof(parm));
		else
			strncpy(parm, in + strlen(command[0]) + 1, ln - strlen(command[0]) + 1);
	//判断是否输出重定向
	while (m < j) {
		if (!strcmp(command[m], commandall[5])) {
			//输出重定向失败，创建目标文件
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
	//判断命令并开始调用
	if (!strcmp(command[0], commandall[0]))//运行时间
		time(parm);
	else if (!strcmp(command[0], commandall[1]))//复制文件
		cp(parm);
	else if (!strcmp(command[0], commandall[4]))//退出
		exit(0);
	else if (!strcmp(command[0], commandall[8]))//查看文件内容
		cat(parm);
	else if (!strcmp(command[0], commandall[9]))//计算行数，单词数，字符数
		wc(parm);
	else if (!strcmp(command[0], commandall[2]))//进入文件夹
		cd(parm);
	else if (!strcmp(command[0], "dirwalk"))//;显示目录信息
		DirWalk(parm, true);
	else if (!strcmp(command[0], "system"))//显示系统信息
		GetSysInfo();
	else if (!strcmp(command[0], "windows")) //显示系统版本
		windows();
	else if (!strcmp(command[0], "back"))//返回上级工作目录
		SetCurrentDirectory(Stringsub());
	else if (!strcmp(command[0], "root"))//返回根目录
		SetCurrentDirectory(WindowsPath());
	else if (!strcmp(command[0], "cpu"))//查看cpu占用率
		cpul();
	else if (!strcmp(command[0], "memory")) {//查看内存占用率
		cout << "当前内存使用率";
		memorylook();
	}
	else if (!strcmp(command[0], "search")) //查找文件
		search(parm, "");
	else if (!strcmp(command[0], "ls"))//查看目录下文件
		ls(parm);
	else if (!strcmp(command[0], "mkdir"))//创建目录
		mkdir(parm);
	else if (!strcmp(command[0], "rmdir"))//删除目录
		rmdir(parm);
	else if (!strcmp(command[0], "mv"))//移动或者重命名文件
		mv(parm);
	else if (!strcmp(command[0], "rm"))//删除文件
		rm(parm);
	else if (!strcmp(command[0], "gedit"))//使用记事本编辑文件
		gedit(parm);
	else if (!strcmp(command[0], "vim"))//使用记事本编辑文件
		vim(parm);
	else if (!strcmp(command[0], "du"))//计算目录大小
		du(parm);
	//非命令检查是否为可执行文件
	else
		pro(command[0], parm);
	//重定向到cmd
	freopen("CON", "w", stdout);
	freopen("CON", "r", stdin);
}

//主函数，将接收到的字符串分开判断是否使用管道
int main() {
	//接收的字符串
	//连接管道
	OpenNamedPipeInClient();
	char in[4096];
	while (1) {
		flag2 = 0;
		flag1 = 0;
		//用于保存当前路径
		char buffer[MAX_PATH];		
		//设置字体颜色
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
		while (i < ln) {//将字符串按照空格隔开
						//需要管道
			if (in[i] == '|') {
				flag2 = 1;
			}
			i++;
		}
		//不需要管道
		if (!flag2)
			command(in);
		//需要管道将命令隔开不断执行
		else {
			char parm[4096];
			memset(parm, 0, sizeof(parm));
			char *str;
			i = 0;
			j = 0;
			while (i < ln) {//将每个命令隔开
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