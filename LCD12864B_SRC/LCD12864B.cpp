//LCD12864(ST7920控制器)的PROTEUS仿真模型
//Author: cdhigh
//Repository: <https://github.com/cdhigh/lcd12864_st7920_proteus>
//Initial release: 2021-09-29
#include "stdafx.h"
#include <fstream>
#include <string>
#include <stdio.h>
#include "LCD12864B.h"
#include "gb2312.h"
#include "ascii.h"

#define AUTH_KEY    0x12344321

static LCD12864B * pLcd12864b;
static int pLcd12864bRefCount;

//----------------------------------------------------------------------------
//电气模型的实现

//构造数字电气模型实例
extern "C" IDSIMMODEL __declspec(dllexport) * createdsimmodel (CHAR *device, ILICENCESERVER *ils)
{ 
    ils->authorize(AUTH_KEY);
    if (pLcd12864b == NULL) {
        pLcd12864b = new LCD12864B;
        pLcd12864bRefCount = 1;
    } else {
        pLcd12864bRefCount++;
    }
    return pLcd12864b;
}

//析构数字电气模型实例
extern "C" VOID __declspec(dllexport) deletedsimmodel (IDSIMMODEL *model)
{
    if (pLcd12864bRefCount > 0) {
        pLcd12864bRefCount--;
    }

    if ((pLcd12864bRefCount == 0) && (pLcd12864b != NULL)) {
        delete (LCD12864B *)model;
        pLcd12864b = NULL;
    }
}

// Exported constructor for active component models.
extern "C" IACTIVEMODEL __declspec(dllexport) * createactivemodel (CHAR *device, ILICENCESERVER *ils)
{
    ils->authorize(AUTH_KEY);
    if (pLcd12864b == NULL) {
        pLcd12864b = new LCD12864B;
        pLcd12864bRefCount = 1;
    } else {
        pLcd12864bRefCount++;
    }
    return pLcd12864b;
}


// Exported destructor for active component models.
extern "C" VOID  __declspec(dllexport) deleteactivemodel (IACTIVEMODEL *model)
{ 
    if (pLcd12864bRefCount > 0) {
        pLcd12864bRefCount--;
    }

    if ((pLcd12864bRefCount == 0) && (pLcd12864b != NULL)) {
        delete (LCD12864B *)model;
        pLcd12864b = NULL;
    }
}

LCD12864B::LCD12864B(void)
{
    srand(0);
    initVariables();
}

//数字电路总是返回TRUE
INT LCD12864B::isdigital (CHAR *pinname)
{
    return 1;
}

//当创建模型实例时被调用，做初始化工作
VOID LCD12864B::setup (IINSTANCE *inst, IDSIMCKT *dsim)
{
    instance = inst;    //PROSPICE仿真原始模型
    ckt = dsim;        //DSIM的数字元件

    //获取引脚
    rs = instance->getdsimpin("RS,rs", true);
    rs->setstate(FLT);    //FLOAT
    rw = instance->getdsimpin("R/W,r/w", true);
    rw->setstate(FLT);
    en = instance->getdsimpin("E,e", true);
    en->setstate(FLT);
    psb = instance->getdsimpin("PSB,psb", true);
    psb->setstate(FLT);
    rst = instance->getdsimpin("RST,rst", true);
    rst->setstate(FLT);
    d[0] = instance->getdsimpin("D0,d0", true);
    d[0]->setstate(FLT);
    d[1] = instance->getdsimpin("D1,d1", true);
    d[1]->setstate(FLT);
    d[2] = instance->getdsimpin("D2,d2", true);
    d[2]->setstate(FLT);
    d[3] = instance->getdsimpin("D3,d3", true);
    d[3]->setstate(FLT);
    d[4] = instance->getdsimpin("D4,d4", true);
    d[4]->setstate(FLT);
    d[5] = instance->getdsimpin("D5,d5", true);
    d[5]->setstate(FLT);
    d[6] = instance->getdsimpin("D6,d6", true);
    d[6]->setstate(FLT);
    d[7] = instance->getdsimpin("D7,d7", true);
    d[7]->setstate(FLT);
    //串口引脚
    cs = rs;
    sid = rw;
    sclk = en;

    //为方便操作，将D0~D7映射为8位总线
    databus = instance->getbuspin("LCD_DBUS", d, 8);
    databus->settiming(100, 100, 100);    //设置时间延迟
    databus->setstates(SHI, SLO, FLT);    //设置总线逻辑为[1,0,三态]时的驱动状态
    
    initVariables();
}

