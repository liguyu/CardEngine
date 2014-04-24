#include "Decard.h"
#include "../ThirdParty/decard/include/dcrf32.h"
#include <stdio.h>
#include <string.h>
#include "../Commons/GlobalFunc.h"

namespace Engine
{
  namespace Device
  {
    CDecardService::CDecardService()
    {
      this->deviceHandle = 0;
	  strcpy(this->deviceName,"德卡读卡器");
	  deviceInfo.port = 100;
	  deviceInfo.baud = 9600;
    }

    char* CDecardService::GetLastErrorMsg( char *message )
    {
      if ( message )
        strcpy( message, this->errorMsg );
      return this->errorMsg;
    }

    int CDecardService::OpenDevice( int port, int baud )
    {
      int ret;
      if ( port != USB_PORT )
      {
        //port:取值为0～19时，表示串口1～20；
        ret = dc_init( port - 1, baud );
        if ( ret <= 0 )
        {
          sprintf( this->errorMsg, "打开串口 COM%d 失败", port );
          return DEV_OPENDEVICE;
        }
      }
      else
      {
        //port:取值为100时，表示USB口通讯，此时波特率无效
        ret = dc_init( USB_PORT, 0 );
        if ( ret <= 0 )
        {
          strcpy( errorMsg, "打开读卡器USB端口失败" );
          return DEV_OPENDEVICE;
        }
      }
      this->deviceHandle = ( HANDLE )ret;
      this->deviceInfo.port = port;
      this->deviceInfo.baud = baud;
      return SUCCESS;
    }

    int CDecardService::CloseDevice()
    {
      if ( this->deviceHandle != NULL )
      {
        dc_exit( this->deviceHandle );
      }
      this->deviceHandle = 0;
      return SUCCESS;
    }

    int CDecardService::RequestCard( char *phyid, int& cardType , CARD_SLOT cardSlot )
    {
      int ret;
      unsigned long snr;
      unsigned short tagType;
      uint8 size;
      uint8 ucCardPhyID[8];

      if ( !this->deviceHandle )
      {
        ret = OpenDevice( deviceInfo.port, deviceInfo.baud );
        if ( ret )
          return ret;
      }
      if ( cardSlot == CARDSLOT_RF )
      {
        //调用德卡的射频复位函数,1为复位时间，单位为毫秒
        dc_reset( this->deviceHandle, 1 );
        //寻卡请求，1为寻卡模式，此处为ALL模式，表示一次可对多张卡操作
        ret = dc_request( this->deviceHandle, 1, &tagType );
        if ( ret )
        {
          GetErrorMsgByErrorCode( ret, this->errorMsg );
          return DEV_REQUESTCARD;
        }
        //防卡冲突，返回卡的序列号
        ret = dc_anticoll( this->deviceHandle, 0, &snr );
        if ( ret )
        {
          GetErrorMsgByErrorCode( ret, this->errorMsg );
          return DEV_REQUESTCARD;
        }
        ret = dc_select( this->deviceHandle, snr, &size );
        if ( ret )
        {
          GetErrorMsgByErrorCode( ret, this->errorMsg );
          return DEV_REQUESTCARD;
        }

        switch ( tagType )
        {
        case 8:
          cardType = CPUCARD;
          break;
        case 4:
          if ( size == 40 )
            cardType = FIXCARD; //cpu7+1
          else
            cardType = MFCARD; //S50
          break;
        case 2:
          cardType = MFCARD; //S70
          break;
        }

		Engine::Func::set_n_byte_int<int, 4>( ucCardPhyID, sizeof( ucCardPhyID ), snr );
        Engine::Func::dec2hex( ucCardPhyID, 4, phyid );
        phyid[8] = 0;
      }
      else
      {
        sprintf( this->errorMsg, "不支持的参数cardSlot[%d]", cardSlot );
        return DEV_REQUESTCARD;
      }
      return SUCCESS;
    }

    int CDecardService::Halt()
    {
      if ( !this->deviceHandle )
      {
        strcpy( errorMsg, "端口未打开" );
        return DEV_PORT_NOTOPEN;
      }
      dc_halt( this->deviceHandle );
      return SUCCESS;
    }

    int CDecardService::Beep()
    {
      int ret;
      if ( !this->deviceHandle )
      {
        ret = OpenDevice( deviceInfo.port, deviceInfo.baud );
        if ( ret )
          return ret;
      }
      dc_beep( this->deviceHandle, 10 );
      return SUCCESS;
    }

