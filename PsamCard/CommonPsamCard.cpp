#include <sstream>
#include <string>
#include <vector>

#include "CommonPsamCard.h"
#include "../Commons/Des/des.h"
#include "../Commons/GlobalFunc.h"
#include "../Commons/Logger.h"

using namespace std;
using namespace Engine::Device;
using namespace Engine::Func;

namespace Engine
{
  namespace Card
  {
    CCommonPsamCardService::CCommonPsamCardService()
    {
      hasReset = false;  //δ�ϵ縴λ
	  hasLoadMainKey = false;

      //ѡ���ļ� SELECT���ο����ܴ�PSAM��COS6.0���й�����PSAM��Ӧ�ù淶
      //����Ĺ淶
      //CLA: 00, INS: A4, P1: 00/04, P2: 00, Lc: XX, Data: �ջ��ļ���ʶ����DF����, Le:00
      //P1=00:���ļ���ʶ��ѡ��ѡ��ǰĿ¼�»����ļ�����Ŀ¼�ļ�
      //P1=04:��Ŀ¼��ѡ��ѡ��MF��ǰĿ¼�������뵱ǰĿ¼ƽ����Ŀ¼����ǰĿ¼���¼���Ŀ¼
      apdu.SELECT_MF = "00A4000000";

      ///ѡ���ļ� SELECT���ο����ܴ�PSAM��COS6.0���й�����PSAM��Ӧ�ù淶
      ///��Ŀ¼��ѡ��
      apdu.SELECT = "00A4%s00%02X%s";

      //���������ļ� READ BINARY���ο����ܴ�PSAM��COS6.0���й�����PSAM��Ӧ�ù淶
      //����Ĺ淶
      //CLA: 00, INS: B0, P1: XX, P2:XX, Le: XX
      //��P1�ĸ���λΪ100�������λΪ���ļ���ʶ��P2Ϊ����ƫ����
      //��P1�����λ��Ϊ1����P1P2Ϊ�����ļ���ƫ������P1Ϊƫ�����ĸ��ֽڣ�P2Ϊƫ�����ĵ��ֽڣ������ļ�Ϊ��ǰ�ļ�
      apdu.READ_BINARY = "00B0%02X%02X%02X";

      //ȡ����� EXTERNAL AUTHENTICATE���ο����ܴ�PSAM��COS6.0���й�����PSAM��Ӧ�ù淶
      //����Ĺ淶
      //CLA: 00, INS: 84, P1: 00, P2: 00, Le: 04/08
      //ע�������������ֻ��ȡ4�ֽ�
      apdu.GET_CHALLENGE = "00840000%02X";

      //�����ļ�ϵͳ ERASE MF���ο����ܴ�PSAM��COS6.0���й�����PSAM��Ӧ�ù淶
      //����Ĺ淶
      //CLA: 84, INS: 8A, P1: 01, P2: 00, Le: 04, Data: MAC
      //������ֻ����MF ��ִ�У��̻�Ҫ�������ð�ȫ����
      //ʹ��MF ��������Կ��Ƭ������Կ����MAC������ִ�гɹ��󣬿�Ƭ�ص��׿�״̬
      apdu.ERASE_MF = "848A010004%.8s";

      //�������˻� END ISSUE���ο����ܴ�PSAM��COS6.0���й�����PSAM��Ӧ�ù淶
      //����Ĺ淶
      //CLA: 80, INS: 1C, P1: 00, P2: 00, Lc: 00
      apdu.END_ISSUE = "801C000000";

      //��MAC�������˻� END ISSUE���ο����ܴ�PSAM��COS6.0���й�����PSAM��Ӧ�ù淶
      //����Ĺ淶
      //CLA: 84, INS: 1C, P1: 00, P2: 00, Lc: 04, Data: MAC
      apdu.END_ISSUE_MAC = "841C000004";

      //�������ļ� CREATE MF���ο����ܴ�PSAM��COS6.0���й�����PSAM��Ӧ�ù淶
      //����Ĺ淶
      //CLA: 80, INS: 8A, P1: 00, P2: 00, Lc: 2A, Data:�ļ���Ϣ
      //�ļ���Ϣ����:
      //(2�ֽ�)�ļ���ʶ��
      //(1�ֽ�)�ļ����ͣ�MF ��FTP �̶�Ϊ��10��
      //(1�ֽ�)��MF �´����ļ���Ȩ�ޣ�����ʱ����д��00��
      //(2�ֽ�)RFU FFFF
      //(1�ֽ�)MF ��DIR �ļ���SFI
      //(1�ֽ�)���������ʼ��������
      //(2�ֽ�)MF����Կ�ļ��Ĵ�С
      //(1�ֽ�)Ӧ����������
      //(1�ֽ�) 0x81
      //(3�ֽ�)RFU FFFFFF
      //(1�ֽ�)ָ����������DF ������AID����ʵ���ֽ�����ȡֵ��Χ5-16��ʮ���ƣ�
      //(16�ֽ�)DF ����AID�����ݲ���16 �ֽ�ʱ�����ԡ�FF������16 �ֽڣ����ڿ��ڱ�����Ψһ��
      //(10�ֽ�)MF �µ�������Կ������
      apdu.CREATE_MF = "808A00002A%s";

      //�����ļ� CREATE FILE���ο����ܴ�PSAM��COS6.0���й�����PSAM��Ӧ�ù淶
      //����Ĺ淶
      //CLA: 80, INS: 80, P1: 00, P2: 00, Lc: 08/0C ����EF �ļ� 2A/2E ����DF �ļ�, Data:�ļ���Ϣ
      apdu.CREATE_FILE = "80800000%02X%s";

      //����͸���ļ� UPDATE BINARY���ο����ܴ�PSAM��COS6.0���й�����PSAM��Ӧ�ù淶
      //����Ĺ淶
      //CLA: 00, INS: D6, P1: XX, P2: XX, Lc: XX, Data:д�������
      //��P1��3λΪ100��P1 ��5λΪ�ļ�SFI��ֵΪ1��31����P2 ��ʾƫ����
      //��P1���λΪ0��P1P2 ��ʾƫ�������Ե�ǰ�ļ�����
      apdu.UPDATE_BINARY = "00D6%02X%02X%02X%s";

      //��ɢ��Կ DELIVERY KEY���ο����ܴ�PSAM��COS6.0���й�����PSAM��Ӧ�ù淶
      //����Ĺ淶
      //CLA: 80, INS: 1A, P1: ��Կ��;, P2: ��Կ��ʶ, Lc: 8*N N Ϊ��ɢ������N=0/1/2/3, Data:��ɢ����
      //��Կ��Կ��;�ĸ���λΪ��ɢ����������λΪ��Կ����
      //������Կ���Զ�������:
      //00   ������Կ                      ������Կ��ÿ��DF ֻ��һ��������Կ�������ļ���������Կ����
      //02   ������Կ                      Ӧ����Կ���������ѽ��׵İ�ȫ����
      //05   Ӧ��ά����Կ                  ������Կ�����ڰ�ȫ���ĵļ���
      //06   MAC ��Կ                      Ӧ����Կ��ֻ�ܽ���MAC ����
      //07   ������Կ                      Ӧ����Կ��ֻ�ܽ��м��ܼ���
      //08   MAC��������Կ��������Կ       Ӧ����Կ�����Խ���MAC�����ܼ���
      //09   Ȧ����Կ                      Ӧ����Կ�������ʻ����뽻�׵İ�ȫ���ƣ�
      //0C   TAC ��Կ                      Ӧ����Կ�����ڽ�����TAC �ļ���
      //0D   PIN                           ������Կ���ֿ��˸�������
      //0E   �ⲿ��֤��Կ                  ������Կ���ⲿ��֤����ʹ�õ�ȱʡ��Կ�ࡣ
      //0F   �ڲ���֤��Կ                  ������Կ���ڲ���֤����ʹ�õ�ȱʡ��Կ��
      //1E                                 Ӧ����Կ��������Կ������ĸ����
      apdu.DELIVERY_KEY = "801A%s%s%02X%s";

      //�������� CIPHER DATA���ο����ܴ�PSAM��COS6.0���й�����PSAM��Ӧ�ù淶
      //����Ĺ淶
      //CLA: 80, INS: FA, P1: 00/01/02/03/05/07, P2: 00, Lc: XX, Data:�����������, Le: XX
      //P1:00 �� �޺��������
      //P1:01 �� ���һ��MAC ����
      //P1:02 �� �к��������
      //P1:03 �� ��һ��MAC ����
      //P1:05 �� Ψһһ��MAC ����
      //P1:07 �� ��һ��MAC ����
      apdu.CIPHER_DATA = "80FA%s00%02X%s";

      //���ѽ��׳�ʼ�� INIT SAM FOR PURCHASE���ο����ܴ�PSAM��COS6.0���й�����PSAM��Ӧ�ù淶
      //����Ĺ淶
      //CLA: 80, INS: 70, P1: 00, P2: 00, Lc: 14��8��N��N��1��2��3��, Data:�����������, Le: 08
      apdu.INIT_SAM_FOR_PURCHASE = "80700000%02X%s";

    }