//初始化变量
void LCD12864B::initVariables(void)
{
    memset(displayMemCache, 0, sizeof(displayMemCache));
    memset(GDRAM, 0, sizeof(GDRAM));
    memset(DDRAM, 0xff, sizeof(DDRAM)); //不能初始化为0，因为编码0x0000为第一个用户自定义字符
    memset(CGRAM, 0, sizeof(CGRAM));
    
    hzCodePage = HZCODE_GB2312;

    gdramX = 0;
    gdramY = 0;
    incAddr = 1;
    addr4BitMode = 0;
    extCmdMode = 0;
    graphicOn = 0;
    displayOn = 0;
    currCmd = 0;
    nextCmdIsLow4Bit = 0;
    outputLow4Bit = 0;
    gdramXReceiving = 0;
    currAddrType = ADDR_TYPE_INVALID;

    srlStarted = SRL_STARTED_INVALID;
    srlByteIdx = 7;
    srlHighCnt = 0;
    srlRw = 0;
    srlRs = 0;
    srlCmd = 0;
    srlLow4Bit = 0;
    srlNextIsData = 0;
    
    dataLowReceiving = 0;

    status = 0;    //状态（见手册）
    newFlag = TRUE;    //新数据到达标志
}

//仿真运行模式控制，交互仿真中每帧开始时被调用
VOID LCD12864B::runctrl (RUNMODES mode)
{
}


//交互仿真时用户改变按键等的状态时被调用
VOID LCD12864B::actuate (REALTIME time, ACTIVESTATE newstate)
{
}


//交互仿真时每帧结束时被调用，通过传递ACTIVEDATA数据与绘图模型通信，从而调用animate()进行绘图
BOOL LCD12864B::indicate (REALTIME time, ACTIVEDATA *data)
{    
    if (newFlag) {    //有新数据到达
        data->type = ADT_REAL;    //call back animate() to refresh lcd
        data->realval = (float)time*DSIMTICK;
    }
    return TRUE;
}

//当引脚状态变化时被调用，主要用来处理数据输入和输出
VOID LCD12864B::simulate (ABSTIME time, DSIMMODES mode)
{
    if (PIN_IS_LOW(rst)) {
        initVariables();
        newFlag = TRUE;
        return;
    }

    if (PIN_IS_HIGH(psb)) {
        parallelSimulate(time, mode);
    } else {
        serialSimulate(time, mode);
    }
}

//MCU写显示数据到对应的存储器
void LCD12864B::storeData(BYTE data)
{
    if (currAddrType == ADDR_TYPE_GDRAM) { //绘图
        if (dataLowReceiving == 0) { //正在接收GDRAM高8位
            debugPrint("store high GDRAM at addr x, y", data, gdramX, gdramY);

            GDRAM[gdramXyToIndex(gdramX, gdramY)] = data; //每行32个字节

        } else if (dataLowReceiving == 1) { //正在接收GDRAM低8位
            debugPrint("store low GDRAM at addr x, y", data, gdramX, gdramY);
            
            GDRAM[gdramXyToIndex(gdramX, gdramY) + 1] = data; //每行32个字节

            gdramX = ((gdramX + incAddr) % LCD_ROW_LEN);        //X地址自动加1

            newFlag = TRUE;    //新数据到达标志
        }
        dataLowReceiving ^= 0x01;
    } else if (currAddrType == ADDR_TYPE_DDRAM) {
        if (dataLowReceiving == 0) { //正在接收DDRAM高8位
            //debugPrint("Received high data at ddram addr ", data, ddramAddr);
            DDRAM[ddramAddrToIndex(ddramAddr)] = data;
        } else if (dataLowReceiving == 1) { //正在接收DDRAM低8位
            //debugPrint("Received low data at ddram addr ", data, ddramAddr);
            DDRAM[ddramAddrToIndex(ddramAddr) + 1] = data;
            ddramAddr = (ddramAddr + incAddr) % 0x7f;  //地址自动增加

            newFlag = TRUE;    //新数据到达标志
        }
        dataLowReceiving ^= 0x01;
    } else if (currAddrType == ADDR_TYPE_CGRAM) {
        if (dataLowReceiving == 0) { //正在接收CGRAM高8位
            CGRAM[cgramAddrToIndex(cgramAddr)] = data;
        } else if (dataLowReceiving == 1) { //正在接收CGRAM低8位
            CGRAM[cgramAddrToIndex(cgramAddr) + 1] = data;
            cgramAddr = (cgramAddr + incAddr) % (sizeof(CGRAM) / 2);
        }

        if ((cgramAddr & 0x0f) == 0x00) {
            newFlag = TRUE;
        }
        dataLowReceiving ^= 0x01;
    }
}

