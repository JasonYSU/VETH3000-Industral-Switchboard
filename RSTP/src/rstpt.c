//#include <intrins.h>
#include <stdio.h>
#include <stdint.h>
//#include "IP210reg.h"
//#include "type_def.h"   
//#include "prerstp.h"	/*For WRITE or READ MDIO and TX_packet functions */
//#include "rstpt.h"

#include "base.h"
#include "stpm.h"
#include "stp_in.h"
#include "uid_stp.h"
#include "stp_to.h"
#include "uid.h"
#include "stp_bpdu.h"
#include "common.h"

#define VLAN_ID       1		
#define THE_MAX_NAME  10

/*****REG for the IP113s*********/
#define MDIO_PX_STATUS   0x38




#if 0  /*these are for the test for some function*/
typedef struct tx_tcn_bpdu_t {
  MAC_HEADER_T  mac;
  ETH_HEADER_T  eth;
  BPDU_HEADER_T hdr;
} TCN_BPDU_T;

typedef struct tx_stp_bpdu_t {
  MAC_HEADER_T  mac;
  ETH_HEADER_T  eth;
  BPDU_HEADER_T hdr;
  BPDU_BODY_T   body;
} CONFIG_BPDU_T;

typedef struct tx_rstp_bpdu_t {
  MAC_HEADER_T  mac;
  ETH_HEADER_T  eth;
  BPDU_HEADER_T hdr;
  BPDU_BODY_T   body;
  unsigned char ver_1_length[2];
} RSTP_BPDU_T;

static RSTP_BPDU_T bpdu_packet  = {
  {/* MAC_HEADER_T */
    {0x01, 0x80, 0xc2, 0x00, 0x00, 0x00},   /* dst_mac */
    {0x20, 0x15, 0x11, 0x31, 0x13, 0xf8}    /* src_mac */
  },
  { /* ETH_HEADER_T */
    {0x00, 0x00},               /* len8023 */
    BPDU_L_SAP, BPDU_L_SAP, LLC_UI      /* dsap, ssap, llc */
  },
  {/* BPDU_HEADER_T */
    {0x00, 0x00},               /* protocol */
    BPDU_VERSION_ID, 0x00           /* version, bpdu_type */
  },
  {
    0x00,                   /*  flags; */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},  /*  root_id[8]; */
    {0x00,0x00,0x00,0x00},          /*  root_path_cost[4]; */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},  /*  bridge_id[8]; */
    {0x00,0x00},                /*  port_id[2]; */
    {0x00,0x00},                /*  message_age[2]; */
    {0x00,0x00},                /*  max_age[2]; */
    {0x00,0x00},                /*  hello_time[2]; */
    {0x11,0xaa},                /*  forward_delay[2]; */
  },
   {0xbb,0xcc},                 /*  ver_1_length[2]; */
};
#endif

char      bridge_name[THE_MAX_NAME] = "VETH2000";
BITMAP_T  enabled_ports;

/******************************************************************/
//some function or struct for my test programme for keilC51
/******************************************************************/
//typedef void (*CALLMY_TEST)(void);

typedef void (*CALLMY_TEST)(/*STATE_MACH_T* */);						
//TEST maclloc 
void func()
 {
    int x=20; 
	printf("my x is :\n");
	printf("%d\n",x);
 	//return x;
 }


typedef struct mine 
  {
  	uint8_t  a;
	  uint8_t  b;
	  uint8_t  c;
  }A;
typedef struct test1
  {
      int   b;
	  int  (*rx)(int);
  }TEST;
void my_test()
  { 
    A* pA;
	A* pB;
	
	pA  =(A*)malloc(sizeof(A));
	pA->a = 'a';
	pA->b =	'b';
	pA->c =	'c';
	printf("%c\n",pA->a);
	printf("%c\n",pA->b);
	printf("%c\n",pA->c);
     
  	pB=(A*)calloc(1,sizeof(A));

	if(pB->a == 0)  printf("A is right!\n");
	if(pB->b == 0)  printf("B is right!\n");
	if(pB->c == 0)  printf("C is right!\n");
	
	pB->a = 'x';
	pB->b = 'y';
	pB->c = 'z'; 
  printf("%c\n",pB->a);
	printf("%c\n",pB->b);
	printf("%c\n",pB->c);
  }




