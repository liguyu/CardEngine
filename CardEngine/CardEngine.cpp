#include "CardEngine.h"
#include "../Commons/Logger.h"
#include "AppContext.h"
using namespace Engine::Device;
using namespace Engine::Card;

AppContext appContext;
bool initialized = FALSE; 

namespace
{
  const char * VERSION = "1.0";
  const char * COMPILED_DATE = __DATE__;
  char lastErrorMsg[ERROR_MSG_LEN];
}

void ENG_SetLastErrorMsg( char *errorMsg )
{
  strcpy( lastErrorMsg, errorMsg );
}

int ENG_Init()
{
	if(initialized == TRUE)
	{
		return SUCCESS;
	}
	int ret = appContext.InitContext();
	if(ret == SUCCESS)
	{
		initialized = TRUE;
	}
	return ret;
}

const char* ENG_GetVersion()
{
  return VERSION;
}

const char* ENG_GetCompiledDate()
{
  return COMPILED_DATE;
}

int ENG_OpenPort( int port, int baud )
{
  CDeviceService *deviceService = appContext.GetDeviceService();
  int ret = deviceService->OpenDevice( port, baud );
  if ( ret )
    ENG_SetLastErrorMsg( deviceService->GetLastErrorMsg( NULL ) );
  return ret;
}

int ENG_ClosePort()
{
  CDeviceService *deviceService = appContext.GetDeviceService();
  int ret = deviceService->CloseDevice();
  if ( ret )
    ENG_SetLastErrorMsg( deviceService->GetLastErrorMsg( NULL ) );
  return ret;
}

void ENG_Beep()
{
  CDeviceService *deviceService = appContext.GetDeviceService();
  deviceService->Beep();
}

void ENG_BeepError()
{
  CDeviceService *deviceService = appContext.GetDeviceService();
  deviceService->BeepError();
}

int ENG_RequestCard( char *phyid, int& cardType )
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  int ret = userCardService->RequestCard(phyid,cardType);
  return ret;
}

char* ENG_GetLastErrorMsg( char *errorMsg )
{
  if ( errorMsg )
    strcpy( errorMsg, lastErrorMsg );

  return lastErrorMsg;
}

int ENG_Halt()
{
  CDeviceService *deviceService = appContext.GetDeviceService();
  int ret = deviceService->Halt();
  if ( ret )
    ENG_SetLastErrorMsg( deviceService->GetLastErrorMsg( NULL ) );
  return ret;
}

int ENG_SetFieldReadMode( int fieldId )
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  return userCardService->SetFieldReadMode( fieldId );
}

int ENG_ReadCard()
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }

  int ret = userCardService->ReadCard();
  if ( ret )
    ENG_SetLastErrorMsg( userCardService->GetLastErrorMsg( NULL ) );

  return ret;
}

int ENG_RecycleUserCard()
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  int ret = userCardService->RecycleUserCard();
  if ( ret )
    ENG_SetLastErrorMsg( userCardService->GetLastErrorMsg( NULL ) );
  return ret;
}

int ENG_InitUserCard( const char *phyid, const char *showCardNo )
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  int ret = userCardService->InitUserCard( showCardNo );
  if ( ret )
    ENG_SetLastErrorMsg( userCardService->GetLastErrorMsg( NULL ) );
  return ret;
}

int ENG_ClearAllFieldAllMode()
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  return userCardService->ClearAllFieldAllMode();
}

int ENG_WriteCard ( const char *phyid )
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  int ret = userCardService->WriteCard();
  if ( ret )
    ENG_SetLastErrorMsg( userCardService->GetLastErrorMsg( NULL ) );
  return ret;
}

int ENG_SetFieldWriteMode( int fieldId )
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  return userCardService->SetFieldWriteMode( fieldId );
}

int ENG_GetFieldValue( int fieldId, char *fieldValue )
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  int ret = userCardService->GetFieldValue( fieldId, fieldValue );
  if ( ret )
    ENG_SetLastErrorMsg( userCardService->GetLastErrorMsg( NULL ) );
  return ret;
}

int ENG_SetFieldValue( int fieldId, const char *fieldValue )
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  int ret = userCardService->SetFieldValue( fieldId, fieldValue );
  if ( ret )
    ENG_SetLastErrorMsg( userCardService->GetLastErrorMsg( NULL ) );
  return ret;
}

int ENG_SetFieldAllMode( int fieldId )
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  return userCardService->SetFieldAllMode( fieldId );
}

int ENG_ClearFieldReadMode( int fieldId )
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  return userCardService->ClearFieldReadMode( fieldId );
}

int ENG_ClearFieldWriteMode( int fieldId )
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  return userCardService->ClearFieldWriteMode( fieldId );
}

int ENG_ClearFieldAllMode( int fieldId )
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  return userCardService->ClearFieldAllMode( fieldId );
}

