// plug.cpp : Defines the entry point for the DLL application.
//MemViewer 1.0 for OllyDbg v1.0 and Immunity Debugger v1.8x
//������Ϣ��ȫ�о�Ժ
//by��obaby
//http://www.h4ck.org.cn

//#define IMMDBG


#include "stdafx.h"
#define _CHAR_UNSIGNED
#include "windows.h"
#ifdef IMMDBG
	#include "ImmSdk\\plugin.h"
#else
	#include "OllSdk\\Plugin.h"
#endif
#include "pe.h"
#include "str.h"

#ifdef IMMDBG
	#pragma comment(lib, "ImmSdk\\immunitydebugger.lib")
#else
	#pragma comment(lib, "OllSdk\\ollydbgvc7.lib")
#endif


//---------------------------------------------------------------------------------
HINSTANCE hinst ; //���ʵ��
HWND mainWnd;
HWND OdMem_handle = NULL; //���ݴ��ھ��
int  WindowsCount = 1;
int  WindowOffset; //������ʼleft side offset

static TCHAR c_About[] =
    "MemViewer plugin v1.0\n"
    "coded by obaby\n\n"
    "Thanks to all those, who had contributed\n"
    "their code, ideas and bugreports!\n\n"
	"http://www.h4ck.org.cn\n\n"
	"Email:root@h4ck.ws\n\n"
    " ������Ϣ��ȫ�о�Ժ, 2009-2012";



//---------exports-------------------------


