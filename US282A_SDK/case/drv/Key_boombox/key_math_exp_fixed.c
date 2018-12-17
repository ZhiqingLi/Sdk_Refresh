/*******************************************************************************
 *                         
 * 计算下面公式的函数
 * 10^((Input)/20)*2^31*0.5  input参数的范围是(-60.0dB - 0dB)
 *
 *******************************************************************************/
#include <psp_includes.h>

#define EE          364841611           //Q27
#define INV_EE      49375942            //Q27

#define MUL32_32(a,b)      (long long)((long long)(a)*(long long)(b))
#define MUL32_32_RS15(a,b) (int)(((long long)(a)*(long long)(b))>>15)
#define MUL32_32_RS(a,b,c) (int)(((long long)(a)*(long long)(b))>>(c))

static int exp_taylor_coeff[6] = {134217728,67108864,22369621,5592405,1118481,186413};        

int exp_fast(int db)        
{
    int i;
    int temp = 32768;
    long long sum = 4398046511104L;
    int percent;
    int xin;
    int count = 0;

    xin = (db*75451)/200;

    while (xin < -32768)
    {
        xin += 32768;
        count ++;
    }

    while (xin > 32768)
    {
        xin -= 32768;
        count ++;
    }

    for (i = 0; i < 6; i++)
    {
        temp = MUL32_32_RS15(temp,xin);
        sum += MUL32_32(temp, exp_taylor_coeff[i]);
    }

    percent = (int)(sum>>12);

    if (count > 0)
    {
        if (db < 0)
        {
            for (i = 0; i < count; i++)
            {
                percent = MUL32_32_RS(percent,INV_EE,27);
            }
        }
        else
        {
            for (i = 0; i < count; i++)
            {
                percent = MUL32_32_RS(percent,EE,27);
            }
        }
    }
    return percent;
    
}

