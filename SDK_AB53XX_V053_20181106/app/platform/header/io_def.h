#ifndef __IO_DEF_H
#define __IO_DEF_H


#include "global.h"
#include "config.h"
#include "sfr.h"


#if (SD0_MAPPING == SD0MAP_G1)
    //SDCMD(PA5), SDCLK(PA6), SDDAT0(PA7), SDDAT1(PA4), SDDAT2(PA3), SDDAT3(PA2)
    #define SD_MUX_IO_INIT()        {GPIOADE |= BIT(5)|BIT(6)|BIT(7);\
                                    GPIOACLR  = BIT(6);\
                                    GPIOADIR &= ~BIT(6);\
                                    GPIOAPU  &= ~BIT(6);\
                                    GPIOADIR |= BIT(5) | BIT(7);\
                                    GPIOAPU  |= BIT(5) | BIT(7);\
                                    GPIOAFEN |= BIT(5) | BIT(7);\
                                    FUNCMCON0 = SD0MAP_G1;}
    #define SD_IO_INIT()            {GPIOADE |= BIT(5)|BIT(6)|BIT(7);\
                                    GPIOADIR &= ~BIT(6);\
                                    GPIOADIR |= BIT(5) | BIT(7);\
                                    GPIOAPU  |= BIT(5) | BIT(7);\
                                    GPIOAFEN |= BIT(5) | BIT(7);\
                                    FUNCMCON0 = SD0MAP_G1;}
    #define SD_CLK_DIR_IN()         {GPIOADIR |= BIT(6); GPIOAPU  |= BIT(6);}
    #define SD_CLK_IN_DIS_PU10K()   {GPIOADIR |= BIT(6); GPIOAPU  &= ~BIT(6);}
    #define SD_CLK_DIR_OUT()        {GPIOAPU  &= ~BIT(6); GPIOADIR &= ~BIT(6);}
    #define SD_MUX_DETECT_INIT()    {GPIOADE |= BIT(6); GPIOAPU  |= BIT(6); GPIOADIR |= BIT(6);}
    #define SD_MUX_IS_ONLINE()      ((GPIOA & BIT(6)) == 0)
    #define SD_MUX_IS_BUSY()        ((GPIOADIR & BIT(6)) == 0)
    #define SD_CMD_MUX_PU300R()     {GPIOAPU300 |= BIT(5); GPIOAPU &= ~BIT(5);}
    #define SD_CMD_MUX_PU10K()      {GPIOAPU |= BIT(5); GPIOAPU300 &= ~BIT(5);}
    #define SD_DAT_MUX_PU300R()     {GPIOAPU300 |= BIT(7); GPIOAPU &= ~BIT(7);}
    #define SD_DAT_MUX_PU10K()      {GPIOAPU |= BIT(7); GPIOAPU300 &= ~BIT(7);}
    #define SD_CMD_MUX_IS_ONLINE()  ((GPIOA & BIT(5)) == 0)


    #define SD_CLK_OUT_H()          {GPIOASET = BIT(6);}
    #define SD_CLK_OUT_L()          {GPIOACLR = BIT(6);}
    #define SD_CLK_STA()            (GPIOA & BIT(6))

    #define SD_DAT_DIR_OUT()        {GPIOADE |= BIT(7); GPIOADIR &= ~BIT(7);}
    #define SD_DAT_DIR_IN()         {GPIOADIR |= BIT(7); GPIOAPU  |= BIT(7);}
    #define SD_DAT_OUT_H()          {GPIOASET = BIT(7);}
    #define SD_DAT_OUT_L()          {GPIOACLR = BIT(7);}
    #define SD_DAT_STA()            (GPIOA & BIT(7))

    #define SD_CMD_DIR_OUT()        {GPIOADE |= BIT(5); GPIOADIR &= ~BIT(5);}
    #define SD_CMD_DIR_IN()         {GPIOADIR |= BIT(5); GPIOAPU  |= BIT(5);}
    #define SD_CMD_OUT_H()          {GPIOASET = BIT(5);}
    #define SD_CMD_OUT_L()          {GPIOACLR = BIT(5);}
    #define SD_CMD_STA()            (GPIOA & BIT(5))

