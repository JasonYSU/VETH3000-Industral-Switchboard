/**
*************************************************************************
@File   : hal\incl\veth_marvell_88e6097f.h
@Author : Duke Lee
@Date   : 18-June-2014
@Brief  : Registers of Marvell_88e6097f and functions  
*************************************************************************
**/
/*PS:  Since the device can work in single-chip mode or multi-chip mode,
    before including this File choose the mode here by modifying the micro 
		define.
		if @single-chip mode, '#define SINGLE_CHIP_MODE';
		if @multi-chip mode , '#define MULTI_CHIP_MODE'.
*/  
#ifndef _MARVELL_88E6097F_H__
#define _MARVELL_88E6097F_H__

#include "veth_common.h"  //struct API_ROUGH_COUNTER_T and API_DETAIL_COUNTER_T also in
#include "uid_stp.h"
 
#define    SINGLE_CHIP_MODE
//#define  MULTI_CHIP_MODE

/**
Some defination for parameters used for just Marvell chip 88e6097f
**/
/*enum for the set_stats*/
typedef enum port_sta{
 SPEED100_FULLDPX,
 SPEED100_HALFDPX,
 SPEED10_FULLDPX,
 SPEED10_HALFDPX,
 SPEED1000_FULLDPX,
 AUTO,
 DISABLED
}PORT_STAS;




/**
Struct for the Statistics counter
**/
/*The statistic counters for the ingress and egress packets*/
typedef struct stats_counter_in  {
/*Ingress*/
/*set 1*/
	uint32_t InGoodOctetsLo;      //The sum of lengths of all the good packets recieved 
	uint32_t InGoodOctetsHi;
	uint32_t InBadOctets;         //The sum of lengths of all the bad packets recieved
      
/*set 2*/
	uint32_t InUnicast;           //The number of good recieved frames with a unicast desitination MAC address 
	uint32_t InBroadcast;         //The number of good recieved frames with a broadcast desitination MAC address
	uint32_t InMulticast;         //The number of good recieved frames with a multicast desitination MAC address
	uint32_t InPause;             //The number of good recieved frames with a pause desitination MAC address
/*set 3*/
	uint32_t InUndersize;         //Total frames recieved with its length less than 64,but the FCS is valid
	uint32_t InFragment; 	        //Total frames recieved with its length less than 64,and the FCS is invalid
	uint32_t InOversize;          //Total frames recieved with its length more than Maxsize,but the FCS is valid
	uint32_t InJabber;            //Total frames recieved with its length more than Maxsize,and the FCS is invalid
	uint32_t InRxErr;             //Total frames recieved with an error signal from the PHY
	uint32_t InFCSErr;              //Total frames recieved with CRC error noty counted in Infragment
	
/*set 4 */
	uint32_t Octets_64;           //Total frames of 64       bytes ,including error frames
	uint32_t Octets_65to127;      //Total frames of 64~127   bytes ,including error frames
	uint32_t Octets_128to255;     //Total frames of 128~255  bytes ,including error frames
	uint32_t Octets_256to511;     //Total frames of 256~511  bytes ,including error frames
	uint32_t Octets_512to1023;    //Total frames of 512~1023 bytes ,including error frames
	uint32_t Octets_1024toMAX;    //Total frames of 1024~max bytes ,including error frames
/*Egress*/
/*set 5 */
	uint32_t OutOctetsLo;         //The sum of lengths of all frames sent from this MAC
	uint32_t OutOctetsHi;
/*set 6 */
	uint32_t OutUnicast;          //The number of frames sent from this MAC with a Unicast   desitination MAC address 
	uint32_t OutBroadcast;        //The number of frames sent from this MAC with a Broadcast desitination MAC address
	uint32_t OutMulticast;        //The number of frames sent from this MAC with a Multicast desitination MAC address
	uint32_t OutPause;            //The number of flow control frames sent
/*set 7*/
	uint32_t Deferred;            //Total number of successfully transmitted frames exactly one collision but a delayed for the media is busy,half duplex only
	uint32_t Collisions;          //Total number of collision events seen by the MAC ,half duplex only
	uint32_t Single;              //Total number of successfully transmitted frames exactly one collision,half duplex only
	uint32_t Multiple;            //Total number of successfully transmitted frames more than one collision   
	uint32_t Excessive;           //The number frames dropped in the transmit MAC because the frames experienced 16 consecutive collision
	uint32_t Late;                //The number of times a collision is detected later than 512 bits-times into the transmission of a frame
	uint32_t OutFCSErr;           //The number of frames transmited with an invalid FCS

}STATS_COUNTER;

/**

statistic counter registers

**/
/*set 1*/
#define  REG_INGOODLO      (uint8_t)0x00
#define  REG_INGOODHI      (uint8_t)0x01
#define  REG_INBAD         (uint8_t)0x02
/*set 2*/
#define  REG_INUNICAST     (uint8_t)0x04
#define  REG_INBROADCAST   (uint8_t)0x06
#define  REG_INMULTICAST   (uint8_t)0x07
#define  REG_INPAUSE       (uint8_t)0x16
/*set 3*/
#define  REG_INUNDERSIZE   (uint8_t)0x18
#define  REG_INFRAGMENT    (uint8_t)0x19
#define  REG_INOVERSIZE    (uint8_t)0x1a
#define  REG_INJABBER      (uint8_t)0x1b
#define  REG_INRXERR       (uint8_t)0x1c
#define  REG_INFCSERR      (uint8_t)0x1d

