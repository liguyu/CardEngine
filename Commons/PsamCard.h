/*
  Psam���ӿ�

  Author:       yiming.you
  Version:      1.0
*/

#ifndef PSAM_CARD_H_
#define PSAM_CARD_H_

#ifdef COMMONS_EXPORTS
#define COMMONS_API __declspec(dllexport)
#else
#define COMMONS_API __declspec(dllimport)
#endif

#include "Card.h"
#include "Device.h"
#include <map>

namespace Engine
{
  namespace Card
  {
    class COMMONS_API CPsamCardService
    {
    public:

      /// @brief ����Psam��
      ///
      /// @param[in ] :psamCard Psam����Ϣ�ṹ��
      /// @return     :0 ��ʾ�ɹ�
      virtual int InitPsamCard( PsamCard* psamCard ) = 0;

      /// @brief ��ȡPSAM���ն˱��
      ///
      /// @param[out] :termNo psam���ն˱��
      /// @return     :0 ��ʾ�ɹ�
      virtual int ReadPsamTermNo( char *termNo ) = 0;

      /// @brief ����PSAM��
      ///
      /// @return     :0 ��ʾ�ɹ�
      virtual int RecyclePsamCard() = 0;

	  /// @brief ��������MAC
      ///
      /// ͨ�����KEY���������Դ���ݽ��м��ܣ��õ�MACֵ
      /// @param[in ] :randomNumber �����
      /// @param[in ] :key ��Կ
      /// @param[in ] :keyLen ��Կ����
      /// @param[in ] :data ���ڼ���MAC��Դ����
      /// @param[in ] :dataLen Դ���ݵĳ���
      /// @param[out] :mac
      virtual void CalculateMac( uint8 *randomNumber, uint8 *key, int keyLen, const uint8 *data, int dataLen, uint8 *mac ) = 0;

      /// @brief ͨ��PSAM������MAC
      ///
      /// ���ÿ�����ID����ɢ���ӣ����������macCommand����MAC����
      /// @param[in ] :phyid ������ID
      /// @param[in ] :hexRandom �����
      /// @param[in ] :macCommand ��Ҫ����MAC������
      /// @param[out] :mac
      /// @return     :0 ��ʾ�ɹ�
      virtual int CalculateSafeMac( char* phyid, char* hexRandom, const char *macCommand, char *mac ) = 0;

      /// @brief ͨ��PSAM������MAC
      ///
      /// @param[in ] :appSerialNo ��Ӧ�����к�
      /// @param[in ] :keyVersion ��Կ�汾
      /// @param[in ] :hexRandom �����
      /// @param[in ] :macCommand ��Ҫ����MAC������
      /// @param[out] :mac
      /// @return     :0 ��ʾ�ɹ�
      virtual int CalculateSafeMac( char *appSerialNo, int keyVersion, char* hexRandom, const char *macCommand, char *mac ) = 0;

      virtual int EncryptRandom( char* phyid, int keyIndex, const char *hexRandom, char *encryptedRandom ) = 0;

      /// @brief ��ȡ����Կ
      ///
      /// @param[out] :mainKey ����Կ
      /// @return     :0 ��ʾ�ɹ�
      virtual int ReadMainKey( uint8 *mainKey ) = 0;

      /// @brief ��ȡPSAM����Ϣ
      ///
      /// @param[out] :psamCard psam����Ϣ
      /// @return     :0 ��ʾ�ɹ�
      virtual int ReadPsamCard( PsamCard* psamCard ) = 0;

      virtual char* GetErrorMsgByErrorCode( int errorCode, char *errorMsg ) = 0;

      /// @brief ͨ��PSAM����������
      ///
      /// @param[in ] :appSerialNo ��Ӧ�����к�
      /// @param[in ] :keyVersion ��Կ�汾
      /// @param[in ] :random �����
      /// @param[in ] :data �����ܵ�����
      /// @param[in ] :dataLen ���������ݵĳ���
      /// @param[out] :encryptedData ����֮�������
      /// @param[out] :encryptedDataLen ����֮������ݳ���
      /// @return     :0 ��ʾ�ɹ�
      virtual int EncryptData( char *appSerialNo, int keyVersion, uint8 *random, uint8 *data, int dataLen, uint8 *encryptedData, int& encryptedDataLen  ) = 0;

      /// @brief PSAM���ϵ縴λ�ɹ�֮����÷����������
      ///
      /// @return     :0 ��ʾ�ɹ�
      virtual int SuccessResetPsam() = 0;

