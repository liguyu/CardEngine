#include <sstream>
#include <string>
#include <stdarg.h>

#include "CPUUserCard.h"
#include "../Commons/GlobalFunc.h"
#include "../Commons/Logger.h"
#include "../Commons/Des/des.h"
#include "../Commons/hex/hex.h"

using namespace std;
using namespace Engine::Device;
using namespace Engine::Func;

namespace Engine
{
  namespace Card
  {
    char CCPUUserCardService::hexCCK[33] = {"19081A14141A0819E6F7E5EBEBE5F7E6"};

    CCPUUserCardService::CCPUUserCardService()
    {
      readFileLen = 64;
      writeFileLen = 32;
      defaultPin = "000000";
      mainAppDF = "DF02";
      memset( this->lastErrorMsg, 0, sizeof( this->lastErrorMsg ) );
      InitCardDefintion();
      //ȡ����� EXTERNAL AUTHENTICATE���ο�FMCOS2.0
      //����Ĺ淶
      //CLA: 00, INS: 84, P1: 00, P2: 00, Le: 04/08
      //ע�������������ֻ��ȡ4�ֽ�
      apdu.GET_CHALLENGE = "00840000%02X";

      //�ⲿ��֤ EXTERNAL AUTHENTICATE���ο�FMCOS2.0
      //����Ĺ淶
      //CLA: 00, INS: 82, P1: 00, P2: �ⲿ��֤��Կ��ʶ��, Lc: 08, Data: 8�ֽڼ��ܺ�������
      //P2=00 ��ʾû����Ϣ�������ڷ�������֮ǰ���õ���ԿΪ��֪���������ֶ����ṩ
      apdu.EXTERNAL_AUTHENTICATE = "008200%02X08%.16s";

      //����Ŀ¼�ļ� ERASE DF���ο�FMCOS2.0
      //����Ĺ淶
      //CLA: 80, INS: 0E, P1: 00, P2: 00, Lc: 00
      apdu.ERASE_DF = "800E000000";

      //ѡ���ļ� SELECT���ο�FMCOS2.0
      //����Ĺ淶
      //CLA: 00, INS: A4, P1: 00/04, P2: 00, Lc: XX, Data: �ջ��ļ���ʶ����DF����, Le:00
      //P1=00:���ļ���ʶ��ѡ��ѡ��ǰĿ¼�»����ļ�����Ŀ¼�ļ�
      //P1=04:��Ŀ¼��ѡ��ѡ��MF��ǰĿ¼�������뵱ǰĿ¼ƽ����Ŀ¼����ǰĿ¼���¼���Ŀ¼
      apdu.SELECT_MF = "00A4000000";

      ///ѡ���ļ� SELECT���ο�FMCOS2.0
      ///��Ŀ¼��ѡ��
      apdu.SELECT = "00A4%s00%02X%s";

      //�����ļ� CREATE FILE���ο�FMCOS2.0
      //����Ĺ淶
      //CLA: 80, INS: E0, P1P2: �ļ���ʶ File ID, Lc: XX, Data: �ļ�������Ϣ��DF����
      apdu.CREATE_FILE = "80E0%s%02X%s";

      //���ӻ��޸���Կ WRITE KEY���ο�FMCOS2.0
      //����Ĺ淶
      //CLA: 80/84, INS: D4, P1: 01/XX, P2: ��Կ��ʶ, Lc: XX, Data: ��ͬ������Կ���ֶθ�ʽ��һ��
      //P1=00:��ʾ����WRITE KEY��������������Կ
      //P1=XX:��ʾ����WRITE KEY������������P1ָ�����͵���Կ
      apdu.WRITE_KEY = "80D4%s%s%s%s";

      //д�������ļ� UPDATE BINARY���ο�FMCOS2.0
      //����Ĺ淶
      //CLA: 00/04, INS: D6/D0, P1: XX, P2: XX, Lc: XX, Data: д�������
      apdu.UPDATE_BINARY_MAC = "04D6%02X%02X%02X%s";

      //��֤���� VERIFY PIN���ο�FMCOS2.0
      //����Ĺ淶
      //CLA: 00, INS: 20, P1: 00, P2: ������Կ��ʶ��, Lc: 02-06, Data: �ⲿ����Ŀ�����Կ
      apdu.VERIFY_PIN = "002000%s%02X%s";

      //Ȧ���ʼ�� INITIALIZE FOR LOAD���ο�FMCOS2.0
      //����Ĺ淶
      //CLA: 80, INS: 50, P1: 00, P2: 01/02, Lc: 0B, Data: ��Կ��ʶ��(1�ֽ�)+���׽��(4�ֽ�)+�ն˻����(6�ֽ�), Le:10
      //P1=01:��ʾ�����������ڵ��Ӵ���
      //P1=02:��ʾ�����������ڵ���Ǯ��
      //����ִ�гɹ�֮�����Ӧ���ݸ�ʽ
      //���Ӵ��ۻ����Ǯ�������(4�ֽ�)+���Ӵ��ۻ����Ǯ�������������(2�ֽ�)
      //+��Կ�汾��(1�ֽ�)+�㷨��ʶ(1�ֽ�)+α�����(4�ֽ�)+MAC1(4�ֽ�)
      apdu.INITIALIZE_FOR_LOAD = "805000%s0B%s10";

      //Ȧ������ DEBIT FOR CAPP PURCHASE���ο�FMCOS2.0
      //����Ĺ淶
      //CLA: 80, INS: 52, P1: 00, P2: 00, Lc: 0B, Data: ��������(4�ֽ�)+����ʱ��(3�ֽ�)+MAC2(4�ֽ�), Le: 04
      apdu.CREDIT_FOR_LOAD = "805200000B%s04";

      //���ѳ�ʼ�� INITIALIZE FOR PURCHASE���ο�FMCOS2.0
      //����Ĺ淶
      //CLA: 80, INS: 50, P1: 01, P2: 01/02, Lc: 0B, Data: ��Կ��ʶ��(1�ֽ�)+���׽��(4�ֽ�)+�ն˻����(6�ֽ�), Le:0F
      //P1=01:��ʾ�����������ڵ��Ӵ���
      //P1=02:��ʾ�����������ڵ���Ǯ��
      //����ִ�гɹ�֮�����Ӧ���ݸ�ʽ
      //���Ӵ��ۻ����Ǯ�������(4�ֽ�)+���Ӵ��ۻ����Ǯ�������������(2�ֽ�)
      //+͸֧�޶�(3�ֽ�)+��Կ�汾��(1�ֽ�)+�㷨��ʶ(1�ֽ�)+α�����(4�ֽ�)
      apdu.INITIALIZE_FOR_PURCHASE = "805001%s0B%s0F";

      //�������� DEBIT FOR CAPP PURCHASE���ο�FMCOS2.0
      //����Ĺ淶
      //CLA: 80, INS: 54, P1: 01, P2: 00, Lc: 0F, Data: �ն˽������(4�ֽ�)+��������(4�ֽ�)+����ʱ��(3�ֽ�)+MAC1(4�ֽ�), Le: 08
      apdu.DEBIT_FOR_CAPP_PURCHASE = "805401000F%s08";

      //����� GET BALANCE���ο�FMCOS2.0
      //����Ĺ淶
      //CLA: 80, INS: 5C, P1: 00, P2: 01/02, Le: 04
      apdu.GET_BALANCE = "805C00%s04";

      //����¼�ļ� READ RECORD���ο�FMCOS2.0
      //����Ĺ淶
      //CLA: 00/04, INS: B2, P1: ��¼��, P2:��¼��ʶ , Le: 00/XX
      //Le=00��ʾ��ȡ������¼
      //Le=XX��ʾ��ȡXX�ֽڳ��ȵļ�¼
      //P2��4-8λΪ���ļ���ʶ��
      apdu.READ_RECORD = "00B2%02X%02X%02X";

      //���������ļ� READ BINARY���ο�FMCOS2.0
      //����Ĺ淶
      //CLA: 00, INS: B0, P1: XX, P2:XX, Le: XX
      //��P1�ĸ���λΪ100�������λΪ���ļ���ʶ��P2Ϊ����ƫ����
      //��P1�����λ��Ϊ1����P1P2Ϊ�����ļ���ƫ������P1Ϊƫ�����ĸ��ֽڣ�P2Ϊƫ�����ĵ��ֽڣ������ļ�Ϊ��ǰ�ļ�
      apdu.READ_BINARY = "00B0%02X%02X%02X";

      //��MAC��ȡ�������ļ� READ BINARY���ο�FMCOS2.0
      //����Ĺ淶
      //CLA: 04, INS: B0, P1: XX, P2:XX, Lc XX, Data macУ����,Le: XX
      //��P1�ĸ���λΪ100�������λΪ���ļ���ʶ��P2Ϊ����ƫ����
      //��P1�����λ��Ϊ1����P1P2Ϊ�����ļ���ƫ������P1Ϊƫ�����ĸ��ֽڣ�P2Ϊƫ�����ĵ��ֽڣ������ļ�Ϊ��ǰ�ļ�
      apdu.READ_BINARY_MAC = "04B0%02X%02X04";

	  int len;
	  hex2dec( "22222222222222222222222222222222", 32, defaultCardKeys.DAMK, len );   //ά����Կ
	  hex2dec( "33333333333333333333333333333333", 32, defaultCardKeys.DACK, len );   //Ӧ��������Կ
	  hex2dec( "44444444444444444444444444444444", 32, defaultCardKeys.DACK1, len );  //�ⲿ��֤1
	  hex2dec( "44444444444444444444444444444444", 32, defaultCardKeys.DACK2, len );  //�ⲿ��֤2
	  hex2dec( "44444444444444444444444444444444", 32, defaultCardKeys.DACK3, len );  //�ⲿ��֤3
    }