/*set 4*/
#define  REG_OCTET64       (uint8_t)0x08
#define  REG_OCTET65_127   (uint8_t)0x09
#define  REG_OCTET128_255  (uint8_t)0x0a
#define  REG_OCTET256_511  (uint8_t)0x0b
#define  REG_OCTET512_1023 (uint8_t)0x0c
#define  REG_OCTET1024_MAX (uint8_t)0x0d

/*set 5*/
#define  REG_OUTOCTETLO    (uint8_t)0x0e
#define  REG_OUTOCTETHI    (uint8_t)0x0f

/*set 6*/
#define  REG_OUTUNICAST    (uint8_t)0x10
#define  REG_OUTBROADCAST  (uint8_t)0x13
#define  REG_OUTMULTICAST  (uint8_t)0x12
#define  REG_OUTPAUSE      (uint8_t)0x15

/*set 7*/
#define  REG_DEFERRED      (uint8_t)0x05
#define  REG_COLLISION     (uint8_t)0x1e
#define  REG_SINGLE        (uint8_t)0x14
#define  REG_MULTIPLE      (uint8_t)0x17
#define  REG_EXCESSIVE     (uint8_t)0x11
#define  REG_LATE          (uint8_t)0x1f
#define  REG_OUTFCSERR     (uint8_t)0x03

/**

@Multi_chip MODE 

**/
#ifdef  MULTI_CHIP_MODE0
/*********************************************************/
//SMI address for @multi-chip mode
/*********************************************************/
/*Define the addresses for the '5-bit PHY register' area*/


#define SMI_COMMOND_REG  0x00
#define SMI_DATA_REG     0x01




#endif


/**

@Single-chip MODE

**/

/*********************************************************/
//@description :The SMI device addresses
/*********************************************************/
/*Define the PHY address for each PHY*/
#define SWITCH_PHY_0   0x00 
#define SWITCH_PHY_1   0x01
#define SWITCH_PHY_2   0x02
#define SWITCH_PHY_3   0x03
#define SWITCH_PHY_4   0x04
#define SWITCH_PHY_5   0x05
#define SWITCH_PHY_6   0x06
#define SWITCH_PHY_7   0x07

#define SWITCH_PHY_8   0x08
#define SWITCH_PHY_9   0x09
#define SWITCH_PHY_10  0x0A

/*Define the port address of each port for SMI*/
#define PORT_0         0x10
#define PORT_1         0x11
#define PORT_2         0x12
#define PORT_3         0x13
#define PORT_4         0x14
#define PORT_5         0x15
#define PORT_6         0x16
#define PORT_7         0x17

#define PORT_8         0x18
#define PORT_9         0x19
#define PORT_10        0x1A

#define PORT(x)        PORT_##x

/*Define the global address for SMI*/
#define GLOBAL_1_ADDR  0x1B
#define GLOBAL_2_ADDR  0x1C

/*********************************************************/
//@description: The register addresses for each PHY
/*********************************************************/

#define REG_PHY_CONTROL              0x00  /*@ref REG_PHY_CONTROL*/
#define REG_PHY_STATUS               0x01
#define REG_PHY_IDENTIFIER1          0x02
#define REG_PHY_IDENTIFIER2          0x03
#define REG_AUTO_NEG_ADVERTISE       0x04 /*@ref REG_AUTO_NEG_ADVERTISE*/
#define REG_LINK_PARTNER_ABILITY     0x05
#define REG_AUTO_NEG_EXPANSION       0x06
#define REG_NEXT_PAGE_TRANSMIT       0x07
#define REG_LINK_PARTNER_NEXT_PAGE   0x08

/*These are for PHY8/9/10 only*/
#define REG_MASTER_OR_SLAVE_CONTROL  0x09
#define REG_MASTER_OR_SLAVE_STATUS   0x0A
#define REG_EXTENDED_STATUS          0x0F

/*These are for PHY0~7 only */
#define REG_PHY_SPECIFIC_CONTROL1    0x10   /*@ref REG_PHY_SPECIFIC_CONTROL1*/
#define REG_PHY_SPECIFIC_STATUS      0x11   /*@ref REG_PHY_SPECIFIC_STATUS bits*/
#define REG_PHY_INTERRUPT_ENBALE     0x12
#define REG_PHY_INTERRUPT_STATUS     0x13
#define REG_INTERRUPT_PORT_SUMMARY   0x14
#define REG_RECIEVE_ERROR_COUNTER    0x15
#define REG_LED_PARALLEL_SELECT      0x16
#define REG_LED_STREAM_SELECT        0x17
#define REG_LED_CONTROL              0x18
#define REG_LED_OVERRIDE             0x19
#define REG_PHY_SPECIFIC_CONTROL2    0x1C  /*@ref REG_PHY_SPECIFIC_CONTROL2*/


/*********************************************************/
//@description: The register addresses for each port
/*********************************************************/