      /// @brief ���ô�����Ϣ��ͬʱ��¼��־
      ///
      /// ֻ�������һ��������Ϣ��������֮���ͨ��GetLastErrorMsg()������ȡ
      /// @param[in ] :errorMsg ������Ϣ
      void SetLastErrorMsg( const char* errorMsg );

      /// @brief ��ȡ���һ��������Ϣ
      ///
      /// ����ʱ��ͨ������ֵ���߳���errorMsg�õ�������Ϣ������ֵһ��
      /// @param[out] :errorMsg ������Ϣ
      /// @return     :char* ������Ϣ
      char* GetLastErrorMsg ( char *errorMsg );

      /// @brief �����豸������
      ///
      /// @param[in ] : deviceService �豸������
      void SetDeviceService( Engine::Device::CDeviceService *deviceService );

      /// @brief ����PSAM����
      ///
      /// @param[out] :slot ������
      /// @return     :0 ��ʾ�ɹ�
      int SetPsamSlot( int slot );

	  /// @brief ���ÿ�Ƭ����Կ
      ///
      /// @param[in ] :mainKey ��Ƭ����Կ
      /// @return     :0 ��ʾ�ɹ�
      virtual int SetMainKey( char mainKey[33] ) = 0;

	  virtual void GenerateWorkKey(CPUCardKeys *cardKeys) = 0;

      /// @brief CPU��PSAM���ϵ縴λ
      ///
      /// @param[out] :len ���ظ�λ��Ϣ�ĳ���
      /// @param[out] :buffer ��ŷ��صĸ�λ��Ϣ
      /// @return     :0 ��ʾ�ɹ�
      int ResetPsam( uint8 *len, unsigned char *buffer );

      /// @brief CPU��PSAM���ϵ縴λ
      ///
      /// ���ܺ�ResetPsam( unsigned char *len, unsigned char *buffer )һ��
      /// @param[in ] :enforce �Ƿ�ǿ���ϵ縴λ��0Ϊ����Ҫǿ���ϵ縴λ
      /// @return     :0 ��ʾ�ɹ�
      int ResetPsam( int enforce = 0 );

      /// @brief ��ȡapdu���������
      ///
      /// @param[in ] :format apdu����ĸ�ʽ
      /// @return     :����format��ʽ��װ֮���apdu�����
      std::string GetCommand( std::string format, ... );

      /// @brief ִ��apdu����
      ///
      /// ִ�����֮�󣬿�ͨ��cmd�ṹ���е�receiveBuffer��retCode�õ��������ݺͷ��ش���
      /// @param[in ] :cmd apdu����ṹ��
      /// @return     :0 ��ʾ�ɹ�
      int ExecuteCpuCmd( Engine::Device::CpuCmd *cmd );

      /// @brief ִ��apdu����
      ///
      /// ִ�����֮�󣬿�ͨ��cmd�ṹ���е�receiveBuffer��retCode�õ��������ݺͷ��ش���
      /// @param[in ] :command apdu�����
      /// @param[in ] :cmd apdu����ṹ��
      /// @return     :0 ��ʾ�ɹ�
      int ExecuteCpuCmd( std::string command, Engine::Device::CpuCmd *cmd );

      /// @brief ִ��hex��ʽapdu����
      ///
      /// ִ�����֮�󣬿�ͨ��cmd�ṹ���е�receiveBuffer��retCode�õ��������ݺͷ��ش���
      /// @param[in ] :command apdu�����
      /// @param[in ] :cmd apdu����ṹ��
      /// @return     :0 ��ʾ�ɹ�
      int ExecuteHexCpuCmd( std::string command, Engine::Device::CpuCmd *cmd );

      /// @brief ���apdu����
      ///
      /// @param[in ] :cmd apdu����ṹ��
      void ClearCpuCmd( Engine::Device::CpuCmd *cmd );

	  /// @brief �Ƿ��Ѽ��ظ���Կ
      ///
      /// @return     :true �Ѽ��ظ���Կ
      bool HasLoadMainKey();

    protected:

      char lastErrorMsg[ERROR_MSG_LEN];                    //��������Ϣ

      Engine::Device::CDeviceService *deviceService;   //�豸������� һ��ָ������

      bool hasReset;          //PSAM���Ƿ����ϵ縴λ

	  char mainKey[33];

	  bool hasLoadMainKey;
    };

  } // namespace Card
} // namespace Engine


#endif // PSAM_CARD_H_
