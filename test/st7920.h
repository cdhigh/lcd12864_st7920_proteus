//ST7920 LCD 驱动，使用AVR GCC编译
//Author: cdhigh <2021-09-22>
//ST7920的绘图结构：左上角为坐标原点，写入的字节表示从某个坐标开始的8个横向像素，
//每行 128/8=16 个字节，每个字节左边为最低位，右边为最高位，纵向64行。
//DDRAM地址
//0x80  0x81  0x82  0x83  0x84  0x85  0x86  0x87    //第一行汉字位置
//0x90  0x91  0x92  0x93  0x94  0x95  0x96  0x97    //第二行汉字位置
//0x88  0x89  0x8a  0x8b  0x8c  0x8d  0x8e  0x8f     //第三行汉字位置
//0x98  0x99  0x9a  0x9b  0x9c  0x9d  0x9e  0x9f     //第四行汉字位置
#ifndef __ST7290_H_
#define __ST7290_H_

#include <avr/io.h>
#include <util/delay.h>

#define MODE_8BIT       8  //8bit并行模式 
#define MODE_4BIT       4  //4bit并行模式
#define MODE_SERIAL     1  //串行-PSB接地

//定义使用的接口模式，MODE_8BIT, MODE_4BIT, MODE_SERIAL
#define LCD_INTERFACE   MODE_SERIAL

//串口模式下的管脚对应：RS-CS(一般直接接电源),RW-SID,E-SCLK
#define LCD_SID_PORT     PORTB  //SID=RW
#define LCD_SID_PIN      0
#define LCD_SCLK_PORT    PORTB  //SCLK=E
#define LCD_SCLK_PIN     1

//8位模式
#define LCD_DATA_DDR     DDRB
#define LCD_DATA_PIN     PINB
#define LCD_DATA_PORT    PORTB

//8/4bit公用
#define LCD_RS_PORT      PORTD
#define LCD_RS_PIN       2
#define LCD_EN_PORT      PORTD
#define LCD_EN_PIN       0
#define LCD_RW_PORT      PORTD
#define LCD_RW_PIN       1

//四位模式，RS/EN/RW使用上面的宏定义
#define LCD_DATA_PORT_D7 LCD_DATA_PORT
#define LCD_DATA_PORT_D6 LCD_DATA_PORT
#define LCD_DATA_PORT_D5 LCD_DATA_PORT
#define LCD_DATA_PORT_D4 LCD_DATA_PORT
#define LCD_DATA_PIN_D7  7
#define LCD_DATA_PIN_D6  6
#define LCD_DATA_PIN_D5  5
#define LCD_DATA_PIN_D4  4

#define LCD_EN_HIGH()      LCD_EN_PORT |= (1<<LCD_EN_PIN)
#define LCD_EN_LOW()       LCD_EN_PORT &=~(1<<LCD_EN_PIN)

#define LCD_CMD_MODE()     LCD_RS_PORT &=~(1<<LCD_RS_PIN)
#define LCD_DATA_MODE()    LCD_RS_PORT |= (1<<LCD_RS_PIN)

#define LCD_RW_HIGH()      LCD_RW_PORT |= (1<<LCD_RW_PIN)
#define LCD_RW_LOW()       LCD_RW_PORT &=~(1<<LCD_RW_PIN)

#define LCD_DATA_DDR_OUTPUT()  LCD_DATA_DDR = 0xff
#define LCD_DATA_DDR_INPUT()   LCD_DATA_DDR = 0x00

void LCD_clear(void);
void LCD_init(void);
void LCD_write_command(unsigned char command);
void LCD_write_data(unsigned char data);
void LCD_write_byte(unsigned char byte);
void LCD_write_half_byte(unsigned char half_byte);
void LCD2_spi_write_byte(unsigned char data);
void LCD2_spi_write_byte_standard(unsigned char data);
unsigned char LCD_read_data(void);
unsigned char LCD_read_status(void);
unsigned char LCD_read_byte(void);
void LCD_startGraphic(void);
void LCD_endGraphic(void);
void LCD_Inverse_16X16(unsigned int rowCol, unsigned char charNum, unsigned char reverse);
unsigned int LCD_rowCol_to_inter_Xy(unsigned int rowCol);
void LCD_set_text_address(unsigned int rowCol);
void LCD_set_graphic_address(unsigned char x, unsigned char y);
void LCD_write_char(unsigned int rowCol, unsigned int code);
void LCD_write_string(unsigned int rowCol, const char * p);

#if LCD_INTERFACE != MODE_SERIAL
void LCD_write_dot(unsigned char x, unsigned char y);
#endif

#define BYTE_BIT(bitno) (1 << (bitno))
#define TEST_BIT(value, bitno) ((1 << (bitno)) & (value))
#define SET_BIT(value, bitno)  ((value) |= (1 << (bitno)))
#define CLR_BIT(value, bitno)  ((value) &= ~(1 << (bitno)))

//两个字节凑成行列整型
//为了更好的区分使用X/Y和ROW/COL，使用ROW/COL的都是一个整型参数，X/Y为两个字节参数
#define ROW_COL(r, c) (((r) << 8) | (c & 0xff))

#endif
