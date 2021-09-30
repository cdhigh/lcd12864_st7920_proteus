//ST7920 LCD 驱动
#include "st7920.h"

//初始化
void LCD_init(void)
{
    LCD_DATA_DDR_OUTPUT();

    _delay_ms(50);
    LCD_write_command(0x33);
    _delay_ms(1);
    LCD_write_command(0x32);
    _delay_ms(1);
#if LCD_INTERFACE == MODE_4BIT
    LCD_write_command(0x28);
    _delay_ms(1);
    LCD_write_command(0x28);
    _delay_ms(1);
#endif
    LCD_write_command(0x08);  //显示关
    _delay_ms(1);             //大于100uS的延时程序
    LCD_write_command(0x10);  //光标设置
    _delay_ms(1);             //大于100uS的延时程序
    LCD_write_command(0x0C);  //显示开
    _delay_ms(1);             //大于100uS的延时程序
    LCD_write_command(0x01);  //Display Clear
    _delay_ms(10);            //大于10mS的延时程序
    LCD_write_command(0x06);  //光标从右向左加1位移动
    _delay_ms(10);            //大于10mS的延时程序
}

//写命令
void LCD_write_command(unsigned char command)
{
#if LCD_INTERFACE == MODE_SERIAL
    CLR_BIT(LCD_SCLK_PORT, LCD_SCLK_PIN);
    LCD2_spi_write_byte(0xf8);
    asm("nop");
    LCD2_spi_write_byte_standard(command);
#else
    LCD_CMD_MODE();   //RS=0
    LCD_write_byte(command);
    _delay_ms(1);
#endif
}

//写数据
void LCD_write_data(unsigned char data)
{
#if LCD_INTERFACE == MODE_SERIAL
    CLR_BIT(LCD_SCLK_PORT, LCD_SCLK_PIN);
    LCD2_spi_write_byte(0xfa);
    asm("nop");
    LCD2_spi_write_byte_standard(data);
#else
    LCD_DATA_MODE();    //RS=1
    LCD_write_byte(data);
#endif
}

//并口模式写一个字节到控制器
void LCD_write_byte(unsigned char data)
{
#if LCD_INTERFACE == MODE_4BIT
    LCD_write_half_byte(data);
    data <<= 4;
    LCD_write_half_byte(data);
#else
    LCD_DATA_PORT = data;
    LCD_RW_LOW();
    LCD_EN_HIGH();     //EN端产生一个由低电平变高电平，写LCD
    _delay_us(10);
    LCD_EN_LOW();      //EN端产生一个由高电平变低电平，写LCD
    _delay_us(10);
    LCD_RW_HIGH();
#endif
}

//4位模式下写半个字节（高4位）
void LCD_write_half_byte(unsigned char half_byte)
{
    if (half_byte & 0x80)
        SET_BIT(LCD_DATA_PORT_D7, LCD_DATA_PIN_D7);
    else
        CLR_BIT(LCD_DATA_PORT_D7, LCD_DATA_PIN_D7);
    
    if (half_byte & 0x40)
        SET_BIT(LCD_DATA_PORT_D6, LCD_DATA_PIN_D6);
    else
        CLR_BIT(LCD_DATA_PORT_D6, LCD_DATA_PIN_D6);
    
    if (half_byte & 0x20)
        SET_BIT(LCD_DATA_PORT_D5, LCD_DATA_PIN_D5);
    else
        CLR_BIT(LCD_DATA_PORT_D5, LCD_DATA_PIN_D5);
    
    if (half_byte & 0x10)
        SET_BIT(LCD_DATA_PORT_D4, LCD_DATA_PIN_D4);
    else
        CLR_BIT(LCD_DATA_PORT_D4, LCD_DATA_PIN_D4);
    
    LCD_RW_LOW();
    LCD_EN_HIGH();         //EN端产生一个由低电平变高电平，写LCD
    _delay_us(10);
    LCD_EN_LOW();          //EN端产生一个由高电平变低电平，写LCD
    _delay_us(10); 
    LCD_RW_HIGH();
}