    int CCPUUserCardService::InitCardDefintion()
    {
      FieldInfo fieldInfo;
      memset( &fieldInfo, 0, sizeof( fieldInfo ) );
      fieldInfo.unused = 1;
      //�ȳ�ʼ��Ϊ0
      for ( int i = 0; i <= CF_END; i++ )
        fieldList.push_back( fieldInfo );

      UserInfo	userInfo;
	  CardInfo cardInfo;

	  //����
      fieldList[CF_CARDNO].unused = 0;
      fieldList[CF_CARDNO].offset = userInfo.cardNo - ( uint8* )&userInfo;
      fieldList[CF_CARDNO].length = sizeof( userInfo.cardNo );
      fieldList[CF_CARDNO].fid = 0x15;
      fieldList[CF_CARDNO].type = FT_ANSI;

      //���ŵ���Ϣ
      fieldList[CF_SECONDTRACK].unused = 0;
      fieldList[CF_SECONDTRACK].offset = cardInfo.secondTrack - ( uint8* )&cardInfo;
	  fieldList[CF_SECONDTRACK].length = sizeof( cardInfo.secondTrack );
      fieldList[CF_SECONDTRACK].fid = 0x16;
      fieldList[CF_SECONDTRACK].type = FT_ANSI;

      FieldBuffer fieldBuffer;

      memset( fieldBuffer.ReadBuf, 0, sizeof( fieldBuffer.ReadBuf ) );
      memset( fieldBuffer.WriteBuf, 0, sizeof( fieldBuffer.WriteBuf ) );

      fieldBuffer.FileSize = sizeof( UserInfo );
      fieldBuffer.ReadRight = FRR_FREE;
      fieldBuffer.WriteRight = FRW_MAC;
      fieldMap[0x15] = fieldBuffer;

      fieldBuffer.FileSize = sizeof( CardInfo );
      fieldBuffer.ReadRight = FRR_EXTAUTH;
      fieldBuffer.WriteRight = FRW_MAC;
      fieldMap[0x16] = fieldBuffer;

      return SUCCESS;
    }

    int CCPUUserCardService::SuccessRequestCard()
    {
      currentDF = ""; //Ѱ���ɹ�֮������յ�ǰѡ��Ŀ¼
      return this->ResetCpuCard();
    }

    int CCPUUserCardService::CheckCardType( int cardType )
    {
      switch ( cardType )
      {
      case CPUCARD:
      case FIXCARD: //֧��CPU��
        return SUCCESS;
      case MFCARD:
      default:
        SET_ERR( "��֧�ֵĿ�����" );
        return SYS_CARD_TYPE_ERR;
      }
    }

    int CCPUUserCardService::IsLoadKeys ()
    {
      return SUCCESS;
    }

    int CCPUUserCardService::ResetCpuCard( unsigned char *len, unsigned char *buffer )
    {
      int ret;

      ret = this->deviceService->CpuReset( CPUCARD, len, buffer );
      if ( ret )
      {
        SET_ERR( "CPU���ϵ縴λ����,ret[" << ret << "]" );
        return ret;
      }
      currentDF = MF; //CPU���ϵ縴λ֮��Ĭ��ѡ��MFĿ¼

      return SUCCESS;
    }

    int CCPUUserCardService::ResetCpuCard()
    {
      unsigned char len[2] = {0};
      unsigned char buffer[256] = {0};
      return this->ResetCpuCard( len, buffer );
    }

    string CCPUUserCardService::GetCommand( string format, ... )
    {
      char command[MAX_APDU_COMMAND_LEN] = {0};
      va_list args;
      va_start ( args, format );
      vsprintf ( command, format.c_str(), args );
      va_end ( args );
      return std::string( command );
    }

    int CCPUUserCardService::ExecuteCpuCmd( CpuCmd *cmd )
    {
      int ret;
      ret = this->deviceService->ExecuteCpuCmd( CPUCARD, cmd, CARDSLOT_RF );
      if ( ret )
      {
        if ( cmd->retCode )
        {
          SET_ERR( this->GetErrorMsgByErrorCode( cmd->retCode, NULL ) );
        }
        else
        {
          SET_ERR( this->deviceService->GetLastErrorMsg( NULL ) );
        }
      }

      return ret;
    }

    int CCPUUserCardService::ExecuteCpuCmd( string command, CpuCmd *cmd )
    {
      this->ClearCpuCmd( cmd );
      strcpy( ( char* )cmd->sendBuffer, command.c_str() );
      cmd->sendLen = command.size();
      return this->ExecuteCpuCmd( cmd );
    }

    int CCPUUserCardService::ExecuteHexCpuCmd( string command, CpuCmd *cmd )
    {
      this->ClearCpuCmd( cmd );
      cmd->cmdType = 1;
      strcpy( ( char* )cmd->sendBuffer, command.c_str() );
      cmd->sendLen = command.size();
      return this->ExecuteCpuCmd( cmd );
    }

    void CCPUUserCardService::ClearCpuCmd( CpuCmd *cmd )
    {
      memset( cmd->sendBuffer, 0, cmd->sendLen );
      memset( cmd->receiveBuffer, 0, cmd->receiveLen );
      cmd->retCode = 0;
      cmd->sendLen = 0;
      cmd->receiveLen = 0;
      cmd->retry = 0;
      cmd->cmdType = 0;
      memset( cmd->retMsg, 0, sizeof( cmd->retMsg ) );
    }

