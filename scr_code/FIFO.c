/***************************************************************************************
函数库：FIFO.C
原始版本号：V1.0
创始人：熊强
更新版本号：V1.0
更新说明：
更新人：
***************************************************************************************/



#include "FIFO.h"

//FIFO myFIFO;

/***************************************************************************************
函  数：void CreateFIFO(uint32 FIFOLength)
输入量：FIFOLength是要创建的FIFO的长度大小
返回值：无
备  注：该函数使用了动态内存分配，所以要减少使用量
***************************************************************************************/
void CreateFIFO(FIFO *DmaFIFO,uint32_t FIFOLength)
{
    uint8_t *pmyFIFO;
    pmyFIFO = (uint8_t *)malloc(FIFOLength);
    DmaFIFO->pFirst = pmyFIFO;
    DmaFIFO->pLast = pmyFIFO + FIFOLength;
    DmaFIFO->Length = FIFOLength;
    DmaFIFO->pCurrentData = DmaFIFO->pFirst;
    DmaFIFO->CanWriteNum = 0;
    DmaFIFO->CanReadNum = 0;
    DmaFIFO->FlagWrite = 0;
    DmaFIFO->FlagRead = 0;
    DmaFIFO->CurrentWriteNum = 0;
    DmaFIFO->CurrentReadNum = 0;
}

void InitFIFO(FIFO *DmaFIFO,uint8_t *pbuffer,uint32_t FIFOLength){
	DmaFIFO->pFirst = pbuffer;
    DmaFIFO->pLast = pbuffer + FIFOLength;
    DmaFIFO->Length = FIFOLength;
    DmaFIFO->pCurrentData = DmaFIFO->pFirst;
    DmaFIFO->CanWriteNum = 0;
    DmaFIFO->CanReadNum = 0;
    DmaFIFO->FlagWrite = 0;
    DmaFIFO->FlagRead = 0;
    DmaFIFO->CurrentWriteNum = 0;
    DmaFIFO->CurrentReadNum = 0;
}


/***************************************************************************************
函  数：uint8 WriteFIFO(int8* pSource,uint32 WriteLength)
输入量：pSource是指向要数据源的一个指针
        WriteLength是要写入FIFO的数据长度
返回值：0--表示要写入的长度超过了FIFO可写入的长度，
        1--表示成功写入
备  注：该函数的作用是向FIFO里面写入指定长度的数据，如果数据超过可写长度将返回错误值
***************************************************************************************/
uint8_t WriteFIFO(FIFO *myFIFO,uint8_t* pSource,uint32_t WriteLength)
{
    uint32_t i;
    if(ZeroCircle == myFIFO->FlagWrite)
    {
        myFIFO->CanWriteNum = myFIFO->Length - myFIFO->CurrentWriteNum + myFIFO->CurrentReadNum;
    }
    else
    {
        myFIFO->CanWriteNum = myFIFO->CurrentReadNum - myFIFO->CurrentWriteNum;
    }
    if(myFIFO->CanWriteNum >= WriteLength)
    {
        myFIFO->CanWriteNum = WriteLength;
    }
    else
    {
        return NOMORENUM;
    }
    for(i=0;i<myFIFO->CanWriteNum;i++)
    {
        *(myFIFO->pFirst + myFIFO->CurrentWriteNum) = *pSource;
        pSource++;
        myFIFO->CurrentWriteNum++;
        if(myFIFO->Length == myFIFO->CurrentWriteNum)
        {
            //IRQDisable();
            myFIFO->CurrentWriteNum = 0;
            myFIFO->FlagWrite = OneCircle;
            //IRQEnable();
        }
    }
    return WriteSuccess;
}