#define REG_PORT_STATUS              0x00       /*Show  the shatus of the specific port @ref REG_PORT_STATUS*/
#define REG_PCS_CONTROL              0x01       /*Set the port to the specific port status @ref REG_PCS_CONTROL*/ 
#define REG_JAMMING_CONTROL          0x02
#define REG_PRODUCT_IDENTIFIER       0x03
#define REG_PORT_CONTROL             0x04       /*Port control register controls the packets ingress or egress the port,also*/
                                                /*What's more important, change the port state for RSTP on bits[1:0] @ref REG_PORT_CONTROL*/                                           
#define REG_PORT_CONTROL1            0x05       //@ref REG_PORT_CONTROL1
#define REG_PORTBASE_VLAN_MAP        0x06
#define REG_PORT_DEFAULT_VLANID      0x07       //@ref REG_PORT_DEFAULT_VLANID
#define REG_PORT_CONTROL2            0x08       //@ref REG_PORT_CONTROL2
#define REG_EGRESS_RATE_CONTROL      0x09
#define REG_EGRESS_RATE_CONTROL2     0x0A
#define REG_PORT_ASSOCIATION_VECTOR  0x0B      //@ref  REG_PORT_ASSOCIATION_VECTOR
#define REG_PORT_ATU_CONTROL         0x0C
#define REG_PRIORITY_OVERRIDE_PORT   0x0D
#define REG_POLICY_CONTROL           0x0E
#define REG_PORTE_TYPE               0x0F
#define REG_INDISCARDS_nFRAME_LO     0x10     //nFRAMES MEANS FRAME COUNTER
#define REG_INDISCARDS_nFRAME_HI     0x11
#define REG_INFILTERED_nFRAME        0x12
#define REG_OUTFILTERED_nFRAME       0x13
#define REG_TAG_REMAP3_0             0x18
#define REG_TAG_REMAP7_4             0x19
#define REG_QUEUE_COUNTERS           0x1B


/*********************************************************/
//@description: The register addresses for Global 1
/*********************************************************/
#define REG_GLOBAL_STATUS           0x00
#define REG_ATU_FID                 0x01   // @ref REG_ATU_FID
#define REG_VTU_FID                 0x02   // @ref REG_VTU_FID
#define REG_VTU_SID                 0x03
#define REG_GLOBAL_CONTROL          0x04   
#define REG_VTU_OPERATION           0x05   // @ref REG_VTU_OPERATION
#define REG_VTU_VID                 0x06   // @ref REG_VTU_VID
#define REG_VTU_DATA_PORTS3_0       0x07   // @ref REG_VTU_DATA_PORTS3_0
#define REG_VTU_DATA_PORTS7_4       0x08   // @ref REG_VTU_DATA_PORTS7_4
#define REG_VTU_DATA_PORTSA_8       0x09   // @ref REG_VTU_DATA_PORTSA_8
#define REG_ATU_CONTROL             0x0A   //These registers are for MAC table operations  @ref REG_ATU_CONTROL
#define REG_ATU_OPERATION           0x0B   //
#define REG_ATU_DATA                0x0C   //
#define REG_ATU_MAC0                0x0D   //
#define REG_ATU_MAC1                0x0E   //
#define REG_ATU_MAC2                0x0F   //
#define REG_IP_PRI0                 0x10
#define REG_IP_PRI1                 0x11
#define REG_IP_PRI2                 0x12
#define REG_IP_PRI3                 0x13
#define REG_IP_PRI4                 0x14
#define REG_IP_PRI5                 0x15
#define REG_IP_PRI6                 0x16
#define REG_IP_PRI7                 0x17
#define REG_IEEE_TAG_PRIORITY       0x18
#define REG_MONITOR_DESTINATIONS    0x1A    //@ref REG_MONITOR_DESTINATION
#define REG_FREE_POOL               0x1B
#define REG_GLOBAL_CONTROL2         0x1C    //@ref REG_GLOBAL_CONTROL2 ,for setting the target device for the DSA 
#define REG_STATS_OPERATION         0x1D    //@ref REG_STATS_OPERATION  
#define REG_STATS_DATA_BYTES3_2     0x1E    
#define REG_STATS_DATA_BYTES1_0     0x1F



/*********************************************************/
//@description: The register addresses for Global 2
/*********************************************************/      
#define REG_INTERRUPT_SOURCE        0x00
#define REG_INTERRUPT_MASK          0x01
#define REG_MGMT_ENABLES_2X         0x02     // These two registers use to forward the frames to CPU
#define REG_MGMT_ENBALES_0X         0x03     // 
#define REG_FLOW_CONTROL_DELAYS     0x04
#define REG_MANAGEMENT              0x05     
#define REG_DEVICE_MAPPING          0x06
#define REG_TRUNK_MASK              0x07     //@ref REG_TRUNK_MASK
#define REG_TRUNK_MEMBERS           0x08     //@ref REG_TRUNK_MEMBER
#define REG_INGRESS_RATE_COMMAND    0x09
#define REG_INGRESS_RATE_DATA       0x0A
#define REG_CROSSCHIP_PORTVLAN_ADDR 0x0B
#define REG_CROSSCHIP_PORTVLAN_DATA 0x0C
#define REG_SWITCH_MAC              0x0D     //The switch store its MAC address through this register @ref REG_SWITCH_MAC
#define REG_ATU_STATS               0x0E     //can't understand
#define REG_PRIORITY_OVERRIDE_G2    0x0F     //can't understand
#define REG_SMI_PHY_COMMAND         0x18     //@ref REG_SMI_PHY_CONMMAND
#define REG_SMI_PHY_DATA            0x19     //@ref REG_SMI_PHY_DATA
#define REG_WATCHDOG_CONTROL        0x1B
#define REG_QOS_WEIGHTS             0x1C
#define REG_SDET_POLARITY           0x1D
 