    int CDecardService::BeepError()
    {
      int ret;
      if ( !this->deviceHandle )
      {
        ret = OpenDevice( deviceInfo.port, deviceInfo.baud );
        if ( ret )
          return ret;
      }

      dc_beep( this->deviceHandle, BEEP_ERR_TIME );
      return SUCCESS;
    }

    int CDecardService::Reset( size_t msec, CARD_SLOT cardSlot )
    {
      int ret;
      if ( !this->deviceHandle )
      {
        ret = OpenDevice( deviceInfo.port, deviceInfo.baud );
        if ( ret )
          return ret;
      }
      if ( cardSlot == CARDSLOT_RF )
      {
        ret = dc_reset( this->deviceHandle, msec );
        if ( ret )
        {
          GetErrorMsgByErrorCode( ret, this->errorMsg );
          return DEV_OPENDEVICE;
        }
      }
      else
      {
        //TODO(youyiming) cpu卡重置
        //ret = CpuReset();
      }
      return SUCCESS;
    }

    char* CDecardService::GetErrorMsgByErrorCode( int errorCode, char* errorMsg )
    {
      switch ( errorCode )
      {
      case -0x10:
        sprintf( errorMsg, "Err-%04X:通讯错误", errorCode );
        break;
      case -0x11:
        sprintf( errorMsg, "Err-%04X:超时错误", errorCode );
        break;
      case -0x20:
        sprintf( errorMsg, "Err-%04X:打开端口错误", errorCode );
        break;
      case -0x21:
        sprintf( errorMsg, "Err-%04X:获得端口参数错误", errorCode );
        break;
      case -0x22:
        sprintf( errorMsg, "Err-%04X:设置端口参数错误", errorCode );
        break;
      case -0x23:
        sprintf( errorMsg, "Err-%04X:关闭端口出错", errorCode );
        break;
      case -0x24:
        sprintf( errorMsg, "Err-%04X:端口被占用", errorCode );
        break;
      case -0x30:
        sprintf( errorMsg, "Err-%04X:格式错误", errorCode );
        break;
      case -0x31:
        sprintf( errorMsg, "Err-%04X:数据格式错误", errorCode );
        break;
      case -0x32:
        sprintf( errorMsg, "Err-%04X:数据长度错误", errorCode );
        break;
      case -0x40:
        sprintf( errorMsg, "Err-%04X:读错误", errorCode );
        break;
      case -0x41:
        sprintf( errorMsg, "Err-%04X:写错误", errorCode );
        break;
      case -0x42:
        sprintf( errorMsg, "Err-%04X:无接收错误", errorCode );
        break;
      case -0x50:
        sprintf( errorMsg, "Err-%04X:不够减错误", errorCode );
        break;
      case -0x51:
        sprintf( errorMsg, "Err-%04X:CPU数据异或和错误", errorCode );
        break;
      case -0x52:
        sprintf( errorMsg, "Err-%04X:485通讯时地址号错误", errorCode );
        break;
      case -0x73:
        sprintf( errorMsg, "Err-%04X:取版本号错误", errorCode );
        break;
      case -0xc2:
        sprintf( errorMsg, "Err-%04X:CPU卡响应错误", errorCode );
        break;
      case -0xd3:
        sprintf( errorMsg, "Err-%04X:CPU卡响应超时", errorCode );
        break;
      case -0xd6:
        sprintf( errorMsg, "Err-%04X:CPU卡校验错误", errorCode );
        break;
      case -0xd7:
        sprintf( errorMsg, "Err-%04X:CPU卡命令过程字错误", errorCode );
        break;
        //////////////////////////////////////////////////////////////////
      case 0x01:
        sprintf( errorMsg, "Err-%04X:未放置卡片或认证错误", errorCode );
        break;
      case 0x02:
        sprintf( errorMsg, "Err-%04X:数据校验错误", errorCode );
        break;
      case 0x03:
        sprintf( errorMsg, "Err-%04X:数值为空错误", errorCode );
        break;
      case 0x04:
        sprintf( errorMsg, "Err-%04X:认证失败", errorCode );
        break;
      case 0x05:
        sprintf( errorMsg, "Err-%04X:奇偶校验错误", errorCode );
        break;
      case 0x06:
        sprintf( errorMsg, "Err-%04X:读写设备与卡片通讯错误", errorCode );
        break;
      case 0x08:
        sprintf( errorMsg, "Err-%04X:读卡序列号错误", errorCode );
        break;
      case 0x09:
        sprintf( errorMsg, "Err-%04X:密码类型错误", errorCode );
        break;
      case 0x0a:
        sprintf( errorMsg, "Err-%04X:卡片尚未被认证", errorCode );
        break;
      case 0x0b:
        sprintf( errorMsg, "Err-%04X:读卡操作比特数错误", errorCode );
        break;
      case 0x0c:
        sprintf( errorMsg, "Err-%04X:读卡操作字节数错误", errorCode );
        break;
      case 0x0f:
        sprintf( errorMsg, "Err-%04X:写卡操作失败", errorCode );
        break;
      case 0x10:
        sprintf( errorMsg, "Err-%04X:增值操作失败", errorCode );
        break;
      case 0x11:
        sprintf( errorMsg, "Err-%04X:减值操作失败", errorCode );
        break;
      case 0x12:
        sprintf( errorMsg, "Err-%04X:读卡操作失败", errorCode );
        break;
      case 0x13:
        sprintf( errorMsg, "Err-%04X:传输缓冲区溢出", errorCode );
        break;
      case 0x15:
        sprintf( errorMsg, "Err-%04X:传输帧错误", errorCode );
        break;
      case 0x17:
        sprintf( errorMsg, "Err-%04X:未知的传输需求", errorCode );
        break;
      case 0x18:
        sprintf( errorMsg, "Err-%04X:防冲突错误", errorCode );
        break;
      case 0x19:
        sprintf( errorMsg, "Err-%04X:感应模块复位错误", errorCode );
        break;
      case 0x1a:
        sprintf( errorMsg, "Err-%04X:非认证接口", errorCode );
        break;
      case 0x1b:
        sprintf( errorMsg, "Err-%04X:模块通讯超时", errorCode );
        break;
      case 0x3c:
        sprintf( errorMsg, "Err-%04X:非正常操作", errorCode );
        break;
      case 0x64:
        sprintf( errorMsg, "Err-%04X:错误的数据", errorCode );
        break;
      case 0x7c:
        sprintf( errorMsg, "Err-%04X:错误的参数值", errorCode );
        break;
      default:
        sprintf( errorMsg, "Err-%04X:读卡器未知的错误", errorCode );
        break;
      }
      return errorMsg;
    }

