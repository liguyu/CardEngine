#include <sstream>
#include "Logger.h"
#include "UserCard.h"
#include "GlobalFunc.h"
using namespace Engine::Func;
using namespace Engine::Logging;

namespace Engine
{
  namespace Card
  {
    void CUserCardService::SetLastErrorMsg( const char* errorMsg )
    {
      strcpy( lastErrorMsg, errorMsg );
      ErrorLogging( "%s", errorMsg );
    }

    char* CUserCardService::GetLastErrorMsg ( char *errorMsg )
    {
      if ( errorMsg != NULL )
      {
        strcpy( errorMsg, lastErrorMsg );
      }
      return lastErrorMsg;
    }

    int CUserCardService::SetFieldReadMode( int fieldId )
    {
      FieldInfo fieldInfo = fieldList[fieldId];
      if ( fieldId > CF_END || fieldId < CF_START )
      {
        SET_ERR( "SetFieldReadMode:字段索引" << fieldId << "不存在" );
        return CF_INDEX_ERR;
      }
      if ( fieldInfo.unused )
      {
        SET_ERR( "SetFieldReadMode:字段索引" << fieldId << "未使用" );
        return CF_INDEX_UNUSED;
      }
      if ( fieldInfo.rw == RW_W )
      {
        SET_ERR( "SetFieldWriteMode:字段索引" << fieldId << "不允许读" );
        return CF_READ_RIGHT_ERR;
      }
      fieldMap[fieldInfo.fid].ReadFieldSet.insert( fieldId );

      return SUCCESS;
    }

    int CUserCardService::SetFieldWriteMode( int fieldId )
    {
      if ( fieldId > CF_END || fieldId < CF_START )
      {
        SET_ERR( "SetFieldWriteMode:字段索引" << fieldId << "不存在" );
        return CF_INDEX_ERR;
      }
      if ( fieldList[fieldId].unused )
      {
        SET_ERR( "SetFieldWriteMode:字段索引" << fieldId << "未使用" );
        return CF_INDEX_UNUSED;
      }
      if ( fieldList[fieldId].rw == RW_R )
      {
        SET_ERR( "SetFieldWriteMode:字段索引" << fieldId << "不允许写" );
        return CF_READ_RIGHT_ERR;
      }
      fieldMap[fieldList[fieldId].fid].WriteFieldSet.insert( fieldId );

      return SUCCESS;
    }

    int CUserCardService::SetFieldAllMode( int fieldId )
    {
      int ret = SetFieldReadMode( fieldId );
      if ( ret )
      {
        return ret;
      }
      ret = SetFieldWriteMode( fieldId );
      if ( ret )
      {
        return ret;
      }

      return SUCCESS;
    }

    int CUserCardService::ClearFieldReadMode( int fieldId )
    {
      if ( fieldId > CF_END || fieldId < CF_START )
      {
        SET_ERR( "SetFieldWriteMode:字段索引" << fieldId << "不存在" );
        return CF_INDEX_ERR;
      }
      if ( fieldList[fieldId].unused )
      {
        SET_ERR( "SetFieldWriteMode:字段索引" << fieldId << "未使用" );
        return CF_INDEX_UNUSED;
      }
      std::set<int>::iterator it;
      it = fieldMap[fieldList[fieldId].fid].ReadFieldSet.find( fieldId );
      if ( it != fieldMap[fieldList[fieldId].fid].ReadFieldSet.end() )
      {
        fieldMap[fieldList[fieldId].fid].ReadFieldSet.erase( it );
        return SUCCESS;
      }
      else
      {
        SET_ERR( "SetFieldWriteMode:字段索引" << fieldId << "不存在" );
        return CF_INDEX_ERR;
      }

      return SUCCESS;
    }

    int CUserCardService::ClearFieldWriteMode( int fieldId )
    {
      if ( fieldId > CF_END || fieldId < CF_START )
      {
        SET_ERR( "SetFieldWriteMode:字段索引" << fieldId << "不存在" );
        return CF_INDEX_ERR;
      }
      if ( fieldList[fieldId].unused )
      {
        SET_ERR( "SetFieldWriteMode:字段索引" << fieldId << "未使用" );
        return SUCCESS;
      }
      std::set<int>::iterator it;
      it = fieldMap[fieldList[fieldId].fid].WriteFieldSet.find( fieldId );
      if ( it != fieldMap[fieldList[fieldId].fid].WriteFieldSet.end() )
      {
        fieldMap[fieldList[fieldId].fid].WriteFieldSet.erase( it );
        return SUCCESS;
      }
      else
      {
        SET_ERR( "SetFieldWriteMode:字段索引" << fieldId << "不存在" );
        return CF_INDEX_ERR;
      }

      return SUCCESS;
    }