    int CCPUUserCardService::ExternalAuthenticate( const uint8 externalKey[16], int keyIndex )
    {
      int ret = 0;
      uint8 encryptedRandom[64] = {0};
      uint8 randomNumber[9] = {0};
      uint8 authKey[16];
      memcpy( authKey, externalKey, 16 );

      ret = GetChallenge( randomNumber );
      if ( ret )
        return ret;

      des3_context ctx3;
      des3_set_2keys( &ctx3, authKey, authKey + 8 );
      des3_encrypt( &ctx3, randomNumber, encryptedRandom );
      char hexRandom[17] = {0};
      dec2hex( encryptedRandom, 8, hexRandom );

      CpuCmd cmd;
      return this->ExecuteHexCpuCmd( this->GetCommand( apdu.EXTERNAL_AUTHENTICATE, keyIndex, hexRandom ), &cmd );
    }

    int CCPUUserCardService::ExternalAuthenticate( int keyIndex )
    {
      uint8 randomNumber[9] = {0};
      char hexRandom[17] = {0};
      char encryptedRandom[24] = {0};
      int ret;

      ret = this->SelectADFByFI( mainAppDF );
      if ( ret )
        return ret;
      ret = this->GetChallenge( randomNumber );
      if ( ret )
        return ret;
      dec2hex( randomNumber, 8, hexRandom );

      ret = this->psamCardService->EncryptRandom( this->phyid, 1, hexRandom, encryptedRandom );
      if ( ret )
        return ret;

      CpuCmd cmd;
      return this->ExecuteHexCpuCmd( this->GetCommand( apdu.EXTERNAL_AUTHENTICATE, keyIndex, encryptedRandom ), &cmd );
    }

    int CCPUUserCardService::GetChallenge( uint8 *randomNumber )
    {
      int ret;
      uint8 hexCommand[255] = {0};
      memset( randomNumber, 0, 8 );
      string command = this->GetCommand( apdu.GET_CHALLENGE, 4 );
      CpuCmd cmd;

      ret = this->ExecuteHexCpuCmd( command, &cmd );
      if ( ret )
      {
        return ret;
      }
      memcpy( randomNumber, cmd.receiveBuffer, 4 );

      return SUCCESS;
    }

    int CCPUUserCardService::EraseDF()
    {
      CpuCmd cmd;
      return this->ExecuteHexCpuCmd( apdu.ERASE_DF, &cmd );
    }

    int CCPUUserCardService::RecycleUserCard()
    {
      int ret = 0;
      int len = 0;
      uint8 externalKey[17] = {0};
	  memset( externalKey, 0x00, sizeof externalKey );
      

	  memset( externalKey, 0x44, 16 );
      ret = this->ExternalAuthenticate( externalKey,5 );

      hex2dec( hexCCK, 32, externalKey, len );
      //��Ĭ��������Կ�����ⲿ��֤
      ret = this->ExternalAuthenticate( externalKey,1 );
      if ( ret )
      {
        if ( 0x6A88 != ret )
        {
          if ( 0x63 == ret >> 8 )
          {
            //����ʹ�ó�����Կ������֤
            memset( externalKey, 0x44, sizeof externalKey );
            ret = this->ExternalAuthenticate( externalKey,1 );
            if ( ret )
            {
              return ret;
            }
          }
        }
        else
        {
          //6A88������Կδ�ҵ���˵���ÿ��ѱ�����
          return SUCCESS;
        }
      }

      //����Ŀ¼�ļ�
      ret = EraseDF();
      if ( ret )
      {
        SET_ERR( "����CPU����ʧ��" );
        return ret;
      }

      return SUCCESS;
    }

    uint8* CCPUUserCardService::PbocDiverKey( uint8 phyidStr[9], uint8 srcKey[17], uint8 destKey[17] )
    {
      int i;
      uint8 tmp1[20], tmp2[20];
      uint8 ret1[20], ret2[20];

      memset( tmp1, 0x00, sizeof tmp1 );
      memset( tmp2, 0x00, sizeof tmp2 );
      memset( ret1, 0x00, sizeof ret1 );
      memset( ret2, 0x00, sizeof ret2 );

      memcpy ( tmp1, phyidStr, 8 );
      for ( i = 0; i < 8; i++ )
      {
        tmp2[i] = ~tmp1[i];
      }
      des3_context ctx3;
      des3_set_2keys( &ctx3, srcKey, srcKey + 8 );
      des3_encrypt( &ctx3, tmp1, ret1 );
      des3_encrypt( &ctx3, tmp2, ret2 );
      memcpy( destKey, ret1, 8 );
      memcpy( destKey + 8, ret2, 8 );

      return destKey;
    }

