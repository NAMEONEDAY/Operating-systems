//2018.4----2018.5
//模拟cmd 

#include <windows.h>
#include <windowsx.h>

#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>		//为了把格式数据写成串（函数指令） 
#include <string.h>
#include <direct.h>
//#include <unistd.h>
#include <time.h> 
//#include <shlwapi.h>	//调用 PathFileExists()函数 

#define MAXPATH  1024
#define chDIMOF( Array )( sizeof(Array)/sizeof(Array[0]) )
#define DIV 1024    
#define WIDTH 7    
#define GB(x) (((x).HighPart << 2) + ((DWORD)(x).LowPart) / 1024.0 / 1024.0 / 1024.0)    

#pragma warning(disable: 4996)

typedef unsigned char U_CHAR;

//判断是否是一个目录
static BOOL IsChildDir(WIN32_FIND_DATA * lpFindData) {
	return(
		((lpFindData->dwFileAttributes &
			FILE_ATTRIBUTE_DIRECTORY) != 0) &&
			(lstrcmp(lpFindData->cFileName, _TEXT(".")) != 0) &&
		(lstrcmp(lpFindData->cFileName, _TEXT("..")) != 0));
}

//查找下一个目录
static BOOL FindNextChildDir(HANDLE hFindFile,
	WIN32_FIND_DATA * lpFindData) {
	BOOL fFound = FALSE;

	do {
		fFound = FindNextFile(hFindFile, lpFindData);
	} while (fFound && !IsChildDir(lpFindData));

	return(fFound);
}

//找到第一个子目录
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
/*为了减少堆栈的使用，
DIRWALKDATA结构的一个实例被创建为DirWalk中的一个局部变量，
并且指向它的指针被传递给了DirWalk递归。*/

//DirWalkRecurse所使用的数据
typedef struct {
	int dDepth;				//嵌套深度
	BOOL fRecurse;			//设置到实际列表的子目录
	TCHAR szBuf[1000];		//格式化输出缓冲区
	int nIndent;			//缩进字符数
	BOOL fOk;				//循环控制标志
	BOOL fIsDir;			//循环控制标志
	WIN32_FIND_DATA FindData;//文件信息
} DIRWALKDATA, *LPDIRWALKDATA;
/*遍历目录结构并在列表框中填充文件名。
如果设置了pDW->fRecurse，则通过递归地调用DirwalkRecurse来列出所有子目录。
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
		//获得第一个子目录
		hFind = FindFirstChildDir(_TEXT("*.*"), &pDW->FindData);
		pDW->fOk = (hFind != INVALID_HANDLE_VALUE);
		while (pDW->fOk) {
			//变换到子目录
			if (SetCurrentDirectory(pDW->FindData.cFileName)) {
				/*执行递归路径到子目录。
				此时一些pDW成员会被这个调用覆盖。*/
				DirWalkRecurse(pDW);

				//回到子目录的父目录 
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

/*遍历目录结构。该函数设置了对正在工作的DirWalkRecurse的调用*/
void DirWalk(
	LPCTSTR pszRootPath,		//树的起点
	BOOL fRecurse) {			//扩展子目录

	TCHAR szCurrDir[_MAX_DIR];
	DIRWALKDATA DW;

	//保存当前目录，以至于之后能被恢复
	GetCurrentDirectory(chDIMOF(szCurrDir), szCurrDir);

	//将当前目录设置为要开始的地方
	SetCurrentDirectory(pszRootPath);

	/*nDepth用于控制缩进，其值等于-1将导致第一级显示左对齐 */
	DW.dDepth = -1;
	DW.fRecurse = fRecurse;

	//调用递归函数来遍历子目录
	DirWalkRecurse(&DW);

	//将当前目录恢复到调用函数之前的位置
	SetCurrentDirectory(szCurrDir);
}

//将给定的两个字符串连接起来，并返回为一个新的字符串
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

//用StringJoin函数，在字符串末尾添加'\'，用于目录路径
char* add_slash(char* string)
{
	int len = strlen(string);
	//查看最后一个字符是否为'\'
	if (string[len - 1] != '\\')
		return StringJoin(string, "\\");
	else
		return string;
}

//获取Windows目录
char* WindowsPath()
{
	char *Buffer;
	DWORD dwRet;
	//获取Windows目录
	//获取Windows目录字符串大小，包含末尾的'\0'
	dwRet = GetWindowsDirectoryA(NULL, 0);
	if (dwRet == 0)
	{
		printf("GetCurrentDirectory failed (%d)\n", GetLastError());
		return NULL;
	}
	/*根据获取的空间，分配内存，并置零*/
	Buffer = (char *)malloc(sizeof(char)*dwRet);
	memset(Buffer, 0, dwRet * sizeof(char));
	/*获取临时目录*/
	GetWindowsDirectoryA(Buffer, dwRet);
	return add_slash(Buffer);
}

//除去本级目录 ，和Stringsub()函数配合使用 
char* sub_slash(char* string) {
	int i;
	int len = strlen(string);
	//查看最后一个字符是否为'\'
	for (i = len - 1; ; i--) {
		if (string[i] == '\\') {
			string[i] = '\0';
			break;
		}
	}
	return string;
}

