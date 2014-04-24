/*
  ����ز�������

  Author:       yiming.you
	Version:      1.0
	Create Date:  2012-06-11
	Review Date:
*/

#ifndef CARD_H_
#define CARD_H_

#include "Global.h"
#include "Header.h"
	
namespace Engine
{
  namespace Card
  {
    /// ��������
    /// 0-Ȧ�� 1-���� 2-���� 3-�������� 4-�������� 5-�󸶷�
    typedef enum { TRANS_LOAD = 0, TRANS_PURCHASE = 1, TRANS_GRAYLOCK = 2,
                   TRANS_GRAYUNLOCK = 3 , TRANS_CAPP_PURCHASE = 4, TRANS_POSTPAID = 5
                 } TRANS_TYPE;

    /// MF��Կ�㷨
    enum
    {
      KA_1ST = 0,   // ��һ���汾
      KA_PSAM = 1,  // PSAM ��
      KA_CMCC = 2,  // �ƶ��淶ʱʹ�õķ���
    };

    static const unsigned char staticKey[17] = "3230303530313331"; //��ɢ��Կ�����ڼ���

	typedef struct tagCPUCardKeys
    {
      uint8 MK[17];	    //������Կ
      uint8 DPK1[17];	  //����1
      uint8 DPK2[17];	  //����2
      uint8 DLK1[17];   //Ȧ��1
      uint8 DLK2[17];   //Ȧ��2
      uint8 DTAC[17];	  //�ڲ���ԿTAC
      uint8 DACK[17];	  //�ⲿ��֤
      uint8 DUK[17];    //PIN����
      uint8 DRPK[17];	  //PIN��װ
      uint8 DAMK[17];	  //ά����Կ
      uint8 DACK1[17];	//�ⲿ��֤1
      uint8 DACK2[17];	//�ⲿ��֤2
      uint8 DACK3[17];	//�ⲿ��֤3
      uint8 LDACK[17];	//СǮ���ⲿ��֤��Կ
      uint8 LDAMK[17];	//СǮ��ά����Կ
      int  bLoad;       //�Ƿ��Ѽ���
    } CPUCardKeys;      //CPU����Կ

	

  } // namespace Card
} // namespace Engine


#endif // KEY_CARD_H_