    int CCPUUserCardService::InitUserCard( const char *showCardNo )
    {
      int ret = 0;
      uint8 phyidStr[9] = {0};

      IS_LOAD_KEY();

      if ( strlen( this->phyid ) == 8 )
      {
        strcat( this->phyid, "80000000" );
      }
      if ( strlen( this->phyid ) != 16 )
      {
        SET_ERR( "������ID[" << this->phyid << "]���ȴ���" );
        return CARD_PHYID_LENGTH_ERR;
      }

      int len = 0;
	  Engine::Func::hex2dec( this->phyid, 16, phyidStr, len );
	  
	  //TODO
      //��Կ���е���Կ��
      //CPUCardKeys *cardKeys = this->keyCardService->GetCPUCardKeys();
	  CPUCardKeys cardKeys;
	  memset( &cardKeys, 0, sizeof( cardKeys ) );
	  
	  hex2dec( "33333333333333333333333333333333", 32, cardKeys.DACK, len );
	  hex2dec( "22222222222222222222222222222222", 32, cardKeys.DAMK, len );
	  hex2dec( "44444444444444444444444444444444", 32, cardKeys.DACK1, len );

      //��ǰ�û����е���Կ��
      CPUCardKeys localCardKeys;
      memset( &localCardKeys, 0, sizeof( localCardKeys ) );

      //PbocDiverKey( phyidStr, cardKeys.DPK1, localCardKeys.DPK1 );			  //����1
      //PbocDiverKey( phyidStr, cardKeys.DPK1, localCardKeys.DPK2 );			  //����2
      //PbocDiverKey( phyidStr, cardKeys.DLK1, localCardKeys.DLK1 );			  //Ȧ��1
      //PbocDiverKey( phyidStr, cardKeys.DLK2, localCardKeys.DLK2 );			  //Ȧ��2
      //PbocDiverKey( phyidStr, cardKeys.DTAC, localCardKeys.DTAC );			  //�ڲ���ԿTAC
      PbocDiverKey( phyidStr, cardKeys.DACK, localCardKeys.DACK );			  //Ӧ��������Կ
      //PbocDiverKey( phyidStr, cardKeys.DUK, localCardKeys.DUK );			  //PIN����
      //PbocDiverKey( phyidStr, cardKeys.DRPK, localCardKeys.DRPK );			  //PIN��װ
      PbocDiverKey( phyidStr, cardKeys.DAMK, localCardKeys.DAMK );			  //ά����Կ
      PbocDiverKey( phyidStr, cardKeys.DACK1, localCardKeys.DACK1 );		  //�ⲿ��֤1
      //PbocDiverKey( phyidStr, cardKeys.DACK2, localCardKeys.DACK2 );		  //�ⲿ��֤2
      //PbocDiverKey( phyidStr, cardKeys.DACK3, localCardKeys.DACK3 );		  //�ⲿ��֤3
      //PbocDiverKey( phyidStr, cardKeys.LDACK, localCardKeys.LDACK );		  //СǮ���ⲿ��֤��Կ
      //PbocDiverKey( phyidStr, cardKeys.LDAMK, localCardKeys.LDAMK );		  //СǮ��ά����Կ

      uint8 externalKey[17] = {0};
      //ʹ�ó�����Կ�����ⲿ��֤
      memset( externalKey, 0xFF, sizeof ( externalKey ) );
      //�����ⲿ��֤�������ѻ��յĿ�����ʱӦ������Կδ�ҵ�
      ret = this->ExternalAuthenticate( externalKey );
      if ( ret )
      {
        if ( 0x6A88 != ret )
        {
          SET_ERR( "�ⲿ��֤ʧ��,�ÿ�����δ����" );
          return ret;
        }
      }

      //TODO(youyiming) Ϊʲô�˴�����Ҫ����MFĿ¼������ʱ��ִ�й��ò���
      ret = this->EraseDF();
      if ( ret )
      {
        return ret;
      }

      vector<string> cmdList;
      char hexKey[64];
      string data;    //apdu������data��

      cmdList.push_back( apdu.SELECT_MF ); //ѡ��MF

      //������Կ�ļ�
      //0000��ʾ�ļ���ʶ��
      //3F004001AAFFFFΪ�ļ�������Ϣ��DF����
      //��Կ�ļ���Ӧ���ļ�������Ϣ�������ݸ�ʽΪ
      //3F+�ļ��ռ�(2�ֽ�)+DF�ļ��̱�ʶ��(1�ֽ�)+����Ȩ��(1�ֽ�)+FF+FF
      //cmdList.push_back( this->GetCommand( apdu.CREATE_FILE, "0000", 7, "3F004001AAFFFF" ) );

      //д���������Կ������WRITE KEY�е�Case2 �����ⲿ��֤��Կ
      //Case2�����Ϊ
      //CLA: 80, INS: D4, P1: 01, P2: ��Կ��ʶ, Lc: 0D/15, Data: 39+ʹ��Ȩ+����Ȩ+����״̬+���������+8��10�ֽ���Կ
      //���������AA:�߰��ֽ�ָ����Կ����������������������Ͱ��ֽ�ָ�����������ԵĴ���
      //A=10,����Կ��������������10�Σ�ͬʱ����Դ���Ϊ10��
      data = "39F0AAAAAA";
      data += hexCCK;
      //cmdList.push_back( this->GetCommand( apdu.WRITE_KEY, "01", "00", "15", data.c_str() ) );

      //����DF02Ŀ¼
      //DF02��ʾ�ļ���ʶ��
      //Ŀ¼�ļ���Ӧ���ļ�������Ϣ�������ݸ�ʽΪ
      //38+�ļ��ռ�(2�ֽ�)+����Ȩ��(1�ֽ�)+����Ȩ��(1�ֽ�)+Ӧ���ļ�ID(XX)+������(FFFF)+DF����(5-16�ֽ�)
      cmdList.push_back( this->GetCommand( apdu.CREATE_FILE, "DF02", 24, "380230F0AA95FFFFD15600000145415359454E5452590000" ) );

      //ѡ��DF02Ŀ¼
      cmdList.push_back( this->GetCommand( apdu.SELECT, "04", 16, "D15600000145415359454E5452590000" ) );

      //������Կ�ļ�
      cmdList.push_back( this->GetCommand( apdu.CREATE_FILE, "0000", 7, "3F014001FFFFFF" ) );

      //д��Ӧ��������ԿDACK
      memset( hexKey, 0, sizeof( hexKey ) );
      Engine::Func::dec2hex( localCardKeys.DACK, 16, hexKey );
      data = "39F0FFFF33";
      data += hexKey;
      cmdList.push_back( this->GetCommand( apdu.WRITE_KEY, "01", "00", "15", data.c_str() ) );

      //д��Ӧ��ά����Կ
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( localCardKeys.DAMK, 16, hexKey );
      data = "36F00BFF33";
      data += hexKey;
      cmdList.push_back( this->GetCommand( apdu.WRITE_KEY, "01", "01", "15", data.c_str() ) );

	  //д���ⲿ��֤��Կ1
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( localCardKeys.DACK1, 16, hexKey );
      data = "39F00E2233";
      data += hexKey;
      cmdList.push_back( this->GetCommand( apdu.WRITE_KEY, "01", "01", "15", data.c_str() ) );

      //��������Ӧ�û����ļ�0015
      //��С00 3C===60�ֽ�
      cmdList.push_back( this->GetCommand( apdu.CREATE_FILE, "0015", 7, "A8003CF0F0FFFE" ) );

      //�����ֿ��˻����ļ�0016
      //��С00 3C===60�ֽ�
      cmdList.push_back( this->GetCommand( apdu.CREATE_FILE, "0016", 7, "A8003CF0F0FFFE" ) );

      CpuCmd cmd;
      string command;
      for ( int i = 0; i < cmdList.size(); i++ )
      {
        command = cmdList[i];
        ret = this->ExecuteHexCpuCmd( command, &cmd );
        if ( ret )
        {
          return ret;
        }
      }

      ret = this->SelectADFByFI( MF );
      if ( ret )
      {
        return ret;
      }

      return SUCCESS;
    }

	void CCPUUserCardService::DESEncrytCardKey(uint8* srcData, uint8* key, uint8* destData)
	{
		des3_context ctx3;
		des3_set_2keys( &ctx3, key, key + 8 );
		des3_encrypt( &ctx3, srcData, destData );
		des3_encrypt( &ctx3, srcData+8, destData+8 );
		des3_encrypt( &ctx3, srcData+16, destData+16 );
	}