#elif (SD0_MAPPING == SD0MAP_G2)
    //SDCMD(PB0), SDCLK(PB1), SDDAT0(PB2)
    #define SD_MUX_IO_INIT()        {GPIOBDE |= BIT(0)|BIT(1)|BIT(2);\
                                    GPIOBCLR  = BIT(1);\
                                    GPIOBDIR &= ~BIT(1);\
                                    GPIOBPU &= ~BIT(1);\
                                    GPIOBDIR |= BIT(2) | BIT(0);\
                                    GPIOBPU  |= BIT(2) | BIT(0);\
                                    GPIOBSET |= BIT(2) | BIT(1) | BIT(0);\
                                    GPIOBFEN |= BIT(2) | BIT(1) | BIT(0);\
                                    FUNCMCON0 = SD0MAP_G2;}
    #define SD_IO_INIT()            {GPIOBDE |= BIT(0)|BIT(1)|BIT(2);\
                                    GPIOBDIR &= ~BIT(1);\
                                    GPIOBDIR |= BIT(2) | BIT(0);\
                                    GPIOBPU  |= BIT(2) | BIT(0);\
                                    GPIOBFEN |= BIT(2) | BIT(1) | BIT(0);\
                                    FUNCMCON0 = SD0MAP_G2;}
    #define SD_CLK_DIR_IN()         {GPIOBDIR |= BIT(1); GPIOBPU  |= BIT(1);}
    #define SD_CLK_IN_DIS_PU10K()   {GPIOBDIR |= BIT(1); GPIOBPU  &= ~BIT(1);}
    #define SD_CLK_DIR_OUT()        {GPIOBPU  &= ~BIT(1); GPIOBDIR &= ~BIT(1);}
    #define SD_MUX_DETECT_INIT()    {GPIOBDE |= BIT(1); GPIOBPU  |= BIT(1); GPIOBDIR |= BIT(1);}
    #define SD_MUX_IS_ONLINE()      ((GPIOB & BIT(1)) == 0)
    #define SD_MUX_IS_BUSY()        ((GPIOBDIR & BIT(1)) == 0)
    #define SD_CMD_MUX_PU300R()     {GPIOBPU300 |= BIT(0); GPIOBPU &= ~BIT(0);}
    #define SD_CMD_MUX_PU10K()      {GPIOBPU |= BIT(0); GPIOBPU300 &= ~BIT(0);}
    #define SD_DAT_MUX_PU300R()     {GPIOBPU300 |= BIT(2); GPIOBPU &= ~BIT(2);}
    #define SD_DAT_MUX_PU10K()      {GPIOBPU |= BIT(2); GPIOBPU300 &= ~BIT(2);}
    #define SD_CMD_MUX_IS_ONLINE()  ((GPIOB & BIT(0)) == 0)

    #define SD_CLK_OUT_H()          {GPIOBSET = BIT(1);}
    #define SD_CLK_OUT_L()          {GPIOBCLR = BIT(1);}
    #define SD_CLK_STA()            (GPIOB & BIT(1))

    #define SD_DAT_DIR_OUT()        {GPIOBDE |= BIT(2); GPIOBDIR &= ~BIT(2);}
    #define SD_DAT_DIR_IN()         {GPIOBDIR |= BIT(2); GPIOBPU  |= BIT(2);}
    #define SD_DAT_OUT_H()          {GPIOBSET = BIT(2);}
    #define SD_DAT_OUT_L()          {GPIOBCLR = BIT(2);}
    #define SD_DAT_STA()            (GPIOB & BIT(2))

    #define SD_CMD_DIR_OUT()        {GPIOBDE |= BIT(0); GPIOBDIR &= ~BIT(0);}
    #define SD_CMD_DIR_IN()         {GPIOBDIR |= BIT(0); GPIOBPU  |= BIT(0);}
    #define SD_CMD_OUT_H()          {GPIOBSET = BIT(0);}
    #define SD_CMD_OUT_L()          {GPIOBCLR = BIT(0);}
    #define SD_CMD_STA()            (GPIOB & BIT(0))

