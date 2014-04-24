/*
  德卡D8读卡器通用操作

  Author:       yiming.you
  Version:      1.0
*/

#ifndef DECARD_H_
#define DECARD_H_

#ifdef DECARD_EXPORTS
#define DECARD_API __declspec(dllexport)
#else
#define DECARD_API __declspec(dllimport)
#endif

#include "../Commons/Device.h"
#include "../ThirdParty/decard/include/dcrf32.h"

namespace Engine
{
  namespace Device
  {
    typedef struct tagDecardPara
    {
      int port;         //端口
      int baud;         //波特率
      int psamSlot;     //psam卡插槽
    } DecardPara;       //Decard读卡器的参数

    class CDecardService: public CDeviceService
    {
    public:
      CDecardService();
      char* GetLastErrorMsg( char *message );

      /// @brief 初始化通讯端口
      ///
      /// 在使用读卡器对卡进行操作之前，需首先调用该方法
      /// @param[in ] :port 端口号，取值为0-19，表示串口1-20，为100时表示USB口通讯
      /// @param[in ] :baud 通讯波特率9600～115200，采用USB口通讯时，波特率无效
      /// @return     :0 表示成功
      int OpenDevice( int port, int baud );

      int CloseDevice();
      int RequestCard( char *phyid, int& cardType, CARD_SLOT cardSlot = CARDSLOT_RF );
      int Halt();
      int Beep();
      int BeepError();
      int Reset( size_t msec, CARD_SLOT cardSlot = CARDSLOT_RF );
      int CpuReset( CARD_TYPE cardType, uint8 *len, uint8 *buffer );
      int ExecuteCpuCmd( CARD_TYPE cardType, CpuCmd *cmd, CARD_SLOT cardSlot );
      int SetPsamSlot( int slot );


    private:
      //通过错误代码得到错误信息，此处的错误代码翻译均参考自德卡读卡器“函数错误代码说明”
      char* GetErrorMsgByErrorCode( int errorCode, char* errorMsg );

      //德卡读卡器设备标识句柄，程序退出时需释放掉
      HANDLE deviceHandle;

      //设备操作错误信息，仅保存最后一次错误信息
      char errorMsg[ERROR_MSG_LEN];

      //最后一次操作的卡类型
      int lastCardType;

      ///设备信息
      DeviceInfo deviceInfo;

      ///设备初始化参数
      DecardPara para;
    };

  } // namespace Device
} // namespace Engine

extern "C"
{

  /// @brief 创建设备管理类
  ///
  ///系统会通过动态加载dll的方式调用该方法，创建出CDeviceService对卡片进行操作
  /// @return     :CDecardService对象
  DECARD_API Engine::Device::CDeviceService* CreateDeviceService();
}

#endif // DECARD_H_