	int CCPUUserCardService::UpdateCardKeys()
    {
      int ret = 0;
      uint8 phyidStr[9] = {0};

      IS_LOAD_KEY();

      if ( strlen( this->phyid ) == 8 )
      {
        strcat( this->phyid, "80000000" );
      }
      if ( strlen( this->phyid ) != 16 )
      {
        SET_ERR( "������ID[" << this->phyid << "]���ȴ���" );
        return CARD_PHYID_LENGTH_ERR;
      }

      int len = 0;
	  Engine::Func::hex2dec( this->phyid, 16, phyidStr, len );

      uint8 externalKey[17] = {0};
	  memset(externalKey,0x00,sizeof(externalKey));
      //ʹ�ó�����Կ�����ⲿ��֤
      memset( externalKey, 0x00, 16 );
      //�����ⲿ��֤�������ѻ��յĿ�����ʱӦ������Կδ�ҵ�
      ret = this->ExternalAuthenticate( externalKey,0 );
      if ( ret )
      {
        if ( 0x6A88 != ret )
        {
		  if ( 0x63 == ret >> 8 )
          {
            //����ʹ�ó�����Կ������֤
            memset( externalKey, 0xFF, sizeof externalKey );
            ret = this->ExternalAuthenticate( externalKey );
            if ( ret )
            {
			  SET_ERR( "���Գ�����Կ�ⲿ��֤ʧ��" );
              return ret;
            }
          } else 
		  {
			  SET_ERR( "�ⲿ��֤ʧ��" );
			  return ret;
		  }
        }
      }

	   //��ǰ�û����е���Կ��
      CPUCardKeys newCardKeys;
      memset( &newCardKeys, 0, sizeof( newCardKeys ) );

	  CPUCardKeys workKeys;
      memset( &workKeys, 0, sizeof( workKeys ) );

	  this->psamCardService->GenerateWorkKey(&workKeys);

      PbocDiverKey( phyidStr, workKeys.DACK, newCardKeys.DACK );			  //Ӧ��������Կ
      PbocDiverKey( phyidStr, workKeys.DAMK, newCardKeys.DAMK );			  //ά����Կ
      PbocDiverKey( phyidStr, workKeys.DACK1, newCardKeys.DACK1 );		  //�ⲿ��֤1
	  PbocDiverKey( phyidStr, workKeys.DACK1, newCardKeys.DACK2 );		  //�ⲿ��֤2
	  PbocDiverKey( phyidStr, workKeys.DACK1, newCardKeys.DACK3 );		  //�ⲿ��֤3

	  ret = this->SelectADFByFI( MF );
      if ( ret )
      {
        return ret;
      }
	  ret = this->SelectADFByFI( mainAppDF );
      if ( ret )
      {
        return ret;
      }
	  
	  //�����ⲿ��֤��Կ05
	  ret = UpdateCardKey("84F000031C","14100105FF",defaultCardKeys.DACK,newCardKeys.DACK1);
	  if(ret)
	  {
		return ret;
	  }
	  //�����ⲿ��֤��Կ06
	  ret = UpdateCardKey("84F000041C","14100106FF",defaultCardKeys.DACK,newCardKeys.DACK2);
	  if(ret)
	  {
		return ret;
	  }
	  //�����ⲿ��֤��Կ07
	  ret = UpdateCardKey("84F000051C","14100107FF",defaultCardKeys.DACK,newCardKeys.DACK3);
	  if(ret)
	  {
		return ret;
	  }
	  //����Ӧ��ά����Կ
	  ret = UpdateCardKey("84F000021C","14300102FF",defaultCardKeys.DACK,newCardKeys.DAMK);
	  if(ret)
	  {
		return ret;
	  }
	  
	  /*//����Ӧ��������Կ
	  ret = UpdateCardKey("84F000011C","14100101FF",defaultCardKeys.DACK,newCardKeys.DACK);
	  if(ret)
	  {
		return ret;
	  }*/

      ret = this->SelectADFByFI( MF );
      if ( ret )
      {
        return ret;
      }

      return SUCCESS;
    }

	int CCPUUserCardService::UpdateCardKey(const char* preCommand,const char* data,uint8* workKey, uint8* newKey)
	{
		int ret = 0;
		CpuCmd cmd;
		uint8 encryptedData[25];
		uint8 plainData[25];
		char hexStr[100];
		string command;
		int len;

		memset(plainData,0x00,sizeof(plainData));
		Engine::Func::hex2dec(data,12,plainData,len);
		memcpy(plainData+5,newKey,16);
		plainData[21] = 0x80;
		plainData[22] = 0x00;
		plainData[23] = 0x00;

		memset(encryptedData,0x00,sizeof(encryptedData));
		DESEncrytCardKey(plainData,workKey,encryptedData);
 
		uint8 randomNumber[9] = {0};
		ret = this->GetChallenge( randomNumber );
		if ( ret )
			return ret;

		uint8 plainMacData[100];
		uint8 encryptedMacData[100];

		memset(plainMacData,0x00,sizeof(plainMacData));
		Engine::Func::hex2dec(preCommand,10,plainMacData,len);
		memcpy(plainMacData+5,encryptedData,24);

		unsigned char mac[9] = {0};
	  
		this->psamCardService->CalculateMac(randomNumber,workKey,2,plainMacData,29,mac);

		command.append(preCommand);
		memset(hexStr,0x00,sizeof(hexStr));
		Engine::Func::dec2hex(encryptedData,24,hexStr);
		command.append(hexStr);
		memset(hexStr,0x00,sizeof(hexStr));
		Engine::Func::dec2hex(mac,4,hexStr);
		command.append(hexStr);
	  
		return this->ExecuteHexCpuCmd( command, &cmd );
	}

    int CCPUUserCardService::CalculateSafeMacWithSam( const char* command, char *hexMac )
    {
      int ret;
      uint8 random[8] = {0};
      char hexRandom[17] = {0};
      ret = this->GetChallenge( random );
      if ( ret )
        return ret;
	  Engine::Func::dec2hex( random, 4, hexRandom );
      memcpy( hexRandom + 8, "00000000", 8 );
      return this->psamCardService->CalculateSafeMac( this->phyid, hexRandom, command, hexMac );
    }

    int CCPUUserCardService::SelectADFByFI( string FI )
    {
      if ( strcmp( FI.c_str(), currentDF.c_str() ) == 0 ) //����Ѿ�ѡ����SFIĿ¼�����سɹ�
      {
        return SUCCESS;
      }

      CpuCmd cmd;
      int ret = this->ExecuteHexCpuCmd( this->GetCommand( apdu.SELECT, "00", FI.length() / 2, FI.c_str() ), &cmd );
      if ( ret )
      {
        SET_ERR( "ѡ��Ŀ¼" << FI << "ʧ��" );
        return ret;
      }
      //ѡ��Ŀ¼�ɹ�֮�󣬸��µ�ǰѡ��Ŀ¼
      currentDF = FI;
      return SUCCESS;
    }

    int CCPUUserCardService::ReadCard()
    {
	  int ret = 0;
      
      ret = this->SelectADFByFI( mainAppDF );
      if ( ret )
      {
        return ret;
      }

      map<int, FieldBuffer>::iterator it;
      int SFID = 0;
      for ( it = fieldMap.begin(); it != fieldMap.end(); ++it )
      {
        SFID = it->first;

        if ( !it->second.ReadFieldSet.size() )
          continue;
        memset( it->second.ReadBuf, 0, sizeof( it->second.ReadBuf ) );
        switch ( it->second.ReadRight )
        {
        case FRR_NULL:
          break;
        case FRR_REC_FREE:
        case FRR_REC_MAC:
          ret = ReadRecordFile( it->first, 1, it->second.FileSize, it->second.ReadBuf );
          if ( ret )
          {
            if ( ret != 0x6A83 )
              return ret;
            else
            {
              memset( it->second.ReadBuf, 0, it->second.FileSize );
            }
          }
          break;
        case FRR_EXTAUTH:
          ret = ExternalAuthenticate( 5 );
          if ( ret )
            return ret;
        case FRR_FREE:
          if ( it->second.FileSize > readFileLen )
          {
            ret = ReadBinaryFile( it->first, 0, readFileLen, it->second.ReadBuf );
            if ( ret )
              return ret;
            ret = ReadBinaryFile( it->first, readFileLen, it->second.FileSize - readFileLen, it->second.ReadBuf + readFileLen );
            if ( ret )
              return ret;
          }
          else
          {
            ret = ReadBinaryFile( it->first, 0, it->second.FileSize, it->second.ReadBuf );
            if ( ret )
              return ret;
          }
          break;
        case FRR_EXTAUTH_MAC:
          ret = ExternalAuthenticate( 5 );
          if ( ret )
            return ret;
        case FRR_MAC:
          if ( it->second.FileSize > readFileLen )
          {
            ret = ReadBinaryFileWithMac( it->first, 0, readFileLen, it->second.ReadBuf );
            if ( ret )
              return ret;
            ret = ReadBinaryFileWithMac( it->first, readFileLen, it->second.FileSize - readFileLen, it->second.ReadBuf + readFileLen );
            if ( ret )
              return ret;
          }
          else
          {
            ret = ReadBinaryFileWithMac( it->first, 0, it->second.FileSize, it->second.ReadBuf );
            if ( ret )
              return ret;
          }
          break;
        default:
          break;
        }
      }

      return SUCCESS;
    }

