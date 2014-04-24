// CardEngineTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../CardEngine/CardEngine.h"
#include "../Commons/Global.h"
#include "../Commons/PsamCard.h"
using namespace Engine::Card;

void InitPsamCard()
{
  std::string psamNo = "000000000001";

  PsamCard psamCard;
  memset( &psamCard, 0, sizeof( psamCard ) );

  psamCard.cardType = 2;
  psamCard.cardVersion = 1;
  strcpy( psamCard.publishId, "20" );
  strcpy( psamCard.userId, "10" );
  strcpy( psamCard.psamCardNo, "1234567" );
  strcpy( psamCard.termId, psamNo.c_str() );
  strcpy( psamCard.startDate, "20090101" );
  strcpy( psamCard.expireDate, "20991231" );

  int ret;
  ret = ENG_InitPsamCard( &psamCard );
  if ( ret )
  {
    return;
  }
}

void InitUserCard()
{
	char phyid[17];
	int cardType = 0;

	ENG_RequestCard(phyid,cardType);
	ENG_RecycleUserCard();
	//ENG_InitUserCard(phyid,"");
}

void ReadCard()
{
	char phyid[17];
	int cardType = 0;
	char fieldValue[256];

	ENG_RequestCard(phyid,cardType);
    ENG_SetFieldReadMode(CF_CARDNO);
	ENG_SetFieldReadMode(CF_SECONDTRACK);
	ENG_ReadCard();
	ENG_GetFieldValue(CF_CARDNO,fieldValue);
	memset(fieldValue,0x00,sizeof(fieldValue));
	ENG_GetFieldValue(CF_SECONDTRACK,fieldValue);
}

void WriteCard()
{
	char phyid[17];
	int cardType = 0;
	char fieldValue[256];

	ENG_RequestCard(phyid,cardType);
	ENG_SetFieldWriteMode(CF_SECONDTRACK);
	ENG_SetFieldValue(CF_SECONDTRACK,"9800880300000000169D000072038855921");
	ENG_WriteCard();
}

int _tmain(int argc, _TCHAR* argv[])
{
	ENG_Init();
	char phyid[17];
	int cardType = 0;

	ENG_SetMainKey("33333333333333333333333333333333");
	//ENG_RequestCard(phyid,cardType);
	//InitUserCard();
	ENG_SetPsamSlot(1);
	ENG_ResetPsam();
	//ENG_UpdateCardKeys();
	//ENG_RecyclePsamCard();
	//InitPsamCard();
	ReadCard();
	//WriteCard();

	return 0;
}

