/***********lcd1602命令字定义***********************/
#define CLR_SCREEN 0x01  //清屏
#define CURSOR_CLR 0x02  //光标归位
#define INPUT_SET1  0x04  //AC减1，光标左移1格，字符不动
#define INPUT_SET2  0x05  //AC减1，光标不动，字符右移1格
#define INPUT_SET3  0x06  //AC加1，光标右移1格，字符不动
#define INPUT_SET4  0x06  //AC加1，光标不动，字符左移1格
#define DISPLAY_SET 0x0c   //显示控制，00001_ _ _，LCD开关设置/1开，光标有无设置/1有，光标闪烁设置/1闪烁
#define CURSOR_SET  0x18      //光标移动，0x10光标左移，0x14右移，0x18字符左移，0x1c右移
#define FUNCTION_SET 0x38         //功能设置001_ _ _00,总线长度/1是8位，字符行显示数目/1是2行，字体控制/1是5*10字体，0是5*7
void write_CMD(char CMD,char buzy);
void write_Data(char Data);
void LCD1602_init(void);
