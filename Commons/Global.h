#pragma once
#ifndef GLOBAL_H_
#define GLOBAL_H_

#ifndef uint8
#define uint8  unsigned char
#endif

//ϵͳ�����붨��
#define SUCCESS 0                           //�ɹ�
#define FILE_NOT_FOUND 6001                 //δ�ҵ��ļ�

#define DEV_PORT_NOTOPEN 6100               //�˿�δ��
#define DEV_REQUESTCARD 6101                //Ѱ��ʧ��
#define DEV_LOGINCARD 6102                  //��½��Ƭʧ��
#define DEV_WRITECARD 6103                  //д��ʧ��
#define DEV_READCARD 6104                   //����ʧ��
#define DEV_UNCONFIRM 6105                  //����ȷ�ϵ�д������
#define DEV_WRITEPURSE 6106                 //д��Ǯ��ʧ��
#define DEV_OPENDEVICE 6107                 //���豸ʧ��
#define DEV_BLOCK_OVERFLOW 6108             //��д�鳬����Χ
#define DEV_PARAMERR 6109                   //��������������
#define DEV_CMDERROR 6110                   //CPUָ��ʧ��
#define DEV_CMDFAILD 6111                   //CPUָ��ش���
#define DEV_NOTSUPPORT 6112                 //��֧�ֵĲ���
#define DEV_NOPARAMFOUND 6113               //δ�ҵ�ָ������
#define DEV_PURSEERROR 6114                 //��Ǯ����ʽ����
#define DEV_EXCEEDMONEY	6115                //��ֵ��������
#define DEV_UNAUTH_TERM 6116                //�ն�δ��Ȩ
#define DEV_PURSECRCERR	6117                //��Ǯ�����ݿ�У���
#define DEV_COMM_ERR 6199                   //�豸δ֪����
#define DEV_POWON_ERR 6100                  //�ϵ縴λʧ��
#define DEV_SETCPU_ERR 6101                 //����SAM����ʧ��
#define DEV_PSAM_SLOT_ERR 6102              //��֧�ֵ�PSAM����


#define SYS_UNREALIZED_FUNC 6200            //δʵ�ֵĹ���
#define SYS_LOAD_DLL_ERR 6201               //���ض�̬��ʧ��
#define SYS_LOAD_CFG_ERR 6202               //���������ļ�ʧ��
#define SYS_CREATE_FUNC_ERR 6203            //��ʼ��������������
#define SYS_CREATE_INSTANCE_ERR 6204        //�������ʵ��ʧ��
#define SYS_LOAD_STARTUP_ERR 6205           //���������ļ�����������ʧ��
#define SYS_UNSUPPORTED_FUNC 6206           //��֧�ֵķ��� 
#define SYS_CFG_INDEX_ERR 6207              //��֧�ֵĲ�������
#define SYS_UNKNOW_PARA 6208                //δ�ҵ�ϵͳ����
#define SYS_UNENOUGH_LEN 6209               //�������ݳ��Ȳ���
#define SYS_UNKNOW_ATTRIBUTE 6210           //δ�ҵ���Ӧ����
#define SYS_UNKNOW_NODE 6211                //δ�ҵ���Ӧ�ڵ�
#define SYS_CARD_TYPE_ERR 6212              //��֧�ֵĿ�����
#define SYS_PARA_LENGTH_ERR 6213            //�������Ȳ���
#define SYS_SUBAPP_UNKNOW 6214              //SIM����һ��ͨ��Ӧ��δ����

#define CF_INDEX_ERR 6300                   //�ֶ�����������
#define CF_INDEX_UNUSED 6301                //�ֶ�δʹ��
#define CF_READ_RIGHT_ERR 6302              //�ֶ�δ���ö�Ȩ��
#define CF_WRITE_RIGHT_ERR 6303             //�ֶ�δ����дȨ��
#define CF_UNUSED_ERR 6304                  //�ֶ�δʹ��
#define CF_NO_READ_MODE 6305                //�ֶ�δ���ö�ģʽ
#define CF_NO_WRITE_MODE 6306               //�ֶ�δ����дģʽ
#define CF_NO_READ_ERR 6307                 //�ֶ�δ��ȡ
#define CF_NO_WRITE_ERR 6308                //�ֶ�δд
#define CF_FIELD_DEF_ERR 6309               //�ֶζ������
#define CF_UNSUPPORTED_CRC 6310             //��֧�ֵ�CRCУ�鷽ʽ
#define CF_CHECK_CRC_ERR 6311               //CRCУ��ʧ��

#define CARD_PHYID_LENGTH_ERR 6401          //������ID��������
#define CARD_PIN_LENGTH_ERR 6402            //pin���������
#define CARD_READ_LENGTH_ERR 2014           //��֧�ֵĶ�ȡ����

#define CARD_PSAM_DATE_LENGTH_ERR 6501      //psam�����ڳ��ȴ���
#define CARD_PSAM_KEY_INDEX_ERR 6502        //psam������Կ�����Ŵ���

#define CARD_UNLOAD_MAIN_KEY 6600           //δ���ظ���Կ

#define ERROR_MSG_LEN 1024                  //������Ϣ��󳤶�
#define MF "3F00"                           //CPU��MFĿ¼��ʶ��
#define MAX_APDU_COMMAND_LEN 512            //apdu������󳤶�

//MFCARD=S50��FIXCARD=CPU7+1
typedef enum {MFCARD = 1, CPUCARD, FIXCARD, PSAM, KEYCARD, SIMCARD} CARD_TYPE;

//��������
typedef enum {CARDSLOT_1 = 1, CARDSLOT_2 = 2, CARDSLOT_3 = 3, CARDSLOT_4 = 4, CARDSLOT_5 = 5, CARDSLOT_RF = 99} CARD_SLOT;

#define SET_ERR(message) do \
{ std::ostringstream oss;oss<<message;SetLastErrorMsg(oss.str().c_str());}while(0)

//���ֶ���������
#define CF_START 1		        //��ʼ��
#define CF_CARDNO	2		    //����
#define CF_SECONDTRACK	3		//���ŵ���Ϣ
#define CF_END 99		        //������־

#endif