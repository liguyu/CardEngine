/*
  �¿�D8������ͨ�ò���

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
      int port;         //�˿�
      int baud;         //������
      int psamSlot;     //psam�����
    } DecardPara;       //Decard�������Ĳ���

    class CDecardService: public CDeviceService
    {
    public:
      CDecardService();
      char* GetLastErrorMsg( char *message );

      /// @brief ��ʼ��ͨѶ�˿�
      ///
      /// ��ʹ�ö������Կ����в���֮ǰ�������ȵ��ø÷���
      /// @param[in ] :port �˿ںţ�ȡֵΪ0-19����ʾ����1-20��Ϊ100ʱ��ʾUSB��ͨѶ
      /// @param[in ] :baud ͨѶ������9600��115200������USB��ͨѶʱ����������Ч
      /// @return     :0 ��ʾ�ɹ�
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
      //ͨ���������õ�������Ϣ���˴��Ĵ�����뷭����ο��Ե¿��������������������˵����
      char* GetErrorMsgByErrorCode( int errorCode, char* errorMsg );

      //�¿��������豸��ʶ����������˳�ʱ���ͷŵ�
      HANDLE deviceHandle;

      //�豸����������Ϣ�����������һ�δ�����Ϣ
      char errorMsg[ERROR_MSG_LEN];

      //���һ�β����Ŀ�����
      int lastCardType;

      ///�豸��Ϣ
      DeviceInfo deviceInfo;

      ///�豸��ʼ������
      DecardPara para;
    };

  } // namespace Device
} // namespace Engine

extern "C"
{

  /// @brief �����豸������
  ///
  ///ϵͳ��ͨ����̬����dll�ķ�ʽ���ø÷�����������CDeviceService�Կ�Ƭ���в���
  /// @return     :CDecardService����
  DECARD_API Engine::Device::CDeviceService* CreateDeviceService();
}

#endif // DECARD_H_
