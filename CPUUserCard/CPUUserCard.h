/*
  CPU�û���ͨ�ò���

  Author:       yiming.you
	Version:      1.0
	Create Date:  2012-05-23
	Review Date:
*/

#ifndef CPU_USER_CARD_H_
#define CPU_USER_CARD_H_

#ifdef CPUUSERCARD_EXPORTS
#define CPUUSERCARD_API __declspec(dllexport)
#else
#define CPUUSERCARD_API __declspec(dllimport)
#endif

#include "../Commons/UserCard.h"
#include "../Commons/Global.h"

namespace Engine
{
  namespace Card
  {
    typedef struct tagApduUserCard
    {
      std::string GET_CHALLENGE;                //ȡ�����
      std::string EXTERNAL_AUTHENTICATE;        //�ⲿ��֤
      std::string ERASE_DF;                     //����Ŀ¼�ļ�
      std::string SELECT;                       //ѡ���ļ�
      std::string SELECT_MF;                    //ѡ��MFĿ¼
      std::string CREATE_FILE;                  //�����ļ�
      std::string WRITE_KEY;                    //���ӻ��޸���Կ
      std::string UPDATE_BINARY_MAC;            //��MACд�������ļ�
      std::string VERIFY_PIN;                   //��֤����
      std::string INITIALIZE_FOR_LOAD;          //Ȧ���ʼ��
      std::string CREDIT_FOR_LOAD;              //Ȧ��
      std::string INITIALIZE_FOR_PURCHASE;      //���ѳ�ʼ��
      std::string DEBIT_FOR_CAPP_PURCHASE;      //����
      std::string GET_BALANCE;                  //�����
      std::string READ_RECORD;                  //����¼�ļ�
      std::string READ_BINARY;                  //���������ļ�
      std::string READ_BINARY_MAC;              //��MAC���������ļ�

    } ApduUserCard;

    class CCPUUserCardService: public CUserCardService
    {
    public:
      CCPUUserCardService();

      //��ʼ�����ṹ
      int InitCardDefintion();

      /// @brief Ѱ���ɹ��ĺ�������
      ///
      /// �÷�����Ѱ���ɹ�֮���Զ����ã���Ҫ���ڸ����ʼ�����ν����Ļ���
      /// �˴�����CPU���ϵ縴λ
      /// @return     :0 ��ʾ�ɹ�
      int SuccessRequestCard();

	  int IsLoadKeys ();

      /// @brief ����
      ///
      /// @return     :0 ��ʾ�ɹ�
      int ReadCard();

      /// @brief д��
      ///
      /// @return     :0 ��ʾ�ɹ�
      int WriteCard();

      /// @brief �û�����ʼ��
      ///
      /// ��ʼ���û����и�����������
      /// @param[out] :showCardNo ��ʾ����
      /// @return     :0 ��ʾ�ɹ�
      int InitUserCard( const char *showCardNo );

	  /// @brief 
      ///
      /// ���¿���Կ
      /// @return     :0 ��ʾ�ɹ�
      int UpdateCardKeys();

      /// @brief �����û���
      ///
      /// ���û����ָ�������״̬
      /// @return     :0 ��ʾ�ɹ�
      int RecycleUserCard();

      /// @brief ����д�ֶ�ֵ
      ///
      /// @param[in ] :fieldId �ֶ�����
      /// @param[in ] :fieldValue �ֶ�ֵ
      /// @return     :0 ��ʾ�ɹ�
      int SetFieldValue( int fieldId, const char *fieldValue );

      /// @brief ��ȡ�ֶ�ֵ
      ///
      /// ����֮����ø÷������������Ϊ��ģʽ���ֶε�ֵ
      /// @param[in ] :fieldId �ֶ�����
      /// @param[out] :fieldValue �ֶ�ֵ
      /// @return     :0 ��ʾ�ɹ�
      int GetFieldValue( int fieldId, char *fieldValue );

	  int CheckCardType( int cardType );

    protected:
      /// @brief CPU���ϵ縴λ
      ///
      /// @param[out] :len ִ���ϵ縴λ��������ݳ���
      /// @param[out] :buffer ִ���ϵ縴λ���������
      /// @return     :0 ��ʾ�ɹ�
      int ResetCpuCard( unsigned char *len, unsigned char *buffer );

