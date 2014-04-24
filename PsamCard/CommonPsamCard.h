/*
  ���ܴ�Psam��ʵ��

  Author:       yiming.you
  Version:      1.0
*/

#ifndef COMMON_PSAM_CARD_H_
#define COMMON_PSAM_CARD_H_

#ifdef PSAMCARD_EXPORTS
#define PSAMCARD_API __declspec(dllexport)
#else
#define PSAMCARD_API __declspec(dllimport)
#endif

#include "../Commons/PsamCard.h"

namespace Engine
{
  namespace Card
  {
    typedef struct tagApduPsamCard
    {
      std::string SELECT;                 //ѡ���ļ�
      std::string SELECT_MF;              //ѡ��MFĿ¼
      std::string READ_BINARY;            //���������ļ�
      std::string GET_CHALLENGE;          //ȡ�����
      std::string ERASE_MF;               //�����ļ�ϵͳ
      std::string END_ISSUE;              //�������˻�
      std::string END_ISSUE_MAC;          //��MAC�������˻�
      std::string	CREATE_MF;              //�������ļ�
      std::string CREATE_FILE;            //�����ļ�
      std::string UPDATE_BINARY;          //���¶������ļ�
      std::string DELIVERY_KEY;           //��ɢ��Կ
      std::string CIPHER_DATA;            //��������
      std::string INIT_SAM_FOR_PURCHASE;  //���ѽ��׳�ʼ��

    } ApduPsamCard;

    enum {MAX_KEY_INDEX = 2};

    class CCommonPsamCardService: public CPsamCardService
    {
    public:
      CCommonPsamCardService();

      /// @brief ��ȡPSAM���ն˱��
      ///
      /// @param[out] :termNo psam���ն˱��
      /// @return     :0 ��ʾ�ɹ�
      int ReadPsamTermNo( char *termNo );

      /// @brief ����PSAM��
      ///
      /// @return     :0 ��ʾ�ɹ�
      int RecyclePsamCard();

      /// @brief ����Psam��
      ///
      /// @param[in ] :psamCard Psam����Ϣ�ṹ��
      /// @return     :0 ��ʾ�ɹ�
      int InitPsamCard( PsamCard* psamCard );

      /// @brief ͨ��PSAM������MAC
      ///
      /// ���ÿ�����ID����ɢ���ӣ����������macCommand����MAC����
      /// @param[in ] :phyid ������ID
      /// @param[in ] :hexRandom �����
      /// @param[in ] :macCommand ��Ҫ����MAC������
      /// @param[out] :mac
      /// @return     :0 ��ʾ�ɹ�
      int CalculateSafeMac( char* phyid, char* hexRandom, const char *macCommand, char *mac );

      /// @brief ͨ��PSAM������MAC
      ///
      /// @param[in ] :appSerialNo ��Ӧ�����к�
      /// @param[in ] :keyVersion ��Կ�汾
      /// @param[in ] :hexRandom �����
      /// @param[in ] :macCommand ��Ҫ����MAC������
      /// @param[out] :mac
      /// @return     :0 ��ʾ�ɹ�
      int CalculateSafeMac( char *appSerialNo, int keyVersion, char* hexRandom, const char *macCommand, char *mac );

      /// @brief ��ȡPSAM����Ϣ
      ///
      /// @param[out] :psamCard psam����Ϣ
      /// @return     :0 ��ʾ�ɹ�
      int ReadPsamCard( PsamCard* psamCard );

      /// @brief ��ȡ����Կ
      ///
      /// @param[out] :mainKey ����Կ
      /// @return     :0 ��ʾ�ɹ�
      int ReadMainKey( uint8 *mainKey );

      /// @brief PSAM���ϵ縴λ�ɹ�֮����÷����������
      ///
      /// @return     :0 ��ʾ�ɹ�
      int SuccessResetPsam();

	   /// @brief ���ÿ�Ƭ����Կ
      ///
      /// @param[in ] :mainKey ��Ƭ����Կ
      /// @return     :0 ��ʾ�ɹ�
      int SetMainKey( char mainKey[33] );

	  void GenerateWorkKey(CPUCardKeys *cardKeys);

    protected:

