#pragma once
#ifndef NAMED_PIPE_CLIENT_H
#define NAMED_PIPE_CLIENT_H
 
#include <Windows.h>
#include <iostream>
 
using namespace std;
 
//用来保存在客户端通过 CreateFile 打开的命名管道句柄
HANDLE            hNamedPipe1;
 
const char * pPipeName1    = "\\\\.\\pipe\\ZacharyPipe";
 
//打开命名管道
void OpenNamedPipeInClient();
 
//客户端从命名管道中读取数据
char *NamedPipeReadInClient();
 
//客户端往命名管道中写入数据
void NamedPipeWriteInClient(char *pStr);
 
#endif