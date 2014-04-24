/*
  CardEngine对外接口
*/
#ifdef CARDENGINE_EXPORTS
#define CARDENGINE_API __declspec(dllexport)
#else
#define CARDENGINE_API __declspec(dllimport)
#endif
#include "../Commons/Header.h"

#ifdef __cplusplus
extern "C"
{
#endif

	/// @brief 初始化CardInterface工作环境
	///
    /// @return     :0 表示成功
	CARDENGINE_API int ENG_Init();

	/// @brief 获得CardEngine版本号
	///
	/// @return     :CardEngine版本号
	CARDENGINE_API const char* ENG_GetVersion();

	/// @brief 获得CardEngine编译日期
	///
	/// @return     :CardEngine编译日期
	CARDENGINE_API const char* ENG_GetCompiledDate();

	/// @brief 初始化通讯端口
	///
	/// 在使用读卡器对卡进行操作之前，需首先调用该方法
	/// @param[in ] :port 端口号，取值为0-19，表示串口1-20，为100时表示USB口通讯
	/// @param[in ] :baud 通讯波特率9600～115200，采用USB口通讯时，波特率无效
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_OpenPort( int port, int baud );

	/// @brief 关闭端口
	///
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_ClosePort();

	/// @brief 读卡器蜂鸣
	///
	/// 一般执行某项操作成功的时候，可以调用该方法提示用户操作成功
	/// @return     :0 表示成功
	CARDENGINE_API void ENG_Beep();

	/// @brief 读卡器出错时蜂鸣
	///
	/// 蜂鸣时间较Beep长一点
	/// @return     :0 表示成功
	CARDENGINE_API void ENG_BeepError();

	/// @brief 寻卡
	///
	/// 卡类型值参考typedef enum {MFCARD = 1, CPUCARD, FIXCARD, PSAM, KEYCARD, SIMCARD} CARD_TYPE
	/// @param[out] :phyid 卡物理ID
	/// @param[out] :cardType 卡类型
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_RequestCard( char *phyid, int& cardType );

	/// @brief 获取最后错误信息
	///
	/// @param[in ] :errorMsg 错误信息
	/// @return     :0 表示成功
	CARDENGINE_API char* ENG_GetLastErrorMsg( char *errorMsg );

	/// @brief 中止对该卡操作
	///
	/// 在对卡进行操作完毕后，执行ENG_Halt();则该卡进入HALT模式
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_Halt();

	/// @brief 设置字段读模式
	///
	/// @param[in ] :fieldId 字段索引
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_SetFieldReadMode( int fieldId );

	/// @brief 读卡
	/// 读取卡中的信息，用法如下:
	///      ENG_SetFieldReadMode(field1);
	///      ENG_SetFieldReadMode(field2);
	///      ......
	///      ENG_ReadCard();
	///      ENG_GetFieldValue(field1,field1Vlaue);
	///      ENG_GetFieldValue(field2,field2Vlaue);
	///      ......
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_ReadCard();

	/// @brief 回收用户卡
	///
	/// 将用户卡恢复到出厂状态
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_RecycleUserCard();

	/// @brief 用户卡初始化
	///
	/// 初始化用户卡中各扇区中数据
	/// @param[in ] :phyid 卡物理ID
	/// @param[in ] :showCardNo 显示卡号
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_InitUserCard( const char *phyid, const char *showCardNo );

	/// @brief 清除所有字段的读写模式
	///
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_ClearAllFieldAllMode();

	/// @brief 写卡
	/// 将信息写入卡中，用法如下:
	///      ENG_SetFieldValue(field1,field1value);
	///      ENG_SetFieldValue(field2,field2value);
	///      ......
	///      ENG_WriteCard();
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_WriteCard ( const char *phyid = 0 );

	/// @brief 设置字段写模式
	///
	/// 调用ENG_SetFieldValue方法时，会自动调用该方法
	/// @param[in ] :fieldId 字段索引
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_SetFieldWriteMode( int fieldId );

	/// @brief 获取字段值
	///
	/// 读卡之后调用该方法获得已设置为读模式的字段的值
	/// @param[in ] :fieldId 字段索引
	/// @param[out] :fieldValue 字段值
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_GetFieldValue( int fieldId, char *fieldValue );

	/// @brief 设置写字段值
	///
	/// @param[in ] :fieldId 字段索引
	/// @param[in ] :fieldValue 字段值
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_SetFieldValue( int fieldId, const char *fieldValue );

	/// @brief 设置字段读写模式
	///
	/// @param[in ] :fieldId 字段索引
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_SetFieldAllMode( int fieldId );

	/// @brief 清除字段读模式
	///
	/// @param[in ] :fieldId 字段索引
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_ClearFieldReadMode( int fieldId );

	/// @brief 清除字段写模式
	///
	/// @param[in ] :fieldId 字段索引
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_ClearFieldWriteMode( int fieldId );

	/// @brief 清除字段所有模式
	///
	/// @param[in ] :fieldId 字段索引
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_ClearFieldAllMode( int fieldId );

	/// @brief 清除所有字段的读模式
	///
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_ClearAllFieldReadMode();

	/// @brief 清除所有字段的写模式
	///
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_ClearAllFieldWriteMode();

	/// @brief 获取字段值
	///
	/// 调用该方法读取字段的值
	/// @param[in ] :fieldId 字段索引
	/// @param[out] :fieldValue 字段值
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_ReadFieldValue( int fieldId, char *fieldValue );

	/// @brief 写字段
	///
	/// @param[in ] :fieldId 字段索引
	/// @param[in ] :fieldValue 字段值
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_WriteFieldValue( int fieldId, const char *fieldValue );

	/// @brief 读取卡号和二磁道信息
	///
	/// 调用该方法读取字段的值
	/// @param[out] :cardNo 卡号
	/// @param[out] :secondTrack 二磁道信息
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_ReadCardNoAndSecondTrack( char *cardNo,char *secondTrack );

	/// @brief 设置要操作的psam卡座
	///
	/// @param[in ] :slot psam卡座，1、 2、 3 各为SAM1(大卡座)、SAM2、SAM3
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_SetPsamSlot( int slot );

	/// @brief PSAM卡上电复位
	///
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_ResetPsam();

	/// @brief 读取PSAM卡终端编号
	///
	/// @param[out] :termNo psam卡终端编号
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_ReadPsamTermNo( char *termNo );

	/// @brief 发行PSAMK卡
	///
	/// @param[in ] :psamCard psam卡信息
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_InitPsamCard( PsamCard* psamCard );

	/// @brief 回收PSAM卡
	///
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_RecyclePsamCard();

	/// @brief 读取PSAM卡信息
	///
	/// @param[out] :psamCard psam卡信息
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_ReadPsamCard( PsamCard* psamCard );

	/// @brief 更新初始化秘钥
	///
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_UpdateCardKeys();

	/// @brief 设置根密钥
	///
	/// @return     :0 表示成功
	CARDENGINE_API int ENG_SetMainKey(char mainKey[33]);
 
#ifdef __cplusplus
}
#endif
