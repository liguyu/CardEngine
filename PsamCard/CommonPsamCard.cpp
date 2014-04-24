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
      hasReset = false;  //未上电复位
	  hasLoadMainKey = false;

      //选择文件 SELECT，参考毅能达PSAM卡COS6.0、中国金融PSAM卡应用规范
      //命令报文规范
      //CLA: 00, INS: A4, P1: 00/04, P2: 00, Lc: XX, Data: 空或文件标识符或DF名称, Le:00
      //P1=00:用文件标识符选择，选择当前目录下基本文件或子目录文件
      //P1=04:用目录名选择，选择MF或当前目录本身，或与当前目录平级的目录，或当前目录的下级子目录
      apdu.SELECT_MF = "00A4000000";

      ///选择文件 SELECT，参考毅能达PSAM卡COS6.0、中国金融PSAM卡应用规范
      ///按目录名选择
      apdu.SELECT = "00A4%s00%02X%s";

      //读二进制文件 READ BINARY，参考毅能达PSAM卡COS6.0、中国金融PSAM卡应用规范
      //命令报文规范
      //CLA: 00, INS: B0, P1: XX, P2:XX, Le: XX
      //若P1的高三位为100，则低五位为短文件标识，P2为读的偏移量
      //若P1的最高位不为1，则P1P2为欲读文件的偏移量，P1为偏移量的高字节，P2为偏移量的低字节，所读文件为当前文件
      apdu.READ_BINARY = "00B0%02X%02X%02X";

      //取随机数 EXTERNAL AUTHENTICATE，参考毅能达PSAM卡COS6.0、中国金融PSAM卡应用规范
      //命令报文规范
      //CLA: 00, INS: 84, P1: 00, P2: 00, Le: 04/08
      //注意国民技术读卡器只能取4字节
      apdu.GET_CHALLENGE = "00840000%02X";

      //擦除文件系统 ERASE MF，参考毅能达PSAM卡COS6.0、中国金融PSAM卡应用规范
      //命令报文规范
      //CLA: 84, INS: 8A, P1: 01, P2: 00, Le: 04, Data: MAC
      //此命令只能在MF 下执行，固化要求必须采用安全报文
      //使用MF 的主控密钥或卡片传输密钥计算MAC，命令执行成功后，卡片回到白卡状态
      apdu.ERASE_MF = "848A010004%.8s";

      //结束个人化 END ISSUE，参考毅能达PSAM卡COS6.0、中国金融PSAM卡应用规范
      //命令报文规范
      //CLA: 80, INS: 1C, P1: 00, P2: 00, Lc: 00
      apdu.END_ISSUE = "801C000000";

      //带MAC结束个人化 END ISSUE，参考毅能达PSAM卡COS6.0、中国金融PSAM卡应用规范
      //命令报文规范
      //CLA: 84, INS: 1C, P1: 00, P2: 00, Lc: 04, Data: MAC
      apdu.END_ISSUE_MAC = "841C000004";

      //创建主文件 CREATE MF，参考毅能达PSAM卡COS6.0、中国金融PSAM卡应用规范
      //命令报文规范
      //CLA: 80, INS: 8A, P1: 00, P2: 00, Lc: 2A, Data:文件信息
      //文件信息包括:
      //(2字节)文件标识符
      //(1字节)文件类型，MF 的FTP 固定为‘10’
      //(1字节)在MF 下创建文件的权限，不用时必须写’00’
      //(2字节)RFU FFFF
      //(1字节)MF 的DIR 文件的SFI
      //(1字节)更新年度起始日期条件
      //(2字节)MF下密钥文件的大小
      //(1字节)应用锁定条件
      //(1字节) 0x81
      //(3字节)RFU FFFFFF
      //(1字节)指出紧跟其后的DF 名（或AID）的实际字节数，取值范围5-16（十进制）
      //(16字节)DF 名或AID，内容不足16 字节时，后以’FF’补齐16 字节，它在卡内必须是唯一的
      //(10字节)MF 下的主控密钥的属性
      apdu.CREATE_MF = "808A00002A%s";

      //创建文件 CREATE FILE，参考毅能达PSAM卡COS6.0、中国金融PSAM卡应用规范
      //命令报文规范
      //CLA: 80, INS: 80, P1: 00, P2: 00, Lc: 08/0C 建立EF 文件 2A/2E 建立DF 文件, Data:文件信息
      apdu.CREATE_FILE = "80800000%02X%s";

      //更新透明文件 UPDATE BINARY，参考毅能达PSAM卡COS6.0、中国金融PSAM卡应用规范
      //命令报文规范
      //CLA: 00, INS: D6, P1: XX, P2: XX, Lc: XX, Data:写入的数据
      //若P1高3位为100：P1 低5位为文件SFI（值为1～31），P2 表示偏移量
      //若P1最高位为0：P1P2 表示偏移量，对当前文件操作
      apdu.UPDATE_BINARY = "00D6%02X%02X%02X%s";

      //分散密钥 DELIVERY KEY，参考毅能达PSAM卡COS6.0、中国金融PSAM卡应用规范
      //命令报文规范
      //CLA: 80, INS: 1A, P1: 密钥用途, P2: 密钥标识, Lc: 8*N N 为分散级数，N=0/1/2/3, Data:分散因子
      //密钥密钥用途的高三位为分散级数，低五位为密钥属性
      //其中密钥属性定义如下:
      //00   主控密钥                      功能密钥，每个DF 只有一个主控密钥，控制文件建立和密钥导入
      //02   消费密钥                      应用密钥，用于消费交易的安全控制
      //05   应用维护密钥                  功能密钥，用于安全报文的计算
      //06   MAC 密钥                      应用密钥，只能进行MAC 计算
      //07   加密密钥                      应用密钥，只能进行加密计算
      //08   MAC、加密密钥，保护密钥       应用密钥，可以进行MAC、加密计算
      //09   圈存密钥                      应用密钥，用于帐户划入交易的安全控制；
      //0C   TAC 密钥                      应用密钥，用于交易中TAC 的计算
      //0D   PIN                           功能密钥，持卡人个人密码
      //0E   外部认证密钥                  功能密钥，外部认证命令使用的缺省密钥类。
      //0F   内部认证密钥                  功能密钥，内部认证命令使用的缺省密钥类
      //1E                                 应用密钥，导出密钥，用于母卡中
      apdu.DELIVERY_KEY = "801A%s%s%02X%s";

      //加密数据 CIPHER DATA，参考毅能达PSAM卡COS6.0、中国金融PSAM卡应用规范
      //命令报文规范
      //CLA: 80, INS: FA, P1: 00/01/02/03/05/07, P2: 00, Lc: XX, Data:待处理的数据, Le: XX
      //P1:00 － 无后续块加密
      //P1:01 － 最后一块MAC 计算
      //P1:02 － 有后续块加密
      //P1:03 － 下一块MAC 计算
      //P1:05 － 唯一一块MAC 计算
      //P1:07 － 第一块MAC 计算
      apdu.CIPHER_DATA = "80FA%s00%02X%s";

      //消费交易初始化 INIT SAM FOR PURCHASE，参考毅能达PSAM卡COS6.0、中国金融PSAM卡应用规范
      //命令报文规范
      //CLA: 80, INS: 70, P1: 00, P2: 00, Lc: 14＋8×N（N＝1，2，3）, Data:待处理的数据, Le: 08
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
      char psamTermNo[13] = {0};    //psam终端编号，用于暂存读出来的数据

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

    int CCommonPsamCardService::ReadBinaryFile( int SFI, int offset, int len, uint8 *binaryBuffer )
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

    char* CCommonPsamCardService::GetErrorMsgByErrorCode( int errorCode, char *errorMsg )
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

      //计算MAC
      command = "848A010004800000";
      hex2dec( command.c_str(), command.size(), data, dataLen );
      for ( int i = 0; i < 8; i++ )
        data[i] = data[i] ^ random[i];
      //加密
      des3_context ctx3;
      uint8 key[16];
      uint8 encryptedMac[255] = {0};
      memcpy( key, "\x01\x02\x03\x04\x05\x06\x07\x08\x11\x12\x13\x14\x15\x16\x17\x18", 16 );
      des3_set_2keys( &ctx3, key, key + 8 );
      des3_encrypt( &ctx3, data, encryptedMac );

      char hexMac[9] = {0};
      dec2hex( encryptedMac, 4, hexMac );
      command = this->GetCommand( apdu.ERASE_MF, hexMac );          //擦除MF
      ret = this->ExecuteHexCpuCmd( command, &cmd );
      if ( ret )
      {
        if ( cmd.retCode == 0x6985 )
        {
          SET_ERR( "该PSAM卡已回收" );
        }
        return ret;
      }

      return SUCCESS;
    }

    int CCommonPsamCardService::CheckPsamCardInfo( PsamCard* psamCard )
    {
      if ( strlen( psamCard->startDate ) != 8 )
      {
        SET_ERR( "开始日期长度错误" );
        return CARD_PSAM_DATE_LENGTH_ERR;
      }
      if ( strlen( psamCard->expireDate ) != 8 )
      {
        SET_ERR( "有效日期长度错误" );
        return CARD_PSAM_DATE_LENGTH_ERR;
      }
      if ( psamCard->keyIndex > MAX_KEY_INDEX )
      {
        SET_ERR( "密钥索引不能大于" << MAX_KEY_INDEX );
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
		memcpy(cardKeys->DACK,appKey,16);    //应用主控密钥
		memcpy(cardKeys->DAMK,appKey,16);    //维护密钥
		memcpy(cardKeys->DACK1,externKey,16);  //外部认证密钥

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
		  SET_ERR( "未加载根密钥,ret=" << ltohex( CARD_UNLOAD_MAIN_KEY ) );
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

      //复位上电
      ret = this->ResetPsam( 1 );
      if ( ret )
      {
        return ret;
      }

      //创建MF
      string data = "3F001000FFFF0A00012AFF81FFFFFF00315041592E5359532E4444463031FFFF00180100010002030300";
      command = this->GetCommand( apdu.CREATE_MF, data.c_str() );
      ret = this->ExecuteHexCpuCmd( command, &cmd );
      if ( ret )
      {
        if ( cmd.retCode == 0x6985 )
        {
          SET_ERR( "该卡已发行,ret=" << ltohex( cmd.retCode ) );
        }
        return ret;
      }

      //创建EF 0001
      data = "0001340000400002";
      command = this->GetCommand( apdu.CREATE_FILE, 8, data.c_str() );
      cmdList.push_back( command );

      //创建卡片公共信息文件0015
      data = "00153100000E0002";
      command = this->GetCommand( apdu.CREATE_FILE, 8, data.c_str() );
      cmdList.push_back( command );

      //更新公共信息文件0015
      data = lpad( psamCard->psamCardNo, 20, '0' );   //PSAM序列号
      data.append( ltohex( psamCard->cardVersion ) ); //PSAM版本号
      data.append( ltohex( psamCard->cardType ) );    //密钥卡类型
      data.append( "01" );                            //指令集版本
      data.append( "00" );                            //FCI数据
      //149==10010101,即SFI为10101=0x15
      command = this->GetCommand( apdu.UPDATE_BINARY, 149, 0, 14, data.c_str() );
      cmdList.push_back( command );

      //创建终端信息文件0016
      data = "00163100000600FF";
      command = this->GetCommand( apdu.CREATE_FILE, 8, data.c_str() );
      cmdList.push_back( command );

      //创建公共信息文件0017
      data = "0017310000260002";
      command = this->GetCommand( apdu.CREATE_FILE, 8, data.c_str() );
      cmdList.push_back( command );

      //更新公共信息文件0017
      command = "00D6970019";
      data = ltohex( psamCard->keyIndex );                  //消费密钥索引号
      data.append( lpad( psamCard->publishId, 16, '0' ) );  //应用发行者标识
      data.append( lpad( psamCard->userId, 16, '0' ) );     //应用接收者标识
      data.append( psamCard->startDate );                   //应用启用日期
      data.append( psamCard->expireDate );                  //应用有效日期
      //151==10010111,即SFI为10111=0x17
      command = this->GetCommand( apdu.UPDATE_BINARY, 151, 0, 25, data.c_str() );
      cmdList.push_back( command );

      //更新0016，写入终端机编号
      data = string( lpad( psamCard->termId, 12, '0' ) );
      //150==10010110,即SFI为10110=0x16
      command = this->GetCommand( apdu.UPDATE_BINARY, 150, 0, 6, data.c_str() );
      cmdList.push_back( command );

      //写外部认证密钥1
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( cardKeys.DACK1, 16, hexKey );
      command = "80D000001A0E180000010002030300";
      command += hexKey;
      cmdList.push_back( command );

      //创建DF02
      data = "DF022000FFFF0AFF01C80081FFFFFF00D156000001BDF0CACBB4EFD6A7B8B6FF00180100010002030300";
      command = this->GetCommand( apdu.CREATE_FILE, 42, data.c_str() );
      cmdList.push_back( command );

      //选择DF02
      cmdList.push_back( this->GetCommand( apdu.SELECT, "00", 2, "DF02" ) );

      //创建交易序号文件0019
      data = "00193100000400FF";
      command = this->GetCommand( apdu.CREATE_FILE, 8, data.c_str() );
      cmdList.push_back( command );

      //写终端交易序号文件
      //153==10011001,即SFI为11001=0x19
      command = this->GetCommand( apdu.UPDATE_BINARY, 153, 0, 4, "00000000" );
      cmdList.push_back( command );

      //外部认证密钥1
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( cardKeys.DACK1, 16, hexKey );
      command = "80D000001A27180100010010030300";
      command += hexKey;
      cmdList.push_back( command );

      //消费密钥1
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( cardKeys.DPK1, 16, hexKey );
      command = "80D000001A22180100010010030300";
      command += hexKey;
      cmdList.push_back( command );

      //消费密钥2
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( cardKeys.DPK2, 16, hexKey );
      command = "80D000001A22180200010010030300";
      command += hexKey;
      cmdList.push_back( command );

      //16字节应用维护主密钥
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( cardKeys.DAMK, 16, hexKey );
      command = "80D000001A28180100010010030300";
      command += hexKey;
      cmdList.push_back( command );

      //16字节口令解锁主密钥
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( cardKeys.DUK, 16, hexKey );
      command = "80D000001A28180200010010030300";
      command += hexKey;
      cmdList.push_back( command );

      //16字节口令重装主密钥
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( cardKeys.DRPK, 16, hexKey );
      command = "80D000001A28180300010010030300";
      command += hexKey;
      cmdList.push_back( command );

      //16字节内部认证主密钥
      memset( hexKey, 0, sizeof( hexKey ) );
      dec2hex( cardKeys.DTAC, 16, hexKey );
      command = "80D000001A28180400010010030300";
      command += hexKey;
      cmdList.push_back( command );

      cmdList.push_back( "END_ISSUE" ); //DF02结束个人化

      cmdList.push_back( apdu.SELECT_MF ); //选择MF

      cmdList.push_back( "END_ISSUE" );   //MF结束个人化

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
        if ( 0x6E00 == cmd.retCode )  //6E00===>CLA错误
        {
          ret = EndIssueWithMac();    //尝试使用带MAC方式结束个人化
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

      //取随机数
      ret = this->GetChallenge( random );
      if ( ret )
        return ret;

      //计算指令MAC
      command = apdu.END_ISSUE_MAC; //带MAC结束个人化
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
      // 初始化MAC计算初值 ==> temp
      memset( temp, 0, sizeof( temp ) );
      memcpy( temp, randomNumber, 8 * sizeof( uint8 ) );
      // 格式化输入值: 填 "80 00 00 ...", 凑8字节长度 ==> buf
      int block = 0;
      if ( dataLen % 8 )
        block = dataLen / 8 + 1; // 总块数
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
      // temp 左四位 ==> MAC
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

      //使用命令数据中的分散因子对指定的密钥进行分散，作为临时密钥存放在卡中
      data = string( phyid );
      if ( strlen( phyid ) == 8 )
        data += "80000000";
      //密钥用途 28=00101000，分散级数为001，密钥属性为01000，即08(MAC、加密密钥，保护密钥)，可以进行MAC、加密计算
      //密钥标识01
      command = this->GetCommand( apdu.DELIVERY_KEY, "28", "01", 8, data.c_str() );
      cmdList.push_back( command );

      //将随机数和原始命令作为数据源进行MAC计算
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
      int maxBlockLen = 0x30;   //单次处理数据最大长度

      int n = dataLen / maxBlockLen;
      if ( dataLen % maxBlockLen )    //如果待处理数据的长度%单次处理数据最大长度不为0的话，那么需要处理的次数++1
      {
        n += 1;
      }

      if ( n == 1 ) //05 唯一一块MAC 计算
      {
        command = this->GetCommand( apdu.CIPHER_DATA, "05", dataLen, macData.c_str() );
        cmdList.push_back( command );
      }
      else
      {
        for ( int i = 0; i < n; i++ )
        {
          data = "";
          if ( i == 0 )          //第一块MAC计算
          {
            data.append( pData, maxBlockLen * 2 );
            command = this->GetCommand( apdu.CIPHER_DATA, "07", maxBlockLen, data.c_str() );
          }
          else if ( i == n - 1 ) //最后一块MAC计算
          {
            data.append( pData, dataLen * 2 );
            command = this->GetCommand( apdu.CIPHER_DATA, "01", dataLen, data.c_str() );
          }
          else                   //下一块MAC计算
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
      //密钥用途 27=00100111，分散级数为001，密钥属性为00111，即07(加密密钥)，只能进行加密计算
      command = this->GetCommand( apdu.DELIVERY_KEY, "27", ltohex( keyIndex ).c_str(), 8, data.c_str() );
      ret = this->ExecuteHexCpuCmd( command, &cmd );
      if ( ret )
      {
        return ret;
      }

      //将随机数和原始命令作为数据源进行MAC计算
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

      //05 唯一一块MAC 计算
      //command = this->GetCommand( apdu.CIPHER_DATA, "05", dataLen, data.c_str() );
      command = this->GetCommand( apdu.CIPHER_DATA, "00", dataLen, data.c_str() );  //无后续快加密
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
        SET_ERR( "选择目录MF失败" );
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

      //读取终端编号
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
        SET_ERR( "选择目录DF02失败" );
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