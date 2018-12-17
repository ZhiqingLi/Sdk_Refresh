#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

typedef struct
{
	char id[4];
	int  file_len;//文件长度减8
	char type[4];
} riff_chunk_t;

typedef struct
{
	char id[4];
	int  size;
	short format_tag;
	short channels;
	int  sample_rate;
	int  bytes_per_second;
	short sample_bytes;
	short sample_bites;
} wav_format_chunk_t;

typedef struct
{
	char id[4];
	int  size;
	char data[4];
} wav_fact_chunk_t;

typedef struct
{
	char id[4];
	int  size;
} wav_data_chunk_t;


typedef enum 
{
	TTS_TYPE_MP3,
	TTS_TYPE_WAV,
} tts_type_e;

tts_type_e g_tts_type;
unsigned char g_frame_header[4];
unsigned int g_sample_rate;
unsigned int g_frame_size;
unsigned int g_channels;

/* 1: MPEG-1, 0: MPEG-2 LSF*/
const unsigned short mp3_table_bitrate[2][3][15] =
{
    {
        { 0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256 },
        { 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160 },
        { 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160 }
    },
    {
        { 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448 },
        { 0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384 },
        { 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 }
    }
};

const unsigned short mp3_samplerate_table[3] =
{ 44100, 48000, 32000 };

unsigned int mp3_get_framesize(unsigned char *p_frame_header, 
    unsigned int *p_sample_rate, unsigned int *p_frame_size, unsigned char display)
{
    unsigned int curframesize, t;
    unsigned int mpeg_lsf, mpeg_ver, mpeglayer, padding;
    unsigned int bitrate;
    unsigned int curframehead, frame_samples, frame_duration;
    unsigned int samplerate;
    
    if (display == 1)
    {
        printf("header:0x%x,0x%x,0x%x,0x%x\n", p_frame_header[0], p_frame_header[1], p_frame_header[2], p_frame_header[3]);
    }
    
    curframehead = ((unsigned int) p_frame_header[1] << 16) 
                 + ((unsigned int) p_frame_header[2] << 8) 
                 + (unsigned int) p_frame_header[3]; //24bit head
    mpeg_ver = (unsigned int) (curframehead >> 19) & 0x3;
    //0: mpeg 1, 1: mpeg 2, 2: mpeg 2.5
    if (mpeg_ver == 0)
    {
        mpeg_ver = 2;
    }
    else
    {
        mpeg_ver = 3 - mpeg_ver;
    }
    
    t = (unsigned int) (curframehead >> 10) & 0x3;
    samplerate = mp3_samplerate_table[t];
    samplerate = samplerate >> mpeg_ver;

    mpeglayer = 4 - ((unsigned int) (curframehead >> 17) & 0x3);
    
    //0: mpeg 2 or mpeg 2.5, 1: mpeg 1
    mpeg_lsf = 1;
    if(mpeg_ver != 0)
    {
        mpeg_lsf = 0;
    }
    t = (unsigned int) (curframehead >> 12) & 0xF;
    bitrate = mp3_table_bitrate[mpeg_lsf][mpeglayer - 1][t];

    if (mpeglayer == 1)
    {
        frame_samples = 384;//layer 1 -> 384
    }
    else if((mpeglayer == 3) && (mpeg_lsf == 0))
    {
        frame_samples = 576;
    }
    else
    {
        frame_samples = 1152; //layer 2 & mpeg1layer3
    }

    t = (unsigned int) (curframehead >> 10) & 0x3;
    t = t + 3 * mpeg_ver;//44,48,32,22,24,16,11,12,8
    //frame_duration = frameduration[mpeglayer - 1][t];

    padding = (unsigned int) (curframehead >> 9) & 0x1;
    if (mpeglayer == 3)
    {
        curframesize = (144000 * (unsigned int) bitrate) / (unsigned int) samplerate;
        if (mpeg_lsf == 0)
        {
            curframesize = curframesize >> 1;
        }
        curframesize = curframesize + padding;
    }
    else if (mpeglayer == 2)
    {
        curframesize = (144000 * (unsigned int) bitrate) / (unsigned int) samplerate;
        curframesize = curframesize + padding;
    }
    else //lay1
    {
        curframesize = (12000 * (unsigned int) bitrate) / (unsigned int) samplerate;
        curframesize = curframesize + padding;
        curframesize = curframesize << 2;
    }
    
    if (display == 1)
    {
        if (mpeg_ver == 0)
        {
            printf("mpeg_ver      = 1\n");
        }
        else if (mpeg_ver == 1)
        {
            printf("mpeg_ver      = 2\n");
        }
        else if (mpeg_ver == 2)
        {
            printf("mpeg_ver      = 2.5\n");
        }
        
        printf("mpeglayer     = %d\n", mpeglayer);
        printf("bitrate       = %d\n", bitrate);
        printf("samplerate    = %d\n", samplerate);
        printf("frame_samples = %d\n", frame_samples);
        printf("frame_size    = %d\n", curframesize);
    }
    
    *p_sample_rate = samplerate;
    *p_frame_size = curframesize;
    return curframesize;
}