    int CCommonPsamCardService::GetChallenge( uint8 *randomNumber )
    {
      int ret;
      uint8 hexCommand[255] = {0};
      memset( randomNumber, 0, 8 );
      string command = this->GetCommand( apdu.GET_CHALLENGE, 8 );
      CpuCmd cmd;

      ret = this->ExecuteHexCpuCmd( command, &cmd );
      if ( ret )
      {
        return ret;
      }
      memcpy( randomNumber, cmd.receiveBuffer, 8 );

      return SUCCESS;
    }

    int CCommonPsamCardService::ReadPsamTermNo( char *termNo )
    {
      int ret = 0;
      uint8 binaryBuffer[9] = {0};
      char psamTermNo[13] = {0};    //psam�ն˱�ţ������ݴ������������

      ret = this->ResetPsam();
      if ( ret )
        return ret;

      ret = this->SelectADFByFI( MF );
      if ( ret )
        return ret;

      ret = ReadBinaryFile( 0x16, 0, 6, binaryBuffer );
      if ( ret )
      {
        return ret;
      }
      bcd2asc( binaryBuffer, 6, psamTermNo );
      strcpy( termNo, psamTermNo );

      return SUCCESS;
    }

    int CCommonPsamCardService::SelectADFByFI( string FI )
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

