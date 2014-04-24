/*
  卡相关参数定义

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
    /// 交易类型
    /// 0-圈存 1-消费 2-灰锁 3-联机补扣 4-复合消费 5-后付费
    typedef enum { TRANS_LOAD = 0, TRANS_PURCHASE = 1, TRANS_GRAYLOCK = 2,
                   TRANS_GRAYUNLOCK = 3 , TRANS_CAPP_PURCHASE = 4, TRANS_POSTPAID = 5
                 } TRANS_TYPE;

    /// MF密钥算法
    enum
    {
      KA_1ST = 0,   // 第一个版本
      KA_PSAM = 1,  // PSAM 卡
      KA_CMCC = 2,  // 移动规范时使用的方法
    };

    static const unsigned char staticKey[17] = "3230303530313331"; //分散密钥，用于加密

	typedef struct tagCPUCardKeys
    {
      uint8 MK[17];	    //种子密钥
      uint8 DPK1[17];	  //消费1
      uint8 DPK2[17];	  //消费2
      uint8 DLK1[17];   //圈存1
      uint8 DLK2[17];   //圈存2
      uint8 DTAC[17];	  //内部密钥TAC
      uint8 DACK[17];	  //外部认证
      uint8 DUK[17];    //PIN解锁
      uint8 DRPK[17];	  //PIN重装
      uint8 DAMK[17];	  //维护密钥
      uint8 DACK1[17];	//外部认证1
      uint8 DACK2[17];	//外部认证2
      uint8 DACK3[17];	//外部认证3
      uint8 LDACK[17];	//小钱包外部认证密钥
      uint8 LDAMK[17];	//小钱包维护密钥
      int  bLoad;       //是否已加载
    } CPUCardKeys;      //CPU卡密钥

	

  } // namespace Card
} // namespace Engine


#endif // KEY_CARD_H_
