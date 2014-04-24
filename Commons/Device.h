/*
    �豸��ӿڣ��������β�ͬ������֮��Ĳ���

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
	enum {BEEP_ERR_TIME = 160}; //160Ϊ����������ʱ�䣬��λΪ����
    enum {USB_PORT = 100};      //ʹ��USB�豸ʱ��Ӧ�Ķ˿ں�

    typedef struct tagDeviceInfo
    {
      int port;                 //�������˿ں�
      int baud;                 //������
      int psamSlot;             //psam������
    } DeviceInfo;

    typedef struct tagCpuCmd
    {
      uint8 cmdType;            //0��ʾԭʼ�ַ���ʽ ,1��ʾhex��ʽ
      uint8 sendLen;            //�������ݳ���
      uint8 sendBuffer[255];    //��������
      uint8 receiveLen;         //�������ݳ���
      uint8 receiveBuffer[255]; //ִ��ָ�������
      int	retCode;              //ִ��ָ���ֵ
      char retMsg[256];         //ִ��ָ�����Ϣ
      int retry;                //ʧ��֮����ִ�д���
    } CpuCmd;

    class CDeviceService
    {
    public:
      //��ö��������һ�β����Ĵ�����Ϣ
      virtual char* GetLastErrorMsg( char *message ) = 0;

      //�򿪶˿ڣ����е��ö��������еĲ���������Ҫ�ڳɹ��򿪶˿�֮����ܽ���
      virtual int OpenDevice( int port, int baud ) = 0;

      //�رն˿ڣ���ϵͳ�˳�ʱ�����ø÷������ͷ���Դ
      virtual int CloseDevice() = 0;

      //ֻ����Ѱ�� ����λ
      virtual int RequestCard( char *phyid, int& cardType, CARD_SLOT cardSlot = CARDSLOT_RF ) = 0;

      //��ֹ����ѡ��Ƭ�Ĳ�������ֹ֮���������Ҫ�Ըÿ�Ƭ���в������轫�����Ƴ���Ӧ�������ƽ�������Ѱ����Ƭ
      virtual int Halt() = 0;

      //����������
      virtual int Beep() = 0;

      //ͨ�����ƶ���������ʱ������ʾ�������
      virtual int BeepError() = 0;

      //��������λ,msecΪ��λʱ�䣬��λΪ����
      virtual int Reset( size_t msec, CARD_SLOT cardSlot = CARDSLOT_RF ) = 0;

      //---------------------cpu��psam������------------------------------
      //CPU��PSAM���ϵ縴λ
      virtual int CpuReset( CARD_TYPE cardType, uint8 *len, uint8 *buffer ) = 0;
      //ִ��cpuָ��
      virtual int ExecuteCpuCmd(  CARD_TYPE cardType, CpuCmd *cmd, CARD_SLOT cardSlot ) = 0;

      ///����PSAM����
      virtual int SetPsamSlot( int slot ) = 0;
	  
    protected:
      char version[100];
      char deviceName[100];
    };

  } // namespace Device
} // namespace Engine


#endif // DEVICE_H_