#elif (SD0_MAPPING == SD0MAP_G3)
    //SDCMD(PE5), SDCLK(PE6), SDDAT0(PE7), SDDAT1(PE4), SDDAT2(PE3), SDDAT3(PE2)
    #define SD_MUX_IO_INIT()        {GPIOEDE |= BIT(5)|BIT(6)|BIT(7);\
                                    GPIOECLR  = BIT(6);\
                                    GPIOEDIR &= ~BIT(6);\
                                    GPIOEPU  &= ~BIT(6);\
                                    GPIOEDIR |= BIT(5) | BIT(7);\
                                    GPIOEPU  |= BIT(5) | BIT(7);\
                                    GPIOEFEN |= BIT(5) | BIT(7);\
                                    FUNCMCON0 = SD0MAP_G3;}
    #define SD_IO_INIT()            {GPIOEDE |= BIT(5)|BIT(6)|BIT(7);\
                                    GPIOEDIR &= ~BIT(6);\
                                    GPIOEDIR |= BIT(5) | BIT(7);\
                                    GPIOEPU  |= BIT(5) | BIT(7);\
                                    GPIOEFEN |= BIT(5) | BIT(7);\
                                    FUNCMCON0 = SD0MAP_G3;}
    #define SD_CLK_DIR_IN()         {GPIOEDIR |= BIT(6); GPIOEPU  |= BIT(6);}
    #define SD_CLK_IN_DIS_PU10K()   {GPIOEDIR |= BIT(6); GPIOEPU  &= ~BIT(6);}
    #define SD_CLK_DIR_OUT()        {GPIOEPU  &= ~BIT(6); GPIOEDIR &= ~BIT(6);}
    #define SD_MUX_DETECT_INIT()    {GPIOEDE |= BIT(6); GPIOEPU  |= BIT(6); GPIOEDIR |= BIT(6);}
    #define SD_MUX_IS_ONLINE()      ((GPIOE & BIT(6)) == 0)
    #define SD_MUX_IS_BUSY()        ((GPIOEDIR & BIT(6)) == 0)
    #define SD_CMD_MUX_PU300R()     {GPIOEPU300 |= BIT(5); GPIOEPU &= ~BIT(5);}
    #define SD_CMD_MUX_PU10K()      {GPIOEPU |= BIT(5); GPIOEPU300 &= ~BIT(5);}
    #define SD_DAT_MUX_PU300R()     {GPIOEPU300 |= BIT(7); GPIOEPU &= ~BIT(7);}
    #define SD_DAT_MUX_PU10K()      {GPIOEPU |= BIT(7); GPIOEPU300 &= ~BIT(7);}
    #define SD_CMD_MUX_IS_ONLINE()  ((GPIOE & BIT(5)) == 0)

    #define SD_CLK_OUT_H()          {GPIOESET = BIT(6);}
    #define SD_CLK_OUT_L()          {GPIOECLR = BIT(6);}
    #define SD_CLK_STA()            (GPIOE & BIT(6))

    #define SD_DAT_DIR_OUT()        {GPIOEDE |= BIT(7); GPIOEDIR &= ~BIT(7);}
    #define SD_DAT_DIR_IN()         {GPIOEDIR |= BIT(7); GPIOEPU  |= BIT(7);}
    #define SD_DAT_OUT_H()          {GPIOESET = BIT(7);}
    #define SD_DAT_OUT_L()          {GPIOECLR = BIT(7);}
    #define SD_DAT_STA()            (GPIOE & BIT(7))

    #define SD_CMD_DIR_OUT()        {GPIOADE |= BIT(5); GPIOEDIR &= ~BIT(5);}
    #define SD_CMD_DIR_IN()         {GPIOEDIR |= BIT(5); GPIOEPU  |= BIT(5);}
    #define SD_CMD_OUT_H()          {GPIOESET = BIT(5);}
    #define SD_CMD_OUT_L()          {GPIOECLR = BIT(5);}
    #define SD_CMD_STA()            (GPIOE & BIT(5))