    int CCommonPsamCardService::ReadBinaryFile( int SFI, int offset, int len, uint8 *binaryBuffer )
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

    char* CCommonPsamCardService::GetErrorMsgByErrorCode( int errorCode, char *errorMsg )
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

    int CCommonPsamCardService::RecyclePsamCard()
    {
      int ret = 0;
      uint8 data[255] = {0};
      int dataLen = 0;
      uint8 rbuff[255] = {0};

      string command;
      CpuCmd cmd;
      uint8 random[8] = {0};

      ret = this->ResetPsam( 1 );
      if ( ret )
        return ret;

      ret = this->GetChallenge( random );
      if ( ret )
        return ret;

      //����MAC
      command = "848A010004800000";
      hex2dec( command.c_str(), command.size(), data, dataLen );
      for ( int i = 0; i < 8; i++ )
        data[i] = data[i] ^ random[i];
      //����
      des3_context ctx3;
      uint8 key[16];
      uint8 encryptedMac[255] = {0};
      memcpy( key, "\x01\x02\x03\x04\x05\x06\x07\x08\x11\x12\x13\x14\x15\x16\x17\x18", 16 );
      des3_set_2keys( &ctx3, key, key + 8 );
      des3_encrypt( &ctx3, data, encryptedMac );

      char hexMac[9] = {0};
      dec2hex( encryptedMac, 4, hexMac );
      command = this->GetCommand( apdu.ERASE_MF, hexMac );          //����MF
      ret = this->ExecuteHexCpuCmd( command, &cmd );
      if ( ret )
      {
        if ( cmd.retCode == 0x6985 )
        {
          SET_ERR( "��PSAM���ѻ���" );
        }
        return ret;
      }

      return SUCCESS;
    }

    int CCommonPsamCardService::CheckPsamCardInfo( PsamCard* psamCard )
    {
      if ( strlen( psamCard->startDate ) != 8 )
      {
        SET_ERR( "��ʼ���ڳ��ȴ���" );
        return CARD_PSAM_DATE_LENGTH_ERR;
      }
      if ( strlen( psamCard->expireDate ) != 8 )
      {
        SET_ERR( "��Ч���ڳ��ȴ���" );
        return CARD_PSAM_DATE_LENGTH_ERR;
      }
      if ( psamCard->keyIndex > MAX_KEY_INDEX )
      {
        SET_ERR( "��Կ�������ܴ���" << MAX_KEY_INDEX );
        return CARD_PSAM_KEY_INDEX_ERR;
      }

      return SUCCESS;
    }