//串行模式下写一个字节
void LCD2_spi_write_byte(unsigned char data)
{
    unsigned char i;
    for(i=0; i<8; i++)
    {
        if (data & 0x80)
            SET_BIT(LCD_SID_PORT, LCD_SID_PIN);
        else
            CLR_BIT(LCD_SID_PORT, LCD_SID_PIN);

        asm("nop");
        asm("nop");
        CLR_BIT(LCD_SCLK_PORT, LCD_SCLK_PIN);
        asm("nop");
        SET_BIT(LCD_SCLK_PORT, LCD_SCLK_PIN);
        data <<= 1;
    }
}

//标准处理，一个字节要拆成两个字节发送
void LCD2_spi_write_byte_standard(unsigned char data)
{
    LCD2_spi_write_byte(data & 0xf0);
    LCD2_spi_write_byte((data << 4) & 0xf0);
}

//并口模式下读数据
unsigned char LCD_read_data(void)
{
    unsigned char data;

    LCD_DATA_MODE();
    data = LCD_read_byte();

#if LCD_INTERFACE == MODE_4BIT
    data = (data & 0xf0) | ((LCD_read_byte() >> 4) & 0x0f);
#endif
    return data;
}

//并口模式下读状态
unsigned char LCD_read_status(void)
{
    unsigned char data;
    LCD_CMD_MODE();
    data = LCD_read_byte();
#if LCD_INTERFACE == MODE_4BIT
    data = (data & 0xf0) | ((LCD_read_byte() >> 4) & 0x0f);
#endif
    return data;
}

//并口模式下读一个字节，如果是4bit模式，则只有高4位是有效的
unsigned char LCD_read_byte(void)
{
    unsigned char data;
    LCD_DATA_DDR_INPUT();
    LCD_RW_HIGH();
    
    LCD_EN_LOW();         //EN端产生上升沿，ST7920开始输出数据
    _delay_us(10);
    LCD_EN_HIGH();
    _delay_us(10);

    data = LCD_DATA_PIN;
    LCD_EN_LOW();
    LCD_DATA_DDR_OUTPUT();
    return data;
}

//设置字符模式的地址
//第一行首地址：0x80
//第二行首地址：0x80+0x10 (0x90)
//第三行首地址：0x80+0x08 (0x88)
//第四行首地址：0x80+0x10+0x08 (0x98)
void LCD_set_text_address(unsigned int rowCol)
{
    unsigned char start = 0x80;
    unsigned char row = (rowCol >> 8) & 0xff;
    unsigned char col = rowCol & 0xff;

    if (row == 1) {
        start = 0x90;
    }
    if (row == 2) {
        start = 0x88;
    }
    if (row == 3) {
        start = 0x98;
    }
    LCD_write_command(start + col);
}

//将字符模式的行列号转换为内部的XY坐标，返回的高字节为X，低字节为Y
//row (HIGH_BYTE(rowCol)): 行号，0-3
//col (LOW_BYTE(rowCol)): 列号，0-7
unsigned int LCD_rowCol_to_inter_Xy(unsigned int rowCol)
{
    unsigned char row = (rowCol >> 8) & 0x03;
    unsigned char col = rowCol & 0x07;
    unsigned char x = col + 8 * (unsigned char)(row / 2);
    unsigned char y = (row * 16) & 0x1f;
    return (x << 8) | y;
}

//设置绘图模式的地址
//x: 0-127 (注意x必须为16的整数倍)
//y:0-63
void LCD_set_graphic_address(unsigned char x, unsigned char y)
{
    unsigned char xWord, downPage, yInter;

    x &= 0x7f;
    y &= 0x3f;
    xWord = x / 16;           //内部X地址，一个地址管16位
    downPage = y / 32;        //0:上半屏 1:下半屏
    yInter = y & 0x1f;        //内部Y坐标
    
    LCD_write_command(0x80 + yInter); //先设置Y地址
    LCD_write_command(0x80 + xWord + 8 * downPage);
}

//显示清屏函数
void LCD_clear(void)
{
    unsigned char x, y;
    LCD_write_command(0x08);  //避免清屏过程中闪烁，先关显示
    _delay_us(100);
    LCD_write_command(0x01);  //清DDRAM

    //清GDRAM,12864仅用了一半的GDRAM，清一半即可
    //如果没有使用到绘图GDRAM，也可以不清GDRAM，省点时间和代码空间
    LCD_startGraphic();
    for (y = 0; y < 32; y++)
    {
        LCD_write_command(0x80 + y); //y
        LCD_write_command(0x80 + 0); //x
        for (x = 0; x < 16; x++)
        {
            LCD_write_data(0x00);
            LCD_write_data(0x00);
        }
    }
    LCD_endGraphic();

    LCD_write_command(0x0C);  //显示开
    _delay_ms(10);
}