int ENG_ClearAllFieldReadMode()
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  return userCardService->ClearAllFieldReadMode();
}

int ENG_ClearAllFieldWriteMode()
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  return userCardService->ClearAllFieldWriteMode();
}

int ENG_ReadFieldValue( int fieldId, char *fieldValue )
{
  int ret = SUCCESS;
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  userCardService->ClearAllFieldReadMode();
  userCardService->SetFieldReadMode(fieldId);
  ret = userCardService->ReadCard();
  if(ret)
  {
	  return ret;
  }
  ret = userCardService->GetFieldValue( fieldId, fieldValue );
  if ( ret )
    ENG_SetLastErrorMsg( userCardService->GetLastErrorMsg( NULL ) );
  return ret;
}

int ENG_WriteFieldValue( int fieldId, const char *fieldValue )
{
  int ret = SUCCESS;
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  userCardService->ClearAllFieldWriteMode();
  userCardService->SetFieldWriteMode(fieldId);
  ret = userCardService->WriteCard();
  if ( ret )
    ENG_SetLastErrorMsg( userCardService->GetLastErrorMsg( NULL ) );
  return ret;
}

int ENG_ReadCardNoAndSecondTrack( char *cardNo,char *secondTrack )
{
  int ret = SUCCESS;
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  userCardService->ClearAllFieldReadMode();
  userCardService->SetFieldReadMode(CF_CARDNO);
  userCardService->SetFieldReadMode(CF_SECONDTRACK);
  ret = userCardService->ReadCard();
  if(ret)
  {
	  return ret;
  }
  ret = userCardService->GetFieldValue( CF_CARDNO, cardNo );
  if ( ret )
    ENG_SetLastErrorMsg( userCardService->GetLastErrorMsg( NULL ) );
  ret = userCardService->GetFieldValue( CF_SECONDTRACK, secondTrack );
  if ( ret )
    ENG_SetLastErrorMsg( userCardService->GetLastErrorMsg( NULL ) );
  return ret;
}

int ENG_ReadCardNo( char *cardNo )
{
  int ret = SUCCESS;
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  userCardService->ClearAllFieldReadMode();
  userCardService->SetFieldReadMode(CF_CARDNO);
  ret = userCardService->ReadCard();
  if(ret)
  {
	  return ret;
  }
  ret = userCardService->GetFieldValue( CF_CARDNO, cardNo );
  if ( ret )
    ENG_SetLastErrorMsg( userCardService->GetLastErrorMsg( NULL ) );
  return ret;
}

int ENG_SetPsamSlot( int slot )
{
  CPsamCardService * psamCardService = appContext.GetPsamCardService();
  return psamCardService->SetPsamSlot( slot );
}

int ENG_ResetPsam()
{
  CPsamCardService * psamCardService = appContext.GetPsamCardService();
  int ret = psamCardService->ResetPsam( 1 ); //强制上电复位
  if ( ret )
    ENG_SetLastErrorMsg( "上电复位失败" );
  return ret;
}

int ENG_ReadPsamTermNo( char *termNo )
{
  CPsamCardService * psamCardService = appContext.GetPsamCardService();
  int ret = psamCardService->ReadPsamTermNo( termNo );
  if ( ret )
    ENG_SetLastErrorMsg( psamCardService->GetLastErrorMsg( NULL ) );
  return ret;
}

int ENG_InitPsamCard( PsamCard* psamCard )
{
  CPsamCardService * psamCardService = appContext.GetPsamCardService();
  int ret = psamCardService->InitPsamCard( psamCard );
  if ( ret )
    ENG_SetLastErrorMsg( psamCardService->GetLastErrorMsg( NULL ) );
  return ret;
}

int ENG_RecyclePsamCard()
{
  CPsamCardService * psamCardService = appContext.GetPsamCardService();
  int ret = psamCardService->RecyclePsamCard();
  if ( ret )
    ENG_SetLastErrorMsg( psamCardService->GetLastErrorMsg( NULL ) );
  return ret;
}

int ENG_ReadPsamCard( PsamCard* psamCard )
{
  CPsamCardService * psamCardService = appContext.GetPsamCardService();
  int ret = psamCardService->ReadPsamCard( psamCard );
  if ( ret )
    ENG_SetLastErrorMsg( psamCardService->GetLastErrorMsg( NULL ) );
  return ret;
}

int ENG_UpdateCardKeys()
{
  CUserCardService *userCardService = appContext.GetUserCardService();
  if ( userCardService == NULL )
  {
    return SYS_CARD_TYPE_ERR;
  }
  return userCardService->UpdateCardKeys();
}

int ENG_SetMainKey(char mainKey[33])
{
  CPsamCardService * psamCardService = appContext.GetPsamCardService();
  int ret = psamCardService->SetMainKey(mainKey);
  if ( ret )
    ENG_SetLastErrorMsg( psamCardService->GetLastErrorMsg( NULL ) );
  return ret;
}