      /// @brief CPU���ϵ縴λ
      ///
      /// @return     :0 ��ʾ�ɹ�
      int ResetCpuCard();

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

      /// @brief �ⲿ��֤
      ///
      /// 1.��������������ⲿ��֤��ԿexternalKey���м���
      /// 2.�����ܺ����������ⲿ��֤��Կ�����ŷ��͸�CPU����ִ���ⲿ��֤
      /// @param[in ] :externalKey �ⲿ��Կ
      /// @param[in ] :keyIndex ��Կ������
      /// @return     :0 ��ʾ�ɹ�
      int ExternalAuthenticate( const uint8 externalKey[16], int keyIndex = 0 );

      /// @brief �ⲿ��֤
      ///
      /// 1.���������
      /// 2.PSAM������keyIndex��Ӧ����Կ���û�����Ӧ������ID����������м���
      /// 3.�����ܺ����������ⲿ��֤��Կ�����ŷ��͸�CPU����ִ���ⲿ��֤
      /// @param[in ] :keyIndex ��Կ������
      /// @return     :0 ��ʾ�ɹ�
      int ExternalAuthenticate( int keyIndex );

      /// @brief ��ȡ�����
      ///
      /// GET CHALLENGE��������һ��������·�������̵������
      /// ���ǵ��磬ѡ��������Ӧ�û����ַ���һ��GET CHALLENGE����������������һ��ָ��ʱ��Ч
      /// ����һ��ָ��Ϊ�ⲿ��֤�����ⲿ��֤������ָ�����ⲿ��֤��Կ���ܺ������������бȽ�
      ///
      /// @param[out] :randomNumber 04/08���ȵ������
      /// @return     :0 ��ʾ�ɹ�
      int GetChallenge( uint8 *randomNumber );

      /// @brief ����Ŀ¼�ļ�
      ///
      /// ������Ŀ¼��������������£�������ǰĿ¼DF�µ������ļ���������Ŀ¼����
      /// @return     :0 ��ʾ�ɹ�
      int EraseDF();

      /// @brief �����û�����Կ
      ///
      /// ����ԭ��Կ�Լ��û�������ID����des���㣬�����µ���Կ
      /// �ù������������û����е���Կ�飬��������Կ���е���Կ��͵�ǰ�û�������ID��Ϊ��ǰ�û�������һ���µ���Կ��д���û�����
      /// @param[in ] :phyidStr �û�������ID
      /// @param[in ] :srcKey ԭ��Կ
      /// @param[out] :destKey ����֮�����ɵ���Կ
      /// @return     :0 ��ʾ�ɹ�
      uint8* PbocDiverKey( uint8 phyidStr[9], uint8 srcKey[17], uint8 destKey[17] );

      /// @brief �����ļ���ʶѡ��Ŀ¼
      ///
      /// @param[in ] :FI �ļ���ʶ
      /// @return     :0 ��ʾ�ɹ�
      int SelectADFByFI( std::string FI );

      /// @brief У�������Կ
      ///
      /// @param[in ] :pin ������Կ
      /// @return     :0 ��ʾ�ɹ�
      int VerifyPin( const char* pin );

      /// @brief ��ȡָ�����ȵ�Record�ļ�
      ///
      /// @param[in ] :SFI ���ļ���ʶ
      /// @param[in ] :recordNo ��¼��
      /// @param[in ] :len Ҫ��ȡ�ĳ���
      /// @param[out] :recordBuffer �������ļ�����
      /// @return     :0 ��ʾ�ɹ�
      int ReadRecordFile( int SFI, int recordNo, int len, uint8 *recordBuffer );

      /// @brief ��ȡָ�����ȵĶ������ļ�
      ///
      /// @param[in ] :SFI ���ļ���ʶ
      /// @param[in ] :offset ƫ����
      /// @param[in ] :len Ҫ��ȡ�ĳ���
      /// @param[out] :binaryBuffer �������ļ�����
      /// @return     :0 ��ʾ�ɹ�
      int ReadBinaryFile( int SFI, int offset, int len, uint8 *binaryBuffer );

