/***************************************************************************************
�����⣺FIFO.C
ԭʼ�汾�ţ�V1.0
��ʼ�ˣ���ǿ
���°汾�ţ�V1.0
����˵����
�����ˣ�
***************************************************************************************/



#include "FIFO.h"

//FIFO myFIFO;

/***************************************************************************************
��  ����void CreateFIFO(uint32 FIFOLength)
��������FIFOLength��Ҫ������FIFO�ĳ��ȴ�С
����ֵ����
��  ע���ú���ʹ���˶�̬�ڴ���䣬����Ҫ����ʹ����
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
��  ����uint8 WriteFIFO(int8* pSource,uint32 WriteLength)
��������pSource��ָ��Ҫ����Դ��һ��ָ��
        WriteLength��Ҫд��FIFO�����ݳ���
����ֵ��0--��ʾҪд��ĳ��ȳ�����FIFO��д��ĳ��ȣ�
        1--��ʾ�ɹ�д��
��  ע���ú�������������FIFO����д��ָ�����ȵ����ݣ�������ݳ�����д���Ƚ����ش���ֵ
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
��  ����uint8 ReadFIFO(int8 *pAim,uint32 ReadLength)
��������pAim��ָ��ҪĿ��Դ��һ��ָ��
        ReadLength��Ҫд��FIFO�����ݳ���
����ֵ��0--��ʾҪ��ȡ�ĳ��ȳ�����FIFO�ɶ�ȡ�ĳ��ȣ�
        1--��ʾ�ɹ���ȡ
��  ע���ú����������ǽ�FIFO��������ݶ�����ָ����һ���ط�
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
��  ����uint32 CheckFIFOLength(void)
��������
����ֵ��FIFO���ܳ���
��  ע��
***************************************************************************************/
uint32_t CheckFIFOLength(FIFO *myFIFO)
{
    return myFIFO->Length;
}


/***************************************************************************************
��  ����int8* CheckCurrentWritePoint(void)
��������
����ֵ����ǰдָ��ľ���λ��
��  ע��
***************************************************************************************/
uint8_t * CheckCurrentWritePoint(FIFO *myFIFO)
{
    return (myFIFO->pFirst+myFIFO->CurrentWriteNum);
}

/***************************************************************************************
��  ����int8* CheckCurrentReadPoint(void)
��������
����ֵ����ǰ��ָ��ľ���λ��
��  ע��
***************************************************************************************/
uint8_t * CheckCurrentReadPoint(FIFO *myFIFO)
{
    return (myFIFO->pFirst+myFIFO->CurrentReadNum);
}

/***************************************************************************************
��  ����void FreeFIFO(void)
��������
����ֵ��
��  ע���ͷ�FIFOռ�õ��ڴ�
***************************************************************************************/
void FreeFIFO(FIFO *myFIFO)
{
    free(myFIFO->pFirst);
}

/***************************************************************************************
��  ����uint32 CheckCanWriteNum(void)
��������
����ֵ�����ػ���д��FIFO�����ݳ���
��  ע��
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
��  ����uint32 CheckCanReadNum(void)
��������
����ֵ�������ܱ�������FIFO���ݳ���
��  ע��
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






