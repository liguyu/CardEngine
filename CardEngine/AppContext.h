#ifndef APP_CONTEXT_H_
#define APP_CONTEXT_H_

#include "Config.h"
#include <windows.h>
#include "../Commons/Logger.h"
#include "../Commons/Device.h"
#include "../Commons/UserCard.h"
#include "../Commons/PsamCard.h"

extern "C" IMAGE_DOS_HEADER  __ImageBase; // DLL HMODULE

#define CFG_NAME "RedCard.properties"                  //配置文件名称
#define LOG_CFG "log4cplus.properties"                 //日志配置文件名称

class AppContext
{
    enum { MAX_PATH_SIZE = 1024 };

public:
    AppContext();

    ~AppContext();

    /// @brief 卡引擎工作环境初始化
    ///
    /// @return     :0 表示成功
    int InitContext();  

	/// @brief 获取设备服务类
    ///
    /// @return     :设备服务类
    Engine::Device::CDeviceService* GetDeviceService();

	/// @brief 获取用户卡服务类
    ///
    /// @param[out] :phyid 卡物理ID
    /// @param[out] :cardType 卡类型
    /// @param[out] :message 返回消息
    /// @return     :用户卡服务类
    Engine::Card::CUserCardService* GetUserCardService();

	/// @brief 获取PSAM卡服务类
    ///
    /// @return     :PSAM卡服务类
    Engine::Card::CPsamCardService* GetPsamCardService();

protected:

    /// @brief 获取文件的绝对路径
    ///
    /// @param[in ] :fileName 文件名
    /// @param[out] :path 文件绝对路径
    /// @return     :0 表示成功
    void GetAbsolutePath( char* fileName, char* path );

    /// @brief 获取当前dll所在目录的全路径
    ///
    /// @param[out] :path 当前dll全路径
    /// @return     :0 表示成功
    void GetCurrentDirectory( char *path );

	/// @brief 初始化日志配置
    ///
    int InitLog();

    /// @brief 读取配置文件
    ///
    /// @return     :0 表示成功
    int InitConfig();

	int InitService();

    char currentPath[MAX_PATH_SIZE];   //dll所在目录的全路径

	Config config;  //加载系统配置参数类

private:
	Engine::Device::CDeviceService *deviceService;                 //设备
	Engine::Card::CUserCardService* userCardService;               //用户卡
	Engine::Card::CPsamCardService* psamCardService;               //PSAM卡
};

#endif
