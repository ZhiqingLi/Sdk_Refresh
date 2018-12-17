/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：定点算法实现exp函数，精度为0.1db。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_func.h"

#define EE          364841611           //Q27
#define INV_EE      49375942            //Q27

#define MUL32_32(a,b)      (long long)((long long)(a)*(long long)(b))
#define MUL32_32_RS15(a,b) (int)(((long long)(a)*(long long)(b))>>15)
#define MUL32_32_RS(a,b,c) (int)(((long long)(a)*(long long)(b))>>(c))

const int exp_taylor_coeff[6] = {134217728,67108864,22369621,5592405,1118481,186413};

/*db单位为0.1db*/
/*为了计算方便，将结果转换为百分数，返回分子*/
int com_math_exp_fixed(int db)
{
    int i;
    int temp = 32768;
    long long sum = 4398046511104L;
    int xin;
    int percent;
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

    percent = (int)(sum>>27);

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

    percent = percent*100/32768;

    return percent;
}
