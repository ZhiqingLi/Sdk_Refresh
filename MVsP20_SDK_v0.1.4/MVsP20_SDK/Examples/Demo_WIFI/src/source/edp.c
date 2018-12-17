#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <type.h>
#include <debug.h>
#include <sockets.h>
#include <netdb.h> 
//#include "string_convert.h"

/*----------------------------消息类型---------------------------------------*/
/* 连接请求 */
#define CONNREQ             0x10
/* 连接响应 */
#define CONNRESP            0x20
/* 转发(透传)数据 */
#define PUSHDATA            0x30
/* 存储(转发)数据 */
#define SAVEDATA            0x80
/* 存储确认 */
#define SAVEACK             0x90
/* 命令请求 */
#define CMDREQ              0xA0
/* 命令响应 */
#define CMDRESP             0xB0
/* 心跳请求 */
#define PINGREQ             0xC0
/* 心跳响应 */
#define PINGRESP            0xD0
/* 加密请求 */
#define ENCRYPTREQ          0xE0
/* 加密响应 */
#define ENCRYPTRESP         0xF0

/*----------------------------错误码-----------------------------------------*/
#define ERR_CREATE_SOCKET   -1
#define ERR_HOSTBYNAME      -2
#define ERR_CONNECT         -3
#define ERR_SEND            -4
#define ERR_TIMEOUT         -5
#define ERR_RECV            -6

#define Socket(a,b,c)          socket(a,b,c)
#define Connect(a,b,c)         connect(a,b,c)
#define Close(a)               close(a)
#define Read(a,b,c)            read(a,b,c)
#define Recv(a,b,c,d)          recv(a, (void *)b, c, d)
#define Select(a,b,c,d,e)      select(a,b,c,d,e)
#define Send(a,b,c,d)          send(a, (const int8 *)b, c, d)
#define Write(a,b,c)           write(a,b,c)
#define GetSockopt(a,b,c,d,e)  getsockopt((int)a,(int)b,(int)c,(void *)d,(socklen_t *)e)
#define SetSockopt(a,b,c,d,e)  setsockopt((int)a,(int)b,(int)c,(const void *)d,(int)e)
#define GetHostByName(a)       gethostbyname((const char *)a)
#define MSG_NOSIGNAL 0x2000


static const char *deviceid = "5909186";
static const char *api_key = "3wceEGGt3CSmGYgVrRIxsdWZKOs=";


int sockfd;
char *proess_cmd_buffer;
unsigned char *RecvBuffer;