//MCU都ST7920的RAM数据
void LCD12864B::outputData(ABSTIME time, DSIMMODES mode)
{
    BYTE data = 0;

    if (dummyRead) { //模拟需要假读一次的机制
        data = (BYTE)rand();
        if ((addr4BitMode == 0) || outputLow4Bit) { //4位模式一次输出4位
            dummyRead = 0;
        }
    } else if (currAddrType == ADDR_TYPE_GDRAM) { //绘图RAM
        if (dataLowReceiving) { //输出低8位
            data = GDRAM[gdramXyToIndex(gdramX, gdramY) + 1];
            if ((addr4BitMode == 0) || outputLow4Bit) { //4位模式一次输出4位
                dataLowReceiving = 0;
                gdramX = ((gdramX + incAddr) % 16);        //地址自动加1
            }
        } else { //高8位
            data = GDRAM[gdramXyToIndex(gdramX, gdramY)];
            if ((addr4BitMode == 0) || outputLow4Bit) { //4位模式一次输出4位
                dataLowReceiving = 1;
            }
        }
    } else if (currAddrType == ADDR_TYPE_DDRAM) {
        if (dataLowReceiving) { //输出低8位
            data = DDRAM[ddramAddrToIndex(ddramAddr) + 1];
            if ((addr4BitMode == 0) || outputLow4Bit) { //4位模式一次输出4位
                dataLowReceiving = 0;
                ddramAddr = ((ddramAddr + incAddr) % 0x7f);        //地址自动加1
            }
        } else { //高8位
            data = DDRAM[ddramAddrToIndex(ddramAddr)];
            if ((addr4BitMode == 0) || outputLow4Bit) { //4位模式一次输出4位
                dataLowReceiving = 1;
            }
        }
    } else if (currAddrType == ADDR_TYPE_CGRAM) {
        if (dataLowReceiving) { //输出低8位
            data = CGRAM[cgramAddrToIndex(cgramAddr) + 1];
            if ((addr4BitMode == 0) || outputLow4Bit) { //4位模式一次输出4位
                dataLowReceiving = 0;
                ddramAddr = ((cgramAddr + incAddr) % 0x7f);        //地址自动加1
            }
        } else { //高8位
            data = CGRAM[cgramAddrToIndex(cgramAddr)];
            if ((addr4BitMode == 0) || outputLow4Bit) { //4位模式一次输出4位
                dataLowReceiving = 1;
            }
        }
    }
    
    //输出到总线
    if (addr4BitMode) { //4位模式一次输出4位
        if (outputLow4Bit) {
            data <<= 4;
        } else {
            data = data & 0xf0;
        }
        outputLow4Bit ^= 0x01;
    }

    databus->drivebusvalue(time, data);
}