    int CCPUUserCardService::ReadRecordFile( int SFI, int recordNo, int len, uint8 *recordBuffer )
    {
      int ret;
      if ( len > 256 )
      {
        SET_ERR( "��ȡ��¼���ȳ���256���ֽ�" );
        return CARD_READ_LENGTH_ERR;
      }

      CpuCmd cmd;
      //4=00000100
      //(SFI<<3)+4��������λ��Ϊ100��SFI�еĸ���λΪ���ļ���ʶ��
      ret = this->ExecuteHexCpuCmd( this->GetCommand( apdu.READ_RECORD, recordNo, ( SFI << 3 ) + 4, len ), &cmd );
      if ( ret )
      {
        return ret;
      }
      memcpy( recordBuffer, cmd.receiveBuffer, cmd.receiveLen - 2 );

      return SUCCESS;
    }

    int CCPUUserCardService::ReadBinaryFile( int SFI, int offset, int len, uint8 *binaryBuffer )
    {
      int ret = 0;
      CpuCmd cmd;
      //128=10000000
      //128 + SFI��������λ��Ϊ100����ʱ��SFI�еĵ���λΪ���ļ���ʶ
      ret = this->ExecuteHexCpuCmd( this->GetCommand( apdu.READ_BINARY, 128 + SFI, offset, len ), &cmd );
      if ( ret )
      {
        return ret;
      }
      memcpy( binaryBuffer, cmd.receiveBuffer, cmd.receiveLen );
      return SUCCESS;
    }

    int CCPUUserCardService::ReadBinaryFileWithMac( int SFI, int offset, int len, uint8 *binaryBuffer )
    {
      int ret = 0;
      char mac[9] = {0};
      CpuCmd cmd;

      //128=10000000
      //128 + SFI��������λ��Ϊ100����ʱ��SFI�еĵ���λΪ���ļ���ʶ
      string command = this->GetCommand( apdu.READ_BINARY_MAC, 128 + SFI, offset );
      ret = CalculateSafeMacWithSam( command.c_str(), mac );
      if ( ret )
      {
        return ret;
      }
      command.append( mac );
      ret = this->ExecuteHexCpuCmd( command, &cmd );
      if ( ret )
      {
        return ret;
      }
      memcpy( binaryBuffer, cmd.receiveBuffer, cmd.receiveLen );

      return SUCCESS;
    }

    int CCPUUserCardService::UpdateBinaryFileWithMac( int SFI, int offset, int len, uint8 *binaryBuffer )
    {
      int	ret = 0;
      char mac[9] = {0};
      char hexBuffer[512] = {0};

	  Engine::Func::dec2hex( binaryBuffer, len, hexBuffer );
      //128=10000000
      //128 + SFI��������λ��Ϊ100����ʱ��SFI�еĵ���λΪ���ļ���ʶ
      string command = this->GetCommand( apdu.UPDATE_BINARY_MAC, 128 + SFI, offset, len + 4, hexBuffer );

      //����MAC
      ret = this->CalculateSafeMacWithSam( command.c_str(), mac );
      if ( ret )
      {
        return ret;
      }
      command.append( mac );

      CpuCmd cmd;
      ret = this->ExecuteHexCpuCmd( command, &cmd );
      if ( ret )
      {
        return ret;
      }

      return SUCCESS;
    }

    int CCPUUserCardService::VerifyPin( const char* pin )
    {
      char key[13] = {0};
      if ( NULL == pin )
      {
        strcpy( key, defaultPin.c_str() );
      }
      else
      {
        strcpy( key, pin );
      }
      int len = strlen( key );
      if ( len < 4 || len > 12 )
      {
        SET_ERR( "PIN���ȴ���" );
        return CARD_PIN_LENGTH_ERR;
      }

      int ret = 0;
      CpuCmd cmd;
      ret = this->ExecuteHexCpuCmd( this->GetCommand( apdu.VERIFY_PIN, "00", len / 2, key ), &cmd );
      if ( ret )
      {
        SET_ERR( "��֤����ʧ��" );
      }

      return ret;
    }

    int CCPUUserCardService::GetDpsCnt( int dpsCntLen, int cardBalanceLen, uint8* dpsCnt, uint8* cardBalance )
    {
      int ret = 0;
      ret = this->SelectADFByFI( mainAppDF );
      if ( ret )
      {
        return ret;
      }
      ret = VerifyPin( NULL );
      if ( ret )
      {
        return ret;
      }

      //ͨ��ִ��Ȧ���ʼ����������ó�ֵ����
      CpuCmd cmd;
      ret = this->ExecuteHexCpuCmd( this->GetCommand( apdu.INITIALIZE_FOR_LOAD, "02", "0100002710112233445566" ), &cmd );
      if ( ret )
      {
        return ret;
      }
      memcpy( cardBalance, cmd.receiveBuffer, cardBalanceLen );          //1-4�ֽڱ�ʾ���
      memcpy( dpsCnt, cmd.receiveBuffer + cardBalanceLen, dpsCntLen );   //5-6�ֽڱ�ʾ��ֵ����
      return 0;
    }

    int CCPUUserCardService::GetPayCnt( int payCntLen, int cardBalanceLen, uint8* payCnt, uint8* cardBalance )
    {
      int ret = 0;
      ret = this->SelectADFByFI( mainAppDF );
      if ( ret )
      {
        return ret;
      }

      //ͨ��ִ�����ѳ�ʼ��������������Ѵ���
      CpuCmd cmd;
      ret = this->ExecuteHexCpuCmd( this->GetCommand( apdu.INITIALIZE_FOR_PURCHASE, "02", "0100000000112233445566" ), &cmd );
      if ( ret )
      {
        return ret;
      }
      memcpy( cardBalance, cmd.receiveBuffer, cardBalanceLen );          //1-4�ֽڱ�ʾ���
      memcpy( payCnt, cmd.receiveBuffer + cardBalanceLen, payCntLen );   //5-6�ֽڱ�ʾ���Ѵ���

      return SUCCESS;
    }

    int CCPUUserCardService::GetBalance( int cardBalanceLen, uint8* cardBalance )
    {
      int ret = 0;
      ret = this->SelectADFByFI( mainAppDF );
      if ( ret )
      {
        return ret;
      }

      CpuCmd cmd;
      ret = this->ExecuteHexCpuCmd( this->GetCommand( apdu.GET_BALANCE, "02" ), &cmd );
      if ( ret )
      {
        return ret;
      }
      memcpy( cardBalance, cmd.receiveBuffer, cardBalanceLen );          //1-4�ֽڱ�ʾ���

      return SUCCESS;
    }

    int CCPUUserCardService::SetFieldValue( int fieldId, const char *fieldValue )
    {
      int ret = 0;
      uint8 fieldBuffer[64] = {0};

      ret = SetFieldWriteMode( fieldId );
      if ( ret )
      {
        return ret;
      }

      FieldInfo& fieldInfo = fieldList[fieldId];

      ret = CUserCardService::SetFieldValue( fieldValue, fieldInfo.type, fieldInfo.length, fieldBuffer );
      if ( ret )
      {
        return ret;
      }

      memcpy( fieldMap[fieldInfo.fid].WriteBuf + fieldInfo.offset, fieldBuffer, fieldInfo.length );

      return SUCCESS;
    }

