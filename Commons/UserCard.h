/*
  �û����ӿ�

  Author:       yiming.you
  Version:      1.0
*/

#ifndef USER_CARD_H_
#define USER_CARD_H_

#ifdef COMMONS_EXPORTS
#define COMMONS_API __declspec(dllexport)
#else
#define COMMONS_API __declspec(dllimport)
#endif

#include "Card.h"
#include "Device.h"
#include "PsamCard.h"
#include <set>
#include <map>
#include <vector>

#define ZERO_BLOCK(m) do{ (m)=0; } while(0)
#define SET_BLOCK(m,b) do { (m) |= (1 << (b)); } while(0)
#define CLR_BLOCK(m,b) do { (m) &= (^(1 << (b))); } while(0)
#define ISSET_BLOCK(m,b) (((m) & (1<<(b))) != 0)

#define EASY_SET_BLOCK(m,of) do {int _b=(of)/16;ZERO_BLOCK(m); SET_BLOCK(m,_b); } while(0)

//�ж��Ƿ���װ����Կ
#define IS_LOAD_KEY() do { int ret = IsLoadKeys(); if(ret) return ret; }while(0)

namespace Engine
{
  namespace Card
  {
    //�ļ���Ȩ��
    //FRR_NULL  ����ʹ��read��ʽ��
    //FRR_FREE  ���ɶ�
    //FRR_MAC   ����ʹ��MAC��
    //FRR_EXTAUTH_MAC ������ʹ���ⲿ��֤����ʹ��MAC��
    //�ļ�дȨ��
    //FRW_NULL  ����ʹ��write��ʽд
    //FRW_FREE  ����д
    //FRW_MAC   ����ʹ��MACд
    //FRW_EXTAUTH_MAC ������ʹ���ⲿ��֤����ʹ��MAC��
    //FRW_MAC_DIRECT  д��ʱ����Ҫ��ȡԭ����
    typedef enum {RW_RW, RW_R, RW_W} RW_RIGHT;

    typedef enum {FT_INT8 = 1, FT_INT16, FT_INT24, FT_INT32, FT_INT48, FT_BCD, FT_BCD_LPAD20, FT_BCD_LAPDZERO, FT_ANSI, FT_UCS2,
                  // ��λ��ǰ������
                  FT_LT_INT16, FT_LT_INT24, FT_LT_INT32, FT_HEX_DATE, FT_HEX_TIME, FT_HEX_STR, FT_COMPACT_DATE, FT_STUEMPNO, FT_SUBSIDYBAL, FT_TOTALAMT
                 } FIELD_TYPE;

    typedef enum {FRR_NULL = 0, FRR_FREE, FRR_MAC, FRR_EXTAUTH_MAC, FRR_ENCRYPT, FRR_ENCRYPT_MAC,
                  FRR_REC_FREE, FRR_REC_MAC, FRR_EXTAUTH
                 } FILE_RIGHT_READ;

    typedef enum {FRW_NULL = 0, FRW_MAC, FRW_EXTAUTH_MAC, FRW_MAC_DIRECT, FRW_ENCRYPT, FRW_ENCRYPT_MAC} FILE_RIGHT_WRITE;

    typedef enum {RCT_CPU, RCT_MF1, RCT_ALL} READCARD_TYPE;

    typedef struct tagEfField
    {
      //���ṹ���Զ���
      size_t offset;
      size_t length;
      int type;
      int unused;
      int rw;   //��дȨ��
      //CPU ���ṹʹ��
      int adfid;
      //CPU �б�ʾ �ļ����� , mifare ���б�ʾ������
      int fid;
      //int sectno;
      //mifare card ���ṹʹ��
      int blockno; // 0 0 0 0 1 1 1 1
    } FieldInfo;

    typedef struct tagFieldBuffer
    {
      std::set<int>	FieldSet;
      std::set<int>	ReadFieldSet;
      std::set<int> WriteFieldSet;
      int FileSize;
      int ReadRight;
      int WriteRight;
      uint8 ReadBuf[128];
      uint8 WriteBuf[128];
    } FieldBuffer;

    typedef struct tagUserInfo
    {
      uint8 cardNo[21];		          //����
    } UserInfo;

	typedef struct tagCardInfo
    {
      uint8 secondTrack[60];	      //���ŵ���Ϣ
    } CardInfo;


    class COMMONS_API CUserCardService
    {
    public:

      /// @brief ��Ѱ���ɹ����Զ����ø÷������÷�����������ʵ��
      ///
      /// @return     :0 ��ʾ�ɹ�
      virtual int SuccessRequestCard() = 0;

      /// @brief ����д�ֶ�ֵ
      ///
      /// @param[in ] :fieldId �ֶ�����
      /// @param[in ] :fieldValue �ֶ�ֵ
      /// @return     :0 ��ʾ�ɹ�
      virtual int SetFieldValue( int fieldId, const char *fieldValue ) = 0;