//tts语音片段信息
typedef struct
{
    int offset;              //语音片段起始地址
    int length;              //语音片段数据长度
} tts_section_t;

int file_verification_jump_head(FILE *handle, char first_head, long *p_in_index)
{
	unsigned char head[4];
    unsigned int cur_sample_rate, cur_frame_size;
    int ID3V2_Size;
    int offset = 0;
	int ret = 0;
	
	head[0] = fgetc(handle);
    head[1] = fgetc(handle);
    head[2] = fgetc(handle);
    head[3] = fgetc(handle);
    if ((head[0] == 0x49) && (head[1] == 0x44) && (head[2] == 0x33))//ID3
    {//有ID3
        fgetc(handle);
        fgetc(handle);
        head[0] = fgetc(handle);
        head[1] = fgetc(handle);
        head[2] = fgetc(handle);
        head[3] = fgetc(handle);
        ID3V2_Size = (head[0]&0x7F)*0x200000 + (head[1]&0x7F) * 0x4000 + (head[2]&0x7F) * 0x80 + (head[3]&0x7F);
        
        //跳过ID3
        fseek(handle, ID3V2_Size + 10, 0);
        head[0] = fgetc(handle);
        head[1] = fgetc(handle);
        head[2] = fgetc(handle);
        head[3] = fgetc(handle);
        
        offset = ID3V2_Size + 10;//跳过ID3，接下来即帧头
    }
    
    if (first_head == 1)
    {
        memcpy(g_frame_header, head, 4);
        g_frame_size = mp3_get_framesize(g_frame_header, &cur_sample_rate, &cur_frame_size, 1);
        if (g_frame_size > 512)
        {
            printf("帧长度大于512字节，不支持！\n");
            ret = -1;
        }
        g_sample_rate = cur_sample_rate;
    }
    fseek(handle, offset, 0);
    *p_in_index = offset;
    
    return ret;
}

int frame_verification(FILE *handle, int in_index, int in_len)
{
	unsigned char head[4];
    unsigned int cur_sample_rate, cur_frame_size;
	int ret = 0;
	
	head[0] = fgetc(handle);
    head[1] = fgetc(handle);
    head[2] = fgetc(handle);
    head[3] = fgetc(handle);
    
    //比较帧头是否同frame_header一致，保证格式一致
    mp3_get_framesize(head, &cur_sample_rate, &cur_frame_size, 0);
    //if (memcmp(g_frame_header, head, 4) != 0)
    if ((cur_sample_rate != g_sample_rate) || (cur_frame_size != g_frame_size))
    {//放松条件，只要采样率和帧长度一致即可
        if (head[0] == 'T' && head[1] == 'A' && head[2] == 'G')
        {//ID3，直接跳过
            if (in_len - in_index == 128)
            {//刚好ID3结束
                printf("打包完成！\n");
                ret = 1;
            }
            else
            {
                printf("打包完成！ID3 OFFSET = 0x%x/0x%x\n", in_index, in_len);
                ret = 2;
            }
        }
        else
        {
            if (in_index + g_frame_size <= in_len)
            {
                printf("遇到非法帧 OFFSET = 0x%x/0x%x\n", in_index, in_len);
                ret = -1;
            }
            else
            {
                printf("打包完成！TAIL OFFSET = 0x%x/0x%x\n", in_index, in_len);
                ret = 3;
            }
        }
    }
    
    if (ret == 0)
    {
        fseek(handle, -4, 1);
    }
    
    return ret;
}