#elif (SD0_MAPPING == SD0MAP_G4)
    //SDCMD(PA5), SDCLK(PA6), SDDAT0(PB4)
    #define SD_MUX_IO_INIT()        {GPIOADE |= BIT(5)|BIT(6);\
                                    GPIOACLR  = BIT(6);\
                                    GPIOADIR &= ~BIT(6);\
                                    GPIOAPU  &= ~BIT(6);\
                                    GPIOADIR |= BIT(5);\
                                    GPIOAPU  |= BIT(5);\
                                    GPIOAFEN |= BIT(5);\
                                    GPIOBDE |= BIT(4);\
                                    GPIOBDIR |= BIT(4);\
                                    GPIOBPU  |= BIT(4);\
                                    GPIOBFEN |= BIT(4);\
                                    FUNCMCON0 = SD0MAP_G4;}
    #define SD_IO_INIT()            {GPIOADE |= BIT(5)|BIT(6);\
                                    GPIOADIR &= ~BIT(6);\
                                    GPIOADIR |= BIT(5);\
                                    GPIOAPU  |= BIT(5);\
                                    GPIOAFEN |= BIT(5);\
                                    GPIOBDE |= BIT(4);\
                                    GPIOBDIR |= BIT(4);\
                                    GPIOBPU  |= BIT(4);\
                                    GPIOBFEN |= BIT(4);\
                                    FUNCMCON0 = SD0MAP_G4;}
    #define SD_CLK_DIR_IN()         {GPIOADIR |= BIT(6); GPIOAPU  |= BIT(6);}
    #define SD_CLK_IN_DIS_PU10K()   {GPIOADIR |= BIT(6); GPIOAPU  &= ~BIT(6);}
    #define SD_CLK_DIR_OUT()        {GPIOAPU  &= ~BIT(6); GPIOADIR &= ~BIT(6);}
    #define SD_MUX_DETECT_INIT()    {GPIOADE |= BIT(6); GPIOAPU  |= BIT(6); GPIOADIR |= BIT(6);}
    #define SD_MUX_IS_ONLINE()      ((GPIOA & BIT(6)) == 0)
    #define SD_MUX_IS_BUSY()        ((GPIOADIR & BIT(6)) == 0)
    #define SD_CMD_MUX_PU300R()     {GPIOAPU300 |= BIT(5); GPIOAPU &= ~BIT(5);}
    #define SD_CMD_MUX_PU10K()      {GPIOAPU |= BIT(5); GPIOAPU300 &= ~BIT(5);}
    #define SD_DAT_MUX_PU300R()     {GPIOBPU300 |= BIT(4); GPIOBPU &= ~BIT(4);}
    #define SD_DAT_MUX_PU10K()      {GPIOBPU |= BIT(4); GPIOBPU300 &= ~BIT(4);}
    #define SD_CMD_MUX_IS_ONLINE()  ((GPIOA & BIT(5)) == 0)

    #define SD_CLK_OUT_H()          {GPIOASET = BIT(6);}
    #define SD_CLK_OUT_L()          {GPIOACLR = BIT(6);}
    #define SD_CLK_STA()            (GPIOA & BIT(6))

    #define SD_DAT_DIR_OUT()        {GPIOBDE |= BIT(4); GPIOBDIR &= ~BIT(4);}
    #define SD_DAT_DIR_IN()         {GPIOBDIR |= BIT(4); GPIOBPU  |= BIT(4);}
    #define SD_DAT_OUT_H()          {GPIOBSET = BIT(4);}
    #define SD_DAT_OUT_L()          {GPIOBCLR = BIT(4);}
    #define SD_DAT_STA()            (GPIOB & BIT(4))

    #define SD_CMD_DIR_OUT()        {GPIOADE |= BIT(5); GPIOADIR &= ~BIT(5);}
    #define SD_CMD_DIR_IN()         {GPIOADIR |= BIT(5); GPIOAPU  |= BIT(5);}
    #define SD_CMD_OUT_H()          {GPIOASET = BIT(5);}
    #define SD_CMD_OUT_L()          {GPIOACLR = BIT(5);}
    #define SD_CMD_STA()            (GPIOA & BIT(5))

