#include "type.h"
#include "debug.h"
#include "spi_flash.h"

//LWIP 相关
#include "sockets.h"
#include <net_mgr.h>
#include <lwip/api.h>
#include <host_apis.h>
#include "net_mgr.h"


#include "fsdata.h"//html/图片



const  unsigned char http_html_hdr[]="HTTP/1.1  200 OK\r\nContent-type: text/html\r\n\r\n";
const  unsigned char http_html_img[]="HTTP/1.1  200 OK\r\nContent-type: image/gif\r\n\r\n";

const unsigned char errhtml[] = "\
        <html>\
            <head>\
               <title>Error!</title>\
            </head>\
            <body>\
               <h1>404 - Page not found</h1>\
            </body>\
        </html>";





#define NON_NUM '0'
static char Char2Num(char ch)
{
	if(ch>='0' && ch<='9')return (char)(ch-'0');
	if(ch>='a' && ch<='f')return (char)(ch-'a'+10);
	if(ch>='A' && ch<='F')return (char)(ch-'A'+10);
	return NON_NUM;
}
/************************************************
* 把字符串进行URL解码。
* 输入：
* str: 要解码的字符串
* strSize: 字符串的长度。
* result: 结果缓冲区的地址
* resultSize:结果地址的缓冲区大小，可以<=strSize
* 返回值：
* >0: result中实际有效的字符长度，
* 0: 解码失败，原因是结果缓冲区result的长度太小
************************************************/
static int URLDecode(const char* str, const int strSize, char* result, const int resultSize) //url解码
{
	char ch, ch1, ch2;
	int i;
	int j = 0; /* for result index */

	if ((str == NULL) || (result == NULL) || (strSize <= 0) || (resultSize <= 0)) {
		return 0;
	}

	for (i=0; (i<strSize) && (j<resultSize); i++)
	{
		ch = str[i];
		switch (ch)
		{
			case '+':
			result[j++] = ' ';
			break;

			case '%':
			if (i+2 < strSize)
			{
				ch1 = Char2Num(str[i+1]);
				ch2 = Char2Num(str[i+2]);
				if ((ch1 != NON_NUM) && (ch2 != NON_NUM))
				{
					result[j++] = (char)((ch1<<4) | ch2);
					i += 2;
					break;
				}
			}
			/* goto default */
			default:
			result[j++] = ch;
			break;
		}
	}
	result[j] = 0;
	return j;
}
/**
  * @brief serve tcp connection
  * @param conn: connection socket
  * @retval None
  */
//unsigned char recv_buffer[1500];
//unsigned char content_buffer[512];