int file_verification_jump_head_wav(FILE *handle, char first_head, long *p_in_index)
{
	riff_chunk_t tmp_riff;
	wav_format_chunk_t tmp_fmt_chunk;
	wav_fact_chunk_t tmp_fact_chunk;
	wav_data_chunk_t tmp_data_chunk;
	int ret = 0;
	
	fread(&tmp_riff, 12, 1, handle);
	if (memcmp(tmp_riff.id, "RIFF", 4) != 0)
	{
		ret = -1;
		return ret; 
	}
	if (memcmp(tmp_riff.type, "WAVE", 4) != 0)
	{
		ret = -1;
		return ret;
	}
	
	fread(&tmp_fmt_chunk, 24, 1, handle);
	if (memcmp(tmp_fmt_chunk.id, "fmt ", 4) != 0)
	{
		ret = -1;
		return ret;
	}
	
	//只支持16bit采样深度
	if (tmp_fmt_chunk.sample_bites != 16)
	{
		ret = -1;
		return ret;
	}
	
	if (first_head == 1)
	{
        g_sample_rate = tmp_fmt_chunk.sample_rate;
        g_channels = tmp_fmt_chunk.channels;
	}
	else
	{
		if (tmp_fmt_chunk.sample_rate != g_sample_rate)
		{
			ret = -1;
			return ret;
		}
		
		//只支持单声道
		if (tmp_fmt_chunk.channels != g_channels)
		{
			ret = -1;
			return ret;
		}
	}
	
	if (tmp_fmt_chunk.size == 18)
	{//跳过2个字节额外信息
		fseek(handle, 2, 1);
	}

	//fact trunk
	fread(&tmp_fact_chunk, 12, 1, handle);
	if (memcmp(tmp_fact_chunk.id, "fact", 4) != 0)
	{
		fseek(handle, -12, 1);
	}
	
	//data trunk
	fread(&tmp_data_chunk, 8, 1, handle);
	if (memcmp(tmp_data_chunk.id, "data", 4) != 0)
	{
		ret = -1;
		return ret;
	}
	
	*p_in_index = ftell(handle);
	
	return ret;
}

