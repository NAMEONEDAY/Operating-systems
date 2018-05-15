//2018.4----2018.5
//ģ��cmd 

#include <windows.h>
#include <windowsx.h>

#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>		//Ϊ�˰Ѹ�ʽ����д�ɴ�������ָ� 
#include <string.h>
#include <direct.h>
//#include <unistd.h>
#include <time.h> 
//#include <shlwapi.h>	//���� PathFileExists()���� 

#define MAXPATH  1024
#define chDIMOF( Array )( sizeof(Array)/sizeof(Array[0]) )
#define DIV 1024    
#define WIDTH 7    
#define GB(x) (((x).HighPart << 2) + ((DWORD)(x).LowPart) / 1024.0 / 1024.0 / 1024.0)    

#pragma warning(disable: 4996)

typedef unsigned char U_CHAR;

//�ж��Ƿ���һ��Ŀ¼
static BOOL IsChildDir(WIN32_FIND_DATA * lpFindData) {
	return(
		((lpFindData->dwFileAttributes &
			FILE_ATTRIBUTE_DIRECTORY) != 0) &&
			(lstrcmp(lpFindData->cFileName, _TEXT(".")) != 0) &&
		(lstrcmp(lpFindData->cFileName, _TEXT("..")) != 0));
}

//������һ��Ŀ¼
static BOOL FindNextChildDir(HANDLE hFindFile,
	WIN32_FIND_DATA * lpFindData) {
	BOOL fFound = FALSE;

	do {
		fFound = FindNextFile(hFindFile, lpFindData);
	} while (fFound && !IsChildDir(lpFindData));

	return(fFound);
}

//�ҵ���һ����Ŀ¼
static HANDLE FindFirstChildDir(LPTSTR szPath,
	WIN32_FIND_DATA * lpFindData) {

	BOOL fFound;
	HANDLE hFindFile = FindFirstFile(szPath, lpFindData);

	if (hFindFile != INVALID_HANDLE_VALUE) {
		fFound = IsChildDir(lpFindData);

		if (!fFound) {
			fFound = FindNextChildDir(hFindFile, lpFindData);
		}

		if (!fFound) {
			FindClose(hFindFile);
			hFindFile = INVALID_HANDLE_VALUE;
		}
	}
	return(hFindFile);
}
/*Ϊ�˼��ٶ�ջ��ʹ�ã�
DIRWALKDATA�ṹ��һ��ʵ��������ΪDirWalk�е�һ���ֲ�������
����ָ������ָ�뱻���ݸ���DirWalk�ݹ顣*/

//DirWalkRecurse��ʹ�õ�����
typedef struct {
	int dDepth;				//Ƕ�����
	BOOL fRecurse;			//���õ�ʵ���б����Ŀ¼
	TCHAR szBuf[1000];		//��ʽ�����������
	int nIndent;			//�����ַ���
	BOOL fOk;				//ѭ�����Ʊ�־
	BOOL fIsDir;			//ѭ�����Ʊ�־
	WIN32_FIND_DATA FindData;//�ļ���Ϣ
} DIRWALKDATA, *LPDIRWALKDATA;
/*����Ŀ¼�ṹ�����б��������ļ�����
���������pDW->fRecurse����ͨ���ݹ�ص���DirwalkRecurse���г�������Ŀ¼��
*/