/**
@ref REG_PORT_CONTROL
**/

/*The EGRESS mode with REG_PORT_CONTROL[13:12] */
#define PORT_UNMODIFIED_MODE    0x0000     
#define PORT_UNTAGGED_MODE      0x1000
#define PORT_TAGGED_MODE        0x2000
#define PORT_RESERVED_MODE      0x3000
#define PORT_MODE_RESET         0xCFFF


/*The Frame mode with REG_PORT_CONTROL[9:8] */ // effective along with the bit [13:12],when PORT_FRAME_DSAMODE,THE[13:12] must set '00' 
#define PORT_FRAME_NOMMODE      0x0000
#define PORT_FRAME_DSAMODE      0x0100
#define PORT_FRAME_PROVIDERMODE 0x0200
#define PORT_FRAME_ETHDSAMODE   0x0300
#define PORT_FRAME_RESET        0xFCFF


/*The four port state RSTP with REG_PORT_CONTROL[1:0]*/
#define PORT_STATE_DISABLED     0x0000
#define PORT_STATE_BLOCKING     0x0001
#define PORT_STATE_LEARNING     0x0002
#define PORT_STATE_FORWARDING   0x0003
#define PORT_STATE_RESET        0xFFFC

/**/

/**
@ref REG_PCS_CONTROL
**/
#define PORT_FC_VALUE             0x0080   //bit 7    : '1' for flow control enabled ,'0' for disabled 
#define PORT_FC_FORCED            0x0040   //bit 6    : '1' for enabled FC value     ,'0' for disabled
#define PORT_LINK_VALUE           0x0020   //bit 5    : '1' for link up  ,'0' for link down 
#define PORT_LINK_FORCED          0x0010   //bit 4    : '1' for force enable
#define PORT_DUPLEX_VALUE         0x0080   //bit 3    : '1' for full duplex value,'0' for half
#define PORT_DUPLEX_FORCED        0x0040   //bit 2    : '1' for start duplex force operation

/*bit [1:0]*/
#define PORT_SPEED_10M          0x0000
#define PORT_SPEED_100M         0x0001
#define PORT_SPEED_1000M        0x0002
#define PORT_SPEED_NORMAL       0x0003
#define PORT_SPEED_RESET        0xFFFC
/**
@ref REG_PORT_STATUS 
**/
#define PORT_PAUSE_EN           0x8000   //bit 15   : '1' for flow control enabled
#define PORT_LINK_ON            0x0800   //bit 11   : '1' for link up '0' for down
#define PORT_DUPLEX             0x0400   //bit 10   : '1' for full duplex and '0'for half
#define PORT_SPEED              0x0300   //bits 9:8 : '00' for 10M,'01' for 100M,'10'for 1000M
#define PORT_FLOWCONTROL        0x0010   //bit 4    : 
/**
@ref REG_PHY_SPECIFIC_CONTROL1
**/
#define PHY_SCRAMBLER_DISABLE   0x0200
#define PHY_DTE_DETECT          0x8000
#define PHY_EXTDISTANSE         0x0080
#define PHY_DISNLPGEN           0x0800
#define PHY_FORCE_LINK          0x0020
#define PHY_AUTOPOL             0x0001

/**
@ref REG_PHY_SPECIFIC_STATUS bits
**/
#define PHY_DTE_STATUS        0x8000    //bit 15
#define PHY_RES_SPEED         0x4000    //bit 14
#define PHY_RES_DUPLEX        0x2000    //bit 13
#define PHY_RCV_PAGE          0x1000    //bit 12
#define PHY_RESOLVED          0x0800    //bit 11  PS:if AUTO_NEG disabled ,this bit is ignored 
#define PHY_LINKON_STATUS     0x0400    //bit 10 
#define PHY_MDI_CROSS_STATUS  0x0040    //bit 6 
#define PHY_SLEEP_STATUS      0x0010    //bit 4
#define PHY_PRI_STATUS        0x0002    //bit 2
#define PHY_JABBER_STATUS     0x0001    //bit 0 

/**
@ref REG_PORT_CONTROL1
**/
#define PORT_TRUNK_BIT        0x4000    //bit[14]
#define PORT_TRUNK_ID         0x0f00    //bit[11:8]


/**
@ref REG_PORT_CONTROL2  
**/
#define PORT_VLAN_RESET          0x0c00    //bits[11:10]
#define PORT_VLAN_MODE_DISABLED  0x0000
#define PORT_VLAN_MODE_FALLBACK  0x0400
#define PORT_VLAN_MODE_CHECK     0x0800
#define PORT_VLAN_MODE_SECURE    0x0c00

#define PORT_DISCARD_TAGED      0x0200       //bit 9
#define PORT_DISCARD_UNTAG       0x0100       //bit 8 