#elif (SD0_MAPPING == SD0MAP_G5)
    //SDCMD(PA5), SDCLK(PA6), SDDAT0(PB3)
    #define SD_MUX_IO_INIT()        {GPIOADE |= BIT(5)|BIT(6);\
                                    GPIOACLR  = BIT(6);\
                                    GPIOADIR &= ~BIT(6);\
                                    GPIOAPU  &= ~BIT(6);\
                                    GPIOADIR |= BIT(5);\
                                    GPIOAPU  |= BIT(5);\
                                    GPIOAFEN |= BIT(5);\
                                    GPIOBDE |= BIT(3);\
                                    GPIOBDIR |= BIT(3);\
                                    GPIOBPU  |= BIT(3);\
                                    GPIOBFEN |= BIT(3);\
                                    FUNCMCON0 = SD0MAP_G5;}
    #define SD_IO_INIT()            {GPIOADIR &= ~BIT(6);\
                                    GPIOADIR |= BIT(5);\
                                    GPIOAPU  |= BIT(5);\
                                    GPIOAFEN |= BIT(5);\
                                    GPIOBDE |= BIT(3);\
                                    GPIOBDIR |= BIT(3);\
                                    GPIOBPU  |= BIT(3);\
                                    GPIOBFEN |= BIT(3);\
                                    FUNCMCON0 = SD0MAP_G5;}
    #define SD_CLK_DIR_IN()         {GPIOADIR |= BIT(6); GPIOAPU  |= BIT(6);}
    #define SD_CLK_IN_DIS_PU10K()   {GPIOADIR |= BIT(6); GPIOAPU  &= ~BIT(6);}
    #define SD_CLK_DIR_OUT()        {GPIOAPU  &= ~BIT(6); GPIOADIR &= ~BIT(6);}
    #define SD_MUX_DETECT_INIT()    {GPIOADE |= BIT(6); GPIOAPU  |= BIT(6); GPIOADIR |= BIT(6);}
    #define SD_MUX_IS_ONLINE()      ((GPIOA & BIT(6)) == 0)
    #define SD_MUX_IS_BUSY()        ((GPIOADIR & BIT(6)) == 0)
    #define SD_CMD_MUX_PU300R()     {GPIOAPU300 |= BIT(5); GPIOAPU &= ~BIT(5);}
    #define SD_CMD_MUX_PU10K()      {GPIOAPU |= BIT(5); GPIOAPU300 &= ~BIT(5);}
    #define SD_DAT_MUX_PU300R()     {GPIOBPU300 |= BIT(3); GPIOBPU &= ~BIT(3);}
    #define SD_DAT_MUX_PU10K()      {GPIOBPU |= BIT(3); GPIOBPU300 &= ~BIT(3);}
    #define SD_CMD_MUX_IS_ONLINE()  ((GPIOA & BIT(5)) == 0)

    #define SD_CLK_OUT_H()          {GPIOASET = BIT(6);}
    #define SD_CLK_OUT_L()          {GPIOACLR = BIT(6);}
    #define SD_CLK_STA()            (GPIOA & BIT(6))

    #define SD_DAT_DIR_OUT()        {GPIOBDE |= BIT(3); GPIOBDIR &= ~BIT(3);}
    #define SD_DAT_DIR_IN()         {GPIOBDIR |= BIT(3); GPIOBPU  |= BIT(3);}
    #define SD_DAT_OUT_H()          {GPIOBSET = BIT(3);}
    #define SD_DAT_OUT_L()          {GPIOBCLR = BIT(3);}
    #define SD_DAT_STA()            (GPIOB & BIT(3))

    #define SD_CMD_DIR_OUT()        {GPIOADE |= BIT(5); GPIOADIR &= ~BIT(5);}
    #define SD_CMD_DIR_IN()         {GPIOADIR |= BIT(5); GPIOAPU  |= BIT(5);}
    #define SD_CMD_OUT_H()          {GPIOASET = BIT(5);}
    #define SD_CMD_OUT_L()          {GPIOACLR = BIT(5);}
    #define SD_CMD_STA()            (GPIOA & BIT(5))