static void DirWalkRecurse(LPDIRWALKDATA pDW) {
	HANDLE hFind;

	pDW->dDepth++;
	pDW->nIndent = 3 * pDW->dDepth;
	_stprintf(pDW->szBuf, _TEXT("%*s"), pDW->nIndent, _TEXT(""));

	GetCurrentDirectory(chDIMOF(pDW->szBuf) - pDW->nIndent,
		&pDW->szBuf[pDW->nIndent]);

	printf("%s\n", pDW->szBuf);

	hFind = FindFirstFile(_TEXT("*.*"), &pDW->FindData);
	pDW->fOk = (hFind != INVALID_HANDLE_VALUE);

	while (pDW->fOk) {
		pDW->fIsDir = pDW->FindData.dwFileAttributes &
			FILE_ATTRIBUTE_DIRECTORY;
		if (!pDW->fIsDir ||
			(!pDW->fRecurse && IsChildDir(&pDW->FindData))) {

			_stprintf(pDW->szBuf,
				pDW->fIsDir ? _TEXT("%*s[%s]") : _TEXT("%*s%s"),
				pDW->nIndent, _TEXT(""),
				pDW->FindData.cFileName);

			printf("%s\n", pDW->szBuf);
		}
		pDW->fOk = FindNextFile(hFind, &pDW->FindData);
	}

	if (hFind != INVALID_HANDLE_VALUE)
		FindClose(hFind);

	if (pDW->fRecurse) {
		//��õ�һ����Ŀ¼
		hFind = FindFirstChildDir(_TEXT("*.*"), &pDW->FindData);
		pDW->fOk = (hFind != INVALID_HANDLE_VALUE);
		while (pDW->fOk) {
			//�任����Ŀ¼
			if (SetCurrentDirectory(pDW->FindData.cFileName)) {
				/*ִ�еݹ�·������Ŀ¼��
				��ʱһЩpDW��Ա�ᱻ������ø��ǡ�*/
				DirWalkRecurse(pDW);

				//�ص���Ŀ¼�ĸ�Ŀ¼ 
				SetCurrentDirectory(_TEXT(".."));
			}
			pDW->fOk = FindNextChildDir(hFind, &pDW->FindData);
		}

		if (hFind != INVALID_HANDLE_VALUE) {
			FindClose(hFind);
		}
	}
	pDW->dDepth--;
}

/*����Ŀ¼�ṹ���ú��������˶����ڹ�����DirWalkRecurse�ĵ���*/
void DirWalk(
	LPCTSTR pszRootPath,		//�������
	BOOL fRecurse) {			//��չ��Ŀ¼

	TCHAR szCurrDir[_MAX_DIR];
	DIRWALKDATA DW;

	//���浱ǰĿ¼��������֮���ܱ��ָ�
	GetCurrentDirectory(chDIMOF(szCurrDir), szCurrDir);

	//����ǰĿ¼����ΪҪ��ʼ�ĵط�
	SetCurrentDirectory(pszRootPath);

	/*nDepth���ڿ�����������ֵ����-1�����µ�һ����ʾ����� */
	DW.dDepth = -1;
	DW.fRecurse = fRecurse;

	//���õݹ麯����������Ŀ¼
	DirWalkRecurse(&DW);

	//����ǰĿ¼�ָ������ú���֮ǰ��λ��
	SetCurrentDirectory(szCurrDir);
}

//�������������ַ�������������������Ϊһ���µ��ַ���
char* StringJoin(const char* first, const char* last)
{
	char* result;
	int len = strlen(first) + strlen(last) + 1;
	result = (char*)malloc(len * sizeof(char));
	memset(result, 0, len * sizeof(char));
	strcpy(result, first);
	strcat(result, last);
	return result;
}

//��StringJoin���������ַ���ĩβ���'\'������Ŀ¼·��
char* add_slash(char* string)
{
	int len = strlen(string);
	//�鿴���һ���ַ��Ƿ�Ϊ'\'
	if (string[len - 1] != '\\')
		return StringJoin(string, "\\");
	else
		return string;
}

//��ȡWindowsĿ¼
char* WindowsPath()
{
	char *Buffer;
	DWORD dwRet;
	//��ȡWindowsĿ¼
	//��ȡWindowsĿ¼�ַ�����С������ĩβ��'\0'
	dwRet = GetWindowsDirectoryA(NULL, 0);
	if (dwRet == 0)
	{
		printf("GetCurrentDirectory failed (%d)\n", GetLastError());
		return NULL;
	}
	/*���ݻ�ȡ�Ŀռ䣬�����ڴ棬������*/
	Buffer = (char *)malloc(sizeof(char)*dwRet);
	memset(Buffer, 0, dwRet * sizeof(char));
	/*��ȡ��ʱĿ¼*/
	GetWindowsDirectoryA(Buffer, dwRet);
	return add_slash(Buffer);
}

