/*
    设备层接口，用于屏蔽不同读卡器之间的差异

    Author:       yiming.you
	Version:      1.0
*/

#ifndef DEVICE_H_
#define DEVICE_H_

#include "Global.h"
#include "Logger.h"

namespace Engine
{
  namespace Device
  {
	enum {BEEP_ERR_TIME = 160}; //160为读卡器蜂鸣时间，单位为毫秒
    enum {USB_PORT = 100};      //使用USB设备时对应的端口号

    typedef struct tagDeviceInfo
    {
      int port;                 //读卡器端口号
      int baud;                 //波特率
      int psamSlot;             //psam卡卡座
    } DeviceInfo;

    typedef struct tagCpuCmd
    {
      uint8 cmdType;            //0表示原始字符格式 ,1表示hex格式
      uint8 sendLen;            //发送数据长度
      uint8 sendBuffer[255];    //发送数据
      uint8 receiveLen;         //返回数据长度
      uint8 receiveBuffer[255]; //执行指令返回数据
      int	retCode;              //执行指令返回值
      char retMsg[256];         //执行指令返回信息
      int retry;                //失败之后尝试执行次数
    } CpuCmd;

    class CDeviceService
    {
    public:
      //获得读卡器最近一次操作的错误信息
      virtual char* GetLastErrorMsg( char *message ) = 0;

      //打开端口，所有调用读卡器进行的操作，均需要在成功打开端口之后才能进行
      virtual int OpenDevice( int port, int baud ) = 0;

      //关闭端口，在系统退出时，调用该方法，释放资源
      virtual int CloseDevice() = 0;

      //只负责寻卡 不复位
      virtual int RequestCard( char *phyid, int& cardType, CARD_SLOT cardSlot = CARDSLOT_RF ) = 0;

      //中止对已选卡片的操作，终止之后，如果还需要对该卡片进行操作，需将其先移出感应区，再移进来才能寻到卡片
      virtual int Halt() = 0;

      //读卡器蜂鸣
      virtual int Beep() = 0;

      //通过控制读卡器蜂鸣时间来表示出错情况
      virtual int BeepError() = 0;

      //读卡器复位,msec为复位时间，单位为毫秒
      virtual int Reset( size_t msec, CARD_SLOT cardSlot = CARDSLOT_RF ) = 0;

      //---------------------cpu、psam卡操作------------------------------
      //CPU，PSAM卡上电复位
      virtual int CpuReset( CARD_TYPE cardType, uint8 *len, uint8 *buffer ) = 0;
      //执行cpu指令
      virtual int ExecuteCpuCmd(  CARD_TYPE cardType, CpuCmd *cmd, CARD_SLOT cardSlot ) = 0;

      ///设置PSAM卡座
      virtual int SetPsamSlot( int slot ) = 0;
	  
    protected:
      char version[100];
      char deviceName[100];
    };

  } // namespace Device
} // namespace Engine


#endif // DEVICE_H_