//开启扩展命令的绘图指令
void LCD_startGraphic(void)
{
#if LCD_INTERFACE == MODE_4BIT
    LCD_write_command(0x24);          //扩展指令集
    LCD_write_command(0x26);          //绘图命令开启
#else
    LCD_write_command(0x34);          //扩展指令集
    LCD_write_command(0x36);          //绘图命令开启
#endif
}

//结束扩展命令的绘图指令(可选)
void LCD_endGraphic(void)
{
#if LCD_INTERFACE == MODE_4BIT
    LCD_write_command(0x26); //绘图命令关闭
    LCD_write_command(0x20); //回到基本指令
#else
    LCD_write_command(0x36); //绘图命令关闭
    LCD_write_command(0x30); //回到基本指令
#endif
}

//反白或取消反白对应16X16区域，一般用于菜单选择
//row (HIGH_BYTE(rowCol)): 行号，0-3
//col (LOW_BYTE(rowCol)): 列号，0-7
//charNum: 要反白的字符数量，注意不要超过行末
//ST7920内置的反白作用不大，它只能整行反白，而且反白第一行，第三行也会一起反白
//所以此函数使用另外的方法实现：绘图区对应的位全部写0xff，异或后即可反白
void LCD_Inverse_16X16(unsigned int rowCol, unsigned char charNum, unsigned char reverse)
{
    unsigned char i, ch;
    unsigned int xy = LCD_rowCol_to_inter_Xy(rowCol);
    unsigned char x = (xy >> 8) & 0xff;
    unsigned char y = xy & 0x3f;

    LCD_startGraphic();
    for (i = 0; i < 16; i++) //把对应字符的GDRAM全部写成0xff，异或后就是反白
    {
        LCD_write_command(0x80 + y + i);   //先写垂直地址y
        LCD_write_command(0x80 + x);       //x
        for (ch = 0; ch < charNum; ch++)
        {
            LCD_write_data(reverse ? 0xff : 0x00);
            LCD_write_data(reverse ? 0xff : 0x00);
        }
    }
    LCD_endGraphic();
}

//显示一个字符，12864可以显示4行，每行8个汉字或每行16个字母
//col: 列号，0-15
//row: 行号，0-3
//字符编码：小于0x80的为ASCII，否则为汉字，汉字编码高字节在左边
void LCD_write_char(unsigned int rowCol, unsigned int code)
{
    unsigned char high = (code >> 8) & 0xff;
    unsigned char low = code & 0xff;
    LCD_set_text_address(rowCol);
    if (code > 0x80)
    {
        LCD_write_data(high);
    }
    LCD_write_data(low);
}

//显示一个字符串，注意不要超过一行的长度（8个汉字或16个字母）
//rowCol: 左8位为行(0-3)，右8位为列(0-15)
void LCD_write_string(unsigned int rowCol, const char * p)
{
    LCD_set_text_address(rowCol);
    while (*p != 0)
    {
        LCD_write_data(*p);
        p++;
    }
}

//只有并行接口能读ST7920，如果串行接口需要打点，则需要开辟RAM缓冲区
#if LCD_INTERFACE != MODE_SERIAL
//在对应X/Y位置显示一个点
void LCD_write_dot(unsigned char x, unsigned char y)
{
    unsigned char xBit, high, low;

    xBit = x & 0x0f;
    LCD_startGraphic();
    LCD_set_graphic_address(x, y);
    LCD_read_data();    //根据手册，设置完地址后的第一次读操作返回的数据无效

    high = LCD_read_data();
    low = LCD_read_data();

    //回写
    LCD_set_graphic_address(x, y);
    if (xBit < 8)   //低8位
    {
        LCD_write_data(high | (0x01 << (7 - xBit)));
        LCD_write_data(low);
    }
    else
    {
        LCD_write_data(high);
        LCD_write_data(low | (0x01 << (15 - xBit)));
    }
    LCD_endGraphic();
}
#endif
