/*
  CPU用户卡通用操作

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
      std::string GET_CHALLENGE;                //取随机数
      std::string EXTERNAL_AUTHENTICATE;        //外部认证
      std::string ERASE_DF;                     //擦除目录文件
      std::string SELECT;                       //选择文件
      std::string SELECT_MF;                    //选择MF目录
      std::string CREATE_FILE;                  //建立文件
      std::string WRITE_KEY;                    //增加或修改密钥
      std::string UPDATE_BINARY_MAC;            //带MAC写二进制文件
      std::string VERIFY_PIN;                   //验证口令
      std::string INITIALIZE_FOR_LOAD;          //圈存初始化
      std::string CREDIT_FOR_LOAD;              //圈存
      std::string INITIALIZE_FOR_PURCHASE;      //消费初始化
      std::string DEBIT_FOR_CAPP_PURCHASE;      //消费
      std::string GET_BALANCE;                  //读余额
      std::string READ_RECORD;                  //读记录文件
      std::string READ_BINARY;                  //读二进制文件
      std::string READ_BINARY_MAC;              //带MAC读二进制文件

    } ApduUserCard;

    class CCPUUserCardService: public CUserCardService
    {
    public:
      CCPUUserCardService();

      //初始化卡结构
      int InitCardDefintion();

      /// @brief 寻卡成功的后续操作
      ///
      /// 该方法在寻卡成功之后自动调用，主要用于负责初始化本次交互的环境
      /// 此处用于CPU卡上电复位
      /// @return     :0 表示成功
      int SuccessRequestCard();

	  int IsLoadKeys ();

      /// @brief 读卡
      ///
      /// @return     :0 表示成功
      int ReadCard();

      /// @brief 写卡
      ///
      /// @return     :0 表示成功
      int WriteCard();

      /// @brief 用户卡初始化
      ///
      /// 初始化用户卡中各扇区中数据
      /// @param[out] :showCardNo 显示卡号
      /// @return     :0 表示成功
      int InitUserCard( const char *showCardNo );

	  /// @brief 
      ///
      /// 更新卡秘钥
      /// @return     :0 表示成功
      int UpdateCardKeys();

      /// @brief 回收用户卡
      ///
      /// 将用户卡恢复到出厂状态
      /// @return     :0 表示成功
      int RecycleUserCard();

      /// @brief 设置写字段值
      ///
      /// @param[in ] :fieldId 字段索引
      /// @param[in ] :fieldValue 字段值
      /// @return     :0 表示成功
      int SetFieldValue( int fieldId, const char *fieldValue );

      /// @brief 获取字段值
      ///
      /// 读卡之后调用该方法获得已设置为读模式的字段的值
      /// @param[in ] :fieldId 字段索引
      /// @param[out] :fieldValue 字段值
      /// @return     :0 表示成功
      int GetFieldValue( int fieldId, char *fieldValue );

	  int CheckCardType( int cardType );

    protected:
      /// @brief CPU卡上电复位
      ///
      /// @param[out] :len 执行上电复位命令返回数据长度
      /// @param[out] :buffer 执行上电复位命令返回数据
      /// @return     :0 表示成功
      int ResetCpuCard( unsigned char *len, unsigned char *buffer );

      /// @brief CPU卡上电复位
      ///
      /// @return     :0 表示成功
      int ResetCpuCard();

      /// @brief 获取apdu命令报文数据
      ///
      /// @param[in ] :format apdu命令报文格式
      /// @return     :根据format格式组装之后的apdu命令报文
      std::string GetCommand( std::string format, ... );

      /// @brief 执行apdu命令
      ///
      /// 执行完毕之后，可通过cmd结构体中的receiveBuffer和retCode得到返回数据和返回代码
      /// @param[in ] :cmd apdu命令结构体
      /// @return     :0 表示成功
      int ExecuteCpuCmd( Engine::Device::CpuCmd *cmd );

      /// @brief 执行apdu命令
      ///
      /// 执行完毕之后，可通过cmd结构体中的receiveBuffer和retCode得到返回数据和返回代码
      /// @param[in ] :command apdu命令报文
      /// @param[in ] :cmd apdu命令结构体
      /// @return     :0 表示成功
      int ExecuteCpuCmd( std::string command, Engine::Device::CpuCmd *cmd );

      /// @brief 执行hex格式apdu命令
      ///
      /// 执行完毕之后，可通过cmd结构体中的receiveBuffer和retCode得到返回数据和返回代码
      /// @param[in ] :command apdu命令报文
      /// @param[in ] :cmd apdu命令结构体
      /// @return     :0 表示成功
      int ExecuteHexCpuCmd( std::string command, Engine::Device::CpuCmd *cmd );

      /// @brief 清空apdu命令
      ///
      /// @param[in ] :cmd apdu命令结构体
      void ClearCpuCmd( Engine::Device::CpuCmd *cmd );

      /// @brief 外部认证
      ///
      /// 1.生成随机数，用外部认证密钥externalKey进行加密
      /// 2.将加密后的随机数和外部认证密钥索引号发送给CPU卡，执行外部认证
      /// @param[in ] :externalKey 外部密钥
      /// @param[in ] :keyIndex 密钥索引号
      /// @return     :0 表示成功
      int ExternalAuthenticate( const uint8 externalKey[16], int keyIndex = 0 );

      /// @brief 外部认证
      ///
      /// 1.生成随机数
      /// 2.PSAM卡采用keyIndex对应的密钥和用户卡对应的物理ID对随机数进行加密
      /// 3.将加密后的随机数和外部认证密钥索引号发送给CPU卡，执行外部认证
      /// @param[in ] :keyIndex 密钥索引号
      /// @return     :0 表示成功
      int ExternalAuthenticate( int keyIndex );

      /// @brief 获取随机数
      ///
      /// GET CHALLENGE命令请求一个用于线路保护过程的随机数
      /// 除非掉电，选择了其他应用或者又发出一个GET CHALLENGE命令，该随机数仅在下一条指令时有效
      /// 若下一条指令为外部认证，则外部认证数据用指定的外部认证密钥解密后与该随机数进行比较
      ///
      /// @param[out] :randomNumber 04/08长度的随机数
      /// @return     :0 表示成功
      int GetChallenge( uint8 *randomNumber );

      /// @brief 擦除目录文件
      ///
      /// 在满足目录擦除条件的情况下，擦除当前目录DF下的所有文件，不包括目录本身
      /// @return     :0 表示成功
      int EraseDF();

      /// @brief 计算用户卡密钥
      ///
      /// 根据原密钥以及用户卡物理ID进行des运算，生成新的密钥
      /// 该功能用于生成用户卡中的密钥组，即根据密钥卡中的密钥组和当前用户卡物理ID，为当前用户卡生成一组新的密钥，写入用户卡中
      /// @param[in ] :phyidStr 用户卡物理ID
      /// @param[in ] :srcKey 原密钥
      /// @param[out] :destKey 换算之后生成的密钥
      /// @return     :0 表示成功
      uint8* PbocDiverKey( uint8 phyidStr[9], uint8 srcKey[17], uint8 destKey[17] );

      /// @brief 根据文件标识选择目录
      ///
      /// @param[in ] :FI 文件标识
      /// @return     :0 表示成功
      int SelectADFByFI( std::string FI );

      /// @brief 校验口令密钥
      ///
      /// @param[in ] :pin 口令密钥
      /// @return     :0 表示成功
      int VerifyPin( const char* pin );

      /// @brief 读取指定长度的Record文件
      ///
      /// @param[in ] :SFI 短文件标识
      /// @param[in ] :recordNo 记录号
      /// @param[in ] :len 要读取的长度
      /// @param[out] :recordBuffer 读出的文件内容
      /// @return     :0 表示成功
      int ReadRecordFile( int SFI, int recordNo, int len, uint8 *recordBuffer );

      /// @brief 读取指定长度的二进制文件
      ///
      /// @param[in ] :SFI 短文件标识
      /// @param[in ] :offset 偏移量
      /// @param[in ] :len 要读取的长度
      /// @param[out] :binaryBuffer 读出的文件内容
      /// @return     :0 表示成功
      int ReadBinaryFile( int SFI, int offset, int len, uint8 *binaryBuffer );

      /// @brief 带MAC读取指定长度的二进制文件
      ///
      /// @param[in ] :SFI 短文件标识
      /// @param[in ] :offset 偏移量
      /// @param[in ] :len 要读取的长度
      /// @param[out] :binaryBuffer 读出的文件内容
      /// @return     :0 表示成功
      int ReadBinaryFileWithMac( int SFI, int offset, int len, uint8 *binaryBuffer );

      /// @brief 带MAC写指定长度的二进制文件
      ///
      /// @param[in ] :SFI 短文件标识
      /// @param[in ] :offset 偏移量
      /// @param[in ] :len 要写入的长度
      /// @param[in ] :binaryBuffer 要写入的文件内容
      /// @return     :0 表示成功
      int UpdateBinaryFileWithMac( int SFI, int offset, int len, uint8 *binaryBuffer );

      /// @brief 获取充值次数
      ///
      /// 获取充值次数和余额
      /// @param[in ] :dpsCntLen 充值次数字段长度
      /// @param[in ] :cardBalanceLen 卡余额字段长度
      /// @param[out] :dpsCnt 充值次数
      /// @param[out] :cardBalance 卡余额
      /// @return     :0 表示成功
      int GetDpsCnt( int dpsCntLen, int cardBalanceLen, uint8* dpsCnt, uint8* cardBalance );

      /// @brief 获取消费次数
      ///
      /// 获取消费次数和余额
      /// @param[in ] :payCntLen 消费次数字段长度
      /// @param[in ] :cardBalanceLen 卡余额字段长度
      /// @param[out] :payCnt 消费次数
      /// @param[out] :cardBalance 卡余额
      /// @return     :0 表示成功
      int GetPayCnt( int payCntLen, int cardBalanceLen, uint8* payCnt, uint8* cardBalance );

      /// @brief 获取卡余额
      ///
      /// @param[in ] :cardBalanceLen 卡余额字段长度
      /// @param[out] :cardBalance 卡余额
      /// @return     :0 表示成功
      int GetBalance( int cardBalanceLen, uint8* cardBalance );

      /// @brief 通过PSAM卡计算MAC校验码
      ///
      /// @param[in ] :command 要计算MAC的命令
      /// @param[out] :hexMac 16进制MAC校验码
      /// @return     :0 表示成功
      int CalculateSafeMacWithSam( const char* command, char *hexMac );

      /// @brief 根据apdu执行返回代码，获取错误信息
      ///
      /// 根据iso7816-4和FMCOS2.0，apdu报文在执行成功之后，会返回状态值sw1和sw2
      /// 应用程序根据sw1和sw2的值判断apdu报文执行状态
      /// 该方法返回sw1和sw2对应的错误信息
      ///
      /// @param[in ] :errorCode 错误代码，由sw1和sw2组成
      /// @param[out] :errorMsg 错误信息
      /// @return     :错误信息
      char* GetErrorMsgByErrorCode( int errorCode, char *errorMsg );

	  void DESEncrytCardKey(uint8* srcData, uint8* key, uint8* destData);

	  /// @brief 
      ///
      /// 更新卡秘钥
      /// @return     :0 表示成功
      int UpdateCardKey(const char* preCommand,const char* data,uint8* workKey, uint8* newKey);

      static char hexCCK[];

      int readFileLen;

      int writeFileLen;

      std::string defaultPin;

      ApduUserCard apdu;      //apdu报文

      std::string currentDF;  //当前选择目录，文件标识符

      std::string mainAppDF;  //一卡通应用对应的目录，用户信息和钱包信息，交易信息均存在该应用下

	  CPUCardKeys defaultCardKeys;

    };
  }
}
extern "C"
{
  /// @brief 创建用户卡管理类
  ///
  ///系统会通过动态加载dll的方式调用该方法，创建出CUserCardService对卡片进行操作
  /// @return     :CCPUUserCardService对象
  CPUUSERCARD_API Engine::Card::CUserCardService* CreateUserCardService();
}


#endif // CPU_USER_CARD_H_