      /// @brief ��ȡ�ֶ�ֵ
      ///
      /// ����֮����ø÷������������Ϊ��ģʽ���ֶε�ֵ
      /// @param[in ] :fieldId �ֶ�����
      /// @param[out] :fieldValue �ֶ�ֵ
      /// @return     :0 ��ʾ�ɹ�
      virtual int GetFieldValue( int fieldId, char *fieldValue ) = 0;

      /// @brief ����
      /// ��ȡ���е���Ϣ
      /// @return     :0 ��ʾ�ɹ�
      virtual int ReadCard() = 0;

      /// @brief д��
      /// ����Ϣд�뿨��
      /// @return     :0 ��ʾ�ɹ�
      virtual int WriteCard() = 0;

      /// @brief �û�����ʼ��
      ///
      /// ��ʼ���û���������
      /// @param[in ] :showCardNo ��ʾ����
      /// @return     :0 ��ʾ�ɹ�
      virtual int InitUserCard( const char *showCardNo ) = 0;

	  /// @brief 
      ///
      /// ��ʼ���û���������
      /// @return     :0 ��ʾ�ɹ�
      virtual int UpdateCardKeys() = 0;

      /// @brief �����û���
      ///
      /// ���û����ָ�������״̬
      /// @return     :0 ��ʾ�ɹ�
      virtual int RecycleUserCard() = 0;

      /// @brief ��ֵ
      ///
      /// @param[in ] :transPack ������Ϣ
      /// @return     :0 ��ʾ�ɹ�
      //virtual int Deposit( TransPack *transPack ) = 0;

      /// @brief ֧��
      ///
      /// @param[in ] :transPack ������Ϣ
      /// @return     :0 ��ʾ�ɹ�
      //virtual int Pay( TransPack *transPack ) = 0;

      /// @brief �Ƿ��Ѷ�ȡ��Կ
      ///
      /// @return     :0 ��ʾ��Կ�Ѷ�
      virtual int IsLoadKeys () = 0;

      /// @brief ��鿨����
      ///
      /// ����Ƿ�֧��cardType��Ӧ�Ŀ�������ʵ��
      /// @param[in ] : cardType ������
      /// @return     :0 ��ʾ�ɹ�
      virtual int CheckCardType( int cardType ) = 0;

      /// @brief Ѱ��
      ///
      /// ������ֵ�ο�typedef enum {MFCARD = 1, CPUCARD, FIXCARD, PSAM, KEYCARD, SIMCARD} CARD_TYPE
      /// @param[out] :phyid ������ID
      /// @param[out] :cardType ������
      /// @return     :0 ��ʾ�ɹ�
      int RequestCard( char *phyid, int& cardType );

      /// @brief Ѱ��
      ///
      /// @param[out] :phyid ������ID
      /// @return     :0 ��ʾ�ɹ�
      int RequestCard( char *phyid );

      /// @brief �����ֶζ�ģʽ
      ///
      /// @param[in ] :fieldId �ֶ�����
      /// @return     :0 ��ʾ�ɹ�
      int SetFieldReadMode( int fieldId );

      /// @brief �����ֶ�дģʽ
      ///
      /// ����SetFieldValue����ʱ�����Զ����ø÷���
      /// @param[in ] :fieldId �ֶ�����
      /// @return     :0 ��ʾ�ɹ�
      int SetFieldWriteMode( int fieldId );

      /// @brief �����ֶζ�дģʽ
      ///
      /// @param[in ] :fieldId �ֶ�����
      /// @return     :0 ��ʾ�ɹ�
      int SetFieldAllMode( int fieldId );

      /// @brief ����ֶζ�ģʽ
      ///
      /// @param[in ] :fieldId �ֶ�����
      /// @return     :0 ��ʾ�ɹ�
      int ClearFieldReadMode( int fieldId );

      /// @brief ����ֶ�дģʽ
      ///
      /// @param[in ] :fieldId �ֶ�����
      /// @return     :0 ��ʾ�ɹ�
      int ClearFieldWriteMode( int fieldId );

      /// @brief ����ֶ�����ģʽ
      ///
      /// @param[in ] :fieldId �ֶ�����
      /// @return     :0 ��ʾ�ɹ�
      int ClearFieldAllMode( int fieldId );

      /// @brief ��������ֶεĶ�дģʽ
      ///
      /// @return     :0 ��ʾ�ɹ�
      int ClearAllFieldAllMode();

      /// @brief ��������ֶεĶ�ģʽ
      ///
      /// @return     :0 ��ʾ�ɹ�
      int ClearAllFieldReadMode();

      /// @brief ��������ֶε�дģʽ
      ///
      /// @return     :0 ��ʾ�ɹ�
      int ClearAllFieldWriteMode();

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

	  /// @brief ����PSAM��������
      ///
      /// @param[in ] : psamCardService PSAM��������
      void SetPsamCardervice( Engine::Card::CPsamCardService *psamCardService );

    protected:
      /// @brief ����ֶ��Ƿ������ö�ģʽ
      ///
      /// @param[in ] :fieldId �ֶ�����
      /// @return     :0 ��ʾ�ֶ������ö�ģʽ
      int CheckFieldReadMode( int fieldId );