	int CCommonPsamCardService::SetMainKey( char mainKey[33] )
	{
		strcpy(this->mainKey,mainKey);
		hasLoadMainKey = true;
		return SUCCESS;
	}

	void CCommonPsamCardService::GenerateWorkKey(CPUCardKeys *cardKeys)
	{
		int len;
		uint8 externKey[17];
		uint8 appKey[17];
		memset(externKey,0x00,sizeof(externKey));
		memset(appKey,0x00,sizeof(appKey));
		hex2dec( this->mainKey, 32, externKey, len );
		for ( int i = 0; i < 16; i++ )
		{
			appKey[i] = ~externKey[15-i];
		}
		memcpy(cardKeys->DACK,appKey,16);    //Ӧ��������Կ
		memcpy(cardKeys->DAMK,appKey,16);    //ά����Կ
		memcpy(cardKeys->DACK1,externKey,16);  //�ⲿ��֤��Կ

		memcpy(cardKeys->DPK1,appKey,16);  
		memcpy(cardKeys->DPK2,appKey,16);    
		memcpy(cardKeys->DUK,appKey,16);    
		memcpy(cardKeys->DRPK,appKey,16);    
		memcpy(cardKeys->DTAC,appKey,16);   
	}

    int CCommonPsamCardService::InitPsamCard( PsamCard* psamCard )
    {
	  if(HasLoadMainKey()==false)
	  {
		  SET_ERR( "δ���ظ���Կ,ret=" << ltohex( CARD_UNLOAD_MAIN_KEY ) );
		  return CARD_UNLOAD_MAIN_KEY;
	  }
      int ret = 0;
      char hexKey[64] = {0};
      string command;
      CpuCmd cmd;
      vector<string> cmdList;

	  CPUCardKeys cardKeys;
	  memset( &cardKeys, 0, sizeof( cardKeys ) );
	  GenerateWorkKey(&cardKeys);
	  
      ret = this->CheckPsamCardInfo( psamCard );
      if ( ret )
      {
        return ret;
      }

      psamCard->keyIndex = 1;

      //��λ�ϵ�
      ret = this->ResetPsam( 1 );
      if ( ret )
      {
        return ret;
      }

      //����MF
      string data = "3F001000FFFF0A00012AFF81FFFFFF00315041592E5359532E4444463031FFFF00180100010002030300";
      command = this->GetCommand( apdu.CREATE_MF, data.c_str() );
      ret = this->ExecuteHexCpuCmd( command, &cmd );
      if ( ret )
      {
        if ( cmd.retCode == 0x6985 )
        {
          SET_ERR( "�ÿ��ѷ���,ret=" << ltohex( cmd.retCode ) );
        }
        return ret;
      }

      //����EF 0001
      data = "0001340000400002";
      command = this->GetCommand( apdu.CREATE_FILE, 8, data.c_str() );
      cmdList.push_back( command );

      //������Ƭ������Ϣ�ļ�0015
      data = "00153100000E0002";
      command = this->GetCommand( apdu.CREATE_FILE, 8, data.c_str() );
      cmdList.push_back( command );

      //���¹�����Ϣ�ļ�0015
      data = lpad( psamCard->psamCardNo, 20, '0' );   //PSAM���к�
      data.append( ltohex( psamCard->cardVersion ) ); //PSAM�汾��
      data.append( ltohex( psamCard->cardType ) );    //��Կ������
      data.append( "01" );                            //ָ��汾
      data.append( "00" );                            //FCI����
      //149==10010101,��SFIΪ10101=0x15
      command = this->GetCommand( apdu.UPDATE_BINARY, 149, 0, 14, data.c_str() );
      cmdList.push_back( command );

      //�����ն���Ϣ�ļ�0016
      data = "00163100000600FF";
      command = this->GetCommand( apdu.CREATE_FILE, 8, data.c_str() );
      cmdList.push_back( command );

      //����������Ϣ�ļ�0017
      data = "0017310000260002";
      command = this->GetCommand( apdu.CREATE_FILE, 8, data.c_str() );
      cmdList.push_back( command );

      //���¹�����Ϣ�ļ�0017
      command = "00D6970019";
      data = ltohex( psamCard->keyIndex );                  //������Կ������
      data.append( lpad( psamCard->publishId, 16, '0' ) );  //Ӧ�÷����߱�ʶ
      data.append( lpad( psamCard->userId, 16, '0' ) );     //Ӧ�ý����߱�ʶ
      data.append( psamCard->startDate );                   //Ӧ����������
      data.append( psamCard->expireDate );                  //Ӧ����Ч����
      //151==10010111,��SFIΪ10111=0x17
      command = this->GetCommand( apdu.UPDATE_BINARY, 151, 0, 25, data.c_str() );
      cmdList.push_back( command );

      //����0016��д���ն˻����
      data = string( lpad( psamCard->termId, 12, '0' ) );
      //150==10010110,��SFIΪ10110=0x16
      command = this->GetCommand( apdu.UPDATE_BINARY, 150, 0, 6, data.c_str() );
      cmdList.push_back( command );

      //д�ⲿ��֤��Կ1
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( cardKeys.DACK1, 16, hexKey );
      command = "80D000001A0E180000010002030300";
      command += hexKey;
      cmdList.push_back( command );

      //����DF02
      data = "DF022000FFFF0AFF01C80081FFFFFF00D156000001BDF0CACBB4EFD6A7B8B6FF00180100010002030300";
      command = this->GetCommand( apdu.CREATE_FILE, 42, data.c_str() );
      cmdList.push_back( command );

      //ѡ��DF02
      cmdList.push_back( this->GetCommand( apdu.SELECT, "00", 2, "DF02" ) );

      //������������ļ�0019
      data = "00193100000400FF";
      command = this->GetCommand( apdu.CREATE_FILE, 8, data.c_str() );
      cmdList.push_back( command );

      //д�ն˽�������ļ�
      //153==10011001,��SFIΪ11001=0x19
      command = this->GetCommand( apdu.UPDATE_BINARY, 153, 0, 4, "00000000" );
      cmdList.push_back( command );

      //�ⲿ��֤��Կ1
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( cardKeys.DACK1, 16, hexKey );
      command = "80D000001A27180100010010030300";
      command += hexKey;
      cmdList.push_back( command );

      //������Կ1
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( cardKeys.DPK1, 16, hexKey );
      command = "80D000001A22180100010010030300";
      command += hexKey;
      cmdList.push_back( command );

      //������Կ2
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( cardKeys.DPK2, 16, hexKey );
      command = "80D000001A22180200010010030300";
      command += hexKey;
      cmdList.push_back( command );

      //16�ֽ�Ӧ��ά������Կ
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( cardKeys.DAMK, 16, hexKey );
      command = "80D000001A28180100010010030300";
      command += hexKey;
      cmdList.push_back( command );

      //16�ֽڿ����������Կ
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( cardKeys.DUK, 16, hexKey );
      command = "80D000001A28180200010010030300";
      command += hexKey;
      cmdList.push_back( command );

      //16�ֽڿ�����װ����Կ
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( cardKeys.DRPK, 16, hexKey );
      command = "80D000001A28180300010010030300";
      command += hexKey;
      cmdList.push_back( command );

      //16�ֽ��ڲ���֤����Կ
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( cardKeys.DTAC, 16, hexKey );
      command = "80D000001A28180400010010030300";
      command += hexKey;
      cmdList.push_back( command );

      cmdList.push_back( "END_ISSUE" ); //DF02�������˻�

      cmdList.push_back( apdu.SELECT_MF ); //ѡ��MF

      cmdList.push_back( "END_ISSUE" );   //MF�������˻�

      for ( int i = 0; i < cmdList.size(); i++ )
      {
        command = cmdList[i];
        if ( strcmp( "END_ISSUE", command.c_str() ) == 0 )
        {
          ret = EndIssue();
        }
        else
        {
          ret = this->ExecuteHexCpuCmd( command, &cmd );
        }
        if ( ret )
        {
          return ret;
        }
      }

      return this->ResetPsam();
    }

