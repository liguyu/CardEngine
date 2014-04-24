#include <string>
#include <sstream>
#include <stdarg.h>
#include "PsamCard.h"
#include "Logger.h"

using namespace std;
using namespace Engine::Device;
using namespace Engine::Logging;

namespace Engine
{
  namespace Card
  {
    int CPsamCardService::SetPsamSlot( int slot )
    {
      if ( slot < 1 || slot > 4 )
      {
        SET_ERR( "PSAM卡座参数错误port[" << slot << "]" );
        return DEV_PSAM_SLOT_ERR;
      }

      this->deviceService->SetPsamSlot( slot );
      return this->ResetPsam( 1 );  //强制上电复位
    }

    int CPsamCardService::ResetPsam( unsigned char *len, unsigned char *buffer )
    {
      int ret;

      ret = this->deviceService->CpuReset( PSAM, len, buffer );
      if ( ret )
      {
        SET_ERR( "PSAM卡座上电复位错误,ret[" << ret << "]" );
        return ret;
      }
      hasReset = true;
      ret = this->SuccessResetPsam();

      return ret;
    }

    int CPsamCardService::ResetPsam( int enforce )
    {
      if ( enforce == 0 && hasReset ) //如果不需要强制上电复位且已经上电复位过，那么直接返回
      {
        return SUCCESS;
      }

      unsigned char len[2] = {0};
      unsigned char buffer[256] = {0};
      return this->ResetPsam( len, buffer );
    }

    std::string CPsamCardService::GetCommand( std::string format, ... )
    {
      char command[MAX_APDU_COMMAND_LEN] = {0};
      va_list args;
      va_start ( args, format );
      vsprintf ( command, format.c_str(), args );
      va_end ( args );
      return std::string( command );
    }

    int CPsamCardService::ExecuteCpuCmd( CpuCmd *cmd )
    {
      int ret;
      ret = this->deviceService->ExecuteCpuCmd( PSAM, cmd, CARDSLOT_1 );
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

    int CPsamCardService::ExecuteCpuCmd( string command, CpuCmd *cmd )
    {
      this->ClearCpuCmd( cmd );
      strcpy( ( char* )cmd->sendBuffer, command.c_str() );
      cmd->sendLen = command.size();
      return this->ExecuteCpuCmd( cmd );
    }

    int CPsamCardService::ExecuteHexCpuCmd( string command, CpuCmd *cmd )
    {
      this->ClearCpuCmd( cmd );
      cmd->cmdType = 1;
      strcpy( ( char* )cmd->sendBuffer, command.c_str() );
      cmd->sendLen = command.size();
      return this->ExecuteCpuCmd( cmd );
    }

    void CPsamCardService::ClearCpuCmd( CpuCmd *cmd )
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

    void CPsamCardService::SetLastErrorMsg( const char* errorMsg )
    {
      strcpy( lastErrorMsg, errorMsg );
      DebugLogging( "%s", errorMsg );
    }

    char* CPsamCardService::GetLastErrorMsg ( char *errorMsg )
    {
      if ( errorMsg != NULL )
      {
        strcpy( errorMsg, lastErrorMsg );
      }
      return lastErrorMsg;
    }

    void CPsamCardService::SetDeviceService( Engine::Device::CDeviceService *deviceService )
    {
      this->deviceService = deviceService;
    }

	bool CPsamCardService::HasLoadMainKey()
	{
		return this->hasLoadMainKey;
	}
  }
}