    int CUserCardService::ClearFieldAllMode( int fieldId )
    {
      int ret = ClearFieldReadMode( fieldId );
      if ( ret )
      {
        return ret;
      }
      ret = ClearFieldWriteMode( fieldId );
      if ( ret )
      {
        return ret;
      }

      return SUCCESS;
    }

    int CUserCardService::ClearAllFieldAllMode()
    {
      ClearAllFieldReadMode();
      ClearAllFieldWriteMode();

      return SUCCESS;
    }

    int CUserCardService::ClearAllFieldReadMode()
    {
      DebugLogging( "%s", "清除所有字段读模式..." );
      std::map<int, FieldBuffer>::iterator it;
      for ( it = fieldMap.begin(); it != fieldMap.end(); ++it )
      {
        it->second.ReadFieldSet.clear();
        memset( it->second.ReadBuf, 0, sizeof( it->second.ReadBuf ) );
      }
     
      return SUCCESS;
    }

    int CUserCardService::ClearAllFieldWriteMode()
    {
      std::map<int, FieldBuffer>::iterator it;
      for ( it = fieldMap.begin(); it != fieldMap.end(); ++it )
      {
        it->second.WriteFieldSet.clear();
        memset( it->second.WriteBuf, 0, sizeof( it->second.WriteBuf ) );
      }
      return SUCCESS;
    }

    int CUserCardService::RequestCard( char *phyid, int& cardType )
    {
      memset( this->lastErrorMsg, 0, sizeof( this->lastErrorMsg ) );
      memset( this->phyid, 0, sizeof( this->phyid ) );
      memset( this->phyno, 0, sizeof( this->phyno ) );

      int ret = this->deviceService->RequestCard( this->phyid, cardType );
      if ( ret )
      {
        SET_ERR( "寻卡失败" );
        return ret;
      }

      ret = this->SuccessRequestCard();
      if ( ret )
      {
        return ret;
      }

      if ( phyid != NULL )
      {
        memcpy( phyid, this->phyid, sizeof( this->phyid ) );
      }
      this->GeneratePhyno( this->phyid );

      return SUCCESS;
    }

    int CUserCardService::RequestCard( char *phyid )
    {
      int cardType = 0;
      return this->RequestCard( phyid, cardType );
    }

    int CUserCardService::CheckFieldReadMode( int fieldId )
    {
      //检查字段是否在正常范围内
      if ( fieldId > CF_END || fieldId < CF_START )
      {
        SET_ERR( "CheckFieldReadMode:字段索引" << fieldId << "超出范围" );
        return CF_INDEX_ERR;
      }

      //检查字段是否已设置读模式
      std::set<int>::iterator iter;
      iter = fieldMap[fieldList[fieldId].fid].ReadFieldSet.find( fieldId );
      if ( iter == fieldMap[fieldList[fieldId].fid].ReadFieldSet.end() )
      {
        SET_ERR( "CheckFieldReadMode:字段索引" << fieldId << "未设置读" );
        return CF_NO_READ_MODE;
      }

      return SUCCESS;
    }

    int CUserCardService::CheckFieldWriteMode( int fieldId )
    {
      //检查字段是否在正常范围内
      if ( fieldId > CF_END || fieldId < CF_START )
      {
        SET_ERR( "CheckFieldWriteMode:字段索引" << fieldId << "超出范围" );
        return CF_INDEX_ERR;
      }

      //检查字段是否已设置写模式
      std::set<int>::iterator iter;
      iter = fieldMap[fieldList[fieldId].fid].WriteFieldSet.find( fieldId );
      if ( iter == fieldMap[fieldList[fieldId].fid].WriteFieldSet.end() )
      {
        SET_ERR( "CheckFieldWriteMode:字段索引" << fieldId << "未设置读" );
        return CF_NO_READ_MODE;
      }

      return SUCCESS;
    }

    int CUserCardService::FieldHasRead( int fieldId )
    {
	  //TODO
      return -1;
    }

    void CUserCardService::GetCurrentDateTime( char *datetime )
    {
      time_t t;
      t = time( NULL );
      struct tm * tx = localtime( &t );
      sprintf( datetime, "%04d%02d%02d%02d%02d%02d", tx->tm_year + 1900, tx->tm_mon + 1, tx->tm_mday,
               tx->tm_hour, tx->tm_min, tx->tm_sec );

    }

