/**
*************************************************************************
@File   : hal\src_code\veth_InVariate.c
@Author : Duke Lee
@Date   : 13-Oct-2014
@Brief  : To keep some variate no lose When power supply drop 
*************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "ff_gen_drv.h"
#include "veth_nor.h"
#include "veth_common.h"
#ifdef MY_DEBUG
#include "diskio.h"
#endif
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Block Size in Bytes */
#define BLOCK_SIZE                4096
#define NOR_DEVICE_SIZE          (uint32_t)(0x0200000)
#define NOR_SECTOR_SIZE           0x20000
/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

/*Before write a block, the specific phisical sector need to be erased.But maybe 
  only one sector need to be modified,so a temporary two-dimension array should 
  be defined to store it for a while.
  */
uint8_t tem_sector[NOR_SECTOR_SIZE/BLOCK_SIZE][BLOCK_SIZE];

/* Private function prototypes -----------------------------------------------*/
DSTATUS norInitialize (void);
DSTATUS norStatus (void);
DRESULT norRead (BYTE*, DWORD, BYTE);
#if _USE_WRITE == 1
DRESULT norWrite (const BYTE*, DWORD, BYTE);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
DRESULT norIoctl (BYTE, void*);
#endif  /* _USE_IOCTL == 1 */

Diskio_drvTypeDef  NOR_Driver =
{
  norInitialize,
  norStatus,
  norRead, 
#if  _USE_WRITE == 1
  norWrite,
#endif /* _USE_WRITE == 1 */
  
#if  _USE_IOCTL == 1
  norIoctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
FATFS NORFatFs;     /* File system objects logical drives */
FIL   NORFile;      /* File objects */
char  NORpath[4];   /* RAM disk and SD card logical drives paths */
FIL   HaHaFile;     /*The second file object*/


#ifdef MY_DEBUG
#define THE_BUFFER_SIZE     ((uint32_t)0x0000200)
uint16_t nor_TxBuffer[THE_BUFFER_SIZE];
uint16_t nor_RxBuffer[THE_BUFFER_SIZE];
uint8_t uIDStatus = 0,uEraseStatus = 0,uWriteStatus = 0,uReadStatus = 0,uInitStatus = 0;
#endif
/* Private functions ---------------------------------------------------------*/

static void printfFatStatus(FATFS *s)
  {
	printf("csize: %x\n\rdatabase:%ld\n\rdirbase:%ld\n\r",s->csize,s->database,s->dirbase);
	printf("drv: %x\n\rfatbase:%ld\n\rfree_clust:%ld\n\r",s->drv,s->fatbase,s->free_clust);
	printf("fsize: %ld\n\rfsi_flag:%x\n\rfs_type:%x\n\r",s->fsize,s->fsi_flag,s->fs_type);
    printf("id: %x\n\rlast_clust:%ld\n\rn_fatent:%ld\n\r",s->id,s->last_clust,s->n_fatent);
	printf("n_fats: %d\n\rn_rootdir:%d\n\rvolbase:%ld\n\r",s->n_fats,s->n_rootdir,s->volbase);
	printf("wflag: %x\n\rwinsect:%ld\n\r",s->wflag,s->winsect);	
  }
static void printfFILStatus(FIL *f)
  {
	printf("cltbl:%ld\n\r",*(f->cltbl) );
	printf("dir_sect:%ld\n\r",f->dir_sect);
	printf("dsect:%ld\n\r",f->dsect);
	printf("err:%x\n\r",f->err);
	printf("flag:%x\n\r",f->flag);
	printf("fptr:%ld\n\r",f->fptr);
	printf("fsize:%ld\n\r",f->fsize);
	printf("id:%d\n\r",f->id);
	printf("lockid:%d\n\r",f->lockid);
	printf("sclust:%ld\n\r",f->sclust);
  }


/******************************************************************/
//Function  :  FATfsDemo()
//Brief     :  link and register the nor device 
//Parameter :  None 
//retval    :  None
/******************************************************************/
void FATfsDemo()
  {
	FRESULT res1,res2;                                   /* FatFs function common result codes */
	uint32_t byteswritten1, byteswritten2;                /* File write counts */
	uint32_t bytesread1, bytesread2;                      /* File read counts */
	uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
	uint8_t hahatext[] = "HaHa DouDou!HaHa DouDou!HaHa Yuang";
	uint8_t rtext1[100], rtext2[100];                     /* File read buffers */

	
	/*##-1- Link the disk I/O drivers ##########################################*/
	if((FATFS_LinkDriver(&NOR_Driver, NORpath) == 0))
	{  
//	printfFatStatus(&NORFatFs);
/************************************************************************/
//test for the lower driver
/************************************************************************/

	uint32_t loop;
	disk_initialize(0);
/*Erase the whole chip to set all '0xFFFFs'*/
	if( BSP_NOR_Erase_Chip()!= 0)
	{
//	  uEraseStatus++; 
	}
#ifdef MY_DEBUG
	/*##-4- NOR memory read/write access######################################*/   
	/* Fill the buffer to write */
	Fill_Buffer(nor_TxBuffer, THE_BUFFER_SIZE, 0x5A5A);
//	Fill_Buffer(nor_aTxBuffer, BUFFER_SIZE, 0xC20F); 	
	/*Write data to the NOR memory*/
	for(loop = 0 ;loop < 100;loop++)
	{	
//	  if(BSP_NOR_WriteData((2 * loop * THE_BUFFER_SIZE), nor_TxBuffer, THE_BUFFER_SIZE) != 0)
//	  DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, BYTE count);
	  if(disk_write(0,(uint8_t *)nor_TxBuffer,loop,1) != 0)
	  {
		uWriteStatus++; 
	  }
	}
/*Read back data from the NOR memory */
	for(loop =0 ;loop< 1 ;loop++)
	{ 
//	if(BSP_NOR_ReadData(2*loop*THE_BUFFER_SIZE, nor_RxBuffer + loop*THE_BUFFER_SIZE, THE_BUFFER_SIZE) != 0)
	if( disk_read(0, (uint8_t *)nor_RxBuffer,0,1) != 0 )
	  {
		uReadStatus++; 
	  }
	}
	if(uInitStatus != 0)
	{
	  printf("There must be some problem at initialization!!\n\r");
	}
    if(uEraseStatus != 0)
    {
      printf("The erase operation has some problem!!\n\r");
    }
    if(uWriteStatus != 0)
    {
       printf("Write operation uncrect!!\n\r");
    }
    if(uReadStatus != 0)
    {
      printf("Read operation has met some problem\n\r");
    } 
#endif	
/************************************************************************/
//
/************************************************************************/ 
	  /*##-2- Register the file system object to the FatFs module ##############*/
	  res1 = f_mount(&NORFatFs, (TCHAR const*)NORpath, 0);
	  printf("\n\r After mount:\n\r");
	  printfFatStatus(&NORFatFs);
	  if( res1 != FR_OK)
      {
	  /* FatFs Initialization Error */
		//Error_Handler();
		printf("f_mount error: %d\n\r",res1);
	  }
	  else
	  {
		/*##-3- Create a FAT file system (format) on the logical drives ########*/
		/* WARNING: Formatting the uSD card will delete all content on the device */ 
		res1 = f_mkfs((TCHAR const*)NORpath,0, 0);
		printf("\n\r After mkfs:\n\r");
		printfFatStatus(&NORFatFs);      
		if( res1 != FR_OK )
		{
		  /* FatFs Format Error */
		  //Error_Handler();
		  printf("f_mkfs error: %d\n\r",res1);
		}
		else
		{
		  /*##-4- Create and Open new text file objects with write access ######*/
		  res1 = f_open(&NORFile, "0:STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE); 
		  res2 = f_open(&HaHaFile,"0:YUN.TXT",FA_CREATE_ALWAYS | FA_WRITE);
	  	  printf("\n\r After open:\n\r");
		  printfFatStatus(&NORFatFs);
		  if((res1 != FR_OK))
		  {
			/* 'STM32.TXT' file Open for write Error */
			//Error_Handler();
			printf("f_open error: %d\n\r",res1);
		  }
		  else
		  {
			/*##-5- Write data to the text files ###############################*/
			res1 = f_write(&NORFile, wtext, sizeof(wtext), (void *)&byteswritten1);
			res2 = f_write(&HaHaFile,hahatext,sizeof(hahatext),(void*)&byteswritten2);
			if((byteswritten1 == 0)|| (res1 != FR_OK) ||(byteswritten2 == 0)|| (res2 != FR_OK) )
			{
			  /* 'STM32.TXT' file write Error */
			  //Error_Handler();
			  printf("f_write error: %d\n\r",res1);
			}
			else
			{
			  /*##-6- Close the open text files ################################*/
			  f_close(&NORFile);
			  f_close(&HaHaFile);
			  /*##-7- Open the text files object with read access ##############*/
			  res1 = f_open(&NORFile, "0:STM32.TXT", FA_READ);
			  res2 = f_open(&HaHaFile,"0:YUN.TXT", FA_READ);
			  if((res1 != FR_OK) || res2!= FR_OK)
			  {
				/* 'STM32.TXT' file Open for read Error */
				//Error_Handler();
				printf("f_open error: %d\n\r",res1);
			  }
			  else
			  {
				/*##-8- Read data from the text files ##########################*/
				res1 = f_read(&NORFile, rtext1, sizeof(rtext1), (UINT*)&bytesread1);
				res2 = f_read(&HaHaFile,rtext2,sizeof(rtext2),(UINT*)&bytesread2); 
				if( (res1 != FR_OK) || (res2 != FR_OK) )
				{
				  /* 'STM32.TXT' file Read or EOF Error */
				  //Error_Handler();
				  printf("f_read error: %d\n\r",res1);
				}
				else
				{
				  printf("The readout data file string: %s\n\r",rtext1);
				  printf("The readout data file string: %s\n\r",rtext2);
				  printfFILStatus(&NORFile);
				  printfFILStatus(&HaHaFile);
				  /*##-9- Close the open text files ############################*/
				  f_close(&NORFile);
				  f_close(&HaHaFile);
				  /*##-10- Compare read data with the expected data ############*/
				  if( (bytesread1 != byteswritten1) ||(bytesread2 != byteswritten2) )
				  {                
					/*Read data is different from the expected data */
					//Error_Handler();
				  }
				  else
				  {
					/* Success of the demo: no error occurrence */
					//BSP_LED_On(LED1);
					printf("No error occured!!!\n\r");
				  }
				}
			  }
			}
		  }
		}
	  }
	}
 
	/*##-11- Unlink the disk I/O drivers #######################################*/
	FATFS_UnLinkDriver(NORpath);
	/* Infinite loop */
	while (1)
	{
	  Blink_LED_RUN();
	}
  }
/******************************************************************/
//Function  :	DSTATUS norInitialize(void)
//Brief     :   Initilize the NOR flash for the FAT file system
//Parameter :   None
//retval    :   DSTATUS :return the disk status 
/******************************************************************/
DSTATUS norInitialize(void)
  {
	Stat = STA_NOINIT;
  
	/* Configure the NOR device */
	if(BSP_NOR_Init() == HAL_OK)
	{
	  Stat &= ~STA_NOINIT;
	}
	return Stat;
  }

/******************************************************************/
//Function  :	DSTATUS norStatus(void)
//Brief     :   Get the status of the disk
//Parameter :   None
//retval    :   DSTATUS :return the disk status 
/******************************************************************/
DSTATUS norStatus(void)
  {
	Stat = STA_NOINIT;  
/* Configure the uSD device */
//	if(HAL_NOR_GetStatus() == HAL_OK)
//	{
	Stat &= ~STA_NOINIT;
//  }
	return Stat;
  }

/******************************************************************/
//Function  :	DRESULT norRead(BYTE *buff, DWORD sector, BYTE count)
//Brief     :   Read sector of the disk
//Parameter :   buff   : point a array to contain readout data
//              sector : the start sector number
//              count  : the amount of the sector
//P.S       :   a sector refers to a block that the FATfs defined
//retval    :   DSTATUS :return the read result 
/******************************************************************/
DRESULT norRead(BYTE *buff, DWORD sector, BYTE count)
  {
	DRESULT res = RES_OK;
//	BSP_NOR_ReadData(uint32_t uwStartAddress, uint16_t* pData, uint32_t uwDataSize);
	if( BSP_NOR_ReadData((uint32_t)(sector * BLOCK_SIZE),(uint16_t*)buff,(count*BLOCK_SIZE)/2) != HAL_OK )
	{
	  res = RES_ERROR;
	}
	return res;
  }

/******************************************************************/
//Function  :	DRESULT norWrite(const BYTE *buff, DWORD sector, BYTE count)
//Brief     :   Write the specific data into the sector of the disk
//Parameter :   buff   :point to the writen data array
//              sector :the start sector number
//              count  :the amount of sector for operation
//retval    :   DSTATUS :return the write result 
/******************************************************************/
#if _USE_WRITE == 1
DRESULT norWrite(const BYTE *buff, DWORD sector, BYTE count)
  {
	DRESULT res = RES_OK;
	uint32_t  block_num;
	uint32_t  sector_offset;
	uint32_t  loop;
	uint32_t  i;
	/*Calculate the Block number*/
	block_num = sector/(NOR_SECTOR_SIZE/BLOCK_SIZE);
	/*Calculate the sector offset*/
	sector_offset = sector%(NOR_SECTOR_SIZE/BLOCK_SIZE);
//	BSP_NOR_WriteData(uint32_t uwStartAddress, uint16_t* pData, uint32_t uwDataSize);
	/*Read out the data into my two-dimension array tem_sector*/
	for(loop= 0 ;loop<NOR_SECTOR_SIZE/BLOCK_SIZE;loop++)
	{
	  BSP_NOR_ReadData((uint32_t)(block_num*NOR_SECTOR_SIZE+loop*BLOCK_SIZE),(uint16_t*)&tem_sector[loop][0],BLOCK_SIZE/2);
	}
#ifdef MY_DEBUG_P
	printf("The first volume:\n\r");
	for(i=0 ;i< 5 ;i++) 
	  {
		for(loop =0 ;loop <20;loop++)
		{
		  printf("%02x ",tem_sector[i][loop]);
		}
		printf("\n\r");
	  }
#endif
	/*Erase the corresponding phisical sector area*/
	BSP_NOR_Erase_Block(block_num*NOR_SECTOR_SIZE);
	/*Fill the writen data into my tem_sector*/
	for (loop = 0; loop < (NOR_SECTOR_SIZE/BLOCK_SIZE); loop++)
	{
	  if((loop>=sector_offset)&&(loop < (sector_offset+count))) //if 
	  {
		//BSP_NOR_WriteData( (block_num*NOR_SECTOR_SIZE + loop*BLOCK_SIZE),(uint16_t *)buff,BLOCK_SIZE/2 );
		for(i=0;i<BLOCK_SIZE;i++)
		{
		   tem_sector[loop][i] =buff[i];
		}
		buff = (uint8_t*)buff + BLOCK_SIZE;
	  }
	}
	for (loop = 0; loop < (NOR_SECTOR_SIZE/BLOCK_SIZE); loop++)
	{	
	  BSP_NOR_WriteData( (block_num*NOR_SECTOR_SIZE + loop*BLOCK_SIZE),(uint16_t *)&tem_sector[loop][0], BLOCK_SIZE/2 );
	}
		//STM_Flash_WritePage((u8 *)(&BackUpData[IndexTmp][0]), WriteBlockNum*STM_FLASH_BLOCK_SIZE+IndexTmp, 1);
//	/*Write the data into the corresponding area*/
//	if( BSP_NOR_WriteData((uint32_t)(sector * BLOCK_SIZE),(uint16_t*)buff,(uint32_t)(count*BLOCK_SIZE/2) ) != HAL_OK)
//	{
//	  res = RES_ERROR;
//	}
	return res;
  }
#endif /* _USE_WRITE == 1 */

/******************************************************************/
//Function  :	DRESULT norIoctl(BYTE cmd, void *buff)
//Brief     :   Read sector of the disk
//Parameter :   None
//retval    :   DSTATUS :return the read result 
/******************************************************************/
#if _USE_IOCTL == 1
DRESULT norIoctl(BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;

  if (Stat & STA_NOINIT) return RES_NOTRDY;
  
  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC :
    res = RES_OK;
    break;
  
  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
	*(DWORD*)buff = NOR_DEVICE_SIZE / BLOCK_SIZE;
	res = RES_OK;
    break;
	
  /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :
    *(WORD*)buff = BLOCK_SIZE;
    res = RES_OK;
    break;
  
  /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE :
    *(DWORD*)buff = NOR_SECTOR_SIZE/BLOCK_SIZE;
	res = RES_OK;
    break;
  case CTRL_ERASE_SECTOR:
    /****Here need to do the erase operation****/
	/*******************************************/
	/*******************************************/
	/*******************************************/
	/*******************************************/
  default:
    res = RES_PARERR;
  }
  return res;
}
#endif /* _USE_IOCTL == 1 */