    int CCommonPsamCardService::EndIssue()
    {
      CpuCmd cmd;
      int ret = this->ExecuteHexCpuCmd( apdu.END_ISSUE.c_str(), &cmd );
      if ( ret )
      {
        if ( 0x6E00 == cmd.retCode )  //6E00===>CLA����
        {
          ret = EndIssueWithMac();    //����ʹ�ô�MAC��ʽ�������˻�
        }
      }
      return ret;
    }

    int CCommonPsamCardService::EndIssueWithMac()
    {
      int ret;
      string command;
      uint8 random[8] = {0};
      uint8 data[256];
      uint8 key[16];
      int dataLen = 0;
      uint8 mac[4];
      char hexMac[9] = {0};
      memcpy( key, "\x01\x02\x03\x04\x05\x06\x07\x08\x11\x12\x13\x14\x15\x16\x17\x18", 16 );

      //ȡ�����
      ret = this->GetChallenge( random );
      if ( ret )
        return ret;

      //����ָ��MAC
      command = apdu.END_ISSUE_MAC; //��MAC�������˻�
      hex2dec( command.c_str(), command.size(), data, dataLen );
      CalculateMac( random, key, 2, data, dataLen, mac );
      dec2hex( mac, 4, hexMac );

      command.append( hexMac );

      CpuCmd cmd;
      return this->ExecuteHexCpuCmd( command, &cmd );
    }