//��ȥ����Ŀ¼ ����Stringsub()�������ʹ�� 
char* sub_slash(char* string) {
	int i;
	int len = strlen(string);
	//�鿴���һ���ַ��Ƿ�Ϊ'\'
	for (i = len - 1; ; i--) {
		if (string[i] == '\\') {
			string[i] = '\0';
			break;
		}
	}
	return string;
}

//��õ�ǰĿ¼����һ��Ŀ¼ 
char* Stringsub() {
	char *Buffer;
	DWORD dwRet;
	/*��ȡ��ǰĿ¼*/
	/*��ȡ��ǰĿ¼�ַ�����С������ĩβ��'\0'*/
	dwRet = GetCurrentDirectoryA(0, NULL);
	if (dwRet == 0)
	{
		printf("GetCurrentDirectory failed (%d)\n", GetLastError());
		return NULL;
	}
	/*���ݻ�ȡ�Ŀռ䣬�����ڴ棬������*/
	Buffer = (char *)malloc(sizeof(char)*dwRet);
	memset(Buffer, 0, dwRet * sizeof(char));
	/*��ȡ��ʱĿ¼*/
	GetCurrentDirectoryA(dwRet, Buffer);
	return sub_slash(Buffer);
}

//���ϵͳ��Ϣ 
void GetSysInfo() {
	char szComputerName[MAXBYTE] = { 0 };
	char szUserName[MAXBYTE] = { 0 };
	unsigned long nSize = MAXBYTE;
	OSVERSIONINFO OsVer;

	OsVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&OsVer);

	if (OsVer.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		if (OsVer.dwMajorVersion == 5) {
			switch (OsVer.dwMinorVersion) {
			case 0:	printf("Windows 2000\r\n"); break;
			case 1:	printf("Windows XP %s \r\n", OsVer.szCSDVersion); break;
			case 2: printf("Windows 2003 or Windows 2003 R2\r\n"); break;
			default: printf("Other System \r\n");
			}
		}
		else if (OsVer.dwMajorVersion == 6) {
			switch (OsVer.dwMinorVersion) {
			case 0:	printf("Windows Vista\r\n"); break;
			case 1: printf("Windows 7 \r\n"); break;
			case 2: printf("Windows 8 \r\n"); break;
			case 3: printf("Windows 8.1 \r\n"); break;
			default:printf("Other System \r\n");
			}
		}
		else if (OsVer.dwMajorVersion == 10) {
			switch (OsVer.dwMinorVersion) {
			case 0:	printf("Windows 10 \r\n"); break;
			default: printf("Other System \r\n");
			}
		}
		else {
			printf("Sorry. Unknown System! \r\n");
		}
	}

	GetComputerName(szComputerName, &nSize);
	printf("Computer Name is %s \r\n", szComputerName);

	nSize = MAXBYTE;
	GetUserName(szUserName, &nSize);
	printf("User Name is %s \r\n", szUserName);
}