int32 Open(const uint8 *addr, int16 portno)
{
    int32 sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    /* 创建socket套接字 */
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
//        fprintf(stderr, "ERROR opening socket\n");
        close(sockfd);
        return ERR_CREATE_SOCKET; 
    }
    server = GetHostByName(addr);
    if (server == NULL) {
//        fprintf(stderr, "ERROR, no such host\n");
        Close(sockfd);
        return ERR_HOSTBYNAME;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    {
//    	uint32_t addr = serv_addr.sin_addr.s_addr;
//    	DBG("IP=%d.%d.%d.%d\n",addr&0xFF,(addr>>8)&0xFF,(addr>>16)&0xFF,(addr>>24)&0xFF);
    }
    /* 客户端 建立与TCP服务器的连接 */
    if (Connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    {
//        fprintf(stderr, "ERROR connecting\n");
    	Close(sockfd);
        return ERR_CONNECT;
    }
#ifdef _DEBUG
    DBG("[%s] connect to server %s:%d succ!...\n", __func__, addr, portno);
#endif
    return sockfd;
}


int32 DoSend(int32 sockfd, const char* buffer, uint32 len)
{
    int32 total  = 0;
    int32 n = 0;
    while (len != total)
    {
        /* 试着发送len - total个字节的数据 */
        n = Send(sockfd,buffer + total,len - total,MSG_NOSIGNAL);
        if (n <= 0)
        {
//            fprintf(stderr, "ERROR writing to socket\n");
            return n;
        }
        /* 成功发送了n个字节的数据 */
        total += n;
    }
    /* wululu test print send bytes */
    //hexdump((const unsigned char *)buffer, len);
    return total;
}

//connect
void edp_connect(int32 sockfd,const char* devid, const char* auth_key)
{
	uint8_t *pbuf;// = pvPortMalloc(100);
	int ret;
	//DBG("edp_connect\n");
	pbuf = (uint8_t *)pvPortMalloc(100);
	memset(pbuf,0,100);
	pbuf[0]= 0x10;//message type: link device iot message
	pbuf[1]=  (2+3)+1+1+2+(2+strlen(devid))+(2+strlen(auth_key));;//message length
	pbuf[2]= 0x00;pbuf[3]= 0x03;//descriptor length
	pbuf[4]= 'E';pbuf[5]= 'D';pbuf[6]= 'P';//descriptor
	pbuf[7]= 0x01;//protocol version
	pbuf[8]= 0x40;//connect flag
	pbuf[9]= 0x01;pbuf[10]= 0x2C;//keep time
	pbuf[11]= strlen(devid)>>8;pbuf[12]= strlen(devid)&0xFF;//deviceid len
	memcpy(pbuf+13,devid,strlen(devid));////deviceid
	pbuf[13+strlen(devid)]= strlen(auth_key)>>8;pbuf[14+strlen(devid)]= strlen(auth_key)&0xFF;//deviceid len
	memcpy(pbuf+15+strlen(devid),auth_key,strlen(auth_key));
	ret=DoSend(sockfd, (const char*)pbuf,15+strlen(devid)+strlen(auth_key));
	vPortFree(pbuf);
}

//ping
void edp_ping(int32 sockfd)
{
	int ret;
	uint8_t ping_msg[2] = {0xc0,0x00};
	ret=DoSend(sockfd, (const char*)ping_msg,2);
}


//上传数据
void edp_save_data(int32 sockfd,const char* msg)
{
	uint8_t *pbuf = pvPortMalloc(512);
	int ret;
	pbuf[0] = 0x80;
	if(strlen(msg)+4 > 127)
	{
		pbuf[1] = ((strlen(msg)+4)%128)|0x80;
		pbuf[2] = ((strlen(msg)+4)/128);
		pbuf[3] = 0x00;
		pbuf[4] = 0x03;
		pbuf[5] = strlen(msg)>>8;
		pbuf[6] = strlen(msg)&0xFF;
		memcpy(pbuf+7,msg,strlen(msg));
		ret=DoSend(sockfd, (const char*)pbuf,strlen(msg)+7);
	}
	else
	{
		pbuf[1] = strlen(msg)+4;
		pbuf[2] = 0x00;
		pbuf[3] = 0x03;
		pbuf[4] = strlen(msg)>>8;
		pbuf[5] = strlen(msg)&0xFF;
		memcpy(pbuf+6,msg,strlen(msg));
		ret=DoSend(sockfd, (const char*)pbuf,strlen(msg)+6);
	}
	vPortFree(pbuf);
}


void edp_get_device_info(const char* devid, const char* auth_key,const char* msg)
{
	uint8_t *pbuf = (uint8_t *)pvPortMalloc(1024);
	int sock = Open("api.heclouds.com", 80);
	int ret,n;
	memset(pbuf,0,512);
	sprintf(pbuf,"GET /devices/%s/datastreams/%s HTTP/1.1\r\napi-key: %s\r\nHost: api.heclouds.com\r\nCache-Control: no-cache\r\n\r\n",devid,msg,auth_key);
	DBG("%s",pbuf);
	ret=DoSend(sock, (const char*)pbuf,strlen(pbuf));

	memset(pbuf,0,1024);
	lwip_read(sock,pbuf,1024);
	DBG("%s\n",pbuf);

	Close(sock);
	vPortFree(pbuf);
}

void cmd_to_gbk(uint8_t *in_buf,uint32_t in_len,uint8_t *out_buf,uint32_t* pout_len)
{
    int conut=0;
    int length=0;
    int i;
    int j=0;
    uint32_t tab[4];
    memset(tab,0,4);
    for(i=0;i<in_len;i++)
    {
        if(in_buf[i] == '{')
        {
            length = 0;
        }
        else if((in_buf[i] == ',')||(in_buf[i] == '}'))
        {
            if(length==4)
            {
                uint8_t tabin[2];
                tabin[0] = (tab[0]<<4)+tab[1];
                tabin[1] = (tab[2]<<4)+tab[3];
                //qDebug(">>%02X %02X",tabin[0],tabin[1]);
//                StringConvert(out_buf+j,2,tabin,2,UNICODE_BIG_TO_GBK);
                //qDebug("<<%02X %02X",out_buf[j],out_buf[j+1]);
                j = j+2;
            }
            else if(length==2)
            {
                out_buf[j] = (tab[0]<<4)+tab[1];
                j = j+1;
            }
            else
            {

            }
            conut++;
            length = 0;
            memset(tab,0,4);
        }
        else
        {
            if((in_buf[i] >= '0') && (in_buf[i] <= '9'))
            {
                tab[length] = in_buf[i] - '0';
            }
            else if((in_buf[i] >= 'a') && (in_buf[i] <= 'f'))
            {
                tab[length] = in_buf[i] - 'a'+10;
            }
            else if((in_buf[i] >= 'A') && (in_buf[i] <= 'F'))
            {
                tab[length] = in_buf[i] - 'A'+10;
            }
            else
            {
                //qDebug("error\n");
            }
            length++;
        }
    }
    *pout_len = j;
}

void edp_test()
{
	extern uint32_t wifi_link_ok;
	int n, ret;
	int pingcount;

	DBG("edp_test\n");

_START:
	while(!wifi_link_ok) vTaskDelay(100);

	sockfd = Open("jjfaedp.hedevice.com", 876);
	if(sockfd < 0)
	{
		DBG("socket error\n");
		vTaskDelay(1000);
		goto _START;
	}
	else
	{
		DBG("socket ok\n");
	}
	link_device:
	DBG("link_device\n");
	edp_connect(sockfd,deviceid,api_key);
	while(1)
	{
		vTaskDelay(200);
		DBG(".");
		RecvBuffer = pvPortMalloc(1024);

		if(RecvBuffer == NULL) continue;
		n = Recv(sockfd, RecvBuffer, 1024, MSG_NOSIGNAL);
    	if(n == 4)
    	{
    		if((RecvBuffer[0]==0x20)&&(RecvBuffer[1]==0x02)&&(RecvBuffer[2]==0x00)&&(RecvBuffer[3]==0x00))
    		{
    			DBG("\ndevice connect iot ok\n");
    			break;
    		}
    		else
    		{
    			DBG("\ndevice connect iot error\n");
    		}
    	}
    	vPortFree(RecvBuffer);
	}
	pingcount = 0;
    while(1)
    {
    	vTaskDelay(100);
    	pingcount++;

		//保持连接 1s发送一次ping 1s查询一次命令
    	if(pingcount == 50)
    	{
    		edp_ping(sockfd);
    		pingcount = 0;
    	}
    	RecvBuffer = pvPortMalloc(1024);
		if(RecvBuffer == NULL)
		{
			DBG("RecvBuffer Malloc ERR\n");
			break;
		}

    	memset(RecvBuffer,0,1024);
    	n = Recv(sockfd, RecvBuffer, 1024, MSG_DONTWAIT);

    	if(n != -1)
    	{
    		pingcount = 0;
			switch(RecvBuffer[0])
			{
				case PINGRESP:
					DBG("device ping ok\n");
					DBG("FreeHeap=%x,MinHeap=%x\n",(unsigned int)xPortGetFreeHeapSize(),(unsigned int)xPortGetMinimumEverFreeHeapSize());

					break;

				case 0x40://unlink
					DBG("device link close\n");
					goto link_device;
					break;

				case CMDREQ:
					{
						uint32_t start_pos,end_pos,i;
						DBG("rcv from iot\n");
						start_pos = 0;
						end_pos = 0;
						for(i=0;i<n;i++)
						{
							if(RecvBuffer[i] == '{')
							{
								start_pos = i;
							}
							if(RecvBuffer[i] == '}')
							{
								end_pos = i;
							}
						}
						if(end_pos == 0)//未找到结尾
						{
							DBG("end_pos err\n");
							break;
						}


						proess_cmd_buffer = pvPortMalloc(1024);
						if(proess_cmd_buffer == NULL)
						{
							DBG("proess_cmd_buffer Malloc ERR\n");
							break;
						}
						memset(proess_cmd_buffer,0,1024);
						memcpy(proess_cmd_buffer,RecvBuffer+start_pos,end_pos-start_pos+1);
						DBG("%s\n",proess_cmd_buffer);
						vPortFree(proess_cmd_buffer);

					}
					break;

				default:
					{
						int i;
						DBG("%d\ndata:\n",n);
						for(i=0;i<n;i++)
						{
							DBG("%02X ",RecvBuffer[i]);
						}
						DBG("\n");
					}
					break;
			}
    	}


    	//test for save data
//    	{
//    		char buf[30];
//    		memset(buf,0,30);
//    		sprintf(buf,"{\"pm25\":%d}",pingcount);
//    		///printf("%s\n",buf);
//    		edp_save_data(sockfd,buf);
//    	}
//    	//test for get data
//    	{
//    		edp_get_device_info(deviceid,api_key,"pm25");
//    	}

    	vPortFree(RecvBuffer);
    }
}


