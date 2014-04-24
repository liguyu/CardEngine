#include "AppContext.h"
#include "../Commons/Global.h"
using namespace Engine::Logging;

AppContext::AppContext()
{
}

AppContext::~AppContext()
{
}

int AppContext::InitContext()
{    
	int ret;	
	ret = InitLog();	
	if ( ret )    
	{		
		return ret;    
	}   

	//加载配置文件    
	ret = InitConfig();    
	if ( ret )    
	{		
		return ret;   
	 }	

	//启动服务	
	ret = InitService();    
	if ( ret )    
	{		
		return ret;   
	}  

	return SUCCESS;
 }

void AppContext::GetCurrentDirectory( char *path )
{    
	memset( currentPath, 0, sizeof currentPath );	
	TCHAR ddlPath[100];    
	HMODULE dll = reinterpret_cast <HMODULE> ( &__ImageBase );    
	::GetModuleFileName( dll, ddlPath, MAX_PATH_SIZE );	
	sprintf(currentPath,"%S",ddlPath);    
	char * p = strrchr( currentPath, '\\' );    
	if ( p )    
	{     
		*p = '\0';   
	}    
	if ( path )   
	{    
		memcpy( path, currentPath, sizeof( currentPath ) );    
	}
 }

void AppContext::GetAbsolutePath( char* fileName, char* path )
{    
	memset( path, 0, sizeof( path ) );    
	sprintf( path, "%s\\%s", currentPath, fileName );
}

int AppContext::InitConfig()
{   
	GetCurrentDirectory( NULL );    
	char cfgFile[MAX_PATH_SIZE];    
	GetAbsolutePath( CFG_NAME, cfgFile );	
	try
	{		
		this->config.ReadFile( cfgFile );	
	}catch(Config::File_not_found& e)
	{		
		ErrorLogging("加载配置文件【%s】失败，请检查文件是否存在.",cfgFile);
		return FILE_NOT_FOUND;	
	}	    
	return SUCCESS;
 }
 
 int AppContext::InitLog()
 {	
	GetCurrentDirectory( NULL );    
	char cfgFile[MAX_PATH_SIZE];    
	GetAbsolutePath( LOG_CFG, cfgFile );	
	std::ifstream in( cfgFile );    
	if ( !in ) return FILE_NOT_FOUND;	

	InitLogger(cfgFile);	
	InfoLogging("%s","加载日志配置文件成功");    
	return SUCCESS;
 }

int AppContext::InitService()
{    
	char dllFile[MAX_PATH_SIZE];	
	//TODO	
	//Device    
	GetAbsolutePath( "Decard.dll", dllFile );	
	wchar_t dllPath[MAX_PATH_SIZE];	
	size_t convertedChars = 0;	
	mbstowcs_s(&convertedChars, dllPath, strlen(dllFile)+1, dllFile, _TRUNCATE);	
	HINSTANCE handle=::LoadLibraryEx(dllPath,NULL,LOAD_WITH_ALTERED_SEARCH_PATH);	
	if ( !handle )	
	{		
		ErrorLogging( "加载设备[%s]出错", dllFile );		
		return SYS_LOAD_DLL_ERR;   
	}	
	typedef Engine::Device::CDeviceService* (createInstance)();    
	createInstance* func;   
	func =(createInstance *)::GetProcAddress(handle,"CreateDeviceService");	
	deviceService = (*func)();	
 
	//FreeLibrary(handle);	
 
	//UserCard	
	memset(dllFile,0x00,sizeof(dllFile));	
	GetAbsolutePath( "CPUUserCard.dll", dllFile );	
	memset(dllPath,0x00,sizeof(dllPath)/sizeof(wchar_t));
	mbstowcs_s(&convertedChars, dllPath, strlen(dllFile)+1, dllFile, _TRUNCATE);	
	handle=::LoadLibraryEx(dllPath,NULL,LOAD_WITH_ALTERED_SEARCH_PATH);	
	if ( !handle )
	{		
		ErrorLogging( "加载用户卡[%s]出错", dllFile );	
		return SYS_LOAD_DLL_ERR;    
	}	
	typedef Engine::Card::CUserCardService* (createUserCardInstance)();   
	createUserCardInstance* userCardFunc;   
	userCardFunc =(createUserCardInstance *)::GetProcAddress(handle,"CreateUserCardService");	
	this->userCardService = (*userCardFunc)();   


	//PsamCard	
	memset(dllFile,0x00,sizeof(dllFile));	
	GetAbsolutePath( "PsamCard.dll", dllFile );	
	memset(dllPath,0x00,sizeof(dllPath)/sizeof(wchar_t));
	mbstowcs_s(&convertedChars, dllPath, strlen(dllFile)+1, dllFile, _TRUNCATE);	
	handle=::LoadLibraryEx(dllPath,NULL,LOAD_WITH_ALTERED_SEARCH_PATH);	
	if ( !handle )
	{		
		ErrorLogging( "加载Psam卡[%s]出错", dllFile );	
		return SYS_LOAD_DLL_ERR;    
	}	
	typedef Engine::Card::CPsamCardService* (createPsamCardInstance)();   
	createPsamCardInstance* psamCardFunc;   
	psamCardFunc =(createPsamCardInstance *)::GetProcAddress(handle,"CreatePsamCardService");	
	this->psamCardService = (*psamCardFunc)(); 

	//Setup Releation
	this->userCardService->SetDeviceService(this->deviceService);
	this->psamCardService->SetDeviceService(this->deviceService);
	this->userCardService->SetPsamCardervice(this->psamCardService);

	//set psam slot
	int samSlot = this->config.Read("psamSlot",0);
	if(samSlot!=0)
		this->psamCardService->SetPsamSlot(samSlot);
	
	return SUCCESS;
 }

Engine::Device::CDeviceService* AppContext::GetDeviceService()
{    
	return this->deviceService;
}
Engine::Card::CUserCardService* AppContext::GetUserCardService()
{	
	return this->userCardService;
}

Engine::Card::CPsamCardService* AppContext::GetPsamCardService()
{
    return this->psamCardService;
}