/***************************************************************************************
函  数：uint8 ReadFIFO(int8 *pAim,uint32 ReadLength)
输入量：pAim是指向要目标源的一个指针
        ReadLength是要写入FIFO的数据长度
返回值：0--表示要读取的长度超过了FIFO可读取的长度，
        1--表示成功读取
备  注：该函数的作用是将FIFO里面的数据读出到指定的一个地方
***************************************************************************************/
uint8_t ReadFIFO(FIFO *myFIFO,uint8_t *pAim,uint32_t ReadLength)
{
    uint32_t i;
    if(ZeroCircle == myFIFO->FlagWrite)
    {
        myFIFO->CanReadNum = myFIFO->CurrentWriteNum - myFIFO->CurrentReadNum;
    }
    else
    {
        myFIFO->CanReadNum = myFIFO->Length - myFIFO->CurrentReadNum + myFIFO->CurrentWriteNum;
    }
    if(myFIFO->CanReadNum >= ReadLength)
    {
        myFIFO->CanReadNum = ReadLength;
    }
    else
    {
        return NOMORENUM;
    }
    for(i=0;i<myFIFO->CanReadNum;i++)
    {
        *pAim = *(myFIFO->pFirst + myFIFO->CurrentReadNum);
        pAim++;
        myFIFO->CurrentReadNum++;
        if(myFIFO->Length == myFIFO->CurrentReadNum)
        {
            //IRQDisable();
            myFIFO->CurrentReadNum = 0;
            myFIFO->FlagWrite = ZeroCircle;
            //IRQEnable();
        }      
    }
    return ReadSuccess;    
}

/***************************************************************************************
函  数：uint32 CheckFIFOLength(void)
输入量：
返回值：FIFO的总长度
备  注：
***************************************************************************************/
uint32_t CheckFIFOLength(FIFO *myFIFO)
{
    return myFIFO->Length;
}


/***************************************************************************************
函  数：int8* CheckCurrentWritePoint(void)
输入量：
返回值：当前写指针的具体位置
备  注：
***************************************************************************************/
uint8_t * CheckCurrentWritePoint(FIFO *myFIFO)
{
    return (myFIFO->pFirst+myFIFO->CurrentWriteNum);
}

/***************************************************************************************
函  数：int8* CheckCurrentReadPoint(void)
输入量：
返回值：当前读指针的具体位置
备  注：
***************************************************************************************/
uint8_t * CheckCurrentReadPoint(FIFO *myFIFO)
{
    return (myFIFO->pFirst+myFIFO->CurrentReadNum);
}

/***************************************************************************************
函  数：void FreeFIFO(void)
输入量：
返回值：
备  注：释放FIFO占用的内存
***************************************************************************************/
void FreeFIFO(FIFO *myFIFO)
{
    free(myFIFO->pFirst);
}

/***************************************************************************************
函  数：uint32 CheckCanWriteNum(void)
输入量：
返回值：返回还能写入FIFO的数据长度
备  注：
***************************************************************************************/
uint32_t CheckCanWriteNum(FIFO *myFIFO)
{
    if(ZeroCircle == myFIFO->FlagWrite)
    {
        myFIFO->CanWriteNum = myFIFO->Length - myFIFO->CurrentWriteNum + myFIFO->CurrentReadNum;
    }
    else
    {
        myFIFO->CanWriteNum = myFIFO->CurrentReadNum - myFIFO->CurrentWriteNum;
    }
    return myFIFO->CanWriteNum;
}

/***************************************************************************************
函  数：uint32 CheckCanReadNum(void)
输入量：
返回值：返回能被读出的FIFO数据长度
备  注：
***************************************************************************************/
uint32_t CheckCanReadNum(FIFO *myFIFO)
{
    //IRQDisable();
    if(ZeroCircle == myFIFO->FlagWrite)
    {
        myFIFO->CanReadNum = myFIFO->CurrentWriteNum - myFIFO->CurrentReadNum;
    }
    else
    {
        myFIFO->CanReadNum = myFIFO->Length - myFIFO->CurrentReadNum + myFIFO->CurrentWriteNum;
    }
    //IRQEnable();
    return myFIFO->CanReadNum;
}