/**
@ref REG_PORT_DEFAULT_VLANID
**/
#define PORT_DEFAULT_VID   0x1fff //bit[11:0] 
#define PORT_FORCE_VID     0x1000 //bit 12
/**
@ref  REG_PORT_ASSOCIATION_VECTOR
**/
#define PORT_HOLD_AT1          ((uint16_t)0x8000)    //bit 15
#define PORT_INTON_AGEOUT      ((uint16_t)0x4000)    //bit 14
#define PORT_LOCKED_PORT       ((uint16_t)0x2000)    //bit 13
#define PORT_IGNORE_WRONGDATA  ((uint16_t)0x1000)    //bit 12
#define PORT_REFRESHED_ALOCKED ((uint16_t)0x0800)    //bit 11
#define PORT_PAV               ((uint16_t)0x07ff)    //bits [10:0]   

/*****Global registers*******/
/**
@ref REG_SWITCH_MAC  bits
**/ 
#define GLOBAL_MAC_UPT          0x8000  //bit 15
#define GLOBAL_MAC_POINTER_0    0x0800  //bit 11
#define GLOBAL_MAC_POINTER_1    0x0400  //bit 10
#define GLOBAL_MAC_POINTER_2    0x0200  //bit 9
#define GLOBAL_MAC_POINTER_3    0x0100  //bit 8

#define GLOBAL_MAC_POINTER_SET  GLOBAL_MAC_POINTER_0 | GLOBAL_MAC_POINTER_1 | GLOBAL_MAC_POINTER_2 | GLOBAL_MAC_POINTER_3
                                 
                                 
                                

/**
@ref REG_MONITOR_DESTINATION   
**/
#define GLOBAL_CPU_DEST         0x00f0        //bits[7:4]

/**
@ref REG_GLOBAL_CONTROL2      
**/
#define GLOBAL_DEVICE_NUMBER   0x001f         //bits[4:0]


/***************************************************************************************************/
//ATU operation Codes
/***************************************************************************************************/

/**
@ref REG_ATU_FID
**/
#define ATU_FID_SINGLE         0x00
#define ATU_FID_BITS           0x0fff

/**
@ref REG_ATU_CONTROL
**/
#define  ATU_AGE_TIME           0x0FF0

/**
@ref  REG_ATU_OPERATION
**/
#define ATU_FLUSH_MOVE_ALL      0x9000
#define ATU_FLUSH_MOVE_UNSTA    0xA000
#define ATU_LOAD_PURGE          0xB000
#define ATU_GET_NEXT            0xC000
#define ATU_FLUSH_MOVE_INFID    0xD000
#define ATU_FLUSH_MOVE_UNSTAFID 0xE000
#define ATU_GET_CLAER_VIOL      0xF000

#define ATU_MAC_BUSY            0x8000    //bit15  '1'means operation is on going or an operation will start
#define ATU_MAC_PRI_3           0x0400    //bit10
#define ATU_MAC_PRI_2           0x0200    //bit9
#define ATU_MAC_PRI_1           0x0100    //bit8
#define ATU_MAC_PRI_0           0x0080    //bit7
#define ATU_AGEOUT              0x0040    //bit6
#define ATU_MEMBER              0x0020     
#define ATU_MISS                0x0010
#define ATU_ATUFULL             0x0008    //bit4

#define ATU_MAC_PRI_SET        ATU_MAC_PRI_3 | ATU_MAC_PRI_2 |     \
                               ATU_MAC_PRI_1 | ATU_MAC_PRI_0    

#define ATU_OPER_SET           ATU_GET_CLAER_VIOL       //used to clear the operation bits


/**
@ref  REG_ATU_DATA
**/
#define ATU_TRUNK_ORNOT        0x8000    //bit 15
#define ATU_DPV_10             0x4000
#define ATU_DPV_9              0x2000
#define ATU_DPV_8              0x1000
#define ATU_DPV_7              0x0800
#define ATU_DPV_6              0x0400
#define ATU_DPV_5              0x0200
#define ATU_DPV_4              0x0100
#define ATU_DPV_3              0x0080
#define ATU_DPV_2              0x0040
#define ATU_DPV_1              0x0020
#define ATU_DPV_0              0x0010
#define ATU_ENTRY_STATE_3      0x0008
#define ATU_ENTRY_STATE_2      0x0004
#define ATU_ENTRY_STATE_1      0x0002
#define ATU_ENTRY_STATE_0      0x0001    //bit 0

#define ATU_DPV_SET            ATU_DPV_0 | ATU_DPV_1 | ATU_DPV_2 | ATU_DPV_3 | ATU_DPV_4 | ATU_DPV_5 | ATU_DPV_6 | ATU_DPV_7 | ATU_DPV_8 | ATU_DPV_9 | ATU_DPV_10

#define ATU_ENTRY_STATE_SET    ATU_ENTRY_STATE_3 | ATU_ENTRY_STATE_2 | ATU_ENTRY_STATE_1 | ATU_ENTRY_STATE_0


/**
@ref REG_PHY_CONTROL
**/

