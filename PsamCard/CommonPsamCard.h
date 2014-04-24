/*
  毅能达Psam卡实现

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
      std::string SELECT;                 //选择文件
      std::string SELECT_MF;              //选择MF目录
      std::string READ_BINARY;            //读二进制文件
      std::string GET_CHALLENGE;          //取随机数
      std::string ERASE_MF;               //擦除文件系统
      std::string END_ISSUE;              //结束个人化
      std::string END_ISSUE_MAC;          //带MAC结束个人化
      std::string	CREATE_MF;              //创建主文件
      std::string CREATE_FILE;            //创建文件
      std::string UPDATE_BINARY;          //更新二进制文件
      std::string DELIVERY_KEY;           //分散密钥
      std::string CIPHER_DATA;            //加密数据
      std::string INIT_SAM_FOR_PURCHASE;  //消费交易初始化

    } ApduPsamCard;

    enum {MAX_KEY_INDEX = 2};

    class CCommonPsamCardService: public CPsamCardService
    {
    public:
      CCommonPsamCardService();

      /// @brief 读取PSAM卡终端编号
      ///
      /// @param[out] :termNo psam卡终端编号
      /// @return     :0 表示成功
      int ReadPsamTermNo( char *termNo );

      /// @brief 回收PSAM卡
      ///
      /// @return     :0 表示成功
      int RecyclePsamCard();

      /// @brief 发行Psam卡
      ///
      /// @param[in ] :psamCard Psam卡信息结构体
      /// @return     :0 表示成功
      int InitPsamCard( PsamCard* psamCard );

      /// @brief 通过PSAM卡计算MAC
      ///
      /// 采用卡物理ID做分散因子，对随机数和macCommand进行MAC计算
      /// @param[in ] :phyid 卡物理ID
      /// @param[in ] :hexRandom 随机数
      /// @param[in ] :macCommand 需要计算MAC的命令
      /// @param[out] :mac
      /// @return     :0 表示成功
      int CalculateSafeMac( char* phyid, char* hexRandom, const char *macCommand, char *mac );

      /// @brief 通过PSAM卡计算MAC
      ///
      /// @param[in ] :appSerialNo 卡应用序列号
      /// @param[in ] :keyVersion 密钥版本
      /// @param[in ] :hexRandom 随机数
      /// @param[in ] :macCommand 需要计算MAC的命令
      /// @param[out] :mac
      /// @return     :0 表示成功
      int CalculateSafeMac( char *appSerialNo, int keyVersion, char* hexRandom, const char *macCommand, char *mac );

      /// @brief 读取PSAM卡信息
      ///
      /// @param[out] :psamCard psam卡信息
      /// @return     :0 表示成功
      int ReadPsamCard( PsamCard* psamCard );

      /// @brief 读取主密钥
      ///
      /// @param[out] :mainKey 主密钥
      /// @return     :0 表示成功
      int ReadMainKey( uint8 *mainKey );

      /// @brief PSAM卡上电复位成功之后调用方法清除数据
      ///
      /// @return     :0 表示成功
      int SuccessResetPsam();

	   /// @brief 设置卡片根密钥
      ///
      /// @param[in ] :mainKey 卡片根密钥
      /// @return     :0 表示成功
      int SetMainKey( char mainKey[33] );

	  void GenerateWorkKey(CPUCardKeys *cardKeys);

    protected:

      /// @brief 根据文件标识选择目录
      ///
      /// @param[in ] :FI 文件标识
      /// @return     :0 表示成功
      int SelectADFByFI( std::string FI );

      /// @brief 获取随机数
      ///
      /// GET CHALLENGE命令请求一个用于线路保护过程的随机数
      /// 除非掉电，选择了其他应用或者又发出一个GET CHALLENGE命令，该随机数仅在下一条指令时有效
      /// 若下一条指令为外部认证，则外部认证数据用指定的外部认证密钥解密后与该随机数进行比较
      ///
      /// @param[out] :randomNumber 04/08长度的随机数
      /// @return     :0 表示成功
      int GetChallenge( uint8 *randomNumber );

      /// @brief 读取指定长度的二进制文件
      ///
      /// @param[in ] :SFI 短文件标识
      /// @param[in ] :offset 偏移量
      /// @param[in ] :len 要读取的长度
      /// @param[out] :binaryBuffer 读出的文件内容
      /// @return     :0 表示成功
      int ReadBinaryFile( int SFI, int offset, int len, uint8 *binaryBuffer );

      /// @brief 带MAC方式结束个人化
      ///
      /// 结束当前应用的个人化，进入应用状态
      /// 一个应用的安全访问条件只有在结束个人化后方能启用
      /// 如果不结束个人化，则该应用下无需认证即可获得所有权限
      /// @return     :0 表示成功
      int EndIssueWithMac();

      /// @brief 结束个人化
      ///
      /// 结束当前应用的个人化，进入应用状态
      /// 一个应用的安全访问条件只有在结束个人化后方能启用
      /// 如果不结束个人化，则该应用下无需认证即可获得所有权限
      /// @return     :0 表示成功
      int EndIssue();

      /// @brief 主机计算MAC
      ///
      /// 通过入参KEY对随机数和源数据进行加密，得到MAC值
      /// @param[in ] :randomNumber 随机数
      /// @param[in ] :key 密钥
      /// @param[in ] :keyLen 密钥长度
      /// @param[in ] :data 用于计算MAC的源数据
      /// @param[in ] :dataLen 源数据的长度
      /// @param[out] :mac
      void CalculateMac( uint8 *randomNumber, uint8 *key, int keyLen, const uint8 *data, int dataLen, uint8 *mac );

      /// @brief 校验PSAM卡发行信息
      ///
      /// @param[in ] :psamCard PSAM卡信息
      /// @return     :0 表示成功
      int CheckPsamCardInfo( PsamCard* psamCard );

      /// @brief 对随机数进行加密
      ///
      /// TODO(youyiming) test
      /// @param[in ] :phyid 卡物理ID
      /// @param[in ] :keyIndex 密钥索引号
      /// @param[in ] :hexRandom 16进制随机数字符串
      /// @param[out] :encryptedRandom 加密后的随机数
      /// @return     :0 表示成功
      int EncryptRandom( char* phyid, int keyIndex, const char *hexRandom, char *encryptedRandom );

      /// @brief 通过PSAM卡加密数据
      ///
      /// @param[in ] :appSerialNo 卡应用序列号
      /// @param[in ] :keyVersion 密钥版本
      /// @param[in ] :random 随机数
      /// @param[in ] :data 待加密的数据
      /// @param[in ] :dataLen 待加密数据的长度
      /// @param[out] :encryptedData 加密之后的数据
      /// @param[out] :encryptedDataLen 加密之后的数据长度
      /// @return     :0 表示成功
      int EncryptData( char *appSerialNo, int keyVersion, uint8 *random, uint8 *data, int dataLen, uint8 *encryptedData, int& encryptedDataLen );

      char* GetErrorMsgByErrorCode( int errorCode, char *errorMsg );

      ApduPsamCard apdu;      //apdu报文

      std::string currentDF;  //当前选择目录，文件标识符

      bool hasReset;          //PSAM卡是否已上电复位

    };

  } // namespace Card
} // namespace Engine

extern "C"
{
  /// @brief 创建Psam卡管理类
  ///
  ///系统会通过动态加载dll的方式调用该方法，创建出CPsamCardService对卡片进行操作
  /// @return     :CCommonPsamCardService对象
  PSAMCARD_API Engine::Card::CPsamCardService* CreatePsamCardService();
}

#endif // COMMON_PSAM_CARD_H_
