#ifndef APP_CONTEXT_H_
#define APP_CONTEXT_H_

#include "Config.h"
#include <windows.h>
#include "../Commons/Logger.h"
#include "../Commons/Device.h"
#include "../Commons/UserCard.h"
#include "../Commons/PsamCard.h"

extern "C" IMAGE_DOS_HEADER  __ImageBase; // DLL HMODULE

#define CFG_NAME "RedCard.properties"                  //�����ļ�����
#define LOG_CFG "log4cplus.properties"                 //��־�����ļ�����

class AppContext
{
    enum { MAX_PATH_SIZE = 1024 };

public:
    AppContext();

    ~AppContext();

    /// @brief �����湤��������ʼ��
    ///
    /// @return     :0 ��ʾ�ɹ�
    int InitContext();  

	/// @brief ��ȡ�豸������
    ///
    /// @return     :�豸������
    Engine::Device::CDeviceService* GetDeviceService();

	/// @brief ��ȡ�û���������
    ///
    /// @param[out] :phyid ������ID
    /// @param[out] :cardType ������
    /// @param[out] :message ������Ϣ
    /// @return     :�û���������
    Engine::Card::CUserCardService* GetUserCardService();

	/// @brief ��ȡPSAM��������
    ///
    /// @return     :PSAM��������
    Engine::Card::CPsamCardService* GetPsamCardService();

protected:

    /// @brief ��ȡ�ļ��ľ���·��
    ///
    /// @param[in ] :fileName �ļ���
    /// @param[out] :path �ļ�����·��
    /// @return     :0 ��ʾ�ɹ�
    void GetAbsolutePath( char* fileName, char* path );

    /// @brief ��ȡ��ǰdll����Ŀ¼��ȫ·��
    ///
    /// @param[out] :path ��ǰdllȫ·��
    /// @return     :0 ��ʾ�ɹ�
    void GetCurrentDirectory( char *path );

	/// @brief ��ʼ����־����
    ///
    int InitLog();

    /// @brief ��ȡ�����ļ�
    ///
    /// @return     :0 ��ʾ�ɹ�
    int InitConfig();

	int InitService();

    char currentPath[MAX_PATH_SIZE];   //dll����Ŀ¼��ȫ·��

	Config config;  //����ϵͳ���ò�����

private:
	Engine::Device::CDeviceService *deviceService;                 //�豸
	Engine::Card::CUserCardService* userCardService;               //�û���
	Engine::Card::CPsamCardService* psamCardService;               //PSAM��
};

#endif
