/***********lcd1602�����ֶ���***********************/
#define CLR_SCREEN 0x01  //����
#define CURSOR_CLR 0x02  //����λ
#define INPUT_SET1  0x04  //AC��1���������1���ַ�����
#define INPUT_SET2  0x05  //AC��1����겻�����ַ�����1��
#define INPUT_SET3  0x06  //AC��1���������1���ַ�����
#define INPUT_SET4  0x06  //AC��1����겻�����ַ�����1��
#define DISPLAY_SET 0x0c   //��ʾ���ƣ�00001_ _ _��LCD��������/1���������������/1�У������˸����/1��˸
#define CURSOR_SET  0x18      //����ƶ���0x10������ƣ�0x14���ƣ�0x18�ַ����ƣ�0x1c����
#define FUNCTION_SET 0x38         //��������001_ _ _00,���߳���/1��8λ���ַ�����ʾ��Ŀ/1��2�У��������/1��5*10���壬0��5*7
void write_CMD(char CMD,char buzy);
void write_Data(char Data);
void LCD1602_init(void);