//并行模式处理管脚电平变化
void LCD12864B::parallelSimulate(ABSTIME time, DSIMMODES mode)
{
    BYTE data = 0;
    //debugPrint("parallelSimulate");
    
    if (en->isnegedge()) {        //E的下降沿到达,8位读写
        if (PIN_IS_LOW(rw)) {    //R/W为低表示写
            
            data = (BYTE)databus->getbusvalue();    //读数据
            
            if (addr4BitMode) { //4位模式
                if (nextCmdIsLow4Bit) { //低4位
                    currCmd = (currCmd & 0xf0) | (BYTE)((data >> 4) & 0x0f);
                    nextCmdIsLow4Bit = 0;
                } else { //高4位
                    currCmd = data & 0xf0;
                    nextCmdIsLow4Bit = 1;
                    return;  //等低4位才处理
                }
            } else {
                currCmd = data;
            }

            if (PIN_IS_HIGH(rs)) {    //RS为高表示数据
                storeData(currCmd);
            } else {        //RS为低表示命令
                if (extCmdMode) {
                    processExtCmd(currCmd);
                } else {
                    processBasicCmd(currCmd);
                }
            }
        } else {        //E的下降沿到达,R/W为高表示读数据
            databus->drivetristate(time);    //驱动总线为三态，数据在EN的上升沿开始输出
        }
    //E的上升沿到达，R/W为高表示读
    } else if (en->isposedge() && PIN_IS_HIGH(rw)) {
        if (PIN_IS_HIGH(rs)) {    //RS为高表示读RAM数据
            outputData(time, mode);

        } else {        //RS为低表示读状态
            if (currAddrType == ADDR_TYPE_GDRAM) {
                databus->drivebusvalue(time, gdramX & 0x7f);    //输出状态
            } else {
                databus->drivebusvalue(time, status);    //输出状态
            }
        }
    }
}

//串行模式处理管脚电平变化
void LCD12864B::serialSimulate(ABSTIME time, DSIMMODES mode)
{
    BYTE sidIsHigh;
    //debugPrint("serialSimulate");
    
    //cs为高才能传输数据
    if (PIN_IS_LOW(cs)) {
        srlByteIdx = 7;
        srlCmd = 0;
        srlLow4Bit = 0;
        currCmd = 0;
        srlNextIsData = 0;
        return;
    }

    if (sclk->isposedge()) {  //上升沿接收数据
        sidIsHigh = PIN_IS_HIGH(sid);
        if (sidIsHigh) {
            srlHighCnt++;
        } else {
            srlHighCnt = 0;
        }

        //5个连续的1开始传输
        if (srlHighCnt >= 5) {
            srlByteIdx = 7;
            srlCmd = 0;
            currCmd = 0;
            //srlHighCnt = 0;
            srlStarted = SRL_STARTED_RW;
            srlNextIsData = 0;
            srlLow4Bit = 0;
            return;
        }

        if (srlStarted == SRL_STARTED_RW) { //接收RW
            srlRw = sidIsHigh ? 1 : 0;
            srlStarted = SRL_STARTED_RS;
            return;
        } else if (srlStarted == SRL_STARTED_RS) { //接收RS
            if (sidIsHigh) {
                srlRs = 1;
                srlNextIsData = 1;
            } else {
                srlRs = 0;
                srlNextIsData = 2;
            }
            srlStarted = SRL_STARTED_ZERO;
            return;
        } else if (srlStarted == SRL_STARTED_ZERO) { //应该是零
            if (sidIsHigh) { //非法
                srlStarted = SRL_STARTED_INVALID;
                srlByteIdx = 7;
                srlCmd = 0;
                srlNextIsData = 0;
                srlLow4Bit = 0;
            } else {
                srlStarted = SRL_STARTED_CMD_DATA; //开始接收数据
                srlByteIdx = 7;
                srlCmd = 0;
                currCmd = 0;
                srlLow4Bit = 0;
                //debugPrint("srl started");
            }
            return;
        }

        if (srlStarted != SRL_STARTED_CMD_DATA) {
            return;
        }

        if (sidIsHigh) {
            SET_BIT(srlCmd, srlByteIdx);
        } else {
            CLR_BIT(srlCmd, srlByteIdx);
        }

        if (srlByteIdx == 0) { //已经接收了一个完整的字节
            srlByteIdx = 7;
            if (srlLow4Bit) { //低四位
                currCmd = (currCmd & 0xf0) | ((srlCmd >> 4) & 0x0f);
                srlLow4Bit = 0;
            } else {
                currCmd = srlCmd & 0xf0;
                srlLow4Bit = 1;
                return; //等待接收下一个字节再处理
            }
        } else {
            srlByteIdx--;
            return;
        }

        //开始处理逻辑
        if (srlNextIsData == 1) { //数据
            storeData(currCmd);
        } else if (srlNextIsData == 2) {   //命令
            if (extCmdMode) {
                processExtCmd(currCmd);
            } else {
                processBasicCmd(currCmd);
            }
        }
    }
}