int main()
{
    FILE *in, *in_flist, *out;
    tts_section_t *tts_section_array;
    int section_index = 0;
    int section_total = 0;
    char file_line[1024];
    char tts_file[128];
    long in_index, in_len, in_true_len, i;
    long out_index_begin;
    char ch;
    unsigned char head[4];
    unsigned int cur_sample_rate, cur_frame_size;
    int first_head = 1;
    int need_read_line = 1;
    int cur_line_index = 0;
    int ret;
    int has_index = 0;
    int num_flag = 0; //1表示找到=号，2表示找到数字，3表示遇到非法字符
    int index_num = 0;
    int tts_section_max = 127;

    in_flist = fopen("tts_flist.txt", "r");
    if (in_flist == NULL)
    {
        printf("tts_flist.txt 找不到！\n");
        return 0;
    }

    //看看 tts_flist.txt 头注释有没有 INDEX = 1 这样的内容
    while(1)
    {
        if (feof(in_flist))
        {
            break;
        }

        if (fgets(file_line, 1023, in_flist) == NULL)
        {
            break;
        }

        //非注释
        if ((file_line[0] != '/') || (file_line[1] != '/'))
        {
            break;
        }

        cur_line_index = 0;
        while (1)
        {
            if ((file_line[cur_line_index] == '\n') || (file_line[cur_line_index] == '\r') || (cur_line_index >= 1024))
            {
                break;
            }

            if (memcmp("INDEX", &file_line[cur_line_index], 5) == 0)
            {
                cur_line_index += 5;
                while (1)
                {
                    if ((file_line[cur_line_index] == '\n') || (file_line[cur_line_index] == '\r') || (cur_line_index >= 1024))
                    {
                        break;
                    }

                    if (num_flag == 0)
                    {
                        if (file_line[cur_line_index] == '=')
                        {
                            num_flag = 1;
                        }
                        else if (file_line[cur_line_index] == ' ')
                        {
                            ;//跳过
                        }
                        else
                        {
                            num_flag = 3;
                            break;
                        }
                    }
                    else if (num_flag == 1)
                    {
                        if ((file_line[cur_line_index] >= '0') && (file_line[cur_line_index] <= '9'))
                        {
                            index_num = file_line[cur_line_index] - '0';
                            num_flag = 2;
                            has_index = 1;
                        }
                        else if (file_line[cur_line_index] == ' ')
                        {
                            ;//跳过
                        }
                        else
                        {
                            num_flag = 3;
                            break;
                        }
                    }
                    else if (num_flag == 2)
                    {
                        if ((file_line[cur_line_index] >= '0') && (file_line[cur_line_index] <= '9'))
                        {
                            index_num *= 10;
                            index_num += file_line[cur_line_index] - '0';
                        }
                        else
                        {
                            break;
                        }
                    }

                    cur_line_index++;
                }

                break;
            }

            if ((file_line[cur_line_index] == '\n') || (file_line[cur_line_index] == '\r') || (cur_line_index >= 1024))
            {
                break;
            }

            cur_line_index++;
        }

        if (has_index == 1)
        {
            break;
        }
    }

    fseek(in_flist, 0, 0);

    //找不到INDEX = 1，按照默认长度处理
    if (has_index == 0)
    {
        index_num = 1;
    }

    printf("INDEX = %d\n", index_num);

    tts_section_max = (index_num * 0x400 - 8)/8;

    tts_section_array = (tts_section_t *) malloc(8*tts_section_max);

    memset(tts_section_array, 0x0, sizeof(tts_section_t) * tts_section_max);

    out = fopen("tts_rc.lib", "wb");
    for (i = 0; i < (8 * tts_section_max + 8); i++)
    {
        fputc(0x0, out);
    }
    out_index_begin = (8 * tts_section_max + 8);

    while(1)
    {
        if (feof(in_flist))
        {
            break;
        }
        
        if (need_read_line == 1)
        {
            if (fgets(file_line, 1023, in_flist) == NULL)
            {
                break;
            }
            cur_line_index = 0;
            need_read_line = 0;
        }
        
        if ((file_line[0] == '/') && (file_line[1] == '/'))
        {//继续读取脚本
            need_read_line = 1;
            continue;
        }

        if (section_index >= tts_section_max)
        {
            printf("tts_flist.txt 文件太多！\n");
            break;
        }

        //获取一个文件名
        {
            int n = 0;
            
            while (1)
            {
                if (file_line[cur_line_index] == ';')
                {
                    tts_file[n] = 0;
                    cur_line_index++;
                    break;
                }
                else if (((unsigned char)(file_line[cur_line_index]) == 0xa3) 
                      && ((unsigned char)(file_line[cur_line_index + 1]) == 0xbb))
                {//支持中文分号
                	tts_file[n] = 0;
                    cur_line_index += 2;
                    break;
                }
                else if ((file_line[cur_line_index] == '\n') || (file_line[cur_line_index] == '\r'))
                {
                    tts_file[n] = 0;
                    need_read_line = 1;
                    break;
                }
                
                if ((file_line[cur_line_index] == ' ') || (file_line[cur_line_index] == '\t'))
                {//跳过空格和制表符
                    cur_line_index++;
                }
                else if (((unsigned char)(file_line[cur_line_index]) == 0xa1)
                      && ((unsigned char)(file_line[cur_line_index + 1]) == 0xa1))
                {//支持中文空格
                    cur_line_index += 2;
                }
                else
                {
                    tts_file[n++] = file_line[cur_line_index];
                    cur_line_index++;
                }
            }
        }
        
        if (memcmp(tts_file, "END", 3) == 0)
        {//END表示mp3音源列表结束
            break;
        }
                
        if (memcmp(tts_file, "NULL", 4) == 0)
        {//被删除，填充空白项
            tts_section_array[section_index].offset = 0x0;
            tts_section_array[section_index].length = 0x0;
            section_index++;
            continue;
        }
        
        //TTS资源类型验证
        if (first_head == 1)
        {
	        if (strstr(tts_file, ".mp3") != NULL)
			{
				g_tts_type = TTS_TYPE_MP3;
			}
			else if (strstr(tts_file, ".wav") != NULL)
			{
				g_tts_type = TTS_TYPE_WAV;
			}
			else
			{
	            printf("tts_flist.txt 错误！\n");
	            break;
	        }
		}
        
        in = fopen(tts_file, "rb");
        if (in == NULL)
        {
            {
                //打印文件名
                int name_len = strlen(tts_file);
                int space_count = 0;
                int i;
                
                if (name_len < 24)
                {
                    space_count = 24 - name_len;
                }
                printf("[%s]", tts_file);
                for (i = 0; i < space_count; i++)
                {
                    printf(" ");
                }
            }
            printf("没有此文件！\n");
            break;
        }
        fseek(in, 0, 2);
        in_len = ftell(in);
        
        //查看是否有ID3，有则跳过
        fseek(in, 0, 0);
        
        //验证文件，并跳过文件头
        if (g_tts_type == TTS_TYPE_MP3)
        {
	        if (file_verification_jump_head(in, first_head, &in_index) == -1)
	        {
	            fclose(in);
	            break;
	        }
	    }
	    else
	    {
	    	if (file_verification_jump_head_wav(in, first_head, &in_index) == -1)
	        {
	            fclose(in);
	            break;
	        }
	    }
        first_head = 0;
        
        {
            //打印文件名
            int name_len = strlen(tts_file);
            int space_count = 0;
            int i;
            
            if (name_len < 24)
            {
                space_count = 24 - name_len;
            }
            printf("[%s]", tts_file);
            for (i = 0; i < space_count; i++)
            {
                printf(" ");
            }
        }
        
        in_true_len = 0;
        if (g_tts_type == TTS_TYPE_MP3)
        {
	        while (1)
	        {
	            if (in_index < in_len)
	            {
	                if (frame_verification(in, in_index, in_len) != 0)
	                {
	                	break;
	                }              
	                  
	                for (i = 0; i < g_frame_size; i++)
	                {
	                    ch = fgetc(in);
	                    fputc(ch, out);
	                }
	                in_index += g_frame_size;
	                in_true_len += g_frame_size;
	            }
	            else
	            {
	                printf("打包完成！\n");
	                break;
	            }
	        }
	    }
	    else
	    {
	    	while (1)
	        {
	            if (in_index < in_len)
	            {
	            	ch = fgetc(in);
	                fputc(ch, out);
	                in_index++;
	                in_true_len++;
	            }
	            else
	            {
	                printf("打包完成！\n");
	                break;
	            }
	        }
	    }
        
        fclose(in);
        tts_section_array[section_index].offset = out_index_begin;
        tts_section_array[section_index].length = in_true_len;
        section_index++;
        section_total++;
        out_index_begin += in_true_len;
    }
    
    //回写头部，little endian
    fseek(out, 0, 0);
    if (g_tts_type == TTS_TYPE_MP3)
    {
	    fputc('M', out);
	    fputc('P', out);
	    fputc('3', out);
	    fputc(' ', out);
	    fputc((unsigned char)(g_sample_rate & 0xff), out);
		fputc((unsigned char)((g_sample_rate & 0xff00) >> 8), out);
		fputc((unsigned char)(g_frame_size & 0xff), out);
		fputc((unsigned char)((g_frame_size & 0xff00) >> 8), out);
	}
	else
	{
		fputc('W', out);
	    fputc('A', out);
	    fputc('V', out);
	    fputc(' ', out);
		fputc((unsigned char)(g_sample_rate & 0xff), out);
		fputc((unsigned char)((g_sample_rate & 0xff00) >> 8), out);
	}
	fseek(out, 8, 0);
    for (i = 0; i < (8 * tts_section_max); i++)
    {
        fputc(*((unsigned char *)tts_section_array + i), out);
    }

    free(tts_section_array);
    fclose(in_flist);
    fclose(out);
    
    printf("生成 TTS 文件成功\n");
    printf("总共 %d 语音片段\n", section_total);
    printf("按任意键退出...");
    
    {
        char ch = getchar();
    }
    return 0;
}