//��ȡ����ϵͳ������    
void GetSystemName(char* osname)
{
	char buf[128];
	SYSTEM_INFO info;        //��SYSTEM_INFO�ṹ�ж�64λAMD������     
	GetSystemInfo(&info);    //����GetSystemInfo�������ṹ     
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	strcpy(buf, "unknown OperatingSystem.");

	if (GetVersionEx((OSVERSIONINFO *)&os))
	{
		//������ݰ汾��Ϣ�жϲ���ϵͳ����     
		switch (os.dwMajorVersion)//�ж����汾��    
		{
		case 4:
			switch (os.dwMinorVersion)//�жϴΰ汾��     
			{
			case 0:
				if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
					strcpy(buf, "Microsoft Windows NT 4.0"); //1996��7�·���     
				else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
					strcpy(buf, "Microsoft Windows 95");
				break;
			case 10:
				strcpy(buf, "Microsoft Windows 98");
				break;
			case 90:
				strcpy(buf, "Microsoft Windows Me");
				break;
			}
			break;

		case 5:
			switch (os.dwMinorVersion)  //�ٱȽ�dwMinorVersion��ֵ    
			{
			case 0:
				strcpy(buf, "Microsoft Windows 2000"); //1999��12�·���    
				break;

			case 1:
				strcpy(buf, "Microsoft Windows XP"); //2001��8�·���    
				break;

			case 2:
				if (os.wProductType == VER_NT_WORKSTATION
					&& info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				{
					strcpy(buf, "Microsoft Windows XP Professional x64 Edition");
				}
				else if (GetSystemMetrics(SM_SERVERR2) == 0)
					strcpy(buf, "Microsoft Windows Server 2003"); //2003��3�·���     
				else if (GetSystemMetrics(SM_SERVERR2) != 0)
					strcpy(buf, "Microsoft Windows Server 2003 R2");
				break;
			}
			break;

		case 6:
			switch (os.dwMinorVersion)
			{
			case 0:
				if (os.wProductType == VER_NT_WORKSTATION)
					strcpy(buf, "Microsoft Windows Vista");
				else
					strcpy(buf, "Microsoft Windows Server 2008"); //�������汾     
				break;
			case 1:
				if (os.wProductType == VER_NT_WORKSTATION)
					strcpy(buf, "Microsoft Windows 7");
				else
					strcpy(buf, "Microsoft Windows Server 2008 R2");
				break;
			}
			break;
		}
	}//if(GetVersionEx((OSVERSIONINFO *)&os))    

	strcpy(osname, buf);
}


//CPU������    
FILETIME m_preidleTime;
FILETIME m_prekernelTime;
FILETIME m_preuserTime;

__int64 CompareFileTime2(FILETIME time1, FILETIME time2)
{
	__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime;
	__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime;


	return   b - a;
}

void GetCpuUseage()
{
	GetSystemTimes(&m_preidleTime, &m_prekernelTime, &m_preuserTime);
	Sleep(1000);
}

double CpuUseage()
{
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;
	GetSystemTimes(&idleTime, &kernelTime, &userTime);


	int idle = CompareFileTime2(m_preidleTime, idleTime);
	int kernel = CompareFileTime2(m_prekernelTime, kernelTime);
	int user = CompareFileTime2(m_preuserTime, userTime);


	if (kernel + user == 0)
		return 0.0;
	//���ܵ�ʱ��-����ʱ�䣩/�ܵ�ʱ��=ռ��cpu��ʱ�����ʹ����    
	double cpu = abs((kernel + user - idle) * 100 / (kernel + user));

	m_preidleTime = idleTime;
	m_prekernelTime = kernelTime;
	m_preuserTime = userTime;
	return cpu;
}

//int main() {
//	char cmd[20];		//����ָ�� 
//	char* ch[50];		//����·��
//	char start[20];	//�����ʼ·�� 
//	char buffer[MAX_PATH];		//���ڱ��浱ǰ·��
//	char* path[50];		//���������Ŀ¼ 
//	char osname[30];	//ϵͳ�汾���� 
//	char p;
//	char temp;
//	U_CHAR  sysStateCpu[5];	//cpu
//	double cpu;
//
//	STARTUPINFO si = { sizeof(si) };
//	SECURITY_ATTRIBUTES saProcess, saThread;
//	PROCESS_INFORMATION piProcessB;
//
//	while (TRUE) {
//		printf("%��������Ҫִ�е�����:\n");
//		//gets(cmd);
//		scanf("%s", cmd);
//
//		if (strcmp(cmd, "exit") == 0)
//			break;
//
//		if (strcmp(cmd, "-ls") == 0) {	//��ʾ��ǰ����Ŀ¼ 
//			_getcwd(buffer, MAXPATH);
//			printf("��ǰ����Ŀ¼ %s\n", buffer);
//		}
//
//		//�ָ�ϵͳĿ¼ mycd.. 
//		if (strcmp(cmd, "mycd..") == 0) {
//			SetCurrentDirectory(WindowsPath());
//			_getcwd(buffer, MAXPATH);
//			printf("��ǰ����Ŀ¼ %s\n", buffer);
//		}
//
//		//��������Ŀ¼ mycd 
//		if (strcmp(cmd, "mycd") == 0) {
//			scanf("%s", ch);
//
//			SetCurrentDirectory((char*)ch);
//			_getcwd(buffer, MAXPATH);
//			printf("��ǰ����Ŀ¼ %s\n", buffer);
//		}
//
//		//������һ������Ŀ¼ back 
//		if (strcmp(cmd, "back") == 0) {
//			SetCurrentDirectory(Stringsub());
//			_getcwd(buffer, MAXPATH);
//			printf("��ǰ����Ŀ¼ %s\n", buffer);
//		}
//
//
//		//����Ŀ���ļ��� 
//		if (strcmp(cmd, "dirwalk") == 0) {
//			scanf("%s", ch);
//			//�жϵ�ַ����δ����	
//
//			//Ŀ¼��ѯ	
//			DirWalk((char*)ch, TRUE);
//		}
//
//		//��ѯϵͳ��Ϣ 
//		if (strcmp(cmd, "system") == 0) {
//			GetSysInfo();
//		}
//
//		//��ѯϵͳ�汾 
//		if (strcmp(cmd, "windows") == 0) {
//			GetSystemName(osname);
//			printf("%s\n", osname);
//		}
//
//		//��ѯCPU��������
//		if (strcmp(cmd, "cpu") == 0) {
//			U_CHAR  sysStateCpu[5];
//			cpu = CpuUseage();
//			sprintf((char*)sysStateCpu, "%.2lf", cpu);
//			printf("CPUʹ���ʣ�%s%%\n", sysStateCpu);
//		}
//
//		//�鿴�ڴ�ʹ���� 
//		if (strcmp(cmd, "memory") == 0) {
//			char bufPreCPU[5];
//			MEMORYSTATUSEX statex;
//			statex.dwLength = sizeof(statex);
//			GlobalMemoryStatusEx(&statex);
//			sprintf(bufPreCPU, "%ld%%\n", statex.dwMemoryLoad);
//			puts(bufPreCPU);
//			//Ӳ��ʹ���� ����windows API    
//			ULARGE_INTEGER FreeBytesAvailableC, TotalNumberOfBytesC, TotalNumberOfFreeBytesC;
//			ULARGE_INTEGER FreeBytesAvailableD, TotalNumberOfBytesD, TotalNumberOfFreeBytesD;
//			ULARGE_INTEGER FreeBytesAvailableE, TotalNumberOfBytesE, TotalNumberOfFreeBytesE;
//			ULARGE_INTEGER FreeBytesAvailableF, TotalNumberOfBytesF, TotalNumberOfFreeBytesF;
//
//			GetDiskFreeSpaceEx(_T("C:"), &FreeBytesAvailableC, &TotalNumberOfBytesC, &TotalNumberOfFreeBytesC);
//			GetDiskFreeSpaceEx(_T("D:"), &FreeBytesAvailableD, &TotalNumberOfBytesD, &TotalNumberOfFreeBytesD);
//			GetDiskFreeSpaceEx(_T("E:"), &FreeBytesAvailableE, &TotalNumberOfBytesE, &TotalNumberOfFreeBytesE);
//			GetDiskFreeSpaceEx(_T("F:"), &FreeBytesAvailableF, &TotalNumberOfBytesF, &TotalNumberOfFreeBytesF);
//			/*���� ���ͼ�˵��
//			lpRootPathName String�������������Ĵ��̸�·����
//			lpFreeBytesAvailableToCaller LARGE_INTEGER��ָ��һ���������������ɵ����߿��õ��ֽ�����
//			lpTotalNumberOfBytes LARGE_INTEGER��ָ��һ���������������ɴ����ϵ����ֽ���
//			lpTotalNumberOfFreeBytes LARGE_INTEGER��ָ��һ���������������ɴ����Ͽ��õ��ֽ���
//			����ƽ̨  Windows 95 OSR2��Windows NT 4.0*/
//			float totalHardDisk = GB(TotalNumberOfBytesC) + GB(TotalNumberOfBytesD) + GB(TotalNumberOfBytesE) + GB(TotalNumberOfBytesF);
//			float freeHardDisk = GB(TotalNumberOfFreeBytesC) + GB(TotalNumberOfFreeBytesD) + GB(TotalNumberOfFreeBytesE) + GB(TotalNumberOfFreeBytesF);
//
//			float hardDiskUsage = 1 - freeHardDisk / totalHardDisk;
//		}
//
//	}
//
//	return 0;
//}
void cpul() {
	U_CHAR  sysStateCpu[5];
	double cpu;
	cpu = CpuUseage();
	sprintf((char*)sysStateCpu, "%.2lf", cpu);
	printf("CPUʹ���ʣ�%s%%\n", sysStateCpu);
}
void memorylook() {
	char bufPreCPU[5];
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	sprintf(bufPreCPU, "%ld%%\n", statex.dwMemoryLoad);
	puts(bufPreCPU);
	//Ӳ��ʹ���� ����windows API    
	ULARGE_INTEGER FreeBytesAvailableC, TotalNumberOfBytesC, TotalNumberOfFreeBytesC;
	ULARGE_INTEGER FreeBytesAvailableD, TotalNumberOfBytesD, TotalNumberOfFreeBytesD;
	ULARGE_INTEGER FreeBytesAvailableE, TotalNumberOfBytesE, TotalNumberOfFreeBytesE;
	ULARGE_INTEGER FreeBytesAvailableF, TotalNumberOfBytesF, TotalNumberOfFreeBytesF;

	GetDiskFreeSpaceEx(_T("C:"), &FreeBytesAvailableC, &TotalNumberOfBytesC, &TotalNumberOfFreeBytesC);
	GetDiskFreeSpaceEx(_T("D:"), &FreeBytesAvailableD, &TotalNumberOfBytesD, &TotalNumberOfFreeBytesD);
	GetDiskFreeSpaceEx(_T("E:"), &FreeBytesAvailableE, &TotalNumberOfBytesE, &TotalNumberOfFreeBytesE);
	GetDiskFreeSpaceEx(_T("F:"), &FreeBytesAvailableF, &TotalNumberOfBytesF, &TotalNumberOfFreeBytesF);
	/*���� ���ͼ�˵��
	lpRootPathName String�������������Ĵ��̸�·����
	lpFreeBytesAvailableToCaller LARGE_INTEGER��ָ��һ���������������ɵ����߿��õ��ֽ�����
	lpTotalNumberOfBytes LARGE_INTEGER��ָ��һ���������������ɴ����ϵ����ֽ���
	lpTotalNumberOfFreeBytes LARGE_INTEGER��ָ��һ���������������ɴ����Ͽ��õ��ֽ���
	����ƽ̨  Windows 95 OSR2��Windows NT 4.0*/
	float totalHardDisk = GB(TotalNumberOfBytesC) + GB(TotalNumberOfBytesD) + GB(TotalNumberOfBytesE) + GB(TotalNumberOfBytesF);
	float freeHardDisk = GB(TotalNumberOfFreeBytesC) + GB(TotalNumberOfFreeBytesD) + GB(TotalNumberOfFreeBytesE) + GB(TotalNumberOfFreeBytesF);

	float hardDiskUsage = 1 - freeHardDisk / totalHardDisk;
}

void windows() {
	char osname[30];	//ϵͳ�汾���� 
	GetSystemName(osname);
	printf("%s\n", osname);
}