    int CDecardService::CpuReset( CARD_TYPE cardType, uint8 *len, uint8 *buffer )
    {
      int ret;
      if ( !this->deviceHandle )
      {
        ret = OpenDevice( deviceInfo.port, deviceInfo.baud );
        if ( ret )
          return ret;
      }

      if ( cardType == MFCARD )
      {
        //mifare one card不支持上电复位
        strcpy( errorMsg, "不支持的操作" );
        return DEV_NOTSUPPORT;
      }
      else if ( cardType == PSAM )
      {
        ret = dc_cpureset( this->deviceHandle, len, buffer );
      }
      else
      {
        //CPU卡上电复位
        ret = dc_pro_reset( this->deviceHandle, len, buffer );
      }

      if ( ret )
      {
        GetErrorMsgByErrorCode( ret, this->errorMsg );
        return DEV_POWON_ERR;
      }

      return SUCCESS;
    }

    int CDecardService::SetPsamSlot( int slot )
    {
      int originalSlot = 100;
      switch ( slot )
      {
      case CARDSLOT_1:
        originalSlot = 0x0c;
        break;
      case CARDSLOT_2:
        originalSlot = 0x0d;
        break;
      case CARDSLOT_3:
        originalSlot = 0x0e;
        break;
      case CARDSLOT_4:
        originalSlot = 0x0f;
        break;
      default:
        originalSlot = 100;
        break;
      }
      if ( originalSlot == 100 )
      {
        sprintf( errorMsg, "PSAM 插槽[%d]非法", slot );
        return DEV_PARAMERR;
      }
	  if ( !this->deviceHandle )
      {
        int ret = OpenDevice( deviceInfo.port, deviceInfo.baud );
        if ( ret )
          return ret;
      }
      if ( dc_setcpu( this->deviceHandle, originalSlot ) )
      {
        sprintf( errorMsg, "设置PSAM 插槽[%d]失败", slot );
        return DEV_SETCPU_ERR;
      }
      this->deviceInfo.psamSlot = slot;

      return SUCCESS;
    }