    void CUserCardService::GetCurrentHexDateTime( uint8 datetime[5] )
    {
      char datetimeStr[20] = "";
      char temp[2] = "";
      GetCurrentDateTime( datetimeStr );
      for ( int i = 0; i < 5; ++i )
      {
        memset( temp, 0, sizeof temp );
        memcpy( temp, datetimeStr + 2 + i * 2, 2 );
        datetime[i] = atoi( temp );
      }
    }

    int CUserCardService::GeneratePhyno( char *phyid )
    {
      int len;
      uint8 tmp[16];
      hex2dec( phyid, 8, tmp, len );
      for ( int i = 0; i < 4; ++i )
      {
        tmp[7 - i] = tmp[i];
      }

      memset( this->phyno, 0, sizeof( this->phyno ) );
      memcpy( this->phyno, tmp + 4, 4 );

      return SUCCESS;
    }

    int CUserCardService::SetFieldValue( const char *fieldValue, int fieldType, int len, uint8 *fieldBuffer )
    {
      int num;
      int dataLen = strlen( fieldValue );

      char temp[255] = {0};

      switch ( fieldType )
      {
      case FT_ANSI:
        memcpy( fieldBuffer, fieldValue, dataLen );
        break;
      case FT_BCD:
        asc2bcd( fieldValue, len * 2, fieldBuffer, dataLen );
        break;
      case FT_BCD_LAPDZERO:
        memcpy( temp, fieldValue, dataLen );
        lpad( temp, len * 2, '0' );
        asc2bcd( temp, len * 2, fieldBuffer, num );
        break;
      case FT_BCD_LPAD20:
        if ( dataLen == 12 )
        {
          sprintf( temp, "20%s", fieldValue );
        }
        asc2bcd( temp, dataLen * 2, fieldBuffer, num );
        break;
      case FT_INT8:
        num = atoi( fieldValue );
        fieldBuffer[0] = num % 256;
        break;
      case FT_INT16:
        num = atoi( fieldValue );
        set_n_byte_int<int, 2>( fieldBuffer, sizeof( fieldBuffer ), num );
        break;
      case FT_INT24:
        num = atoi( fieldValue );
        set_n_byte_int<int, 3>( fieldBuffer, sizeof( fieldBuffer ), num );
        break;
      case FT_INT32:
        num = atoi( fieldValue );
        set_n_byte_int<int, 4>( fieldBuffer, sizeof( fieldBuffer ), num );
        break;
      case FT_LT_INT16:
        num = atoi( fieldValue );
        set_n_byte_lt_int<int, 2>( fieldBuffer, sizeof( fieldBuffer ), num );
        break;
      case FT_LT_INT24:
        num = atoi( fieldValue );
        set_n_byte_lt_int<int, 3>( fieldBuffer, sizeof( fieldBuffer ), num );
        break;
      case FT_LT_INT32:
        num = atoi( fieldValue );
        set_n_byte_lt_int<int, 4>( fieldBuffer, sizeof( fieldBuffer ), num );
        break;
      case FT_HEX_DATE:
        SetHexDate( fieldValue, len, fieldBuffer );
        break;
      case FT_HEX_TIME:
        SetHexTime( fieldValue, len, fieldBuffer );
        break;
      case FT_UCS2:
        EncodeUCS2( ( uint8* )fieldValue, fieldBuffer );
        break;
      case FT_HEX_STR:
        if ( dataLen % 2 != 0 ) dataLen++;
        if ( dataLen > len * 2 )
          dataLen = len * 2;
        hex2dec( fieldValue, dataLen, fieldBuffer, dataLen );
        break;
      default:
        return CF_FIELD_DEF_ERR;
      }

      return SUCCESS;
    }

    int CUserCardService::SetHexDate( const char* fieldValue, int len, uint8 *fieldBuffer )
    {
      char temp[10];
      int i;
      memset( temp, 0, sizeof( temp ) );
      if ( len == 3 )
      {
        for ( i = 0; i < 3; ++i )
        {
          memcpy( temp, fieldValue + 2 + i * 2, 2 );
          fieldBuffer[i] = atoi( temp );
        }
      }
      else if ( len == 4 )
      {
        for ( i = 0; i < 4; ++i )
        {
          memcpy( temp, fieldValue + i * 2, 2 );
          fieldBuffer[i] = atoi( temp );
        }
      }
      else
      {
        return CF_FIELD_DEF_ERR;
      }

      return SUCCESS;
    }

