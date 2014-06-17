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
      //取随机数 EXTERNAL AUTHENTICATE，参考FMCOS2.0
      //命令报文规范
      //CLA: 00, INS: 84, P1: 00, P2: 00, Le: 04/08
      //注意国民技术读卡器只能取4字节
      apdu.GET_CHALLENGE = "00840000%02X";

      //外部认证 EXTERNAL AUTHENTICATE，参考FMCOS2.0
      //命令报文规范
      //CLA: 00, INS: 82, P1: 00, P2: 外部认证密钥标识号, Lc: 08, Data: 8字节加密后的随机数
      //P2=00 表示没有信息被给出在发出命令之前引用的密钥为已知或在数据字段中提供
      apdu.EXTERNAL_AUTHENTICATE = "008200%02X08%.16s";

      //擦除目录文件 ERASE DF，参考FMCOS2.0
      //命令报文规范
      //CLA: 80, INS: 0E, P1: 00, P2: 00, Lc: 00
      apdu.ERASE_DF = "800E000000";

      //选择文件 SELECT，参考FMCOS2.0
      //命令报文规范
      //CLA: 00, INS: A4, P1: 00/04, P2: 00, Lc: XX, Data: 空或文件标识符或DF名称, Le:00
      //P1=00:用文件标识符选择，选择当前目录下基本文件或子目录文件
      //P1=04:用目录名选择，选择MF或当前目录本身，或与当前目录平级的目录，或当前目录的下级子目录
      apdu.SELECT_MF = "00A4000000";

      ///选择文件 SELECT，参考FMCOS2.0
      ///按目录名选择
      apdu.SELECT = "00A4%s00%02X%s";

      //建立文件 CREATE FILE，参考FMCOS2.0
      //命令报文规范
      //CLA: 80, INS: E0, P1P2: 文件标识 File ID, Lc: XX, Data: 文件控制信息和DF名称
      apdu.CREATE_FILE = "80E0%s%02X%s";

      //增加或修改密钥 WRITE KEY，参考FMCOS2.0
      //命令报文规范
      //CLA: 80/84, INS: D4, P1: 01/XX, P2: 密钥标识, Lc: XX, Data: 不同类型密钥该字段格式不一样
      //P1=00:表示此条WRITE KEY命令用来增加密钥
      //P1=XX:表示此条WRITE KEY命令用来更新P1指定类型的密钥
      apdu.WRITE_KEY = "80D4%s%s%s%s";

      //写二进制文件 UPDATE BINARY，参考FMCOS2.0
      //命令报文规范
      //CLA: 00/04, INS: D6/D0, P1: XX, P2: XX, Lc: XX, Data: 写入的数据
      apdu.UPDATE_BINARY_MAC = "04D6%02X%02X%02X%s";

      //验证口令 VERIFY PIN，参考FMCOS2.0
      //命令报文规范
      //CLA: 00, INS: 20, P1: 00, P2: 口令密钥标识符, Lc: 02-06, Data: 外部输入的口令密钥
      apdu.VERIFY_PIN = "002000%s%02X%s";

      //圈存初始化 INITIALIZE FOR LOAD，参考FMCOS2.0
      //命令报文规范
      //CLA: 80, INS: 50, P1: 00, P2: 01/02, Lc: 0B, Data: 密钥标识符(1字节)+交易金额(4字节)+终端机编号(6字节), Le:10
      //P1=01:表示此条命令用于电子存折
      //P1=02:表示此条命令用于电子钱包
      //报文执行成功之后的响应数据格式
      //电子存折或电子钱包旧余额(4字节)+电子存折或电子钱包联机交易序号(2字节)
      //+密钥版本号(1字节)+算法标识(1字节)+伪随机数(4字节)+MAC1(4字节)
      apdu.INITIALIZE_FOR_LOAD = "805000%s0B%s10";

      //圈存命令 DEBIT FOR CAPP PURCHASE，参考FMCOS2.0
      //命令报文规范
      //CLA: 80, INS: 52, P1: 00, P2: 00, Lc: 0B, Data: 交易日期(4字节)+交易时间(3字节)+MAC2(4字节), Le: 04
      apdu.CREDIT_FOR_LOAD = "805200000B%s04";

      //消费初始化 INITIALIZE FOR PURCHASE，参考FMCOS2.0
      //命令报文规范
      //CLA: 80, INS: 50, P1: 01, P2: 01/02, Lc: 0B, Data: 密钥标识符(1字节)+交易金额(4字节)+终端机编号(6字节), Le:0F
      //P1=01:表示此条命令用于电子存折
      //P1=02:表示此条命令用于电子钱包
      //报文执行成功之后的响应数据格式
      //电子存折或电子钱包旧余额(4字节)+电子存折或电子钱包联机交易序号(2字节)
      //+透支限额(3字节)+密钥版本号(1字节)+算法标识(1字节)+伪随机数(4字节)
      apdu.INITIALIZE_FOR_PURCHASE = "805001%s0B%s0F";

      //消费命令 DEBIT FOR CAPP PURCHASE，参考FMCOS2.0
      //命令报文规范
      //CLA: 80, INS: 54, P1: 01, P2: 00, Lc: 0F, Data: 终端交易序号(4字节)+交易日期(4字节)+交易时间(3字节)+MAC1(4字节), Le: 08
      apdu.DEBIT_FOR_CAPP_PURCHASE = "805401000F%s08";

      //读余额 GET BALANCE，参考FMCOS2.0
      //命令报文规范
      //CLA: 80, INS: 5C, P1: 00, P2: 01/02, Le: 04
      apdu.GET_BALANCE = "805C00%s04";

      //读记录文件 READ RECORD，参考FMCOS2.0
      //命令报文规范
      //CLA: 00/04, INS: B2, P1: 记录号, P2:记录标识 , Le: 00/XX
      //Le=00表示读取整条记录
      //Le=XX表示读取XX字节长度的记录
      //P2的4-8位为短文件标识符
      apdu.READ_RECORD = "00B2%02X%02X%02X";

      //读二进制文件 READ BINARY，参考FMCOS2.0
      //命令报文规范
      //CLA: 00, INS: B0, P1: XX, P2:XX, Le: XX
      //若P1的高三位为100，则低五位为短文件标识，P2为读的偏移量
      //若P1的最高位不为1，则P1P2为欲读文件的偏移量，P1为偏移量的高字节，P2为偏移量的低字节，所读文件为当前文件
      apdu.READ_BINARY = "00B0%02X%02X%02X";

      //带MAC读取二进制文件 READ BINARY，参考FMCOS2.0
      //命令报文规范
      //CLA: 04, INS: B0, P1: XX, P2:XX, Lc XX, Data mac校验码,Le: XX
      //若P1的高三位为100，则低五位为短文件标识，P2为读的偏移量
      //若P1的最高位不为1，则P1P2为欲读文件的偏移量，P1为偏移量的高字节，P2为偏移量的低字节，所读文件为当前文件
      apdu.READ_BINARY_MAC = "04B0%02X%02X04";

	  int len;
	  hex2dec( "22222222222222222222222222222222", 32, defaultCardKeys.DAMK, len );   //维护密钥
	  hex2dec( "33333333333333333333333333333333", 32, defaultCardKeys.DACK, len );   //应用主控密钥
	  hex2dec( "44444444444444444444444444444444", 32, defaultCardKeys.DACK1, len );  //外部认证1
	  hex2dec( "44444444444444444444444444444444", 32, defaultCardKeys.DACK2, len );  //外部认证2
	  hex2dec( "44444444444444444444444444444444", 32, defaultCardKeys.DACK3, len );  //外部认证3
    }

    int CCPUUserCardService::InitCardDefintion()
    {
      FieldInfo fieldInfo;
      memset( &fieldInfo, 0, sizeof( fieldInfo ) );
      fieldInfo.unused = 1;
      //先初始化为0
      for ( int i = 0; i <= CF_END; i++ )
        fieldList.push_back( fieldInfo );

      UserInfo	userInfo;
	  CardInfo cardInfo;

	  //卡号
      fieldList[CF_CARDNO].unused = 0;
      fieldList[CF_CARDNO].offset = userInfo.cardNo - ( uint8* )&userInfo;
      fieldList[CF_CARDNO].length = sizeof( userInfo.cardNo );
      fieldList[CF_CARDNO].fid = 0x15;
      fieldList[CF_CARDNO].type = FT_ANSI;

      //二磁道信息
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
      currentDF = ""; //寻卡成功之后先清空当前选择目录
      return this->ResetCpuCard();
    }

    int CCPUUserCardService::CheckCardType( int cardType )
    {
      switch ( cardType )
      {
      case CPUCARD:
      case FIXCARD: //支持CPU卡
        return SUCCESS;
      case MFCARD:
      default:
        SET_ERR( "不支持的卡类型" );
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
        SET_ERR( "CPU卡上电复位错误,ret[" << ret << "]" );
        return ret;
      }
      currentDF = MF; //CPU卡上电复位之后默认选择MF目录

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
      //用默认主控密钥进行外部认证
      ret = this->ExternalAuthenticate( externalKey,1 );
      if ( ret )
      {
        if ( 0x6A88 != ret )
        {
          if ( 0x63 == ret >> 8 )
          {
            //尝试使用出厂密钥进行认证
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
          //6A88代表密钥未找到，说明该卡已被回收
          return SUCCESS;
        }
      }

      //擦除目录文件
      ret = EraseDF();
      if ( ret )
      {
        SET_ERR( "回收CPU部分失败" );
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
        SET_ERR( "卡物理ID[" << this->phyid << "]长度错误" );
        return CARD_PHYID_LENGTH_ERR;
      }

      int len = 0;
	  Engine::Func::hex2dec( this->phyid, 16, phyidStr, len );
	  
	  //TODO
      //密钥卡中的密钥组
      //CPUCardKeys *cardKeys = this->keyCardService->GetCPUCardKeys();
	  CPUCardKeys cardKeys;
	  memset( &cardKeys, 0, sizeof( cardKeys ) );
	  
	  hex2dec( "33333333333333333333333333333333", 32, cardKeys.DACK, len );
	  hex2dec( "22222222222222222222222222222222", 32, cardKeys.DAMK, len );
	  hex2dec( "44444444444444444444444444444444", 32, cardKeys.DACK1, len );

      //当前用户卡中的密钥组
      CPUCardKeys localCardKeys;
      memset( &localCardKeys, 0, sizeof( localCardKeys ) );

      //PbocDiverKey( phyidStr, cardKeys.DPK1, localCardKeys.DPK1 );			  //消费1
      //PbocDiverKey( phyidStr, cardKeys.DPK1, localCardKeys.DPK2 );			  //消费2
      //PbocDiverKey( phyidStr, cardKeys.DLK1, localCardKeys.DLK1 );			  //圈存1
      //PbocDiverKey( phyidStr, cardKeys.DLK2, localCardKeys.DLK2 );			  //圈存2
      //PbocDiverKey( phyidStr, cardKeys.DTAC, localCardKeys.DTAC );			  //内部密钥TAC
      PbocDiverKey( phyidStr, cardKeys.DACK, localCardKeys.DACK );			  //应用主控密钥
      //PbocDiverKey( phyidStr, cardKeys.DUK, localCardKeys.DUK );			  //PIN解锁
      //PbocDiverKey( phyidStr, cardKeys.DRPK, localCardKeys.DRPK );			  //PIN重装
      PbocDiverKey( phyidStr, cardKeys.DAMK, localCardKeys.DAMK );			  //维护密钥
      PbocDiverKey( phyidStr, cardKeys.DACK1, localCardKeys.DACK1 );		  //外部认证1
      //PbocDiverKey( phyidStr, cardKeys.DACK2, localCardKeys.DACK2 );		  //外部认证2
      //PbocDiverKey( phyidStr, cardKeys.DACK3, localCardKeys.DACK3 );		  //外部认证3
      //PbocDiverKey( phyidStr, cardKeys.LDACK, localCardKeys.LDACK );		  //小钱包外部认证密钥
      //PbocDiverKey( phyidStr, cardKeys.LDAMK, localCardKeys.LDAMK );		  //小钱包维护密钥

      uint8 externalKey[17] = {0};
      //使用出厂密钥进行外部认证
      memset( externalKey, 0xFF, sizeof ( externalKey ) );
      //先做外部认证，对于已回收的卡，此时应返回密钥未找到
      ret = this->ExternalAuthenticate( externalKey );
      if ( ret )
      {
        if ( 0x6A88 != ret )
        {
          SET_ERR( "外部认证失败,该卡可能未回收" );
          return ret;
        }
      }

      //TODO(youyiming) 为什么此处仍需要擦除MF目录，回收时已执行过该操作
      ret = this->EraseDF();
      if ( ret )
      {
        return ret;
      }

      vector<string> cmdList;
      char hexKey[64];
      string data;    //apdu报文中data域

      cmdList.push_back( apdu.SELECT_MF ); //选择MF

      //创建密钥文件
      //0000表示文件标识符
      //3F004001AAFFFF为文件控制信息和DF名称
      //密钥文件对应的文件控制信息报文数据格式为
      //3F+文件空间(2字节)+DF文件短标识符(1字节)+增加权限(1字节)+FF+FF
      //cmdList.push_back( this->GetCommand( apdu.CREATE_FILE, "0000", 7, "3F004001AAFFFF" ) );

      //写入根主控密钥，符合WRITE KEY中的Case2 增加外部认证密钥
      //Case2命令报文为
      //CLA: 80, INS: D4, P1: 01, P2: 密钥标识, Lc: 0D/15, Data: 39+使用权+更改权+后续状态+错误计数器+8或10字节密钥
      //错误计数器AA:高半字节指出密钥可以连续错误的最多次数，低半字节指出还可以再试的次数
      //A=10,即密钥最多可以连续错误10次，同时最大尝试次数为10次
      data = "39F0AAAAAA";
      data += hexCCK;
      //cmdList.push_back( this->GetCommand( apdu.WRITE_KEY, "01", "00", "15", data.c_str() ) );

      //创建DF02目录
      //DF02表示文件标识符
      //目录文件对应的文件控制信息报文数据格式为
      //38+文件空间(2字节)+建立权限(1字节)+擦除权限(1字节)+应用文件ID(XX)+保留字(FFFF)+DF名称(5-16字节)
      cmdList.push_back( this->GetCommand( apdu.CREATE_FILE, "DF02", 24, "380230F0AA95FFFFD15600000145415359454E5452590000" ) );

      //选择DF02目录
      cmdList.push_back( this->GetCommand( apdu.SELECT, "04", 16, "D15600000145415359454E5452590000" ) );

      //创建密钥文件
      cmdList.push_back( this->GetCommand( apdu.CREATE_FILE, "0000", 7, "3F014001FFFFFF" ) );

      //写入应用主控密钥DACK
      memset( hexKey, 0, sizeof( hexKey ) );
      Engine::Func::dec2hex( localCardKeys.DACK, 16, hexKey );
      data = "39F0FFFF33";
      data += hexKey;
      cmdList.push_back( this->GetCommand( apdu.WRITE_KEY, "01", "00", "15", data.c_str() ) );

      //写入应用维护密钥
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( localCardKeys.DAMK, 16, hexKey );
      data = "36F00BFF33";
      data += hexKey;
      cmdList.push_back( this->GetCommand( apdu.WRITE_KEY, "01", "01", "15", data.c_str() ) );

	  //写入外部认证密钥1
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( localCardKeys.DACK1, 16, hexKey );
      data = "39F00E2233";
      data += hexKey;
      cmdList.push_back( this->GetCommand( apdu.WRITE_KEY, "01", "01", "15", data.c_str() ) );

      //创建公共应用基本文件0015
      //大小00 3C===60字节
      cmdList.push_back( this->GetCommand( apdu.CREATE_FILE, "0015", 7, "A8003CF0F0FFFE" ) );

      //创建持卡人基本文件0016
      //大小00 3C===60字节
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
        SET_ERR( "卡物理ID[" << this->phyid << "]长度错误" );
        return CARD_PHYID_LENGTH_ERR;
      }

      int len = 0;
	  Engine::Func::hex2dec( this->phyid, 16, phyidStr, len );

      uint8 externalKey[17] = {0};
	  memset(externalKey,0x00,sizeof(externalKey));
      //使用出厂密钥进行外部认证
      memset( externalKey, 0x00, 16 );
      //先做外部认证，对于已回收的卡，此时应返回密钥未找到
      ret = this->ExternalAuthenticate( externalKey,0 );
      if ( ret )
      {
        if ( 0x6A88 != ret )
        {
		  if ( 0x63 == ret >> 8 )
          {
            //尝试使用出厂密钥进行认证
            memset( externalKey, 0xFF, sizeof externalKey );
            ret = this->ExternalAuthenticate( externalKey );
            if ( ret )
            {
			  SET_ERR( "尝试出厂秘钥外部认证失败" );
              return ret;
            }
          } else 
		  {
			  SET_ERR( "外部认证失败" );
			  return ret;
		  }
        }
      }

	   //当前用户卡中的密钥组
      CPUCardKeys newCardKeys;
      memset( &newCardKeys, 0, sizeof( newCardKeys ) );

	  CPUCardKeys workKeys;
      memset( &workKeys, 0, sizeof( workKeys ) );

	  this->psamCardService->GenerateWorkKey(&workKeys);

      PbocDiverKey( phyidStr, workKeys.DACK, newCardKeys.DACK );			  //应用主控密钥
      PbocDiverKey( phyidStr, workKeys.DAMK, newCardKeys.DAMK );			  //维护密钥
      PbocDiverKey( phyidStr, workKeys.DACK1, newCardKeys.DACK1 );		  //外部认证1
	  PbocDiverKey( phyidStr, workKeys.DACK1, newCardKeys.DACK2 );		  //外部认证2
	  PbocDiverKey( phyidStr, workKeys.DACK1, newCardKeys.DACK3 );		  //外部认证3

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
	  
	  //更新外部认证秘钥05
	  ret = UpdateCardKey("84F000031C","14100105FF",defaultCardKeys.DACK,newCardKeys.DACK1);
	  if(ret)
	  {
		return ret;
	  }
	  //更新外部认证秘钥06
	  ret = UpdateCardKey("84F000041C","14100106FF",defaultCardKeys.DACK,newCardKeys.DACK2);
	  if(ret)
	  {
		return ret;
	  }
	  //更新外部认证秘钥07
	  ret = UpdateCardKey("84F000051C","14100107FF",defaultCardKeys.DACK,newCardKeys.DACK3);
	  if(ret)
	  {
		return ret;
	  }
	  //更新应用维护秘钥
	  ret = UpdateCardKey("84F000021C","14300102FF",defaultCardKeys.DACK,newCardKeys.DAMK);
	  if(ret)
	  {
		return ret;
	  }
	  
	  /*//更新应用主控秘钥
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
      if ( strcmp( FI.c_str(), currentDF.c_str() ) == 0 ) //如果已经选择了SFI目录，返回成功
      {
        return SUCCESS;
      }

      CpuCmd cmd;
      int ret = this->ExecuteHexCpuCmd( this->GetCommand( apdu.SELECT, "00", FI.length() / 2, FI.c_str() ), &cmd );
      if ( ret )
      {
        SET_ERR( "选择目录" << FI << "失败" );
        return ret;
      }
      //选择目录成功之后，更新当前选择目录
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
        SET_ERR( "读取记录长度超过256个字节" );
        return CARD_READ_LENGTH_ERR;
      }

      CpuCmd cmd;
      //4=00000100
      //(SFI<<3)+4即将低三位置为100，SFI中的高五位为短文件标识符
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
      //128 + SFI即将高三位置为100，此时，SFI中的低五位为短文件标识
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
      //128 + SFI即将高三位置为100，此时，SFI中的低五位为短文件标识
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
      //128 + SFI即将高三位置为100，此时，SFI中的低五位为短文件标识
      string command = this->GetCommand( apdu.UPDATE_BINARY_MAC, 128 + SFI, offset, len + 4, hexBuffer );

      //计算MAC
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
        SET_ERR( "PIN长度错误" );
        return CARD_PIN_LENGTH_ERR;
      }

      int ret = 0;
      CpuCmd cmd;
      ret = this->ExecuteHexCpuCmd( this->GetCommand( apdu.VERIFY_PIN, "00", len / 2, key ), &cmd );
      if ( ret )
      {
        SET_ERR( "验证口令失败" );
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

      //通过执行圈存初始化命令来获得充值次数
      CpuCmd cmd;
      ret = this->ExecuteHexCpuCmd( this->GetCommand( apdu.INITIALIZE_FOR_LOAD, "02", "0100002710112233445566" ), &cmd );
      if ( ret )
      {
        return ret;
      }
      memcpy( cardBalance, cmd.receiveBuffer, cardBalanceLen );          //1-4字节表示余额
      memcpy( dpsCnt, cmd.receiveBuffer + cardBalanceLen, dpsCntLen );   //5-6字节表示充值次数
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

      //通过执行消费初始化命令来获得消费次数
      CpuCmd cmd;
      ret = this->ExecuteHexCpuCmd( this->GetCommand( apdu.INITIALIZE_FOR_PURCHASE, "02", "0100000000112233445566" ), &cmd );
      if ( ret )
      {
        return ret;
      }
      memcpy( cardBalance, cmd.receiveBuffer, cardBalanceLen );          //1-4字节表示余额
      memcpy( payCnt, cmd.receiveBuffer + cardBalanceLen, payCntLen );   //5-6字节表示消费次数

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
      memcpy( cardBalance, cmd.receiveBuffer, cardBalanceLen );          //1-4字节表示余额

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
      //TODO(youyiming) 对于7+1卡，需区分读取的是CPU部分还是MF部分
      //检查要读取的字段是否已读取
      int ret;
      /*
      int ret = FieldHasRead( fieldId );
      if ( ret )
      {
        fieldValue[0] = 0;
        return ret;
      }
      */
      //TODO(youyiming) CF_SUBSIDYNO不能读

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
          //MAC写
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
        sprintf( this->lastErrorMsg, "Err-%04X:成功", errorCode );
        break;
      case 0x6200:
        sprintf( this->lastErrorMsg, "Err-%04X:读卡设备中卡未插入到位", errorCode );
        break;
      case 0x6281:
        sprintf( this->lastErrorMsg, "Err-%04X:回送的数据可能错误", errorCode );
        break;
      case 0x6283:
        sprintf( this->lastErrorMsg, "Err-%04X:选择文件无效，文件或密钥校验错误", errorCode );
        break;
      case 0x63C1:
        sprintf( this->lastErrorMsg, "Err-%04X:密码错,还有1次机会", errorCode );
        break;
      case 0x63C2:
        sprintf( this->lastErrorMsg, "Err-%04X:密码错,还有2次机会", errorCode );
        break;
      case 0x63C3:
        sprintf( this->lastErrorMsg, "Err-%04X:密码错,还有3次机会", errorCode );
        break;
      case 0x63C4:
        sprintf( this->lastErrorMsg, "Err-%04X:密码错,还有4次机会", errorCode );
        break;
      case 0x63C5:
        sprintf( this->lastErrorMsg, "Err-%04X:密码错,还有5次机会", errorCode );
        break;
      case 0x63C6:
        sprintf( this->lastErrorMsg, "Err-%04X:密码错,还有6次机会", errorCode );
        break;
      case 0x63C7:
        sprintf( this->lastErrorMsg, "Err-%04X:密码错,还有7次机会", errorCode );
        break;
      case 0x63C8:
        sprintf( this->lastErrorMsg, "Err-%04X:密码错,还有8次机会", errorCode );
        break;
      case 0x63C9:
        sprintf( this->lastErrorMsg, "Err-%04X:密码错,还有9次机会", errorCode );
        break;
      case 0x63CA:
        sprintf( this->lastErrorMsg, "Err-%04X:密码错,还有10次机会", errorCode );
        break;
      case 0x63CB:
        sprintf( this->lastErrorMsg, "Err-%04X:密码错,还有11次机会", errorCode );
        break;
      case 0x63CC:
        sprintf( this->lastErrorMsg, "Err-%04X:密码错,还有12次机会", errorCode );
        break;
      case 0x63CD:
        sprintf( this->lastErrorMsg, "Err-%04X:密码错,还有13次机会", errorCode );
        break;
      case 0x63CE:
        sprintf( this->lastErrorMsg, "Err-%04X:密码错,还有14次机会", errorCode );
        break;
      case 0x6400:
        sprintf( this->lastErrorMsg, "Err-%04X:状态标志未改变", errorCode );
        break;
      case 0x6581:
        sprintf( this->lastErrorMsg, "Err-%04X:写EEPROM不成功", errorCode );
        break;
      case 0x6700:
        sprintf( this->lastErrorMsg, "Err-%04X:指令的命令长度不正确", errorCode );
        break;
      case 0x6900:
        sprintf( this->lastErrorMsg, "Err-%04X:CLA与线路保护要求不匹配", errorCode );
        break;
      case 0x6901:
        sprintf( this->lastErrorMsg, "Err-%04X:无效的状态", errorCode );
        break;
      case 0x6981:
        sprintf( this->lastErrorMsg, "Err-%04X:命令与文件结构不相容", errorCode );
        break;
      case 0x6982:
        sprintf( this->lastErrorMsg, "Err-%04X:不满足写或修改卡片的安全状态", errorCode );
        break;
      case 0x6983:
        sprintf( this->lastErrorMsg, "Err-%04X:密钥已经被锁死", errorCode );
        break;
      case 0x6984:
        sprintf( this->lastErrorMsg, "Err-%04X:随机数无效,请先取随机数", errorCode );
        break;
      case 0x6985:
        sprintf( this->lastErrorMsg, "Err-%04X:使用条件不满足", errorCode );
        break;
      case 0x6986:
        sprintf( this->lastErrorMsg, "Err-%04X:不满足命令执行条件,当前文件不是EF", errorCode );
        break;
      case 0x6987:
        sprintf( this->lastErrorMsg, "Err-%04X:无安全报文或MAC丢失", errorCode );
        break;
      case 0x6988:
        sprintf( this->lastErrorMsg, "Err-%04X:安全报文数据MAC不正确", errorCode );
        break;
      case 0x6A80:
        sprintf( this->lastErrorMsg, "Err-%04X:数据域参数错误", errorCode );
        break;
      case 0x6A81:
        sprintf( this->lastErrorMsg, "Err-%04X:功能不支持或卡中无MF或卡片已经锁定", errorCode );
        break;
      case 0x6A82:
        sprintf( this->lastErrorMsg, "Err-%04X:卡片中文件未找到", errorCode );
        break;
      case 0x6A83:
        sprintf( this->lastErrorMsg, "Err-%04X:卡片中记录未找到", errorCode );
        break;
      case 0x6A84:
        sprintf( this->lastErrorMsg, "Err-%04X:文件无足够空间", errorCode );
        break;
      case 0x6A86:
        sprintf( this->lastErrorMsg, "Err-%04X:参数P1P2错误", errorCode );
        break;
      case 0x6A87:
        sprintf( this->lastErrorMsg, "Err-%04X:无安全报文", errorCode );
        break;
      case 0x6A88:
        sprintf( this->lastErrorMsg, "Err-%04X:密钥未找到", errorCode );
        break;
      case 0x6B00:
        sprintf( this->lastErrorMsg, "Err-%04X:在达到Le/Lc字节之前文件结束,偏移量错误", errorCode );
        break;
      case 0x6D00:
        sprintf( this->lastErrorMsg, "Err-%04X:无效的INS", errorCode );
        break;
      case 0x6E00:
        sprintf( this->lastErrorMsg, "Err-%04X:无效的CLA", errorCode );
        break;
      case 0x6F00:
        sprintf( this->lastErrorMsg, "Err-%04X:数据无效", errorCode );
        break;
      case 0x6FF0:
        sprintf( this->lastErrorMsg, "Err-%04X:读卡器系统错误", errorCode );
        break;
      case 0xFFFF:
        sprintf( this->lastErrorMsg, "Err-%04X:无法判断的错误", errorCode );
        break;
      case 0x9210:
        sprintf( this->lastErrorMsg, "Err-%04X:卡片该DF下内存不足", errorCode );
        break;
      case 0x9220:
        sprintf( this->lastErrorMsg, "Err-%04X:文件ID已存在", errorCode );
        break;
      case 0x9240:
        sprintf( this->lastErrorMsg, "Err-%04X:卡片内存问题", errorCode );
        break;
      case 0x9302:
        sprintf( this->lastErrorMsg, "Err-%04X:MAC错误", errorCode );
        break;
      case 0x9303:
        sprintf( this->lastErrorMsg, "Err-%04X:应用已被锁定", errorCode );
        break;
      case 0x9400:
        sprintf( this->lastErrorMsg, "Err-%04X:没有选择当前的EF文件ID", errorCode );
        break;
      case 0x9401:
        sprintf( this->lastErrorMsg, "Err-%04X:金额不足", errorCode );
        break;
      case 0x9402:
        sprintf( this->lastErrorMsg, "Err-%04X:超出范围或没有该记录", errorCode );
        break;
      case 0x9403:
        sprintf( this->lastErrorMsg, "Err-%04X:密钥未找到", errorCode );
        break;
      case 0x9404:
        sprintf( this->lastErrorMsg, "Err-%04X:EF的文件ID没有找到", errorCode );
        break;
      case 0x9406:
        sprintf( this->lastErrorMsg, "Err-%04X:所需的MAC不可用", errorCode );
        break;
      case 0x9802:
        sprintf( this->lastErrorMsg, "Err-%04X:没有所要的密钥", errorCode );
        break;
      case 0x9804:
        sprintf( this->lastErrorMsg, "Err-%04X:存在条件没有满足或MAC错", errorCode );
        break;
      case 0x9810:
        sprintf( this->lastErrorMsg, "Err-%04X:的应用被锁定", errorCode );
        break;
      case 0x9835:
        sprintf( this->lastErrorMsg, "Err-%04X:没有取随机数", errorCode );
        break;
      case 0x9840:
        sprintf( this->lastErrorMsg, "Err-%04X:该KEY被锁住", errorCode );
        break;
      default:
        sprintf( this->lastErrorMsg, "Err-%04X:不知道的错误", errorCode );
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