      /// @brief ��MAC��ȡָ�����ȵĶ������ļ�
      ///
      /// @param[in ] :SFI ���ļ���ʶ
      /// @param[in ] :offset ƫ����
      /// @param[in ] :len Ҫ��ȡ�ĳ���
      /// @param[out] :binaryBuffer �������ļ�����
      /// @return     :0 ��ʾ�ɹ�
      int ReadBinaryFileWithMac( int SFI, int offset, int len, uint8 *binaryBuffer );

      /// @brief ��MACдָ�����ȵĶ������ļ�
      ///
      /// @param[in ] :SFI ���ļ���ʶ
      /// @param[in ] :offset ƫ����
      /// @param[in ] :len Ҫд��ĳ���
      /// @param[in ] :binaryBuffer Ҫд����ļ�����
      /// @return     :0 ��ʾ�ɹ�
      int UpdateBinaryFileWithMac( int SFI, int offset, int len, uint8 *binaryBuffer );

      /// @brief ��ȡ��ֵ����
      ///
      /// ��ȡ��ֵ���������
      /// @param[in ] :dpsCntLen ��ֵ�����ֶγ���
      /// @param[in ] :cardBalanceLen ������ֶγ���
      /// @param[out] :dpsCnt ��ֵ����
      /// @param[out] :cardBalance �����
      /// @return     :0 ��ʾ�ɹ�
      int GetDpsCnt( int dpsCntLen, int cardBalanceLen, uint8* dpsCnt, uint8* cardBalance );

      /// @brief ��ȡ���Ѵ���
      ///
      /// ��ȡ���Ѵ��������
      /// @param[in ] :payCntLen ���Ѵ����ֶγ���
      /// @param[in ] :cardBalanceLen ������ֶγ���
      /// @param[out] :payCnt ���Ѵ���
      /// @param[out] :cardBalance �����
      /// @return     :0 ��ʾ�ɹ�
      int GetPayCnt( int payCntLen, int cardBalanceLen, uint8* payCnt, uint8* cardBalance );

      /// @brief ��ȡ�����
      ///
      /// @param[in ] :cardBalanceLen ������ֶγ���
      /// @param[out] :cardBalance �����
      /// @return     :0 ��ʾ�ɹ�
      int GetBalance( int cardBalanceLen, uint8* cardBalance );

      /// @brief ͨ��PSAM������MACУ����
      ///
      /// @param[in ] :command Ҫ����MAC������
      /// @param[out] :hexMac 16����MACУ����
      /// @return     :0 ��ʾ�ɹ�
      int CalculateSafeMacWithSam( const char* command, char *hexMac );

      /// @brief ����apduִ�з��ش��룬��ȡ������Ϣ
      ///
      /// ����iso7816-4��FMCOS2.0��apdu������ִ�гɹ�֮�󣬻᷵��״ֵ̬sw1��sw2
      /// Ӧ�ó������sw1��sw2��ֵ�ж�apdu����ִ��״̬
      /// �÷�������sw1��sw2��Ӧ�Ĵ�����Ϣ
      ///
      /// @param[in ] :errorCode ������룬��sw1��sw2���
      /// @param[out] :errorMsg ������Ϣ
      /// @return     :������Ϣ
      char* GetErrorMsgByErrorCode( int errorCode, char *errorMsg );

	  void DESEncrytCardKey(uint8* srcData, uint8* key, uint8* destData);

	  /// @brief 
      ///
      /// ���¿���Կ
      /// @return     :0 ��ʾ�ɹ�
      int UpdateCardKey(const char* preCommand,const char* data,uint8* workKey, uint8* newKey);

      static char hexCCK[];

      int readFileLen;

      int writeFileLen;

      std::string defaultPin;

      ApduUserCard apdu;      //apdu����

      std::string currentDF;  //��ǰѡ��Ŀ¼���ļ���ʶ��

      std::string mainAppDF;  //һ��ͨӦ�ö�Ӧ��Ŀ¼���û���Ϣ��Ǯ����Ϣ��������Ϣ�����ڸ�Ӧ����

	  CPUCardKeys defaultCardKeys;

    };
  }
}
extern "C"
{
  /// @brief �����û���������
  ///
  ///ϵͳ��ͨ����̬����dll�ķ�ʽ���ø÷�����������CUserCardService�Կ�Ƭ���в���
  /// @return     :CCPUUserCardService����
  CPUUSERCARD_API Engine::Card::CUserCardService* CreateUserCardService();
}


#endif // CPU_USER_CARD_H_