#define PHY_SW_RESET                    ((uint16_t)0x8000)  /*!< PHY Reset */
#define PHY_LOOP_BACK                   ((uint16_t)0x4000)  /*!< Select loop-back mode */
#define PHY_FULLDUPLEX_100M             ((uint16_t)0x2100)  /*!< Set the full-duplex mode at 100 Mb/s */
#define PHY_HALFDUPLEX_100M             ((uint16_t)0x2000)  /*!< Set the half-duplex mode at 100 Mb/s */
#define PHY_FULLDUPLEX_10M              ((uint16_t)0x0100)  /*!< Set the full-duplex mode at 10 Mb/s  */
#define PHY_HALFDUPLEX_10M              ((uint16_t)0x0000)  /*!< Set the half-duplex mode at 10 Mb/s  */
#define PHY_AUTONEGOTIATION             ((uint16_t)0x1000)  /*!< Enable auto-negotiation function     */
#define PHY_RESTART_AUTONEGOTIATION     ((uint16_t)0x0200)  /*!< Restart auto-negotiation function    */
#define PHY_POWERDOWN                   ((uint16_t)0x0800)  /*!< Select the power down mode           */
#define PHY_ISOLATE                     ((uint16_t)0x0400)  /*!< Isolate PHY from MII                 */

#define PHY_AUTONEGO_COMPLETE           ((uint16_t)0x0020)  /*!< Auto-Negotiation process completed   */
#define PHY_LINKED_STATUS               ((uint16_t)0x0004)  /*!< Valid link established               */
#define PHY_JABBER_DETECTION            ((uint16_t)0x0002)  /*!< Jabber condition detected            */

/**
@ref REG_AUTO_NEG_ADVERTISE
**/
#define PHY_ANEG_PAUSE                  ((uint16_t)0x0400) /*Enable the abiliaty of flow control*/ 
#define PHY_ANEG_100FDX                 ((uint16_t)0x0100) 
#define PHY_ANEG_100HALF                ((uint16_t)0x0080)
#define PHY_ANEG_10FDX                  ((uint16_t)0x0040)
#define PHY_ANEG_10HALF                 ((uint16_t)0x0020)
#define PHY_ANEG_ALL                    ((uint16_t)0x01E0)  
/*@ref REG_PHY_SPECIFIC_CONTROL2*/
#define PHY_SOFTMEDIA_SELECT            ((uint16_t)0x0008)      

/***************************************************************************************************/
//VTU operation Codes
/***************************************************************************************************/
/**
@ref REG_VTU_FID
**/
#define VTU_FID_SET                    ((uint16_t)0x0fff)      //bits[11:0] : VTU_FID 

/**
@ref REG_VTU_VID 
**/
#define VTU_VALID                      ((uint16_t)0x1000)       //bit12      : VTU_ENTRY_VALID bit
#define VTU_VID_SET                    ((uint16_t)0x0fff)       //bits[11:0] : VTU_VID bits

/**
@ref REG_VTU_OPERATION
**/
#define VTU_BUSY                       ((uint16_t) 0x8000)     //bit15 :VTU_busy bit
/*bit [14:12]*/
#define VTU_OPERATE_RESET              ((uint16_t) 0x7000)     //this macro used to reset the specific bits
#define VTU_FLUSH_ALL                  ((uint16_t) 0x1000)     
#define VTU_LOAD_PURGE                 ((uint16_t) 0x3000)
#define VTU_GET_NEXT                   ((uint16_t) 0x4000)
#define STU_LOAD_PURGE                 ((uint16_t) 0x5000)
#define STU_GET_NEXT                   ((uint16_t) 0x6000)
#define STU_GET_CLEAR_VIO              ((uint16_t) 0x7000)




/**/
/**reserved***/
///**
//@ref REG_VTU_DATA_PORTS3_0
//**/
//#define VTU_DATA0_RESET                ((uint16_t) 0x0000)

//#define VTU_PORT3_UNMOD                ((uint16_t) 0x0000)   //bit[13:12]
//#define VTU_PORT3_UNTAG                ((uint16_t) 0x1000)
//#define VTU_PORT3_TAGED                ((uint16_t) 0x2000)
//#define VTU_PORT3_NOMEM                ((uint16_t) 0x3000)

//#define VTU_PORT2_UNMOD                ((uint16_t) 0x0000)   //bit[9:8]
//#define VTU_PORT2_UNTAG                ((uint16_t) 0x0100)
//#define VTU_PORT2_TAGED                ((uint16_t) 0x0200)
//#define VTU_PORT2_NOMEM                ((uint16_t) 0x0300)

//#define VTU_PORT1_UNMOD                ((uint16_t) 0x0000)   //bit[5:4]
//#define VTU_PORT1_UNTAG                ((uint16_t) 0x0010)
//#define VTU_PORT1_TAGED                ((uint16_t) 0x0020)
//#define VTU_PORT1_NOMEM                ((uint16_t) 0x0030)

//#define VTU_PORT0_UNMOD                ((uint16_t) 0x0000)   //bit[1:0]
//#define VTU_PORT0_UNTAG                ((uint16_t) 0x0001)
//#define VTU_PORT0_TAGED                ((uint16_t) 0x0002)
//#define VTU_PORT0_NOMEM                ((uint16_t) 0x0003)


///**
//@ref REG_VTU_DATA_PORTS7_4
//**/
//#define VTU_DATA1_RESET                ((uint16_t) 0x0000)