//处理基本命令
void LCD12864B::processBasicCmd(BYTE cmd)
{
    //debugPrint("processBasicCmd", cmd);
    
    if (cmd == 0x01) { //清除显示
        memset(DDRAM, 0xff, sizeof(DDRAM)); //不能初始化为0，因为编码0x0000为第一个用户自定义字符
        ddramAddr = 0;
        newFlag = TRUE;
    } else if ((cmd == 0x02) || (cmd == 0x03)) { //DDRAM地址归零
        ddramAddr = 0;
    } else if ((cmd & 0xfc) == 0x04) { //设定写DDRAM后地址计数器加还是减
        if (TEST_BIT(cmd, 1)) {
            incAddr = 1;
        } else {
            incAddr = -1;
        }
    } else if ((cmd & 0xf8) == 0x08) { //显示状态开关
        displayOn = TEST_BIT(cmd, 2) ? 1 : 0;
        cursorOn = TEST_BIT(cmd, 1) ? 1 : 0;
        cursorInverse = TEST_BIT(cmd, 0) ? 1 : 0;
        newFlag = TRUE;
        //debugPrint("display control", cmd, displayOn);
    } else if ((cmd & 0xf0) == 0x10) { //游标或显示移位控制

    } else if ((cmd & 0xe0) == 0x20) { //功能设定
        if (srlStarted != SRL_STARTED_CMD_DATA) {
            if (addr4BitMode && TEST_BIT(cmd, 4)) {

                debugPrint("EXIT 4 bit mode");
            } else if (!addr4BitMode && !TEST_BIT(cmd, 4)) {
                debugPrint("ENTER 4 bit mode");
            }
            
            addr4BitMode = !TEST_BIT(cmd, 4);
        }
        extCmdMode = TEST_BIT(cmd, 2);
    } else if ((cmd & 0xc0) == 0x40) { //设定CGRAM地址
        currAddrType = ADDR_TYPE_CGRAM;
        cgramAddr = (cmd & 0x3f) % (sizeof(CGRAM) / 2);
        dataLowReceiving = 0;
        outputLow4Bit = 0;
        dummyRead = 1;
    } else if ((cmd & 0x80) == 0x80) { //设定DDRAM地址
        currAddrType = ADDR_TYPE_DDRAM;
        ddramAddr = cmd & 0x7f;
        dataLowReceiving = 0;
        outputLow4Bit = 0;
        dummyRead = 1;
    }
}

//处理扩充命令
void LCD12864B::processExtCmd(BYTE cmd)
{
    //debugPrint("processExtCmd", cmd);

    if (cmd == 0x01) { //待命模式

    } else if ((cmd & 0xfe) == 0x02) {

    } else if ((cmd & 0xe0) == 0x20) { //功能设定
        if (srlStarted != SRL_STARTED_CMD_DATA) {
            if (addr4BitMode && TEST_BIT(cmd, 4)) {
                debugPrint("EXIT 4 bit mode");
            } else if (!addr4BitMode && !TEST_BIT(cmd, 4)) {
                srlLow4Bit = 0;
                outputLow4Bit = 0;
                debugPrint("ENTER 4 bit mode");
            }

            addr4BitMode = !TEST_BIT(cmd, 4);
        }
        extCmdMode = TEST_BIT(cmd, 2);
        graphicOn = TEST_BIT(cmd, 1);
    } else if ((cmd & 0x80) == 0x80) { //设定GDRAM地址
        if (gdramXReceiving) {
            gdramX = cmd & 0x0f;
            gdramXReceiving = 0;
        } else {  //先接收Y地址
            gdramY = cmd & 0x3f;
            gdramXReceiving = 1;
        }
        dataLowReceiving = 0;
        outputLow4Bit = 0;
        currAddrType = ADDR_TYPE_GDRAM;
        dummyRead = 1;
    }
}