TEST*  my_testo(int (*recvd)(int))
  {  
     TEST* t;
	 int   a;  
   	 a        =   20;
	 (*recvd)(a);
	 t->rx =  recvd;
   printf("%x\n",(uint32_t)recvd);
	 printf("%x\n",(uint32_t)t->rx);
	 
	 t->b     =  a;
     
	 (*(t->rx))(a);
	 return t;
  }
void  my_testp(TEST* wtf)
  {	 
      (*(wtf->rx))(10);
  }

//CALLMY_TEST my_testcall=NULL;
#define  my_testcall_LIST(WHAT)                              \
  {                                                          \
     my_testcall=STP_##WHAT##_enter_state;					 \
  }
void myTests(void (*xx)())
 {
      (*xx)();
	   //xx();
 }
/******************************************************************/
//Function: here are some test prongramme for the keiC51
/******************************************************************/
void MAIN_RSTP()
   {  
 
	  //func();
	  //my_testcall = 0x8100;
	  //my_testcall = func;
	  //my_testcall=STP_##WHAT##_enter_state;
	  //my_testcall_LIST(rolesel);
	  //MY_PRINT("%x\n",my_testcall);
	  //my_testcall(); 
      //  myTests(func);
      // HAHA = my_testo(func);
      //my_testp(HAHA);

   }

/******************************************************************/
//Function: When the bridge's status changes,use this function 
/******************************************************************/
int bridge_control (int port_index,UID_CNTRL_BODY_T* cntrl)
{
switch (cntrl->cmd) 
	{
  case UID_PORT_CONNECT:
#ifdef MY_PRINT_RSTP
		  printf("connected port p%02d\n", port_index);
#endif
		BitmapSetBit(&enabled_ports, port_index - 1);
    STP_IN_enable_port (port_index, True);
    break;
    
	case UID_PORT_DISCONNECT:
#ifdef MY_PRINT_RSTP
	  printf("disconnected port p%02d\n", port_index);
#endif	
	  BitmapClearBit(&enabled_ports, port_index - 1);
    STP_IN_enable_port (port_index, False);
    break;
    
	case UID_BRIDGE_SHUTDOWN:
#ifdef  MY_PRINT_RSTP    
	  printf("shutdown from manager :(\n");
#endif
    return 1;
  default:  
    printf("Unknown control command <%d> for port %d\n",
              cntrl->cmd,port_index);

  }
  return 0;
}

/******************************************************************/
//
/******************************************************************/
int bridge_rx_bpdu (UID_MSG_T* msg, size_t msgsize)
{
  register int port_index;

  STP_IN_rx_bpdu (0, msg->header.destination_port,
                    (BPDU_T*) (msg->body.bpdu + sizeof (MAC_HEADER_T)),
                    msg->header.body_len - sizeof (MAC_HEADER_T));


}



/******************************************************************/
//Function :  To create the bridge instance,and then set the 
//            configuration for the bridges.
/******************************************************************/
void CREATE_RSTP_INSTANCE()
   {
  	 int               The_EOR;
	   UID_STP_CFG_T 	   uid_cfg;
	   BITMAP_T          ports;
	          
//init the port number for the bridges and set the bits high for the port creation
		STP_IN_init(NUMBER_OF_PORTS);
		BitmapClear(&enabled_ports);
    BitmapClear(&ports);
		for (The_EOR = 1; The_EOR <= NUMBER_OF_PORTS; The_EOR++) 
		    {
               BitmapSetBit(&ports, The_EOR - 1);
			}
		uid_cfg.field_mask      = BR_CFG_STATE;
		uid_cfg.stp_enabled	    = STP_ENABLED;		

//set the init  info for my bridge instanse,including creatation of the bridge
		The_EOR = STP_IN_stpm_set_cfg (VLAN_ID,&ports,&uid_cfg);  

#ifdef MY_PRINT_RSTP
			if(The_EOR  == STP_OK)
			  printf("Yes! The configuration is done!!!\n");
			else 		    
			  printf("No! I'm a litter upset for the set failure!!!\n");
#endif
    } 	