    int CUserCardService::SetHexTime( const char* fieldValue, int len, uint8 *fieldBuffer )
    {
      char temp[10];
      int i;
      if ( len == 2 )
      {
        for ( i = 0; i < 2; ++i )
        {
          memcpy( temp, fieldValue + i * 2, 2 );
          fieldBuffer[i] = atoi( temp );
        }
        fieldBuffer[2] = 0;
      }
      else if ( len == 3 )
      {
        for ( i = 0; i < 3; ++i )
        {
          memcpy( temp, fieldValue + i * 2, 2 );
          fieldBuffer[i] = atoi( temp );
        }
      }
      else
      {
        return CF_FIELD_DEF_ERR;
      }

      return SUCCESS;
    }

    int CUserCardService::GetFieldValue( uint8 *fieldBuffer, int fieldType, int len, char *fieldValue )
    {
      int num , i;
      char tmp[255] = {0};
      switch ( fieldType )
      {
      case FT_ANSI:
        memcpy( fieldValue, fieldBuffer, len );
        fieldValue[len] = 0;
        break;
      case FT_BCD:
        bcd2asc( fieldBuffer, len, fieldValue );
        fieldValue[len * 2] = 0;
        break;
      case FT_BCD_LPAD20:
        bcd2asc( fieldBuffer, len, fieldValue + 2 );
        fieldValue[len * 2] = 0;
        break;
      case FT_BCD_LAPDZERO:
        bcd2asc( fieldBuffer, len, tmp );
        for ( i = 0; i < len * 2; i++ )
        {
          if ( tmp[i] != '0' )
            break;
        }
        strcpy( fieldValue, tmp + i );
        break;
      case FT_INT8:
        sprintf( fieldValue, "%d", fieldBuffer[0] );
        break;
      case FT_INT16:
        num = get_n_byte_int<int, 2>( fieldBuffer, len );
        sprintf( fieldValue, "%d", num );
        break;
      case FT_INT24:
        num = get_n_byte_int<int, 3>( fieldBuffer, len );
        sprintf( fieldValue, "%d", num );
        break;
      case FT_INT32:
        num = get_n_byte_int<int, 4>( fieldBuffer, len );
        sprintf( fieldValue, "%d", num );
        break;
      case FT_LT_INT16:
        num = get_n_byte_lt_int<int, 2>( fieldBuffer, len );
        sprintf( fieldValue, "%d", num );
        break;
      case FT_LT_INT24:
        num = get_n_byte_lt_int<int, 3>( fieldBuffer, len );
        sprintf( fieldValue, "%d", num );
        break;
      case FT_LT_INT32:
        num = get_n_byte_lt_int<int, 4>( fieldBuffer, len );
        sprintf( fieldValue, "%d", num );
        break;
      case FT_HEX_DATE:
        GetDateFromHex( fieldBuffer, len, fieldValue );
        break;
      case FT_HEX_TIME:
        GetTimeFromHex( fieldBuffer, len, fieldValue );
        break;
      case FT_UCS2:
        DecodeUCS2( fieldBuffer, ( uint8* )fieldValue, len );
        break;
      case FT_HEX_STR:
        dec2hex( fieldBuffer, len, fieldValue );
        break;
      default:
        return CF_FIELD_DEF_ERR;
      }
      return SUCCESS;
    }

    int CUserCardService::GetDateFromHex( uint8 *fieldBuffer, int len, char *fieldValue )
    {
      //如果是三字节的日期，在最前面补上20
      uint8 temp[10];
      if ( len == 3 )
      {
        temp[0] = 20;
        memcpy( temp + 1, fieldBuffer, len );
      }
      else if ( len == 4 )
      {
        memcpy( temp, fieldBuffer, len );
      }
      else
      {
        return CF_FIELD_DEF_ERR;
      }
      sprintf( fieldValue, "%02d%02d%02d%02d", temp[0], temp[1], temp[2], temp[3] );

      return SUCCESS;
    }

    int CUserCardService::GetTimeFromHex( uint8 *fieldBuffer, int len, char *fieldValue )
    {
      //如果是2字节时间，在最后面补上秒(00)
      uint8 temp[10];
      if ( len == 2 )
      {
        memcpy( temp, fieldBuffer, len );
        temp[2] = 00;
      }
      else if ( len == 3 )
      {
        memcpy( temp, fieldBuffer, len );
      }
      else
      {
        return CF_FIELD_DEF_ERR;
      }
      sprintf( fieldValue, "%02d%02d%02d", temp[0], temp[1], temp[2] );

      return SUCCESS;
    }

    void CUserCardService::SetDeviceService( Engine::Device::CDeviceService *deviceService )
    {
      this->deviceService = deviceService;
    }

	void CUserCardService::SetPsamCardervice( Engine::Card::CPsamCardService *psamCardService )
    {
      this->psamCardService = psamCardService;
    }

    uint8* CUserCardService::GetMainKey()
    {
      return NULL;
    }

  } //namespace Card
} //namespace Engine