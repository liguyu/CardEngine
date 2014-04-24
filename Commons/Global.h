#pragma once
#ifndef GLOBAL_H_
#define GLOBAL_H_

#ifndef uint8
#define uint8  unsigned char
#endif

//系统返回码定义
#define SUCCESS 0                           //成功
#define FILE_NOT_FOUND 6001                 //未找到文件

#define DEV_PORT_NOTOPEN 6100               //端口未打开
#define DEV_REQUESTCARD 6101                //寻卡失败
#define DEV_LOGINCARD 6102                  //登陆卡片失败
#define DEV_WRITECARD 6103                  //写卡失败
#define DEV_READCARD 6104                   //读卡失败
#define DEV_UNCONFIRM 6105                  //不能确认的写卡操作
#define DEV_WRITEPURSE 6106                 //写卡钱包失败
#define DEV_OPENDEVICE 6107                 //打开设备失败
#define DEV_BLOCK_OVERFLOW 6108             //读写块超过范围
#define DEV_PARAMERR 6109                   //读卡器参数错误
#define DEV_CMDERROR 6110                   //CPU指令失败
#define DEV_CMDFAILD 6111                   //CPU指令返回错误
#define DEV_NOTSUPPORT 6112                 //不支持的操作
#define DEV_NOPARAMFOUND 6113               //未找到指定参数
#define DEV_PURSEERROR 6114                 //卡钱包格式错误
#define DEV_EXCEEDMONEY	6115                //充值金额超过限制
#define DEV_UNAUTH_TERM 6116                //终端未授权
#define DEV_PURSECRCERR	6117                //卡钱包数据块校验错
#define DEV_COMM_ERR 6199                   //设备未知错误
#define DEV_POWON_ERR 6100                  //上电复位失败
#define DEV_SETCPU_ERR 6101                 //设置SAM卡座失败
#define DEV_PSAM_SLOT_ERR 6102              //不支持的PSAM卡座


#define SYS_UNREALIZED_FUNC 6200            //未实现的功能
#define SYS_LOAD_DLL_ERR 6201               //加载动态库失败
#define SYS_LOAD_CFG_ERR 6202               //加载配置文件失败
#define SYS_CREATE_FUNC_ERR 6203            //初始化对象函数不存在
#define SYS_CREATE_INSTANCE_ERR 6204        //构造对象实例失败
#define SYS_LOAD_STARTUP_ERR 6205           //加载配置文件中启动参数失败
#define SYS_UNSUPPORTED_FUNC 6206           //不支持的方法 
#define SYS_CFG_INDEX_ERR 6207              //不支持的参数索引
#define SYS_UNKNOW_PARA 6208                //未找到系统参数
#define SYS_UNENOUGH_LEN 6209               //接收数据长度不够
#define SYS_UNKNOW_ATTRIBUTE 6210           //未找到对应属性
#define SYS_UNKNOW_NODE 6211                //未找到对应节点
#define SYS_CARD_TYPE_ERR 6212              //不支持的卡类型
#define SYS_PARA_LENGTH_ERR 6213            //参数长度不符
#define SYS_SUBAPP_UNKNOW 6214              //SIM卡中一卡通子应用未创建

#define CF_INDEX_ERR 6300                   //字段索引不存在
#define CF_INDEX_UNUSED 6301                //字段未使用
#define CF_READ_RIGHT_ERR 6302              //字段未设置读权限
#define CF_WRITE_RIGHT_ERR 6303             //字段未设置写权限
#define CF_UNUSED_ERR 6304                  //字段未使用
#define CF_NO_READ_MODE 6305                //字段未设置读模式
#define CF_NO_WRITE_MODE 6306               //字段未设置写模式
#define CF_NO_READ_ERR 6307                 //字段未读取
#define CF_NO_WRITE_ERR 6308                //字段未写
#define CF_FIELD_DEF_ERR 6309               //字段定义错误
#define CF_UNSUPPORTED_CRC 6310             //不支持的CRC校验方式
#define CF_CHECK_CRC_ERR 6311               //CRC校验失败

#define CARD_PHYID_LENGTH_ERR 6401          //卡物理ID长度有误
#define CARD_PIN_LENGTH_ERR 6402            //pin口令长度有误
#define CARD_READ_LENGTH_ERR 2014           //不支持的读取长度

#define CARD_PSAM_DATE_LENGTH_ERR 6501      //psam卡日期长度错误
#define CARD_PSAM_KEY_INDEX_ERR 6502        //psam卡中密钥索引号错误

#define CARD_UNLOAD_MAIN_KEY 6600           //未加载根密钥

#define ERROR_MSG_LEN 1024                  //错误信息最大长度
#define MF "3F00"                           //CPU卡MF目录标识符
#define MAX_APDU_COMMAND_LEN 512            //apdu报文最大长度

//MFCARD=S50，FIXCARD=CPU7+1
typedef enum {MFCARD = 1, CPUCARD, FIXCARD, PSAM, KEYCARD, SIMCARD} CARD_TYPE;

//卡座类型
typedef enum {CARDSLOT_1 = 1, CARDSLOT_2 = 2, CARDSLOT_3 = 3, CARDSLOT_4 = 4, CARDSLOT_5 = 5, CARDSLOT_RF = 99} CARD_SLOT;

#define SET_ERR(message) do \
{ std::ostringstream oss;oss<<message;SetLastErrorMsg(oss.str().c_str());}while(0)

//卡字段索引定义
#define CF_START 1		        //开始号
#define CF_CARDNO	2		    //卡号
#define CF_SECONDTRACK	3		//二磁道信息
#define CF_END 99		        //结束标志

#endif