//获得当前目录的上一级目录 
char* Stringsub() {
	char *Buffer;
	DWORD dwRet;
	/*获取当前目录*/
	/*获取当前目录字符串大小，包含末尾的'\0'*/
	dwRet = GetCurrentDirectoryA(0, NULL);
	if (dwRet == 0)
	{
		printf("GetCurrentDirectory failed (%d)\n", GetLastError());
		return NULL;
	}
	/*根据获取的空间，分配内存，并置零*/
	Buffer = (char *)malloc(sizeof(char)*dwRet);
	memset(Buffer, 0, dwRet * sizeof(char));
	/*获取临时目录*/
	GetCurrentDirectoryA(dwRet, Buffer);
	return sub_slash(Buffer);
}

//获得系统信息 
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

//读取操作系统的名称    
void GetSystemName(char* osname)
{
	char buf[128];
	SYSTEM_INFO info;        //用SYSTEM_INFO结构判断64位AMD处理器     
	GetSystemInfo(&info);    //调用GetSystemInfo函数填充结构     
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	strcpy(buf, "unknown OperatingSystem.");

	if (GetVersionEx((OSVERSIONINFO *)&os))
	{
		//下面根据版本信息判断操作系统名称     
		switch (os.dwMajorVersion)//判断主版本号    
		{
		case 4:
			switch (os.dwMinorVersion)//判断次版本号     
			{
			case 0:
				if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
					strcpy(buf, "Microsoft Windows NT 4.0"); //1996年7月发布     
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
			switch (os.dwMinorVersion)  //再比较dwMinorVersion的值    
			{
			case 0:
				strcpy(buf, "Microsoft Windows 2000"); //1999年12月发布    
				break;

			case 1:
				strcpy(buf, "Microsoft Windows XP"); //2001年8月发布    
				break;

			case 2:
				if (os.wProductType == VER_NT_WORKSTATION
					&& info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				{
					strcpy(buf, "Microsoft Windows XP Professional x64 Edition");
				}
				else if (GetSystemMetrics(SM_SERVERR2) == 0)
					strcpy(buf, "Microsoft Windows Server 2003"); //2003年3月发布     
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
					strcpy(buf, "Microsoft Windows Server 2008"); //服务器版本     
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


//CPU利用率    
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
	//（总的时间-空闲时间）/总的时间=占用cpu的时间就是使用率    
	double cpu = abs((kernel + user - idle) * 100 / (kernel + user));

	m_preidleTime = idleTime;
	m_prekernelTime = kernelTime;
	m_preuserTime = userTime;
	return cpu;
}

//int main() {
//	char cmd[20];		//保存指令 
//	char* ch[50];		//保存路径
//	char start[20];	//保存初始路径 
//	char buffer[MAX_PATH];		//用于保存当前路径
//	char* path[50];		//保存更换的目录 
//	char osname[30];	//系统版本名称 
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
//		printf("%请输入需要执行的命令:\n");
//		//gets(cmd);
//		scanf("%s", cmd);
//
//		if (strcmp(cmd, "exit") == 0)
//			break;
//
//		if (strcmp(cmd, "-ls") == 0) {	//显示当前工作目录 
//			_getcwd(buffer, MAXPATH);
//			printf("当前工作目录 %s\n", buffer);
//		}
//
//		//恢复系统目录 mycd.. 
//		if (strcmp(cmd, "mycd..") == 0) {
//			SetCurrentDirectory(WindowsPath());
//			_getcwd(buffer, MAXPATH);
//			printf("当前工作目录 %s\n", buffer);
//		}
//
//		//更换工作目录 mycd 
//		if (strcmp(cmd, "mycd") == 0) {
//			scanf("%s", ch);
//
//			SetCurrentDirectory((char*)ch);
//			_getcwd(buffer, MAXPATH);
//			printf("当前工作目录 %s\n", buffer);
//		}
//
//		//返回上一级工作目录 back 
//		if (strcmp(cmd, "back") == 0) {
//			SetCurrentDirectory(Stringsub());
//			_getcwd(buffer, MAXPATH);
//			printf("当前工作目录 %s\n", buffer);
//		}
//
//
//		//遍历目标文件夹 
//		if (strcmp(cmd, "dirwalk") == 0) {
//			scanf("%s", ch);
//			//判断地址错误未完善	
//
//			//目录查询	
//			DirWalk((char*)ch, TRUE);
//		}
//
//		//查询系统信息 
//		if (strcmp(cmd, "system") == 0) {
//			GetSysInfo();
//		}
//
//		//查询系统版本 
//		if (strcmp(cmd, "windows") == 0) {
//			GetSystemName(osname);
//			printf("%s\n", osname);
//		}
//
//		//查询CPU的利用率
//		if (strcmp(cmd, "cpu") == 0) {
//			U_CHAR  sysStateCpu[5];
//			cpu = CpuUseage();
//			sprintf((char*)sysStateCpu, "%.2lf", cpu);
//			printf("CPU使用率：%s%%\n", sysStateCpu);
//		}
//
//		//查看内存使用率 
//		if (strcmp(cmd, "memory") == 0) {
//			char bufPreCPU[5];
//			MEMORYSTATUSEX statex;
//			statex.dwLength = sizeof(statex);
//			GlobalMemoryStatusEx(&statex);
//			sprintf(bufPreCPU, "%ld%%\n", statex.dwMemoryLoad);
//			puts(bufPreCPU);
//			//硬盘使用率 调用windows API    
//			ULARGE_INTEGER FreeBytesAvailableC, TotalNumberOfBytesC, TotalNumberOfFreeBytesC;
//			ULARGE_INTEGER FreeBytesAvailableD, TotalNumberOfBytesD, TotalNumberOfFreeBytesD;
//			ULARGE_INTEGER FreeBytesAvailableE, TotalNumberOfBytesE, TotalNumberOfFreeBytesE;
//			ULARGE_INTEGER FreeBytesAvailableF, TotalNumberOfBytesF, TotalNumberOfFreeBytesF;
//
//			GetDiskFreeSpaceEx(_T("C:"), &FreeBytesAvailableC, &TotalNumberOfBytesC, &TotalNumberOfFreeBytesC);
//			GetDiskFreeSpaceEx(_T("D:"), &FreeBytesAvailableD, &TotalNumberOfBytesD, &TotalNumberOfFreeBytesD);
//			GetDiskFreeSpaceEx(_T("E:"), &FreeBytesAvailableE, &TotalNumberOfBytesE, &TotalNumberOfFreeBytesE);
//			GetDiskFreeSpaceEx(_T("F:"), &FreeBytesAvailableF, &TotalNumberOfBytesF, &TotalNumberOfFreeBytesF);
//			/*参数 类型及说明
//			lpRootPathName String，不包括卷名的磁盘根路径名
//			lpFreeBytesAvailableToCaller LARGE_INTEGER，指定一个变量，用于容纳调用者可用的字节数量
//			lpTotalNumberOfBytes LARGE_INTEGER，指定一个变量，用于容纳磁盘上的总字节数
//			lpTotalNumberOfFreeBytes LARGE_INTEGER，指定一个变量，用于容纳磁盘上可用的字节数
//			适用平台  Windows 95 OSR2，Windows NT 4.0*/
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
	printf("CPU使用率：%s%%\n", sysStateCpu);
}
void memorylook() {
	char bufPreCPU[5];
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	sprintf(bufPreCPU, "%ld%%\n", statex.dwMemoryLoad);
	puts(bufPreCPU);
	//硬盘使用率 调用windows API    
	ULARGE_INTEGER FreeBytesAvailableC, TotalNumberOfBytesC, TotalNumberOfFreeBytesC;
	ULARGE_INTEGER FreeBytesAvailableD, TotalNumberOfBytesD, TotalNumberOfFreeBytesD;
	ULARGE_INTEGER FreeBytesAvailableE, TotalNumberOfBytesE, TotalNumberOfFreeBytesE;
	ULARGE_INTEGER FreeBytesAvailableF, TotalNumberOfBytesF, TotalNumberOfFreeBytesF;

	GetDiskFreeSpaceEx(_T("C:"), &FreeBytesAvailableC, &TotalNumberOfBytesC, &TotalNumberOfFreeBytesC);
	GetDiskFreeSpaceEx(_T("D:"), &FreeBytesAvailableD, &TotalNumberOfBytesD, &TotalNumberOfFreeBytesD);
	GetDiskFreeSpaceEx(_T("E:"), &FreeBytesAvailableE, &TotalNumberOfBytesE, &TotalNumberOfFreeBytesE);
	GetDiskFreeSpaceEx(_T("F:"), &FreeBytesAvailableF, &TotalNumberOfBytesF, &TotalNumberOfFreeBytesF);
	/*参数 类型及说明
	lpRootPathName String，不包括卷名的磁盘根路径名
	lpFreeBytesAvailableToCaller LARGE_INTEGER，指定一个变量，用于容纳调用者可用的字节数量
	lpTotalNumberOfBytes LARGE_INTEGER，指定一个变量，用于容纳磁盘上的总字节数
	lpTotalNumberOfFreeBytes LARGE_INTEGER，指定一个变量，用于容纳磁盘上可用的字节数
	适用平台  Windows 95 OSR2，Windows NT 4.0*/
	float totalHardDisk = GB(TotalNumberOfBytesC) + GB(TotalNumberOfBytesD) + GB(TotalNumberOfBytesE) + GB(TotalNumberOfBytesF);
	float freeHardDisk = GB(TotalNumberOfFreeBytesC) + GB(TotalNumberOfFreeBytesD) + GB(TotalNumberOfFreeBytesE) + GB(TotalNumberOfFreeBytesF);

	float hardDiskUsage = 1 - freeHardDisk / totalHardDisk;
}

void windows() {
	char osname[30];	//系统版本名称 
	GetSystemName(osname);
	printf("%s\n", osname);
}