extern "C"
{
_declspec(dllexport)  int __cdecl ODBG_Plugindata(char shortname[32]) 
{
    strcpy(shortname,"MemViewer");       // Name of plugin
    return 110;   //version 1.10
}
_declspec(dllexport) int __cdecl ODBG_Plugininit(int ollydbgversion,HWND hw,ulong *features) 
{
	Addtolist(0,0,"==================================================");
	#ifdef IMMDBG
		Addtolist(0,0,"[*]MemViewer for Immunity Debugger 1.8x ");
	#else
		Addtolist(0,0,"[*]MemViewer for OllyDebuger 1.x ");
	#endif
	mainWnd=hw;
	Addtolist(0,1,"[*]MemViewer started successfully");
	//��ȡ����ƫ��
	WindowOffset = Pluginreadintfromini(hinst,"Left Side",0);
	if (WindowOffset == 0)
	{
		Pluginwriteinttoini(hinst,"Left Side",600);
		WindowOffset = 600;
	}

	Addtolist(0,2,"[*]Need Support ? Visit http://www.h4ck.org.cn ");
	Addtolist(0,0,"==================================================");
  return 0;
}
_declspec(dllexport) int __cdecl ODBG_Pluginmenu(int origin,char data[4096],void *item)
{
   if(origin==PM_MAIN)
   {
		strcpy(data,"0 &Normal MemWindow, 3 &Inside MemWindow(No Bar) ,4 &Inside MemWindow(With Bar), 1 &About MemViewer");
		return 1;
   }
   return 0;
}

BOOL CALLBACK EnumChildWindowProc(HWND hwnd,LPARAM lparam)
{
	TCHAR lpChildWindowName[256],lpChildClassName[256];
	GetWindowTextW(hwnd,(LPWSTR)lpChildWindowName,256-1);
	GetClassNameW(hwnd,(LPWSTR)lpChildClassName,256-1);
	if (wcsicmp((LPWSTR)lpChildWindowName,L"ICPUDUMP") == 0)
	{
		
	}
	return TRUE;
}


BOOL CALLBACK EnumWindowsProc (HWND hwnd,LPARAM lpraram)
{
	TCHAR lpWinTittle[256],lpClassName[256];
	GetWindowText(hwnd,lpWinTittle,256-1);
	GetClassName(hwnd,lpClassName,256-2);
	if (strstr(lpClassName,"1212121"))
	{
		EnumChildWindows(hwnd,EnumChildWindowProc,0);
	}
	return TRUE;
}
//Immdbg���ݴ�����mem���ڻ�ȡԭʼdump���ھ��
HWND ImmDbg_FindMemWindow(HWND hwnd)
{
	HWND Cpu_hwnd = NULL;
	HWND OrgMem_hwnd = NULL;
	Cpu_hwnd = FindWindowEx(GetParent(hwnd),NULL,"ACPU",NULL); //��ȡ������MDIClient�µ�Icpu������
	OrgMem_hwnd = FindWindowEx(Cpu_hwnd,NULL,"ACPUDUMP",NULL);	 //��ȡdump���ھ��

	return OrgMem_hwnd;
}

HWND OllyDbg_FindMemWindow(HWND hwnd)
{
	HWND Cpu_hwnd = NULL;
	HWND OrgMem_hwnd = NULL;
	Cpu_hwnd = FindWindowEx(GetParent(hwnd),NULL,"ICPU",NULL); //��ȡ������MDIClient�µ�Icpu������ ollyice����
	if (!Cpu_hwnd)
	{
		Cpu_hwnd = FindWindowEx(GetParent(hwnd),NULL,"HCPU",NULL);	//�ᰮ�ƽ��od
	}
	if (!Cpu_hwnd)
	{
		Cpu_hwnd = FindWindowEx(GetParent(hwnd),NULL,"XCPU",NULL);	//�޸�od
	}
	if (!Cpu_hwnd)
	{
		Cpu_hwnd = FindWindowEx(GetParent(hwnd),NULL,"OCPU",NULL);	//obaby
	}
	if (!Cpu_hwnd)
	{
		Cpu_hwnd = FindWindowEx(GetParent(hwnd),NULL,"ACPU",NULL);	//obaby
	}

	OrgMem_hwnd = FindWindowEx(Cpu_hwnd,NULL,"ICPUDUMP",NULL);	 //��ȡdump���ھ��
	if (!OrgMem_hwnd)
	{
		OrgMem_hwnd = FindWindowEx(Cpu_hwnd,NULL,"HCPUDUMP",NULL);	 //�ᰮ�ƽ� dump����
	}
	if (!OrgMem_hwnd)
	{
		OrgMem_hwnd = FindWindowEx(Cpu_hwnd,NULL,"XCPUDUMP",NULL);	 //�޸�od
	}
	if (!OrgMem_hwnd)
	{
		OrgMem_hwnd = FindWindowEx(Cpu_hwnd,NULL,"OCPUDUMP",NULL); //obaby
	}
	if (!OrgMem_hwnd)
	{
		OrgMem_hwnd = FindWindowEx(Cpu_hwnd,NULL,"ACPUDUMP",NULL); //obaby
	}

	return OrgMem_hwnd;
}

_declspec(dllexport) int ODBG_Pluginclose(void)
{
	Pluginwriteinttoini(hinst,"Left Side",600);
	return 0;
}

_declspec(dllexport) void __cdecl ODBG_Pluginaction(int origin,int action,void *item) 
{
  //unsigned long value=0;
  //HWND testhandle,cpuhandle,fatherhandle;
  //t_table *pt;
  HWND Mem_hwnd;
  RECT OrgMem_rect;
  if(origin==PM_MAIN) 
  {
	  if (action == 0)
	  {
		  int ImageBase = Plugingetvalue(VAL_MAINBASE);
		  t_memory *t_mem = Findmemory(ImageBase);
		  Createdumpwindow("MemVeiw",t_mem->base,t_mem->size,t_mem->base,0x01101,NULL);

		  /*fatherhandle = FindWindow(NULL,"Immunity Debugger");
		  testhandle=FindWindowEx(fatherhandle,NULL,NULL,"test - 00471000..0047370F");
		  cpuhandle=FindWindowEx(fatherhandle,NULL,"ACPUASM",NULL);
		  SetParent(testhandle,cpuhandle);*/
	  }
	  if (action == 1)
	  {
		  MessageBox(mainWnd,c_About,"MemViewer",0);
	  }
	  if (action == 3)
	  {
		  //pt =(t_table *)Plugingetvalue(VAL_MEMORY);
		  //Quicktablewindow(pt,10,20,"obaby","babt");
		  Mem_hwnd = Createdumpwindow("MemVeiw",Plugingetvalue(VAL_MAINBASE),0x10000,Plugingetvalue(VAL_MAINBASE),0x01101,NULL);
		
		  //�����ڴ洰�ڵ��ޱ߿���
		  ulong dwStyle = GetWindowLong(Mem_hwnd,GWL_STYLE);
		  SetWindowLong(Mem_hwnd,GWL_STYLE,dwStyle& (~(0x00C00000L | 0x00C0000L)));

		  if (!OdMem_handle)
		  {

		  #ifdef IMMDBG
			//Cpu_hwnd = FindWindowEx(GetParent(Mem_hwnd),NULL,"ACPU",NULL); //��ȡ������MDIClient�µ�Icpu������
			//OrgMem_hwnd = FindWindowEx(Cpu_hwnd,NULL,"ACPUDUMP",NULL);	 //��ȡdump���ھ��
			OdMem_handle = ImmDbg_FindMemWindow(Mem_hwnd);
		  #else
			//Cpu_hwnd = FindWindowEx(GetParent(Mem_hwnd),NULL,"ICPU",NULL); //��ȡ������MDIClient�µ�Icpu������
			//OrgMem_hwnd = FindWindowEx(Cpu_hwnd,NULL,"ICPUDUMP",NULL);	 //��ȡdump���ھ��
			 OdMem_handle = OllyDbg_FindMemWindow(Mem_hwnd);
		  #endif
		  }
  		  
		  //����ԭ�ڴ洰�ڹ��������ұߵ����
		  ulong dwExStyle = GetWindowLong(OdMem_handle,GWL_EXSTYLE);
		  SetWindowLong(OdMem_handle,GWL_EXSTYLE,dwExStyle & WS_EX_RIGHTSCROLLBAR | WS_EX_LEFTSCROLLBAR);
		  
		  SetParent(Mem_hwnd,OdMem_handle); //�����½��ڴ洰�ڵĸ�����
		  

		  GetClientRect(OdMem_handle,&OrgMem_rect);
		  SetWindowPos(Mem_hwnd,HWND_TOP,OrgMem_rect.left + 605,OrgMem_rect.top ,600,2000,SWP_SHOWWINDOW);

	  }
	  if (action == 4)
	  {
			  //pt =(t_table *)Plugingetvalue(VAL_MEMORY);
			  //Quicktablewindow(pt,10,20,"obaby","babt");
			  Mem_hwnd = Createdumpwindow("MemVeiw",Plugingetvalue(VAL_MAINBASE),0x10000,Plugingetvalue(VAL_MAINBASE),0x01101,NULL);
			  
			  //ulong dwStyle = GetWindowLong(Mem_hwnd,GWL_STYLE);
			  //SetWindowLong(Mem_hwnd,GWL_STYLE,dwStyle& (~(0x00C00000L | 0x00C0000L)));
			  
			  
			  //ulong dwExStyle = GetWindowLong(Mem_hwnd,GWL_EXSTYLE);
			  //SetWindowLong(Mem_hwnd,GWL_EXSTYLE,dwExStyle|WS_EX_MDICHILD);
			  
			  if (!OdMem_handle)
			  {
			  #ifdef IMMDBG
				//Cpu_hwnd = FindWindowEx(GetParent(Mem_hwnd),NULL,"ACPU",NULL); //��ȡ������MDIClient�µ�Icpu������
				//OrgMem_hwnd = FindWindowEx(Cpu_hwnd,NULL,"ACPUDUMP",NULL);	 //��ȡdump���ھ��
				OdMem_handle = ImmDbg_FindMemWindow(Mem_hwnd);
			  #else
				//Cpu_hwnd = FindWindowEx(GetParent(Mem_hwnd),NULL,"ICPU",NULL); //��ȡ������MDIClient�µ�Icpu������
				//OrgMem_hwnd = FindWindowEx(Cpu_hwnd,NULL,"ICPUDUMP",NULL);	 //��ȡdump���ھ��
				 OdMem_handle = OllyDbg_FindMemWindow(Mem_hwnd);
			  #endif
  
			  }
			  

			  //����ԭ�ڴ洰�ڹ��������ұߵ����
			  ulong dwExStyle = GetWindowLong(OdMem_handle,GWL_EXSTYLE);
			  SetWindowLong(OdMem_handle,GWL_EXSTYLE,dwExStyle & WS_EX_RIGHTSCROLLBAR | WS_EX_LEFTSCROLLBAR);

			  SetParent(Mem_hwnd,OdMem_handle); //�����½��ڴ洰�ڵĸ�����
			  
			  GetClientRect(OdMem_handle,&OrgMem_rect);
			  SetWindowPos(Mem_hwnd,HWND_TOP,OrgMem_rect.left + 605,OrgMem_rect.top -10 ,600,500,SWP_SHOWWINDOW);
			  
	 }
  }
}

}
//------------------------------
BOOL APIENTRY Dllmain(HMODULE hModule,int reason,LPVOID lpReserved)
{
	hinst = hModule;
    return TRUE;
}