      /// @brief �����ļ���ʶѡ��Ŀ¼
      ///
      /// @param[in ] :FI �ļ���ʶ
      /// @return     :0 ��ʾ�ɹ�
      int SelectADFByFI( std::string FI );

      /// @brief ��ȡ�����
      ///
      /// GET CHALLENGE��������һ��������·�������̵������
      /// ���ǵ��磬ѡ��������Ӧ�û����ַ���һ��GET CHALLENGE����������������һ��ָ��ʱ��Ч
      /// ����һ��ָ��Ϊ�ⲿ��֤�����ⲿ��֤������ָ�����ⲿ��֤��Կ���ܺ������������бȽ�
      ///
      /// @param[out] :randomNumber 04/08���ȵ������
      /// @return     :0 ��ʾ�ɹ�
      int GetChallenge( uint8 *randomNumber );

      /// @brief ��ȡָ�����ȵĶ������ļ�
      ///
      /// @param[in ] :SFI ���ļ���ʶ
      /// @param[in ] :offset ƫ����
      /// @param[in ] :len Ҫ��ȡ�ĳ���
      /// @param[out] :binaryBuffer �������ļ�����
      /// @return     :0 ��ʾ�ɹ�
      int ReadBinaryFile( int SFI, int offset, int len, uint8 *binaryBuffer );

      /// @brief ��MAC��ʽ�������˻�
      ///
      /// ������ǰӦ�õĸ��˻�������Ӧ��״̬
      /// һ��Ӧ�õİ�ȫ��������ֻ���ڽ������˻���������
      /// ������������˻������Ӧ����������֤���ɻ������Ȩ��
      /// @return     :0 ��ʾ�ɹ�
      int EndIssueWithMac();

      /// @brief �������˻�
      ///
      /// ������ǰӦ�õĸ��˻�������Ӧ��״̬
      /// һ��Ӧ�õİ�ȫ��������ֻ���ڽ������˻���������
      /// ������������˻������Ӧ����������֤���ɻ������Ȩ��
      /// @return     :0 ��ʾ�ɹ�
      int EndIssue();

      /// @brief ��������MAC
      ///
      /// ͨ�����KEY���������Դ���ݽ��м��ܣ��õ�MACֵ
      /// @param[in ] :randomNumber �����
      /// @param[in ] :key ��Կ
      /// @param[in ] :keyLen ��Կ����
      /// @param[in ] :data ���ڼ���MAC��Դ����
      /// @param[in ] :dataLen Դ���ݵĳ���
      /// @param[out] :mac
      void CalculateMac( uint8 *randomNumber, uint8 *key, int keyLen, const uint8 *data, int dataLen, uint8 *mac );

      /// @brief У��PSAM��������Ϣ
      ///
      /// @param[in ] :psamCard PSAM����Ϣ
      /// @return     :0 ��ʾ�ɹ�
      int CheckPsamCardInfo( PsamCard* psamCard );

      /// @brief ����������м���
      ///
      /// TODO(youyiming) test
      /// @param[in ] :phyid ������ID
      /// @param[in ] :keyIndex ��Կ������
      /// @param[in ] :hexRandom 16����������ַ���
      /// @param[out] :encryptedRandom ���ܺ�������
      /// @return     :0 ��ʾ�ɹ�
      int EncryptRandom( char* phyid, int keyIndex, const char *hexRandom, char *encryptedRandom );

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
      int EncryptData( char *appSerialNo, int keyVersion, uint8 *random, uint8 *data, int dataLen, uint8 *encryptedData, int& encryptedDataLen );

      char* GetErrorMsgByErrorCode( int errorCode, char *errorMsg );

      ApduPsamCard apdu;      //apdu����

      std::string currentDF;  //��ǰѡ��Ŀ¼���ļ���ʶ��

      bool hasReset;          //PSAM���Ƿ����ϵ縴λ

    };

  } // namespace Card
} // namespace Engine

extern "C"
{
  /// @brief ����Psam��������
  ///
  ///ϵͳ��ͨ����̬����dll�ķ�ʽ���ø÷�����������CPsamCardService�Կ�Ƭ���в���
  /// @return     :CCommonPsamCardService����
  PSAMCARD_API Engine::Card::CPsamCardService* CreatePsamCardService();
}

#endif // COMMON_PSAM_CARD_H_