    void CCommonPsamCardService::CalculateMac( uint8 *randomNumber, uint8 *key, int keyLen, const uint8 *data, int dataLen, uint8 *mac )
    {
      int i = 0;
      des_context ctx;
      uint8 temp[16];
      // ��ʼ��MAC�����ֵ ==> temp
      memset( temp, 0, sizeof( temp ) );
      memcpy( temp, randomNumber, 8 * sizeof( uint8 ) );
      // ��ʽ������ֵ: �� "80 00 00 ...", ��8�ֽڳ��� ==> buf
      int block = 0;
      if ( dataLen % 8 )
        block = dataLen / 8 + 1; // �ܿ���
      else
        block = dataLen / 8;

      uint8* buf = new uint8[block * 8 + 1];
      memset( buf, 0, block * 8 * sizeof( uint8 ) );
      memcpy( buf, data, dataLen * sizeof( uint8 ) );
      buf[dataLen] = 0x80;
      des_set_key( &ctx, key );
      for ( i = 0; i < block; i++ )
      {
        // temp .EOR. block[i] ==> temp
        for ( int j = 0; j < 8; j++ )
        {
          temp[j] ^= buf[i * 8 + j];
        }
        // temp ==> DES ==> temp
        des_encrypt( &ctx, temp, temp );
        if ( keyLen == 3 )
        {
          des_set_key( &ctx, key + 8 );
          des_decrypt( &ctx, temp, temp );
          des_set_key( &ctx, key );
          des_encrypt( &ctx, temp, temp );
        }
      }
      delete []buf;

      if ( keyLen == 2 )
      {
        des_set_key( &ctx, key + 8 );
        des_decrypt( &ctx, temp, temp );
        des_set_key( &ctx, key );
        des_encrypt( &ctx, temp, temp );
      }
      // temp ����λ ==> MAC
      memcpy( mac, temp, 4 * sizeof( uint8 ) );
    }

    int CCommonPsamCardService::CalculateSafeMac( char* phyid, char* hexRandom, const char *macCommand, char *mac )
    {
      string command;
      string macData;
      string data;
      vector<string> cmdList;
      int ret;

      ret = this->ResetPsam();
      if ( ret )
        return ret;

      ret = this->SelectADFByFI( "DF02" );
      if ( ret )
        return ret;

      //ʹ�����������еķ�ɢ���Ӷ�ָ������Կ���з�ɢ����Ϊ��ʱ��Կ����ڿ���
      data = string( phyid );
      if ( strlen( phyid ) == 8 )
        data += "80000000";
      //��Կ��; 28=00101000����ɢ����Ϊ001����Կ����Ϊ01000����08(MAC��������Կ��������Կ)�����Խ���MAC�����ܼ���
      //��Կ��ʶ01
      command = this->GetCommand( apdu.DELIVERY_KEY, "28", "01", 8, data.c_str() );
      cmdList.push_back( command );

      //���������ԭʼ������Ϊ����Դ����MAC����
      char temp[512] = {0};
      int macCommandLen = strlen( macCommand );
      memcpy( temp, macCommand, macCommandLen );
      macCommandLen /= 2;
      if ( macCommandLen % 8 )
        memcpy( temp + macCommandLen * 2, "8000000000000000", ( 8 - macCommandLen % 8 ) * 2 );
      else
        memcpy( temp + macCommandLen * 2, "8000000000000000", 16 );

      macData = hexRandom;
      macData += temp;

      int dataLen = macData.size() / 2;
      const char *pData = macData.c_str();
      int maxBlockLen = 0x30;   //���δ���������󳤶�

      int n = dataLen / maxBlockLen;
      if ( dataLen % maxBlockLen )    //������������ݵĳ���%���δ���������󳤶Ȳ�Ϊ0�Ļ�����ô��Ҫ����Ĵ���++1
      {
        n += 1;
      }

      if ( n == 1 ) //05 Ψһһ��MAC ����
      {
        command = this->GetCommand( apdu.CIPHER_DATA, "05", dataLen, macData.c_str() );
        cmdList.push_back( command );
      }
      else
      {
        for ( int i = 0; i < n; i++ )
        {
          data = "";
          if ( i == 0 )          //��һ��MAC����
          {
            data.append( pData, maxBlockLen * 2 );
            command = this->GetCommand( apdu.CIPHER_DATA, "07", maxBlockLen, data.c_str() );
          }
          else if ( i == n - 1 ) //���һ��MAC����
          {
            data.append( pData, dataLen * 2 );
            command = this->GetCommand( apdu.CIPHER_DATA, "01", dataLen, data.c_str() );
          }
          else                   //��һ��MAC����
          {
            data.append( pData, maxBlockLen * 2 );
            command = this->GetCommand( apdu.CIPHER_DATA, "03", maxBlockLen, data.c_str() );
          }

          cmdList.push_back( command );
          dataLen -= maxBlockLen;
          pData += maxBlockLen * 2;
        }
      }

      CpuCmd cmd;
      for ( int i = 0; i < cmdList.size(); i++ )
      {
        command = cmdList[i];
        ret = this->ExecuteHexCpuCmd( command, &cmd );
        if ( ret )
        {
          return ret;
        }
      }
      dec2hex( cmd.receiveBuffer, 4, mac );

      return SUCCESS;
    }

