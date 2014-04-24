/*
  用户卡接口

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

//判断是否已装载密钥
#define IS_LOAD_KEY() do { int ret = IsLoadKeys(); if(ret) return ret; }while(0)

namespace Engine
{
  namespace Card
  {
    //文件读权限
    //FRR_NULL  不能使用read方式读
    //FRR_FREE  自由读
    //FRR_MAC   必须使用MAC读
    //FRR_EXTAUTH_MAC 必须先使用外部认证后再使用MAC读
    //文件写权限
    //FRW_NULL  不能使用write方式写
    //FRW_FREE  自由写
    //FRW_MAC   必须使用MAC写
    //FRW_EXTAUTH_MAC 必须先使用外部认证后再使用MAC读
    //FRW_MAC_DIRECT  写入时不需要读取原内容
    typedef enum {RW_RW, RW_R, RW_W} RW_RIGHT;

    typedef enum {FT_INT8 = 1, FT_INT16, FT_INT24, FT_INT32, FT_INT48, FT_BCD, FT_BCD_LPAD20, FT_BCD_LAPDZERO, FT_ANSI, FT_UCS2,
                  // 低位在前的字数
                  FT_LT_INT16, FT_LT_INT24, FT_LT_INT32, FT_HEX_DATE, FT_HEX_TIME, FT_HEX_STR, FT_COMPACT_DATE, FT_STUEMPNO, FT_SUBSIDYBAL, FT_TOTALAMT
                 } FIELD_TYPE;

    typedef enum {FRR_NULL = 0, FRR_FREE, FRR_MAC, FRR_EXTAUTH_MAC, FRR_ENCRYPT, FRR_ENCRYPT_MAC,
                  FRR_REC_FREE, FRR_REC_MAC, FRR_EXTAUTH
                 } FILE_RIGHT_READ;

    typedef enum {FRW_NULL = 0, FRW_MAC, FRW_EXTAUTH_MAC, FRW_MAC_DIRECT, FRW_ENCRYPT, FRW_ENCRYPT_MAC} FILE_RIGHT_WRITE;

    typedef enum {RCT_CPU, RCT_MF1, RCT_ALL} READCARD_TYPE;

    typedef struct tagEfField
    {
      //卡结构属性定义
      size_t offset;
      size_t length;
      int type;
      int unused;
      int rw;   //读写权限
      //CPU 卡结构使用
      int adfid;
      //CPU 中表示 文件索引 , mifare 卡中表示扇区号
      int fid;
      //int sectno;
      //mifare card 卡结构使用
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
      uint8 cardNo[21];		          //卡号
    } UserInfo;

	typedef struct tagCardInfo
    {
      uint8 secondTrack[60];	      //二磁道信息
    } CardInfo;


    class COMMONS_API CUserCardService
    {
    public:

      /// @brief 在寻卡成功后，自动调用该方法，该方法留给子类实现
      ///
      /// @return     :0 表示成功
      virtual int SuccessRequestCard() = 0;

      /// @brief 设置写字段值
      ///
      /// @param[in ] :fieldId 字段索引
      /// @param[in ] :fieldValue 字段值
      /// @return     :0 表示成功
      virtual int SetFieldValue( int fieldId, const char *fieldValue ) = 0;

      /// @brief 获取字段值
      ///
      /// 读卡之后调用该方法获得已设置为读模式的字段的值
      /// @param[in ] :fieldId 字段索引
      /// @param[out] :fieldValue 字段值
      /// @return     :0 表示成功
      virtual int GetFieldValue( int fieldId, char *fieldValue ) = 0;

      /// @brief 读卡
      /// 读取卡中的信息
      /// @return     :0 表示成功
      virtual int ReadCard() = 0;

      /// @brief 写卡
      /// 将信息写入卡中
      /// @return     :0 表示成功
      virtual int WriteCard() = 0;

      /// @brief 用户卡初始化
      ///
      /// 初始化用户卡中数据
      /// @param[in ] :showCardNo 显示卡号
      /// @return     :0 表示成功
      virtual int InitUserCard( const char *showCardNo ) = 0;

	  /// @brief 
      ///
      /// 初始化用户卡中数据
      /// @return     :0 表示成功
      virtual int UpdateCardKeys() = 0;

      /// @brief 回收用户卡
      ///
      /// 将用户卡恢复到出厂状态
      /// @return     :0 表示成功
      virtual int RecycleUserCard() = 0;

      /// @brief 充值
      ///
      /// @param[in ] :transPack 交易信息
      /// @return     :0 表示成功
      //virtual int Deposit( TransPack *transPack ) = 0;

      /// @brief 支付
      ///
      /// @param[in ] :transPack 交易信息
      /// @return     :0 表示成功
      //virtual int Pay( TransPack *transPack ) = 0;

      /// @brief 是否已读取密钥
      ///
      /// @return     :0 表示密钥已读
      virtual int IsLoadKeys () = 0;

      /// @brief 检查卡类型
      ///
      /// 检查是否支持cardType对应的卡，子类实现
      /// @param[in ] : cardType 卡类型
      /// @return     :0 表示成功
      virtual int CheckCardType( int cardType ) = 0;

      /// @brief 寻卡
      ///
      /// 卡类型值参考typedef enum {MFCARD = 1, CPUCARD, FIXCARD, PSAM, KEYCARD, SIMCARD} CARD_TYPE
      /// @param[out] :phyid 卡物理ID
      /// @param[out] :cardType 卡类型
      /// @return     :0 表示成功
      int RequestCard( char *phyid, int& cardType );

      /// @brief 寻卡
      ///
      /// @param[out] :phyid 卡物理ID
      /// @return     :0 表示成功
      int RequestCard( char *phyid );

      /// @brief 设置字段读模式
      ///
      /// @param[in ] :fieldId 字段索引
      /// @return     :0 表示成功
      int SetFieldReadMode( int fieldId );

      /// @brief 设置字段写模式
      ///
      /// 调用SetFieldValue方法时，会自动调用该方法
      /// @param[in ] :fieldId 字段索引
      /// @return     :0 表示成功
      int SetFieldWriteMode( int fieldId );

      /// @brief 设置字段读写模式
      ///
      /// @param[in ] :fieldId 字段索引
      /// @return     :0 表示成功
      int SetFieldAllMode( int fieldId );

      /// @brief 清除字段读模式
      ///
      /// @param[in ] :fieldId 字段索引
      /// @return     :0 表示成功
      int ClearFieldReadMode( int fieldId );

      /// @brief 清除字段写模式
      ///
      /// @param[in ] :fieldId 字段索引
      /// @return     :0 表示成功
      int ClearFieldWriteMode( int fieldId );

      /// @brief 清除字段所有模式
      ///
      /// @param[in ] :fieldId 字段索引
      /// @return     :0 表示成功
      int ClearFieldAllMode( int fieldId );

      /// @brief 清除所有字段的读写模式
      ///
      /// @return     :0 表示成功
      int ClearAllFieldAllMode();

      /// @brief 清除所有字段的读模式
      ///
      /// @return     :0 表示成功
      int ClearAllFieldReadMode();

      /// @brief 清除所有字段的写模式
      ///
      /// @return     :0 表示成功
      int ClearAllFieldWriteMode();

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

	  /// @brief 设置PSAM卡服务类
      ///
      /// @param[in ] : psamCardService PSAM卡服务类
      void SetPsamCardervice( Engine::Card::CPsamCardService *psamCardService );

    protected:
      /// @brief 检查字段是否已设置读模式
      ///
      /// @param[in ] :fieldId 字段索引
      /// @return     :0 表示字段已设置读模式
      int CheckFieldReadMode( int fieldId );

      /// @brief 检查字段是否已设置写模式
      ///
      /// @param[in ] :fieldId 字段索引
      /// @return     :0 表示字段已设置写模式
      int CheckFieldWriteMode( int fieldId );

      /// @brief 检查字段是否已读取
      ///
      /// @param[in ] :fieldId 字段索引
      /// @return     :0 表示字段已读取
      int FieldHasRead( int fieldId );

      /// @brief 根据字段对应的类型，将字段显示值转换为卡中存放的字节数组
      ///
      /// @param[in ] :fieldValue 字段原始值
      /// @param[in ] :fieldType 字段类型
      /// @param[in ] :len 字段在卡中所占长度
      /// @param[out] :fieldBuffer 转换之后的字段值
      /// @return     :0 表示成功
      int SetFieldValue( const char *fieldValue, int fieldType, int len, uint8 *fieldBuffer );

      /// @brief 将字段值转换成16进制的日期
      ///
      /// @param[in ] :fieldValue 字段原始值
      /// @param[in ] :len 字段在卡中所占长度
      /// @param[out] :fieldBuffer 转换之后的字段值
      /// @return     :0 表示成功
      int SetHexDate( const char* fieldValue, int len, uint8 *fieldBuffer );

      /// @brief 将字段值转换成16进制的时间
      ///
      /// @param[in ] :fieldValue 字段原始值
      /// @param[in ] :len 字段在卡中所占长度
      /// @param[out] :fieldBuffer 转换之后的字段值
      /// @return     :0 表示成功
      int SetHexTime( const char* fieldValue, int len, uint8 *fieldBuffer );

      /// @brief 根据字段对应的类型，将读出的字节数组转换成相应类型的值
      ///
      /// @param[in ] :fieldBuffer 字段在卡中的存储值
      /// @param[in ] :fieldType 字段类型
      /// @param[in ] :len 字段在卡中所占长度
      /// @param[out] :fieldValue 转换之后的字段值
      /// @return     :0 表示成功
      int GetFieldValue( uint8 *fieldBuffer, int fieldType, int len, char *fieldValue );

      /// @brief 获取系统当前时间,如 20120427145129
      ///
      /// @param[out] :datetime 当前时间
      void GetCurrentDateTime( char *datetime );

      /// @brief 获取五位长度的16进制当前时间，年，月，日，时，分，其中年省去前两位
      ///
      /// @param[out] :datetime 16进制当前时间
      void GetCurrentHexDateTime( uint8 datetime[5] );

      /// @brief 将十六进制日期转换成字符串
      ///
      /// @param[in ] :fieldBuffer 日期原始值
      /// @param[in ] :len 长度
      /// @param[out] :fieldValue 日期字符串，如20120312
      /// @return     :0 表示成功
      int GetDateFromHex( uint8 *fieldBuffer, int len, char *fieldValue );

      /// @brief 将十六进制时间转换成字符串
      ///
      /// @param[in ] :fieldBuffer 时间原始值
      /// @param[in ] :len 长度
      /// @param[out] :fieldValue 时间字符串，如163912
      /// @return     :0 表示成功
      int GetTimeFromHex( uint8 *fieldBuffer, int len, char *fieldValue );

      /// @brief 生成卡物理序列号
      ///
      /// 根据卡物理ID变换生成四字节phyno
      /// @param[in ] :phyid 卡物理ID
      /// @return     :0 表示成功
      int GeneratePhyno( char *phyid );

      /// @brief 获取主密钥
      ///
      /// @return     :uint8* 主密钥
      uint8* GetMainKey();

      char lastErrorMsg[ERROR_MSG_LEN];  //最后错误信息

      std::vector<FieldInfo> fieldList;

      std::map<int, FieldBuffer> fieldMap;

      Engine::Device::CDeviceService *deviceService;  //设备服务对象， 一般指读卡器

	  Engine::Card::CPsamCardService *psamCardService;  //PSAM卡服务类

      char phyid[17];     //卡物理ID

      uint8 phyno[4];     //卡物理序列号

      int keyVersion;

    };

  } // namespace Card
} // namespace Engine


#endif // USER_CARD_H_
