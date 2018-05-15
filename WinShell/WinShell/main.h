#pragma once
#include <windows.h>
#include <windowsx.h>

#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>		//为了把格式数据写成串（函数指令） 
#include <string.h>
#include <direct.h>
#include <time.h> 

//NamedPipeClient.c中的函数
//服务端用来保存创建的命名管道句柄
HANDLE            hNamedPipe;
//创建命名管道
void CreateNamedPipeInServer();

//从命名管道中读取数据
char *NamedPipeReadInServer();

//往命名管道中写入数据
void NamedPipeWriteInServer(char *pStr);

//打开命名管道
void OpenNamedPipeInClient();

//客户端从命名管道中读取数据
char *NamedPipeReadInClient();

//客户端往命名管道中写入数据
void NamedPipeWriteInClient(char *pStr);

void ClosePipe();

//lzq.c中的函数
void DirWalk(LPCTSTR pszRootPath, BOOL fRecurse);

void GetSysInfo();

void GetSystemName();

char* WindowsPath();

void memorylook();

void windows();

void cpul();
char* Stringsub();