//可通过setcallback()设置在给定时间调用的回调函数
VOID LCD12864B::callback (ABSTIME time, EVENTID eventid)
{
}


//----------------------------------------------------------------------------
//绘图模型的实现

//当创建模型实例时被调用，做初始化工作
VOID LCD12864B::initialize (ICOMPONENT *cpt)
{ 
    //获取ICOMPONENT接口和初始化
    component = cpt;
    component->setpenwidth(0);
    component->setpencolour(BLACK);
    component->setbrushcolour(BLACK);

    //获取显示区域，模块有三个symbol，-1为整体外形，0为屏幕熄灭状态，1为屏幕亮起状态
    component->getsymbolarea(1, &lcdArea);

    //char* pri = component->getprop("PRIMITIVE");
    //if (strcmp("DIGITAL,LCD12864B_BIG5", pri) == 0) {
    //    hzCodePage = HZCODE_BIG5; //BIG5码
    //} else {
    //    hzCodePage = HZCODE_GB2312; //GB2312
    //}

    //计算每象素对应矩形的宽和高
    pixWidth = (float)(lcdArea.right - lcdArea.left - BLANK_WIDTH * 2 - SYM_LINEWIDTH * 2) / LCD_WIDTH;
    pixHeight = (float)(lcdArea.bottom - lcdArea.top - BLANK_WIDTH * 2 - SYM_LINEWIDTH * 2) / LCD_HEIGHT;

    //屏幕中间点
    lcdCenterX = lcdArea.right - lcdArea.left;
    lcdCenterY = lcdArea.bottom - lcdArea.top;
}

//被PROSPICE调用，返回模拟电气模型
ISPICEMODEL *LCD12864B::getspicemodel (CHAR *) 
{ 
    return NULL;
}


//被PROSPICE调用，返回数字电气模型
IDSIMMODEL  *LCD12864B::getdsimmodel (CHAR *) 
{    
    return this;
}


//当原理图需要重绘时被调用
VOID LCD12864B::plot (ACTIVESTATE state)
{
    //绘制LCD12864B_C元件基本图形
    component->drawsymbol(-1);

    //刷新LCD数据显示
    newFlag = TRUE;
    animate (0, NULL);
}

//画一个像素，当前只是画到内存
inline void LCD12864B::drawPixel(BYTE x, BYTE y)
{
    int index = y * LCD_WIDTH + x;
    if (index < sizeof(displayMemCache)) {
        displayMemCache[index] ^= 0x01;
    }
}


