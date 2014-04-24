#pragma once
#ifndef Header_H_
#define Header_H_

typedef struct tagPsamCard
{
    char psamCardNo[21];		//PSAM卡号
    int cardType;		        //PSAM卡类型
    int cardVersion;		    //卡版本
    char termId[13];	        //终端编号
    int termSeqNo;		    //终端交易序号
    int keyIndex;		        //密钥索引号
    char startDate[9];	    //启用日期
    char expireDate[9];       //有效日期
    char publishId[17];	    //发行方标识
    char userId[17];	        //使用方标识
} PsamCard;                 //PSAM卡信息

#endif