//#define VTU_PORT7_UNMOD                ((uint16_t) 0x0000)   //bit[13:12]
//#define VTU_PORT7_UNTAG                ((uint16_t) 0x1000)
//#define VTU_PORT7_TAGED                ((uint16_t) 0x2000)
//#define VTU_PORT7_NOMEM                ((uint16_t) 0x3000)

//#define VTU_PORT6_UNMOD                ((uint16_t) 0x0000)   //bit[9:8]
//#define VTU_PORT6_UNTAG                ((uint16_t) 0x0100)
//#define VTU_PORT6_TAGED                ((uint16_t) 0x0200)
//#define VTU_PORT6_NOMEM                ((uint16_t) 0x0300)

//#define VTU_PORT5_UNMOD                ((uint16_t) 0x0000)   //bit[5:4]
//#define VTU_PORT5_UNTAG                ((uint16_t) 0x0010)
//#define VTU_PORT5_TAGED                ((uint16_t) 0x0020)
//#define VTU_PORT5_NOMEM                ((uint16_t) 0x0030)

//#define VTU_PORT4_UNMOD                ((uint16_t) 0x0000)   //bit[1:0]
//#define VTU_PORT4_UNTAG                ((uint16_t) 0x0001)
//#define VTU_PORT4_TAGED                ((uint16_t) 0x0002)
//#define VTU_PORT4_NOMEM                ((uint16_t) 0x0003)


///**
//@ref REG_VTU_DATA_PORTSA_8
//**/
//#define VTU_DATA2_RESET                ((uint16_t) 0x0000)

//#define VTU_PORTA_UNMOD                ((uint16_t) 0x0000)   //bit[9:8]
//#define VTU_PORTA_UNTAG                ((uint16_t) 0x0100)
//#define VTU_PORTA_TAGED                ((uint16_t) 0x0200)
//#define VTU_PORTA_NOMEM                ((uint16_t) 0x0300)

//#define VTU_PORT9_UNMOD                ((uint16_t) 0x0000)   //bit[5:4]
//#define VTU_PORT9_UNTAG                ((uint16_t) 0x0010)
//#define VTU_PORT9_TAGED                ((uint16_t) 0x0020)
//#define VTU_PORT9_NOMEM                ((uint16_t) 0x0030)

//#define VTU_PORT8_UNMOD                ((uint16_t) 0x0000)   //bit[1:0]
//#define VTU_PORT8_UNTAG                ((uint16_t) 0x0001)
//#define VTU_PORT8_TAGED                ((uint16_t) 0x0002)
//#define VTU_PORT8_NOMEM                ((uint16_t) 0x0003)

/*PORT tagged or not or if the port is a member of the VLAN*/
#define VTU_DATA_RESET                ((uint16_t) 0xffff) 

#define VTU_PORT_UNMOD                ((uint16_t) 0xcfff)   
#define VTU_PORT_UNTAG                ((uint16_t) 0xdfff)
#define VTU_PORT_TAGED                ((uint16_t) 0xefff)
#define VTU_PORT_NOMEM                ((uint16_t) 0xffff)

/**
@ref REG_SMI_PHY_CONMMAND
**/
#define SMI_BUSY        ((uint16_t)0x8000)
#define SMI_MODE        ((uint16_t)0x1000)
/**
@ref REG_STATS_OPERATION
**/
#define STATS_BUSY                        ((uint16_t)0x8000)

#define STATS_OP_RESET                    ((uint16_t)0x7000)    //bits[14:12]
#define STATS_FLUSH_ALL                   ((uint16_t)0x1000)
#define STATS_FLUSH_PORT                  ((uint16_t)0x2000)
#define STATS_READ_A_COUNTER              ((uint16_t)0x4000)
#define STATS_CAPTURE_PORT_COUNTERS       ((uint16_t)0x5000) 

#define STATS_PORT_BITS                   ((uint16_t)0x01e0)    //bits[8:5]
#define STATS_PTR                         ((uint16_t)0x001f)    //bits[4:0]
#define STATS_HISTOGRAM_MODE              ((uint16_t)0x0c00)    //bits[11:10]




/**
@ref REG_TRUNK_MASK
**/
#define  TRUNK_TABLE_UPDATE               ((uint16_t)0x8000)   //bit[15]
#define  TRUNK_TABLE_MASKNUM              ((uint16_t)0x7000)   //bit[14:12]
#define  TRUNK_TABLE_MASK_HASH            ((uint16_t)0x0800)   //bit[11] 
#define  TRUNK_TABLE_MASK_ENTRY           ((uint16_t)0x07ff)   //bit[10:0]

/**
@ref REG_TRUNK_MEMBERS
**/
#define  TRUNK_UPDATE                     ((uint16_t)0x8000)   //bit[15]
#define  TRUNK_ID                         ((uint16_t)0x7800)   //bit[14:11]
#define  TRUNK_MEMBERS                    ((uint16_t)0x07ff)   //bit[10:0]

/*PHY_STA_SET are for clearing the corresponding bits when set port stas*/
#define PHY_STA_SET                   ((uint16_t)PHY_FULLDUPLEX_100M | PHY_AUTONEGOTIATION)


