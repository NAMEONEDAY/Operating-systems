#pragma once
#include <windows.h>
#include <windowsx.h>

#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>		//Ϊ�˰Ѹ�ʽ����д�ɴ�������ָ� 
#include <string.h>
#include <direct.h>
#include <time.h> 

//NamedPipeClient.c�еĺ���
//������������洴���������ܵ����
HANDLE            hNamedPipe;
//���������ܵ�
void CreateNamedPipeInServer();

//�������ܵ��ж�ȡ����
char *NamedPipeReadInServer();

//�������ܵ���д������
void NamedPipeWriteInServer(char *pStr);

//�������ܵ�
void OpenNamedPipeInClient();

//�ͻ��˴������ܵ��ж�ȡ����
char *NamedPipeReadInClient();

//�ͻ����������ܵ���д������
void NamedPipeWriteInClient(char *pStr);

void ClosePipe();

//lzq.c�еĺ���
void DirWalk(LPCTSTR pszRootPath, BOOL fRecurse);

void GetSysInfo();

void GetSystemName();

char* WindowsPath();

void memorylook();

void windows();

void cpul();
char* Stringsub();