    int CCPUUserCardService::GetFieldValue( int fieldId, char *fieldValue )
    {
      //TODO(youyiming) ����7+1���������ֶ�ȡ����CPU���ֻ���MF����
      //���Ҫ��ȡ���ֶ��Ƿ��Ѷ�ȡ
      int ret;
      /*
      int ret = FieldHasRead( fieldId );
      if ( ret )
      {
        fieldValue[0] = 0;
        return ret;
      }
      */
      //TODO(youyiming) CF_SUBSIDYNO���ܶ�

      FieldInfo& fieldInfo = fieldList[fieldId];

      ret = CUserCardService::GetFieldValue( fieldMap[fieldInfo.fid].ReadBuf + fieldInfo.offset, fieldInfo.type, fieldInfo.length, fieldValue );
      if ( ret )
      {
        return ret;
      }
      return SUCCESS;
    }

    int CCPUUserCardService::WriteCard()
    {
      int ret = 0;
      int sect_len = writeFileLen;
      ret = this->SelectADFByFI( mainAppDF );
      if ( ret )
        return ret;
      map<int, FieldBuffer>::iterator itMap;
      set<int>::iterator itSet;
      for ( itMap = fieldMap.begin(); itMap != fieldMap.end(); ++itMap )
      {
        if ( !itMap->second.WriteFieldSet.size() )
          continue;
        if ( FRW_NULL == itMap->second.WriteRight )
          continue;
        if ( FRW_MAC == itMap->second.WriteRight
             || FRW_ENCRYPT == itMap->second.WriteRight
             || FRW_ENCRYPT_MAC == itMap->second.WriteRight )
        {
          memset( itMap->second.ReadBuf, 0, sizeof( itMap->second.ReadBuf ) );
          switch ( itMap->second.ReadRight )
          {
          case FRR_NULL:
            break;
		  case FRR_EXTAUTH:
			  ret = ExternalAuthenticate( 5 );
			  if ( ret )
				return ret;
          case FRR_FREE:
            if ( itMap->second.FileSize > sect_len )
            {
              ret = ReadBinaryFile( itMap->first, 0x00, sect_len, itMap->second.ReadBuf );
              if ( ret )
                return ret;
              ret = ReadBinaryFile( itMap->first, sect_len, itMap->second.FileSize - sect_len, itMap->second.ReadBuf + sect_len );
              if ( ret )
                return ret;
            }
            else
            {
              ret = ReadBinaryFile( itMap->first, 0x00, itMap->second.FileSize, itMap->second.ReadBuf );
              if ( ret )
                return ret;
            }
            break;
          case FRR_EXTAUTH_MAC:
            ret = ExternalAuthenticate( 5 );
            if ( ret )
              return ret;
          case FRR_MAC:
            if ( itMap->second.FileSize > sect_len )
            {
              ret = ReadBinaryFileWithMac( itMap->first, 0x00, sect_len, itMap->second.ReadBuf );
              if ( ret )
                return ret;
              ret = ReadBinaryFileWithMac( itMap->first, sect_len, itMap->second.FileSize - sect_len, itMap->second.ReadBuf + sect_len );
              if ( ret )
                return ret;
            }
            else
            {
              ret = ReadBinaryFileWithMac( itMap->first, 0x00, itMap->second.FileSize, itMap->second.ReadBuf );
              if ( ret )
                return ret;
            }
            break;
          default:
            break;
          }
          for ( itSet = itMap->second.WriteFieldSet.begin() ; itSet != itMap->second.WriteFieldSet.end(); ++itSet )
          {
            int fieldidx = *itSet;
            memcpy( itMap->second.ReadBuf + fieldList[fieldidx].offset, itMap->second.WriteBuf + fieldList[fieldidx].offset, fieldList[fieldidx].length );
          }
          memcpy( itMap->second.WriteBuf, itMap->second.ReadBuf, sizeof( itMap->second.WriteBuf ) );
          //MACд
          //if(FRW_MAC==itMap->second.WriteRight)
          {
            if ( itMap->second.FileSize > sect_len )
            {
              ret = UpdateBinaryFileWithMac( itMap->first, 0x00, sect_len, itMap->second.WriteBuf );
              if ( ret )
                return ret;
              ret = UpdateBinaryFileWithMac( itMap->first, sect_len, itMap->second.FileSize - sect_len, itMap->second.WriteBuf + sect_len );
              if ( ret )
                return ret;
            }
            else
            {
              ret = UpdateBinaryFileWithMac( itMap->first, 0x00, itMap->second.FileSize, itMap->second.WriteBuf );
              if ( ret )
                return ret;
            }
          }
        }
        else if ( FRW_MAC_DIRECT == itMap->second.WriteRight )
        {
          if ( itMap->second.FileSize > sect_len )
          {
            ret = UpdateBinaryFileWithMac( itMap->first, 0x00, sect_len, itMap->second.WriteBuf );
            if ( ret )
              return ret;
            ret = UpdateBinaryFileWithMac( itMap->first, sect_len, itMap->second.FileSize - sect_len, itMap->second.WriteBuf + sect_len );
            if ( ret )
              return ret;
          }
          else
          {
            ret = UpdateBinaryFileWithMac( itMap->first, 0x00, itMap->second.FileSize, itMap->second.WriteBuf );
            if ( ret )
              return ret;
          }
        }
        itMap->second.WriteFieldSet.clear();
        memset( itMap->second.ReadBuf, 0, sizeof( itMap->second.ReadBuf ) );
        memset( itMap->second.WriteBuf, 0, sizeof( itMap->second.WriteBuf ) );
      }

      return SUCCESS;
    }