/*Functions for the Read operation for PHY*/
HAL_StatusTypeDef READ_PHYREGISTER(uint16_t phy_addr,uint16_t reg_addr,uint16_t *reg_value);
HAL_StatusTypeDef WRITE_PHYREGISTER(uint16_t phy_addr,uint16_t reg_addr,uint16_t reg_value);

/*Functions for the ATU/MAC table operations*/
void set_aging_time(uint8_t aging_time);
uint16_t get_aging_time(void);
void Marvell_ATU_operate(MAC_OPER oprt,MAC_ENTRY_T * entry,uint8_t move);
void Read_ATU_entry(MAC_ENTRY_T* entry);
void Load_Purge_ATUentry(MAC_ENTRY_T* entry);
uint8_t Flush_Move_ATUentry(ATU_OPER_MODE mold,MAC_OPER option,uint16_t move,uint16_t fid);
void Set_Manual_learning_mode(uint16_t port_vector);
uint16_t Get_Manual_learning_mode(void);

/*Functions for the VTU/VLAN table operations*/
void Read_VTU_entry(VLAN_ENTRY_T *entry);
void LOAD_PURGE_VTU(VLAN_ENTRY_T *entry);
void Flush_VTU_entry(void);
uint8_t Creat_Delet_VLAN(uint16_t vlan, uint8_t *ports,uint8_t creat);
void Set_PORT_VLAN_VECTOR(uint16_t port_vector);
uint8_t Set_VLAN_Port_Type(uint8_t type,int portNo);
uint8_t Set_VLAN_ID(uint16_t vlan_id,int portNo);
void transferVLANMember(uint16_t *data,uint8_t *p_data,uint8_t loop);
/*****************************************************************************/
//Function  : Set_VLAN_Port_Type(uint8_t type,int portNo)
//Brief     : 设置vlan端口类型
//Parameter : type : security level
//	          portNo  : The specific port to be set
//retval    : None
/*****************************************************************************/	
uint8_t Set_VLAN_port_Type(SECURITY_LEVEL_T type,int portNo);
/*****************************************************************************/
//Function  : Set_VLAN_ingress_Type(TAG_T tag,int portNo)
//Brief     : 设置vlan过滤类型
//Parameter : 
//retval    : None
/*****************************************************************************/	
uint8_t Set_VLAN_ingress_Type(TAG_T tag,int portNo);
/*****************************************************************************/
//Function  : Set_VLAN_egress_Type(TAG_T tag,int portNo)
//Brief     : 设置输出frame类型
//Parameter : 
//retval    : None
/*****************************************************************************/	
void Set_VLAN_egress_Type( EGRESS_TYPE_T tag , int portNo,uint16_t vlan );

/*Functions for the operation of statistics counters*/
void Clear_ALL_COUNTER(uint8_t port);
void Read_PORT_COUNTERs(STATS_COUNTER* stats,uint8_t port);
void Change_COUNTER_mode(uint8_t histogram);
uint8_t getCounterMode(void);

/*Functions for the trunk operations*/
MYERROR_INFO 
Set_Trunk_port(uint16_t port_vector,uint16_t trunk_id);
MYERROR_INFO
Set_TRUNK_TABLE(uint16_t port_vector,uint16_t trunk_id);
MYERROR_INFO
Clear_A_trunk(uint16_t port_vector,uint16_t trunk_id);
void Clear_ALL_trunks(void);

/*Function for update the MAC address into the switch*/
void Marvell_MAC_upt(void);
void Set_Switch_Port_state(int port_index,RSTP_PORT_STATE state);

/*Functions for get the mode for the port*/
int Get_Port_status(int portNo);
int Get_Oper_speed(int portNo);
int Get_Port_duplex(int portNo);
int Get_Port_cost(int portNo);
int Get_FLOW_STATUS(int portNo);
int Get_Port_Type(int portNo);
int Get_Port_Auto(int portNo);

/*Functions for Set the port status*/
MYERROR_INFO  Set_PORT_STATS(int portNo,PORT_STAS port_sta);
MYERROR_INFO  Set_FLOW_CONTROL(int portNo,int flowcontrol);


/*****************************************************************************/
//Function  :  Get_Port_Counters_rou(API_ROUGH_COUNTER_T* stats,uint8_t port)	
//Brief     :  Get the rough counters for the indicated port 
//Parameter :	 'stats' point to struct API_ROUGH_COUNTER_T which used to contain the gained counter value
//	           'port' refers the indicated port
//retval    :  None
/*****************************************************************************/	 
void Get_Port_Counters_rou(volatile API_ROUGH_COUNTER_T* stats,int portNo);

/*****************************************************************************/
//Function  :  Get_Port_Counters_rou(API_ROUGH_COUNTER_T* stats,uint8_t port)	
//Brief     :  Get the rough counters for the indicated port 
//Parameter :	 'stats' point to struct API_ROUGH_COUNTER_T which used to contain the gained counter value
//	           'port' refers the indicated port
//retval    :  None
/*****************************************************************************/	 
void Get_Port_Counters_det(API_DETAIL_COUNTER_T* stats,int portNo);

#ifdef MY_DEBUG_P
void printf_trunk_vector(void);
void printf_mask_table(void);
void printf_PORT_CONTROL1(void);
#endif

#endif
