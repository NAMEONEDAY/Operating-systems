#pragma once
#ifndef NAMED_PIPE_CLIENT_H
#define NAMED_PIPE_CLIENT_H
 
#include <Windows.h>
#include <iostream>
 
using namespace std;
 
//���������ڿͻ���ͨ�� CreateFile �򿪵������ܵ����
HANDLE            hNamedPipe1;
 
const char * pPipeName1    = "\\\\.\\pipe\\ZacharyPipe";
 
//�������ܵ�
void OpenNamedPipeInClient();
 
//�ͻ��˴������ܵ��ж�ȡ����
char *NamedPipeReadInClient();
 
//�ͻ����������ܵ���д������
void NamedPipeWriteInClient(char *pStr);
 
#endif