    char* CCPUUserCardService::GetErrorMsgByErrorCode( int errorCode, char *errorMsg )
    {
      switch ( errorCode )
      {
      case 0x9000:
      case 0x0000:
        sprintf( this->lastErrorMsg, "Err-%04X:�ɹ�", errorCode );
        break;
      case 0x6200:
        sprintf( this->lastErrorMsg, "Err-%04X:�����豸�п�δ���뵽λ", errorCode );
        break;
      case 0x6281:
        sprintf( this->lastErrorMsg, "Err-%04X:���͵����ݿ��ܴ���", errorCode );
        break;
      case 0x6283:
        sprintf( this->lastErrorMsg, "Err-%04X:ѡ���ļ���Ч���ļ�����ԿУ�����", errorCode );
        break;
      case 0x63C1:
        sprintf( this->lastErrorMsg, "Err-%04X:�����,����1�λ���", errorCode );
        break;
      case 0x63C2:
        sprintf( this->lastErrorMsg, "Err-%04X:�����,����2�λ���", errorCode );
        break;
      case 0x63C3:
        sprintf( this->lastErrorMsg, "Err-%04X:�����,����3�λ���", errorCode );
        break;
      case 0x63C4:
        sprintf( this->lastErrorMsg, "Err-%04X:�����,����4�λ���", errorCode );
        break;
      case 0x63C5:
        sprintf( this->lastErrorMsg, "Err-%04X:�����,����5�λ���", errorCode );
        break;
      case 0x63C6:
        sprintf( this->lastErrorMsg, "Err-%04X:�����,����6�λ���", errorCode );
        break;
      case 0x63C7:
        sprintf( this->lastErrorMsg, "Err-%04X:�����,����7�λ���", errorCode );
        break;
      case 0x63C8:
        sprintf( this->lastErrorMsg, "Err-%04X:�����,����8�λ���", errorCode );
        break;
      case 0x63C9:
        sprintf( this->lastErrorMsg, "Err-%04X:�����,����9�λ���", errorCode );
        break;
      case 0x63CA:
        sprintf( this->lastErrorMsg, "Err-%04X:�����,����10�λ���", errorCode );
        break;
      case 0x63CB:
        sprintf( this->lastErrorMsg, "Err-%04X:�����,����11�λ���", errorCode );
        break;
      case 0x63CC:
        sprintf( this->lastErrorMsg, "Err-%04X:�����,����12�λ���", errorCode );
        break;
      case 0x63CD:
        sprintf( this->lastErrorMsg, "Err-%04X:�����,����13�λ���", errorCode );
        break;
      case 0x63CE:
        sprintf( this->lastErrorMsg, "Err-%04X:�����,����14�λ���", errorCode );
        break;
      case 0x6400:
        sprintf( this->lastErrorMsg, "Err-%04X:״̬��־δ�ı�", errorCode );
        break;
      case 0x6581:
        sprintf( this->lastErrorMsg, "Err-%04X:дEEPROM���ɹ�", errorCode );
        break;
      case 0x6700:
        sprintf( this->lastErrorMsg, "Err-%04X:ָ�������Ȳ���ȷ", errorCode );
        break;
      case 0x6900:
        sprintf( this->lastErrorMsg, "Err-%04X:CLA����·����Ҫ��ƥ��", errorCode );
        break;
      case 0x6901:
        sprintf( this->lastErrorMsg, "Err-%04X:��Ч��״̬", errorCode );
        break;
      case 0x6981:
        sprintf( this->lastErrorMsg, "Err-%04X:�������ļ��ṹ������", errorCode );
        break;
      case 0x6982:
        sprintf( this->lastErrorMsg, "Err-%04X:������д���޸Ŀ�Ƭ�İ�ȫ״̬", errorCode );
        break;
      case 0x6983:
        sprintf( this->lastErrorMsg, "Err-%04X:��Կ�Ѿ�������", errorCode );
        break;
      case 0x6984:
        sprintf( this->lastErrorMsg, "Err-%04X:�������Ч,����ȡ�����", errorCode );
        break;
      case 0x6985:
        sprintf( this->lastErrorMsg, "Err-%04X:ʹ������������", errorCode );
        break;
      case 0x6986:
        sprintf( this->lastErrorMsg, "Err-%04X:����������ִ������,��ǰ�ļ�����EF", errorCode );
        break;
      case 0x6987:
        sprintf( this->lastErrorMsg, "Err-%04X:�ް�ȫ���Ļ�MAC��ʧ", errorCode );
        break;
      case 0x6988:
        sprintf( this->lastErrorMsg, "Err-%04X:��ȫ��������MAC����ȷ", errorCode );
        break;
      case 0x6A80:
        sprintf( this->lastErrorMsg, "Err-%04X:�������������", errorCode );
        break;
      case 0x6A81:
        sprintf( this->lastErrorMsg, "Err-%04X:���ܲ�֧�ֻ�����MF��Ƭ�Ѿ�����", errorCode );
        break;
      case 0x6A82:
        sprintf( this->lastErrorMsg, "Err-%04X:��Ƭ���ļ�δ�ҵ�", errorCode );
        break;
      case 0x6A83:
        sprintf( this->lastErrorMsg, "Err-%04X:��Ƭ�м�¼δ�ҵ�", errorCode );
        break;
      case 0x6A84:
        sprintf( this->lastErrorMsg, "Err-%04X:�ļ����㹻�ռ�", errorCode );
        break;
      case 0x6A86:
        sprintf( this->lastErrorMsg, "Err-%04X:����P1P2����", errorCode );
        break;
      case 0x6A87:
        sprintf( this->lastErrorMsg, "Err-%04X:�ް�ȫ����", errorCode );
        break;
      case 0x6A88:
        sprintf( this->lastErrorMsg, "Err-%04X:��Կδ�ҵ�", errorCode );
        break;
      case 0x6B00:
        sprintf( this->lastErrorMsg, "Err-%04X:�ڴﵽLe/Lc�ֽ�֮ǰ�ļ�����,ƫ��������", errorCode );
        break;
      case 0x6D00:
        sprintf( this->lastErrorMsg, "Err-%04X:��Ч��INS", errorCode );
        break;
      case 0x6E00:
        sprintf( this->lastErrorMsg, "Err-%04X:��Ч��CLA", errorCode );
        break;
      case 0x6F00:
        sprintf( this->lastErrorMsg, "Err-%04X:������Ч", errorCode );
        break;
      case 0x6FF0:
        sprintf( this->lastErrorMsg, "Err-%04X:������ϵͳ����", errorCode );
        break;
      case 0xFFFF:
        sprintf( this->lastErrorMsg, "Err-%04X:�޷��жϵĴ���", errorCode );
        break;
      case 0x9210:
        sprintf( this->lastErrorMsg, "Err-%04X:��Ƭ��DF���ڴ治��", errorCode );
        break;
      case 0x9220:
        sprintf( this->lastErrorMsg, "Err-%04X:�ļ�ID�Ѵ���", errorCode );
        break;
      case 0x9240:
        sprintf( this->lastErrorMsg, "Err-%04X:��Ƭ�ڴ�����", errorCode );
        break;
      case 0x9302:
        sprintf( this->lastErrorMsg, "Err-%04X:MAC����", errorCode );
        break;
      case 0x9303:
        sprintf( this->lastErrorMsg, "Err-%04X:Ӧ���ѱ�����", errorCode );
        break;
      case 0x9400:
        sprintf( this->lastErrorMsg, "Err-%04X:û��ѡ��ǰ��EF�ļ�ID", errorCode );
        break;
      case 0x9401:
        sprintf( this->lastErrorMsg, "Err-%04X:����", errorCode );
        break;
      case 0x9402:
        sprintf( this->lastErrorMsg, "Err-%04X:������Χ��û�иü�¼", errorCode );
        break;
      case 0x9403:
        sprintf( this->lastErrorMsg, "Err-%04X:��Կδ�ҵ�", errorCode );
        break;
      case 0x9404:
        sprintf( this->lastErrorMsg, "Err-%04X:EF���ļ�IDû���ҵ�", errorCode );
        break;
      case 0x9406:
        sprintf( this->lastErrorMsg, "Err-%04X:�����MAC������", errorCode );
        break;
      case 0x9802:
        sprintf( this->lastErrorMsg, "Err-%04X:û����Ҫ����Կ", errorCode );
        break;
      case 0x9804:
        sprintf( this->lastErrorMsg, "Err-%04X:��������û�������MAC��", errorCode );
        break;
      case 0x9810:
        sprintf( this->lastErrorMsg, "Err-%04X:��Ӧ�ñ�����", errorCode );
        break;
      case 0x9835:
        sprintf( this->lastErrorMsg, "Err-%04X:û��ȡ�����", errorCode );
        break;
      case 0x9840:
        sprintf( this->lastErrorMsg, "Err-%04X:��KEY����ס", errorCode );
        break;
      default:
        sprintf( this->lastErrorMsg, "Err-%04X:��֪���Ĵ���", errorCode );
        break;
      }
      if ( errorMsg )
      {
        strcpy( errorMsg, this->lastErrorMsg );
      }
      return this->lastErrorMsg;
    }

  } //namespace Card
} //namespace Engine

Engine::Card::CUserCardService* CreateUserCardService()
{
  return new Engine::Card::CCPUUserCardService();
}