    int CCommonPsamCardService::CalculateSafeMac( char *appSerialNo, int keyVersion, char* hexRandom, const char *macCommand, char *mac )
    {
      return SYS_UNSUPPORTED_FUNC;
    }

    int CCommonPsamCardService::EncryptRandom( char* phyid, int keyIndex, const char *hexRandom, char *encryptedRandom )
    {
      CpuCmd cmd;
      string command;
      string data;
      int ret;

      ret = this->ResetPsam();
      if ( ret )
        return ret;

      ret = this->SelectADFByFI( "DF02" );
      if ( ret )
        return ret;

      data = string( phyid );
      if ( strlen( phyid ) == 8 )
        data += "80000000";
      //��Կ��; 27=00100111����ɢ����Ϊ001����Կ����Ϊ00111����07(������Կ)��ֻ�ܽ��м��ܼ���
      command = this->GetCommand( apdu.DELIVERY_KEY, "27", ltohex( keyIndex ).c_str(), 8, data.c_str() );
      ret = this->ExecuteHexCpuCmd( command, &cmd );
      if ( ret )
      {
        return ret;
      }

      //���������ԭʼ������Ϊ����Դ����MAC����
      char temp[512] = {0};
      int hexRandomLen = strlen( hexRandom );
      memcpy( temp, hexRandom, hexRandomLen );
      hexRandomLen /= 2;
      if ( hexRandomLen % 8 )
        memcpy( temp + hexRandomLen * 2, "8000000000000000", ( 8 - hexRandomLen % 8 ) * 2 );
      /*
      else
        memcpy( temp + hexRandomLen * 2, "8000000000000000", 16 );
      */

      data = string( temp );
      int dataLen = data.size() / 2;

      //05 Ψһһ��MAC ����
      //command = this->GetCommand( apdu.CIPHER_DATA, "05", dataLen, data.c_str() );
      command = this->GetCommand( apdu.CIPHER_DATA, "00", dataLen, data.c_str() );  //�޺��������
      ret = this->ExecuteHexCpuCmd( command, &cmd );
      if ( ret )
      {
        return ret;
      }
      dec2hex( cmd.receiveBuffer, 8, encryptedRandom );

      return SUCCESS;
    }