#elif (SD0_MAPPING == SD0MAP_G6)
    //SDCMD(PB3), SDCLK(PA6), SDDAT0(PB4)
    #define SD_MUX_IO_INIT()        {GPIOADE |= BIT(6);\
                                    GPIOACLR  = BIT(6);\
                                    GPIOADIR &= ~BIT(6);\
                                    GPIOAPU  &= ~BIT(6);\
                                    GPIOBDE |= BIT(3)|BIT(4);\
                                    GPIOBDIR |= BIT(3) | BIT(4);\
                                    GPIOBPU  |= BIT(3) | BIT(4);\
                                    GPIOBFEN |= BIT(3) | BIT(4);\
                                    FUNCMCON0 = SD0MAP_G6;}
    #define SD_IO_INIT()            {GPIOADE |= BIT(6);\
                                    GPIOADIR &= ~BIT(6);\
                                    GPIOBDE |= BIT(3)|BIT(4);\
                                    GPIOBDIR |= BIT(3) | BIT(4);\
                                    GPIOBPU  |= BIT(3) | BIT(4);\
                                    GPIOBFEN |= BIT(3) | BIT(4);\
                                    FUNCMCON0 = SD0MAP_G6;}
    #define SD_CLK_DIR_IN()         {GPIOADIR |= BIT(6); GPIOAPU  |= BIT(6);}
    #define SD_CLK_IN_DIS_PU10K()   {GPIOADIR |= BIT(6); GPIOAPU  &= ~BIT(6);}
    #define SD_CLK_DIR_OUT()        {GPIOAPU  &= ~BIT(6); GPIOADIR &= ~BIT(6);}
    #define SD_MUX_DETECT_INIT()    {GPIOADE |= BIT(6); GPIOAPU  |= BIT(6); GPIOADIR |= BIT(6);}
    #define SD_MUX_IS_ONLINE()      ((GPIOA & BIT(6)) == 0)
    #define SD_MUX_IS_BUSY()        ((GPIOADIR & BIT(6)) == 0)
    #define SD_CMD_MUX_PU300R()     {GPIOBPU300 |= BIT(3); GPIOBPU &= ~BIT(3);}
    #define SD_CMD_MUX_PU10K()      {GPIOBPU |= BIT(3); GPIOBPU300 &= ~BIT(3);}
    #define SD_DAT_MUX_PU300R()     {GPIOBPU300 |= BIT(4); GPIOBPU &= ~BIT(4);}
    #define SD_DAT_MUX_PU10K()      {GPIOBPU |= BIT(4); GPIOBPU300 &= ~BIT(4);}
    #define SD_CMD_MUX_IS_ONLINE()  ((GPIOB & BIT(3)) == 0)

    #define SD_CLK_OUT_H()          {GPIOASET = BIT(6);}
    #define SD_CLK_OUT_L()          {GPIOACLR = BIT(6);}
    #define SD_CLK_STA()            (GPIOA & BIT(6))

    #define SD_DAT_DIR_OUT()        {GPIOBDE |= BIT(4); GPIOBDIR &= ~BIT(4);}
    #define SD_DAT_DIR_IN()         {GPIOBDIR |= BIT(4); GPIOBPU  |= BIT(4);}
    #define SD_DAT_OUT_H()          {GPIOBSET = BIT(4);}
    #define SD_DAT_OUT_L()          {GPIOBCLR = BIT(4);}
    #define SD_DAT_STA()            (GPIOB & BIT(4))

    #define SD_CMD_DIR_OUT()        {GPIOBDE |= BIT(3); GPIOBDIR &= ~BIT(3);}
    #define SD_CMD_DIR_IN()         {GPIOBDIR |= BIT(3); GPIOBPU  |= BIT(3);}
    #define SD_CMD_OUT_H()          {GPIOBSET = BIT(3);}
    #define SD_CMD_OUT_L()          {GPIOBCLR = BIT(3);}
    #define SD_CMD_STA()            (GPIOB & BIT(3))
#else
    #define SD_MUX_IO_INIT()
    #define SD_IO_INIT()
    #define SD_CLK_DIR_IN()
    #define SD_CLK_IN_DIS_PU10K()
    #define SD_CLK_DIR_OUT()
    #define SD_MUX_DETECT_INIT()
    #define SD_MUX_IS_ONLINE()      0
    #define SD_MUX_IS_BUSY()        0
    #define SD_CMD_MUX_PU300R()
    #define SD_CMD_MUX_PU10K()
    #define SD_CMD_MUX_IS_ONLINE()  0

    #define SD_CLK_OUT_H()
    #define SD_CLK_OUT_L()
    #define SD_CLK_STA()            0

    #define SD_DAT_DIR_OUT()
    #define SD_DAT_DIR_IN()
    #define SD_DAT_OUT_H()
    #define SD_DAT_OUT_L()
    #define SD_DAT_STA()            0

    #define SD_CMD_DIR_OUT()
    #define SD_CMD_DIR_IN()
    #define SD_CMD_OUT_H()
    #define SD_CMD_OUT_L()
    #define SD_CMD_STA()            0
#endif

#endif //__IO_DEF_H