void http_server(int conn)
{
	int buflen = 1600;
	int ret;
	int tmp;
	uint8_t * content_buffer;
	uint8_t * recv_buffer = pvPortMalloc(buflen);

  	if(recv_buffer == NULL)
	{
		DBG("recv_buffer Malloc ERR\n");
		return;
	}
	memset(recv_buffer,0,buflen);
	/* Read in the request */
	ret = read(conn, recv_buffer, buflen);
	if(ret <= 0)
	{
		close(conn);
		vPortFree(recv_buffer);
		printf("read failed\r\n");
		return;
	}

    //printf("http server response!\r\n");
    if(strncmp((char *)recv_buffer, "GET /LED ", sizeof("GET /LED ")-1) == 0)//GET主页如：192.168.25.1/LED
    {
		write(conn, http_html_hdr, sizeof(http_html_hdr)-1);
        write(conn, file_index_html[0].data,file_index_html[0].len);
        printf("write:%d\n",(int)file_index_html[0].len);
    }
    else if(strncmp((char *)recv_buffer, "GET /img/led.gif", sizeof("GET /img/led.gif")-1) == 0)//GET图片
	{
    	write(conn, http_html_img, sizeof(http_html_img)-1);//发送帧头

//		int len = 0;
//		tmp = file_img_led_gif[0].len;
//		while(tmp > 1024)
//		{
//			send(conn, file_img_led_gif[0].data + len, 1024,0x2000);//发送图片，分包发送
//			tmp -= 1024;
//			len += 1024;
//			vTaskDelay(2);//延时发送
//		}
//		if(tmp)//发送余下的数据
//		{
//			send(conn, file_img_led_gif[0].data + len, tmp,0x2000);//发送图片余下的数据
//		}


    	write(conn, file_img_led_gif[0].data, file_img_led_gif[0].len);//发送图片

    	//printf("GET /img/led.gif OK!\n");
	}
    else if(strncmp((char *)recv_buffer, "GET /?Value", sizeof("GET /?Value")-1) == 0)//web 提交数据
    {
    	char* ptr;
    	printf("\n提交数据:\n");

    	write(conn, http_html_hdr, sizeof(http_html_hdr)-1);


    	if((ptr = strstr((const char *)recv_buffer,"mode=")) != NULL)//走字方式
		{
    		tmp = atoi(ptr+sizeof("mode=")-1);
    		printf("mode=%d\n",tmp);
		}
    	if((ptr = strstr((const char *)recv_buffer,"light=")) != NULL)//走字亮度
		{
    		tmp = atoi(ptr+sizeof("light=")-1);
    		printf("light=%d\n",tmp);
		}
    	if((ptr = strstr((const char *)recv_buffer,"speed=")) != NULL)//走字速度
		{
    		tmp = atoi(ptr+sizeof("speed=")-1);
    		printf("speed=%d\n",tmp);
		}
    	if((ptr = strstr((const char *)recv_buffer,"number=")) != NULL)//屏幕数量
		{
    		tmp = atoi(ptr+sizeof("number=")-1);
    		printf("number=%d\n",tmp);
    		if(tmp > 50)
    		{
    	    	write(conn, "no", sizeof("no")-1);//提示失败
    	    	goto __END;
    		}
		}
    	if((ptr = strstr((const char *)recv_buffer,"content=")) != NULL)//显示内容
		{
    		char* endPtr = ptr;
    		while((*endPtr) != ' ') endPtr++;

    		ptr = ptr+sizeof("content=")-1;//指针指向数据起始位置
    		tmp = (uint32_t)(endPtr-ptr);
    		if(tmp > 600)//显示内容大于200字节，url编码3个字节为一个byte
    		{
    			printf("err:content bytes =%d \n",tmp/3);
    	    	write(conn, "no", sizeof("no")-1);////网页提示:提交失败
    	    	goto __END;
    		}
    		else
    		{
    			content_buffer = pvPortMalloc(1024);
    			if(content_buffer == NULL)
    			{
    				DBG("content_buffer Malloc ERR\n");
    				goto __END;
    			}
    			memset(content_buffer,0,1024);
				URLDecode((const char *)ptr,tmp,(char*)content_buffer,1024);
				printf("content=%s\n",content_buffer);
				vPortFree(content_buffer);
    		}
		}

    	write(conn, "yes", sizeof("yes")-1);//网页提示:提交成功
    }
    else//其它显示错误
    {
        write(conn, errhtml, sizeof(errhtml)-1);
    }

  __END:

//    printf("\n\nrecv_buffer:%sd\n",recv_buffer);

    close(conn);  /* Close connection socket */
    vPortFree(recv_buffer);

}

/**
  * @brief  http_task
  * @param arg: pointer on argument(not used here)
  * @retval None
  */
void http_task(void *arg)
{
  int sock, newconn, size;
  struct sockaddr_in address, remotehost;

  vTaskDelay(200);
  printf("\n http_task \n");
 /* create a TCP socket */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("can not create socket\n");
    return;
  }
  else
  {
	  printf("socket ok\n");
  }

  /* bind to port 80 at any interface */
  address.sin_family = AF_INET;
  address.sin_port = htons(80);
  address.sin_addr.s_addr = INADDR_ANY;
  if (bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
  {
    printf("can not bind socket\n");
    close(sock);
    return;
  }
  else
  {
	  printf("bind ok\n");
  }

  /* listen for connections (TCP listen backlog = 1) */
  listen(sock, 1);
  size = sizeof(remotehost);
  while (1)
  {
     newconn = accept(sock, (struct sockaddr *)&remotehost, (socklen_t *)&size);
     if (newconn >= 0)
     {
       http_server(newconn);
     }
     else
     {
       close(newconn);
     }
//     DBG("FreeHeap=%x,MinHeap=%x\n",(unsigned int)xPortGetFreeHeapSize(),(unsigned int)xPortGetMinimumEverFreeHeapSize());

  }
}
