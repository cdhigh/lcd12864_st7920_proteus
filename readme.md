## 基于ST7920的LCD12864模块 proteus VSM 仿真模型

## 开发初衷  
网络上已经有了一个12864A(ST7920)仿真模型，但其功能非常有限，不支持4位并行接口，不支持串行接口，不支持ASCII字母和数字显示，也不支持绘图显示，所以我就自己动手，实现了此模型，并分享源代码，方便有需要的朋友修改支持其他规格的器件，仓库托管于 <https://github.com/cdhigh/lcd12864_st7920_proteus>。

## 当前特性  
* 支持接口：并行8位、并行4位、串行
* 支持字符：汉字(内置GB2312字库全部汉字)、数字、字母（CGROM/HCGROM/DDRAM）
* 支持绘图和自定义字符（GDRAM/CGRAM）
* 支持图文混排，和实际模块处理一致，图文重叠的像素将使用异或方法显示

![ScreenShoot1](https://github.com/cdhigh/lcd12864_st7920_proteus/blob/master/scr1.png)
![ScreenShoot2](https://github.com/cdhigh/lcd12864_st7920_proteus/blob/master/scr2.png)
![ScreenShoot3](https://github.com/cdhigh/lcd12864_st7920_proteus/blob/master/scr3.png)
![Screen GIF](https://github.com/cdhigh/lcd12864_st7920_proteus/blob/master/scrShoot.gif)

## 使用方法  
* 将 release/MODELS/LCD12864B.DLL 拷贝到 proteus 的 MODELS 目录
* 将 release/LIBRARY 里的文件拷贝到 proteus 的 LIBRARY 目录
* 在 proteus 里面搜索 12864B 即可找到此器件

## 文件说明  
* 使用 proteus 8.1 及以上版本打开 test/test7920.pdsprj，即可进行功能演示，使用ATMEGA328P为MCU，编译环境为 Atmel studio 6.2
* 12864B.pdsprj 为模型原型，用于创建可显示的 PROTEUS VSM 模型
* DLL源码位于 LCD12864B_SRC 目录，使用 VS2010 及以上版本编译

## 已知BUG或注意事项  
* 有的PROTEUS版本(比如8.6)需要将LCD12864B模块中间的显示区域的左上角移动到原理图的坐标原点才能正常显示，但有的版本在任何位置都能正常显示(比如8.1)，原因不明，如果有朋友能解决，可以提一个pull request。
* 小于0x20的ASCII不能正常显示，这不是BUG，而是没有对小于0x20的字符进行取模，后续添加字模即可。
* 实际硬件模块内置汉字数比GB2312多一些，如果您的生僻汉字没有正常显示，可能其不在GB2312编码表中，这种情况忽略即可，不影响仿真。
* 汉字和ASCII混排时注意连续的ASCII字符数量必须为偶数，不是偶数的请在后面添加一个空格，这是ST7920的设计限制。
* 没弄明白 ICON RAM 是怎么回事，没有实现。
* 当前支持GB2312编码的汉字，如果BIG5编码的需求，请帮忙进行BIG5编码的汉字取模。  
  取模说明（请参照gb2312.h）：  
  1. 字体大小 16x16  
  2. 横向取模，高位在前，先横向两个字节，再纵向逐行取模，每个汉字32个字节  
  3. 生成字模索引表，每两个字节为一个汉字内码，先高字节再低字节  
  
## License  
* GPLV3