//当相应的电气模型产生活动事件时被调用，常用来更新图形
VOID LCD12864B::animate (INT element, ACTIVEDATA *data)
{
    signed int bit;
    int idx;
    BYTE x, y, xPos, yPos, high, low, xOff, t1, t2;
    WORD word;
    const unsigned char* pFont = NULL;
    
    if (newFlag) {    //当有新数据到达
        newFlag = FALSE;
        
        component->begincache(lcdArea);    //打开缓冲
        component->drawsymbol(1);        //显示LCD12864B_1符号（屏幕亮起符号）

        //显示关闭或正在复位
        if ((displayOn == 0) || PIN_IS_LOW(rst)) {
            component->endcache();
            return;
        }

        memset(displayMemCache, 0, sizeof(displayMemCache));

        //显示DDRAM数据，分上下两屏
        for (idx = 0; idx < 32; idx++) { //12864仅使用DDRAM前面一半的区域
            high = DDRAM[ddramAddrToIndex(idx)];
            low = DDRAM[ddramAddrToIndex(idx) + 1];
            if ((high == 0xff) && (low == 0xff)) {
                continue;
            }
            
            if (idx < 8) {
                yPos = 0;
            } else if ((idx >= 16) && (idx < 24)) {
                yPos = 16;
            } else if ((idx >= 8) && (idx < 16)) {
                yPos = 31;
            } else {
                yPos = 48;
            }

            xPos = (idx * 16) % LCD_WIDTH;

            if ((high == 0x00) && ((low == 0x00) || (low == 0x02) || (low == 0x04) || (low == 0x06))) { //CGRAM自定义字符
                pFont = (const unsigned char *)&CGRAM[low * 16];
            } else if (TEST_BIT(high, 7) && TEST_BIT(low, 7)) { //中文
                pFont = binarySearchHzCode(high, low); //获取点阵数据
            } else {
                pFont = NULL;
            }

            if (pFont != NULL) {
                //画16x16点阵
                for (xOff = 0; xOff < 16; xOff++) {
                    //左边一个字节
                    t1 = *pFont++;
                    t2 = *pFont++;
                    word = (t1 << 8) | t2;
                    //计算屏幕位置，这个比特位和实际相反，方便计算
                    for (bit = 0; bit < 16; bit++) {
                        if (TEST_BIT(word, (15 - bit))) {
                            drawPixel(xPos + bit, yPos);
                        }
                    }
                    yPos++;
                }
            } else if ((high >= 0x01) && (high <= 0x7f)) { //两个8x16的ASCII字符
                xPos = (idx * 16) % LCD_WIDTH;
                pFont = &chardot[(high - 0x01) * 16]; //获取点阵数据
                //画8x16点阵
                for (xOff = 0; xOff < 16; xOff++) {
                    //一个字节一行
                    t1 = *pFont++;
                    //计算屏幕位置，这个比特位和实际相反，方便计算
                    for (bit = 0; bit < 8; bit++) {
                        if (TEST_BIT(t1, (7 - bit))) {
                            drawPixel(xPos + bit, yPos);
                        }
                    }
                    yPos++;
                }

                //再一个ASCII字符
                if ((low >= 0x01) && (low <= 0x7f)) {
                    xPos += 8;
                    yPos -= 16;
                    pFont = &chardot[(low - 0x01) * 16]; //获取点阵数据
                    //画8x16点阵
                    for (xOff = 0; xOff < 16; xOff++) {
                        //一个字节一行
                        t1 = *pFont++;
                        //计算屏幕位置，这个比特位和实际相反，方便计算
                        for (bit = 0; bit < 8; bit++) {
                            if (TEST_BIT(t1, (7 - bit))) {
                                drawPixel(xPos + bit, yPos);
                            }
                        }
                        yPos++;
                    }
                }
            }
        }

        //显示GDRAM数据，分上下两屏，共使用了GDRAM前面一半的数据
        //            256 pixel (16 x 16)                    x
        //     ----------------------------------------------->
        // 64 |                     |                        |
        // p  |   上半屏 128x32     |     下半屏 128x32       |32 pixel
        // i  |                     |                        |
        // x  |-----------------------------------------------
        // e  |
        // l  |
        //    |
        //  y >
        for (y = 0; y < LCD_HEIGHT / 2; y++) {
            for (x = 0; x < LCD_ROW_LEN; x++) {
                //取出对应的两个字节，内部GDRAM每行32个字节
                idx = gdramXyToIndex(x, y);
                word = ((WORD)GDRAM[idx] << 8) | (WORD)GDRAM[idx + 1];
                if (word == 0) {
                    continue;
                }

                //内部坐标转换为一个字开始的屏幕坐标
                if (x >= LCD_ROW_LEN / 2) { //下半屏
                    xPos = (x - LCD_ROW_LEN / 2) * 16;
                    yPos = y + LCD_HEIGHT / 2;
                } else {
                    xPos = x * 16;
                    yPos = y;
                }

                //计算屏幕位置，这个比特位和实际相反，方便计算
                for (bit = 0; bit < 16; bit++) {
                    if (TEST_BIT(word, (15 - bit))) {
                        drawPixel(xPos + bit, yPos);
                    }
                }
            }
        }
        
        drawCacheToScreen();
        component->endcache();    //结束缓冲，显示数据
    }
}