/******************************************************************/
//Function: To get the STATUS of the port
//   Name : U16_T GET_STATUS(); 
/******************************************************************/
uint16_t GET_STATUS()
{
      uint16_t my_ports_status;
	  
//	  READ_MDIO(MDIO_PX_STATUS);
	  my_ports_status = 0xaa;//IP210RegRead(REG_MDC_DATA_HIGH)*256 + IP210RegRead(REG_MDC_DATA_LOW);

	  return my_ports_status;
}
/******************************************************************/
//Function: To analyze STATUS of the port
//   Name :  
/******************************************************************/

/******************************************************************/
//Function: To check STATUS of the port (link on or link down)
//   Name : u8_t check_on_down() 
/******************************************************************/
void
CHECK_on_down(uint16_t The_XOR,uint16_t The_current)
{
  uint8_t loops;
	for (loops = 15; loops >= 0; loops--)
	    {
		   if( (The_XOR & 0x8000) && (loops == 7) )
		       	{	
					    STP_IN_changed_port_duplex (2);
				} 
		   if( (The_XOR & 0x8000) && (loops == 6) )
		        {
				   if(The_current & 0x0040 )   STP_IN_changed_port_speed (2, 100000);
				   else
				        STP_IN_changed_port_speed (2, 10000);
				}	
		   if( ( The_XOR & 0x8000 )&& (loops == 5) )
		        {
				  if(The_current & 0x0020 )	 STP_IN_enable_port (2,1);
				  else 
				        STP_IN_enable_port (2,0);
				}
		   if( ( The_XOR & 0x8000 )&& (loops == 2) )
		       { 
				        STP_IN_changed_port_duplex (1);
			   }	
		   if( ( The_XOR & 0x8000 )&& (loops == 1) )
		       {
				   if(The_current & 0x0040 )   STP_IN_changed_port_speed (1, 100000);
				   else
				        STP_IN_changed_port_speed (1, 10000);
				}	
		   if( ( The_XOR & 0x8000 )&& (loops == 0) )		 
		   		{
				   if(The_current & 0x0020 )	 STP_IN_enable_port (1,1);
				   else 
				        STP_IN_enable_port (1,0);
		   		}
		   The_XOR = The_XOR << 1 ;
																		
		}
}

/******************************************************************/
//Function: To get the BPDU packet,and send them inside
//   Name : void GET_BPDU(); 
/******************************************************************/

void GET_BPDU()
{
   UID_MSG_T*  my_msg;
}


/******************************************************************/
//Fuction :  Simulate  that The Bridge get the TCN packet,change some 
//   of the variables in the port bridge
//Name    :  void SIMULATE_TCN()
/******************************************************************/
void SIMULATE_TCN(int port_index)
  {
	 PORT_T* my_port;
	 STPM_T* this;
     this = STP_stpm_get_the_list();
//At first,find my dest port      
     for(my_port = this->ports; my_port ; my_port= my_port -> next)
		 {
		     if(my_port->port_index == port_index)  break;
		 }
//then change some of variables	 
	my_port->rx_tcn_bpdu_cnt++;
	my_port->rcvdBpdu = True;
    my_port->msgBpduVersion = 2;
    my_port->msgBpduType = 0x01; 
  }
/******************************************************************/
//Fuction :  Simulate  that The Bridge get the TCN packet,change some 
//   of the variables in the port bridge
//Name    :  void SIMULATE_CONFIGURE()
/******************************************************************/
void SIMULATE_CONFIGURE(int port_index)
 {
	  PORT_T*      my_port;
	  STPM_T*      this;
	  BPDU_BODY_T  body;
	  
      /*!!! Here need to assign the body !!!*/	  
	  this = STP_stpm_get_the_list();
//find the right dest port
      for(my_port =this -> ports; my_port ; my_port = my_port->next )
	   {
	   	   if(my_port -> port_index == port_index) break;  
	   }
//then do the changing for the information from outside BPDUS
      my_port->rx_cfg_bpdu_cnt++;
      my_port->rcvdBpdu = True;
	    my_port->msgBpduVersion = 2;
      my_port->msgBpduType =    0x00;
      my_port->msgFlags =       body.flags; // here rserved
	  STP_VECT_get_vector(&body, &my_port->msgPrio);
      my_port->msgPrio.bridge_port = my_port->port_id;
      /* 17.18.12 */
      STP_get_times(&body, &my_port->msgTimes);
 }