    int CCommonPsamCardService::ReadPsamCard( PsamCard* psamCard )
    {
      int ret;
      string command;
      uint8 binaryBuffer[256] = {0};
      uint8 fieldValue[256] = {0};
      char temp[256] = {0};
      int i;

      ret = this->ResetPsam( 1 );
      if ( ret )
        return ret;

      ret = this->SelectADFByFI( MF );
      if ( ret )
      {
        SET_ERR( "ѡ��Ŀ¼MFʧ��" );
        return ret;
      }

      memset( binaryBuffer, 0, sizeof( binaryBuffer ) );
      ret = ReadBinaryFile( 0x15, 0, 14, binaryBuffer );
      if ( ret )
      {
        return ret;
      }

      memset( fieldValue, 0, sizeof( fieldValue ) );
      memcpy( fieldValue, binaryBuffer, 10 ); //psamCardNo

      if ( fieldValue[0] != 0xFF )
      {
        bcd2asc( fieldValue, 10, temp );
        for ( i = 0; i < 19; i++ )
        {
          if ( temp[i] != '0' )
            break;
        }
        strcpy( psamCard->psamCardNo, temp + i );

        psamCard->cardVersion = ( int )binaryBuffer[10];
        psamCard->cardType = ( int )binaryBuffer[11];
      }

      //��ȡ�ն˱��
      memset( binaryBuffer, 0, sizeof( binaryBuffer ) );
      ret = ReadBinaryFile( 0x16, 0, 6, binaryBuffer );
      if ( ret )
      {
        return ret;
      }
      memset( psamCard->termId, 0, sizeof( psamCard->termId ) );
      bcd2asc( binaryBuffer, 6, psamCard->termId );

      memset( binaryBuffer, 0, sizeof( binaryBuffer ) );
      ret = ReadBinaryFile( 0x17, 0, 25, binaryBuffer );
      if ( ret )
      {
        return ret;
      }

      memset( fieldValue, 0, sizeof( fieldValue ) );
      memcpy( fieldValue, binaryBuffer, 1 );  //keyIndex
      psamCard->keyIndex = atoi( ( const char* )fieldValue );

      memset( fieldValue, 0, sizeof( fieldValue ) );
      memcpy( fieldValue, binaryBuffer + 1, 8 ); //publishId
      memset( temp, 0, sizeof( temp ) );
      bcd2asc( fieldValue, 8, temp );

      for ( i = 0; i < 15; i++ )
      {
        if ( temp[i] != '0' )
          break;
      }
      strcpy( psamCard->publishId, temp + i );

      memset( fieldValue, 0, sizeof( fieldValue ) );
      memcpy( fieldValue, binaryBuffer + 9, 8 ); //userId
      memset( temp, 0, sizeof( temp ) );
      bcd2asc( fieldValue, 8, temp );

      for ( i = 0; i < 15; i++ )
      {
        if ( temp[i] != '0' )
          break;
      }
      strcpy( psamCard->userId, temp + i );

      memset( fieldValue, 0, sizeof( fieldValue ) );
      memcpy( fieldValue, binaryBuffer + 17, 4 ); //startDate
      bcd2asc( fieldValue, 4, psamCard->startDate );

      memset( fieldValue, 0, sizeof( fieldValue ) );
      memcpy( fieldValue, binaryBuffer + 21, 4 ); //expireDate
      bcd2asc( fieldValue, 4, psamCard->expireDate );

      ret = this->SelectADFByFI( "DF02" );
      if ( ret )
      {
        SET_ERR( "ѡ��Ŀ¼DF02ʧ��" );
        return ret;
      }
      memset( binaryBuffer, 0, sizeof( binaryBuffer ) );
      ret = ReadBinaryFile( 0x19, 0, 4, binaryBuffer );
      if ( ret )
      {
        return ret;
      }

      memset( fieldValue, 0, sizeof( fieldValue ) );
      memcpy( fieldValue, binaryBuffer, 4 ); //termSeqNo

      for ( i = 0; i < 4; i++ )
        if ( fieldValue[i] == 0xFF )
          fieldValue[i] = 0;
      psamCard->termSeqNo = get_n_byte_int<int, 4>( fieldValue, sizeof( fieldValue ) );

      return SUCCESS;
    }

    int CCommonPsamCardService::ReadMainKey( uint8 *mainKey )
    {
      return SYS_UNSUPPORTED_FUNC;
    }

    int CCommonPsamCardService::EncryptData( char *appSerialNo, int keyVersion, uint8 *random, uint8 *data, int dataLen, uint8 *encryptedData, int& encryptedDataLen  )
    {
      return SYS_UNSUPPORTED_FUNC;
    }

    int CCommonPsamCardService::SuccessResetPsam()
    {
		return this->SelectADFByFI( MF );
    }

  }
}

Engine::Card::CPsamCardService* CreatePsamCardService()
{
  return new Engine::Card::CCommonPsamCardService();
}