//将缓存更新到屏幕
void LCD12864B::drawCacheToScreen(void)
{
    int left, top, right, bottom;
    for (int x = 0; x < LCD_WIDTH; x++) {
        for (int y = 0; y < LCD_HEIGHT; y++) {
            if (displayMemCache[y * LCD_WIDTH + x]) {
                left = (int)(BLANK_WIDTH + x * pixWidth + 1);
                top = -(int)(BLANK_WIDTH + y * pixHeight + 1); //转换坐标，原点在左上角
                right = left + (int)(pixWidth - 2);
                bottom = top - (int)(pixHeight - 2);
                component->drawbox(left, top, right, bottom);    //绘制1个象素点
            }
        }
    }
}


//用来处理键盘和鼠标事件
BOOL LCD12864B::actuate (WORD key, INT x, INT y, DWORD flags)  
{ 
    return FALSE; 
}

//汉字索引在 hzIndex[]，两个字节为一个汉字内码，第一个字节为“高位字节”，第二个字节为“低位字节”
//“高位字节”使用了0xA1–0xF7（把01–87区的区号加上0xA0），“低位字节”使用了0xA1–0xFE（把01–94加上0xA0）
//查询到的hzIndex[]索引乘以 hzdot[]每个汉字的点阵大小32即可获取点阵数据
extern "C" const unsigned char * getHzDot(BYTE codeHigh, BYTE codeLow)
{
    BYTE high, low;
    const unsigned char * pIdx;
    
    pIdx = hzIndex;
    for (int idx = 0; idx < hzNum; idx++) {
        high = *pIdx++;
        low = *pIdx++;
        if ((codeHigh == high) && (codeLow == low)) {
            return &hzdot[idx * 32]; //每个汉字16x16=32字节
        }
    }
    return NULL;
}

//二分查找汉字编码，要求汉字编码有序
extern "C" const unsigned char* binarySearchHzCode(BYTE codeHigh, BYTE codeLow)
{
    int left = 0, mid;
    int right = hzNum - 1;
    int hzCode = (codeHigh << 8) | codeLow;
    int code, index;

    while (left <= right)
    {
        mid = (left + right) / 2;

        index = mid * 2; //每个汉字两个字节

        code = (hzIndex[index] << 8) | hzIndex[index + 1];

        if (hzCode == code)
        {
            return &hzdot[mid * 32]; //每个汉字16x16=32字节
        }

        if (hzCode > code)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    return NULL; //未找到
}

#ifdef _DEBUG
void debugPrint(const char * text)
{
    std::ofstream log("C:\\logs\\LCD12864B.log", std::ofstream::app | std::ofstream::out);
    log << text << std::endl;
}
void debugPrint(const char * text, int val)
{
    char buf[100] = {0,};
    std::ofstream log("C:\\logs\\LCD12864B.log", std::ofstream::app | std::ofstream::out);
    sprintf(buf, "%s: 0x%x", text, val);
    log << buf << std::endl;
}
void debugPrint(const char * text, int val1, int val2)
{
    char buf[100] = {0,};
    std::ofstream log("C:\\logs\\LCD12864B.log", std::ofstream::app | std::ofstream::out);
    sprintf(buf, "%s: 0x%x, 0x%x", text, val1, val2);
    log << buf << std::endl;
}
void debugPrint(const char * text, int val1, int val2, int val3)
{
    char buf[100] = {0,};
    std::ofstream log("C:\\logs\\LCD12864B.log", std::ofstream::app | std::ofstream::out);
    sprintf(buf, "%s: 0x%x, 0x%x, 0x%x", text, val1, val2, val3);
    log << buf << std::endl;
}
void debugPrint(const char * text, int val1, int val2, int val3, int val4)
{
    char buf[100] = {0,};
    std::ofstream log("C:\\logs\\LCD12864B.log", std::ofstream::app | std::ofstream::out);
    sprintf(buf, "%s: 0x%x, 0x%x, 0x%x, 0x%x", text, val1, val2, val3, val4);
    log << buf << std::endl;
}
#else
void debugPrint(const char * text) {}
void debugPrint(const char * text, int val) {}
void debugPrint(const char * text, int val1, int val2) {}
void debugPrint(const char * text, int val1, int val2, int val3) {}
void debugPrint(const char * text, int val1, int val2, int val3, int val4) {}
#endif