      /// @brief ����ֶ��Ƿ�������дģʽ
      ///
      /// @param[in ] :fieldId �ֶ�����
      /// @return     :0 ��ʾ�ֶ�������дģʽ
      int CheckFieldWriteMode( int fieldId );

      /// @brief ����ֶ��Ƿ��Ѷ�ȡ
      ///
      /// @param[in ] :fieldId �ֶ�����
      /// @return     :0 ��ʾ�ֶ��Ѷ�ȡ
      int FieldHasRead( int fieldId );

      /// @brief �����ֶζ�Ӧ�����ͣ����ֶ���ʾֵת��Ϊ���д�ŵ��ֽ�����
      ///
      /// @param[in ] :fieldValue �ֶ�ԭʼֵ
      /// @param[in ] :fieldType �ֶ�����
      /// @param[in ] :len �ֶ��ڿ�����ռ����
      /// @param[out] :fieldBuffer ת��֮����ֶ�ֵ
      /// @return     :0 ��ʾ�ɹ�
      int SetFieldValue( const char *fieldValue, int fieldType, int len, uint8 *fieldBuffer );

      /// @brief ���ֶ�ֵת����16���Ƶ�����
      ///
      /// @param[in ] :fieldValue �ֶ�ԭʼֵ
      /// @param[in ] :len �ֶ��ڿ�����ռ����
      /// @param[out] :fieldBuffer ת��֮����ֶ�ֵ
      /// @return     :0 ��ʾ�ɹ�
      int SetHexDate( const char* fieldValue, int len, uint8 *fieldBuffer );

      /// @brief ���ֶ�ֵת����16���Ƶ�ʱ��
      ///
      /// @param[in ] :fieldValue �ֶ�ԭʼֵ
      /// @param[in ] :len �ֶ��ڿ�����ռ����
      /// @param[out] :fieldBuffer ת��֮����ֶ�ֵ
      /// @return     :0 ��ʾ�ɹ�
      int SetHexTime( const char* fieldValue, int len, uint8 *fieldBuffer );

      /// @brief �����ֶζ�Ӧ�����ͣ����������ֽ�����ת������Ӧ���͵�ֵ
      ///
      /// @param[in ] :fieldBuffer �ֶ��ڿ��еĴ洢ֵ
      /// @param[in ] :fieldType �ֶ�����
      /// @param[in ] :len �ֶ��ڿ�����ռ����
      /// @param[out] :fieldValue ת��֮����ֶ�ֵ
      /// @return     :0 ��ʾ�ɹ�
      int GetFieldValue( uint8 *fieldBuffer, int fieldType, int len, char *fieldValue );

      /// @brief ��ȡϵͳ��ǰʱ��,�� 20120427145129
      ///
      /// @param[out] :datetime ��ǰʱ��
      void GetCurrentDateTime( char *datetime );

      /// @brief ��ȡ��λ���ȵ�16���Ƶ�ǰʱ�䣬�꣬�£��գ�ʱ���֣�������ʡȥǰ��λ
      ///
      /// @param[out] :datetime 16���Ƶ�ǰʱ��
      void GetCurrentHexDateTime( uint8 datetime[5] );

      /// @brief ��ʮ����������ת�����ַ���
      ///
      /// @param[in ] :fieldBuffer ����ԭʼֵ
      /// @param[in ] :len ����
      /// @param[out] :fieldValue �����ַ�������20120312
      /// @return     :0 ��ʾ�ɹ�
      int GetDateFromHex( uint8 *fieldBuffer, int len, char *fieldValue );

      /// @brief ��ʮ������ʱ��ת�����ַ���
      ///
      /// @param[in ] :fieldBuffer ʱ��ԭʼֵ
      /// @param[in ] :len ����
      /// @param[out] :fieldValue ʱ���ַ�������163912
      /// @return     :0 ��ʾ�ɹ�
      int GetTimeFromHex( uint8 *fieldBuffer, int len, char *fieldValue );

      /// @brief ���ɿ��������к�
      ///
      /// ���ݿ�����ID�任�������ֽ�phyno
      /// @param[in ] :phyid ������ID
      /// @return     :0 ��ʾ�ɹ�
      int GeneratePhyno( char *phyid );

      /// @brief ��ȡ����Կ
      ///
      /// @return     :uint8* ����Կ
      uint8* GetMainKey();

      char lastErrorMsg[ERROR_MSG_LEN];  //��������Ϣ

      std::vector<FieldInfo> fieldList;

      std::map<int, FieldBuffer> fieldMap;

      Engine::Device::CDeviceService *deviceService;  //�豸������� һ��ָ������

	  Engine::Card::CPsamCardService *psamCardService;  //PSAM��������

      char phyid[17];     //������ID

      uint8 phyno[4];     //���������к�

      int keyVersion;

    };

  } // namespace Card
} // namespace Engine


#endif // USER_CARD_H_
