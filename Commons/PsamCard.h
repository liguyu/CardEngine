/*
  Psam卡接口

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

      /// @brief 发行Psam卡
      ///
      /// @param[in ] :psamCard Psam卡信息结构体
      /// @return     :0 表示成功
      virtual int InitPsamCard( PsamCard* psamCard ) = 0;

      /// @brief 读取PSAM卡终端编号
      ///
      /// @param[out] :termNo psam卡终端编号
      /// @return     :0 表示成功
      virtual int ReadPsamTermNo( char *termNo ) = 0;

      /// @brief 回收PSAM卡
      ///
      /// @return     :0 表示成功
      virtual int RecyclePsamCard() = 0;

	  /// @brief 主机计算MAC
      ///
      /// 通过入参KEY对随机数和源数据进行加密，得到MAC值
      /// @param[in ] :randomNumber 随机数
      /// @param[in ] :key 密钥
      /// @param[in ] :keyLen 密钥长度
      /// @param[in ] :data 用于计算MAC的源数据
      /// @param[in ] :dataLen 源数据的长度
      /// @param[out] :mac
      virtual void CalculateMac( uint8 *randomNumber, uint8 *key, int keyLen, const uint8 *data, int dataLen, uint8 *mac ) = 0;

      /// @brief 通过PSAM卡计算MAC
      ///
      /// 采用卡物理ID做分散因子，对随机数和macCommand进行MAC计算
      /// @param[in ] :phyid 卡物理ID
      /// @param[in ] :hexRandom 随机数
      /// @param[in ] :macCommand 需要计算MAC的命令
      /// @param[out] :mac
      /// @return     :0 表示成功
      virtual int CalculateSafeMac( char* phyid, char* hexRandom, const char *macCommand, char *mac ) = 0;

      /// @brief 通过PSAM卡计算MAC
      ///
      /// @param[in ] :appSerialNo 卡应用序列号
      /// @param[in ] :keyVersion 密钥版本
      /// @param[in ] :hexRandom 随机数
      /// @param[in ] :macCommand 需要计算MAC的命令
      /// @param[out] :mac
      /// @return     :0 表示成功
      virtual int CalculateSafeMac( char *appSerialNo, int keyVersion, char* hexRandom, const char *macCommand, char *mac ) = 0;

      virtual int EncryptRandom( char* phyid, int keyIndex, const char *hexRandom, char *encryptedRandom ) = 0;

      /// @brief 读取主密钥
      ///
      /// @param[out] :mainKey 主密钥
      /// @return     :0 表示成功
      virtual int ReadMainKey( uint8 *mainKey ) = 0;

      /// @brief 读取PSAM卡信息
      ///
      /// @param[out] :psamCard psam卡信息
      /// @return     :0 表示成功
      virtual int ReadPsamCard( PsamCard* psamCard ) = 0;

      virtual char* GetErrorMsgByErrorCode( int errorCode, char *errorMsg ) = 0;

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
      virtual int EncryptData( char *appSerialNo, int keyVersion, uint8 *random, uint8 *data, int dataLen, uint8 *encryptedData, int& encryptedDataLen  ) = 0;

      /// @brief PSAM卡上电复位成功之后调用方法清除数据
      ///
      /// @return     :0 表示成功
      virtual int SuccessResetPsam() = 0;

      /// @brief 设置错误信息，同时记录日志
      ///
      /// 只保留最后一条错误信息，设置完之后可通过GetLastErrorMsg()方法获取
      /// @param[in ] :errorMsg 错误信息
      void SetLastErrorMsg( const char* errorMsg );

      /// @brief 获取最后一条错误信息
      ///
      /// 调用时可通过返回值或者出参errorMsg拿到错误信息，两者值一致
      /// @param[out] :errorMsg 错误信息
      /// @return     :char* 错误信息
      char* GetLastErrorMsg ( char *errorMsg );

      /// @brief 设置设备服务类
      ///
      /// @param[in ] : deviceService 设备服务类
      void SetDeviceService( Engine::Device::CDeviceService *deviceService );

      /// @brief 设置PSAM卡座
      ///
      /// @param[out] :slot 卡座号
      /// @return     :0 表示成功
      int SetPsamSlot( int slot );

	  /// @brief 设置卡片根密钥
      ///
      /// @param[in ] :mainKey 卡片根密钥
      /// @return     :0 表示成功
      virtual int SetMainKey( char mainKey[33] ) = 0;

	  virtual void GenerateWorkKey(CPUCardKeys *cardKeys) = 0;

      /// @brief CPU、PSAM卡上电复位
      ///
      /// @param[out] :len 返回复位信息的长度
      /// @param[out] :buffer 存放返回的复位信息
      /// @return     :0 表示成功
      int ResetPsam( uint8 *len, unsigned char *buffer );

      /// @brief CPU、PSAM卡上电复位
      ///
      /// 功能和ResetPsam( unsigned char *len, unsigned char *buffer )一致
      /// @param[in ] :enforce 是否强制上电复位，0为不需要强制上电复位
      /// @return     :0 表示成功
      int ResetPsam( int enforce = 0 );

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

	  /// @brief 是否已加载根密钥
      ///
      /// @return     :true 已加载根密钥
      bool HasLoadMainKey();

    protected:

      char lastErrorMsg[ERROR_MSG_LEN];                    //最后错误信息

      Engine::Device::CDeviceService *deviceService;   //设备服务对象， 一般指读卡器

      bool hasReset;          //PSAM卡是否已上电复位

	  char mainKey[33];

	  bool hasLoadMainKey;
    };

  } // namespace Card
} // namespace Engine


#endif // PSAM_CARD_H_