/******************************************************************/
//Fuction :  Simulate  that The Bridge get the TCN packet,change some 
//   of the variables in the port bridge
//Name    :  void SIMULATE_RSTBPDU()
/******************************************************************/
void SIMULATE_RSTBPDU(int port_index)
  {
      PORT_T*      my_port;
	    STPM_T*      this;
      BPDU_BODY_T  body;
    uint8_t rootid[8]    = {0x80,0x00,0x00,0x05,0xd1,0x12,0x40,0x00};
	  uint8_t pacost[4]    = {0x00,0x03,0x0d,0x40};
	  uint8_t bridgeid[8]  = {0x80,0x00,0x00,0x05,0xd1,0x12,0x40,0x00};
    uint8_t portid[2]    = {0x80,0x01};
	  uint8_t message[2]	= {0x00,0x03}; 
	  uint8_t maxage[2]	= {0x00,0x14};
	  uint8_t hellotime[2]	= {0x00,0x02};
	  uint8_t frddelay[2]  = {0x00,0x0f};       
//Here forms the simulation body
#ifdef MY_PRINT_RSTP   
		printf("Is the smulate in ?\n");    
#endif  	   				  
//	   body.flags = 0x4e;
//	   strncpy(body.root_id,rootid,8);       
//	   strncpy(body.root_path_cost,pacost,4); 
//	   strncpy(body.bridge_id,bridgeid,8);
//	   strncpy(body.port_id,portid,2);
//	   strncpy(body.message_age,message,2);
//	   strncpy(body.max_age,maxage,2);
//	   strncpy(body.hello_time,hellotime,2);
//	   strncpy(body.forward_delay,frddelay,2);    

//find this port  
  	  this = STP_stpm_get_the_list();
  	  for(my_port = this->ports; my_port ; my_port= my_port -> next)
		 {
		     if(my_port->port_index == port_index)  break;
		 }
     // printf("MY port role:\n");
	 // printf("%d",my_port->role);
//check some value of  my port	  
     // tev = RSTP_PORT_RX_T; INCR100(nev);
//      if (STP_DISABLED == this->admin_state) 
//	   {/* the stpm had not yet been enabled :( */
//         return STP_Had_Not_Yet_Been_Enabled_On_The_Vlan;
//       }
	 if (my_port->operEdge && ! my_port->lnkWhile && my_port->portEnabled)  
       	 { //printf("TC true!\n");
		   my_port->tc = True;
		 }
//	  if (! my_port->portEnabled) 
//	    {/* port link change indication will come later :( */
//          _stp_in_enable_port_on_stpm (this, my_port->port_index, True);
//        }
       my_port->operEdge = False;
       my_port->wasInitBpdu = True;
//then let the variables change for the result of the RSTBPDU      
	  my_port->rx_rstp_bpdu_cnt++;
  	  if (my_port->owner->ForceVersion >= NORMAL_RSTP) 
	  {
		 my_port->rcvdBpdu = True;
  	  }
  	  else  
	      {//printf("confirmed\n");
			 return;
		  }
      my_port->msgBpduVersion  =  2;
      my_port->msgBpduType  =  0x02;
      my_port->msgFlags =      body.flags;
      /* 17.18.11 */
      STP_VECT_get_vector (&body, &my_port->msgPrio);
      my_port->msgPrio.bridge_port = my_port->port_id;
      /* 17.18.12 */
      STP_get_times (&body, &my_port->msgTimes);
#ifdef MY_PRINT_RSTP
      printf("tellme why?\n");
#endif
			//printf("The information Got\n");
  }