    int CDecardService::ExecuteCpuCmd( CARD_TYPE cardType, CpuCmd *cmd, CARD_SLOT cardSlot )
    {
      int ret = -1;
      uint8 sw1, sw2;
      char szCmd[255];
      CpuCmd innerCmd;
      cmd->retCode = 0;

      int receiveLen;
      if ( cardType == CPUCARD || cardType == FIXCARD )
      {
        // 非接触式
        if ( cardSlot == CARDSLOT_RF )
        {
          if ( cmd->cmdType == 0 )
          {
            ret = dc_pro_commandlink( this->deviceHandle, cmd->sendLen, cmd->sendBuffer, ( unsigned char* )&receiveLen, cmd->receiveBuffer, 7, 56 );
            cmd->receiveLen = receiveLen;
          }
          else
          {
            unsigned char ucCmd[512];
            int  iCmdLen = 0;
            Engine::Func::hex2dec( ( char* )cmd->sendBuffer, cmd->sendLen, ucCmd, iCmdLen );
            ret = dc_pro_commandlink( this->deviceHandle, iCmdLen, ucCmd, ( unsigned char* )&receiveLen, ucCmd, 7, 56 );
            cmd->receiveLen = receiveLen;
            memcpy( cmd->receiveBuffer, ucCmd, cmd->receiveLen );
          }
        }
        else
        {
          if ( cmd->cmdType == 0 )
          {
            ret = dc_cpuapdu( this->deviceHandle, cmd->sendLen, cmd->sendBuffer, ( unsigned char* )&receiveLen, cmd->receiveBuffer );
            cmd->receiveLen = receiveLen;
          }
          else
          {
            unsigned char ucCmd[512];
            int  iCmdLen = 0;
            Engine::Func::hex2dec( ( char* )cmd->sendBuffer, cmd->sendLen, ucCmd, iCmdLen );
            ret = dc_cpuapdu( this->deviceHandle, iCmdLen, ucCmd, ( unsigned char* )&receiveLen, cmd->receiveBuffer );
            cmd->receiveLen = receiveLen;
          }
        }
      }
      else
      {
        //PSAM卡接触式
        if ( deviceInfo.psamSlot == CARDSLOT_RF )
        {
          sprintf( errorMsg, "PSAM 插槽[%d]非法", deviceInfo.psamSlot );
          return DEV_CMDERROR;
        }
        else
        {
          if ( cmd->cmdType == 0 )
          {
            ret = dc_cpuapdu( this->deviceHandle, cmd->sendLen, cmd->sendBuffer, ( unsigned char* )&receiveLen, cmd->receiveBuffer );
            cmd->receiveLen = receiveLen;
          }
          else
          {
            unsigned char ucCmd[512];
            int  iCmdLen = 0;
            Engine::Func::hex2dec( ( char* )cmd->sendBuffer, cmd->sendLen, ucCmd, iCmdLen );
            ret = dc_cpuapdu( this->deviceHandle, iCmdLen, ucCmd, ( unsigned char* )&receiveLen, cmd->receiveBuffer );
            cmd->receiveLen = receiveLen;
          }
        }
      }
      if ( ret )
      {
        cmd->retCode = 0;
        sprintf( this->errorMsg, "执行指令[%s]失败，返回值[%d]", cmd->sendBuffer, ret );
        GetErrorMsgByErrorCode( ret, this->errorMsg );
        strcpy( cmd->retMsg, errorMsg );
        return DEV_CMDERROR;
      }

      sw1 = cmd->receiveBuffer[cmd->receiveLen - 2];
      sw2 = cmd->receiveBuffer[cmd->receiveLen - 1];
      if ( sw1 != 0x90 && sw1 != 0x61 )
      {
        ret = sw1;
        ret = ret * 256 + sw2;
        cmd->retCode = ret;
        sprintf( this->errorMsg, "执行指令[%s]失败，状态值sw1[%d],sw2[%d]", cmd->sendBuffer, sw1, sw2 );
        return cmd->retCode;
      }
      if ( sw1 == 0x61 && sw2 != 0x00 )
      {
        memset( &innerCmd, 0, sizeof innerCmd );
        innerCmd.sendLen = sprintf( szCmd, "00C00000%02X", sw2 );
        memcpy( innerCmd.sendBuffer, szCmd, sizeof( szCmd ) );
        innerCmd.cmdType = 1; // 16 hex
        ret = ExecuteCpuCmd( cardType, &innerCmd, cardSlot );
        if ( ret == 0 )
        {
          cmd->receiveLen = innerCmd.receiveLen;
        }
        sprintf( this->errorMsg, "获取后续数据[%s]失败，返回值[%d]", innerCmd.sendBuffer, ret );
        memset( cmd->receiveBuffer, 0, sizeof( cmd->receiveBuffer ) );
        memcpy( cmd->receiveBuffer, innerCmd.receiveBuffer, innerCmd.receiveLen );
        cmd->retCode = innerCmd.retCode;

        return ret;
      }

      return SUCCESS;
    }

  } //namespace Device
} //namespace Engine

Engine::Device::CDeviceService* CreateDeviceService()
{
  return new Engine::Device::CDecardService();
}