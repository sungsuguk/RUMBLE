#include<reg51.h>
#include"8259a.h"
#include"LCD_1602.h"
#include"CH452.h"
#include<math.h>
//#include<stdlib.h>
//#include<stdio.h>
#define uchar unsigned char
#define uint unsigned int
sbit sw6=P1^7;
sbit sw5=P1^6;
code uchar key_table[4][4]={{7,8,9,'+'},
                            {4,5,6,'-'},
							{1,2,3,'*'},
							{0,'.','=','/'}};//按键编码值

xdata char key_num='0',num1[15]={0},num2[15]={0};
char count1=0,count2=0,flag_node1=0,flag_node2=0,flag=0,flag_node=0,flag_server2=0;
void decimal_oprater();

/************************************/
void delay(uint ms)//延时函数
{
uint i;
while(ms--)
for(i=0;i<1000;i++);

}
void CH452_Display_Init(void)//ch452初始化   
{   
        CH452_SDA_SET;   
        CH452_SCL_SET;   
        CH452_Write(CH452_RESET);           //复位显示    
        delay(30);   
        CH452_Write(CH452_BCD);             //设置显示方式    
        CH452_Write(CH452_SYSON2);//打开显示和键盘   
   
} 

void init_INT()//8051外部中断1初始化
 { EX1=1;
   EA=1;
  }
 //*********结果显示*********
 void display(uchar *dat,uchar line)
 {char i;
  i=14;
 while(dat[i]==0&&dat[i-1]!='.'&&i>0)i--;//整数前为0的处理
    if(!line)
      write_CMD(0x8f-i,1);//第一行显示首地址
    else 
	   write_CMD(0xcf-i,1);//第二行显示首地址
    for(;i>=0;i--)
       {
         if(dat[i]<=9&&dat[i]>=0)
	         write_Data(dat[i]|48);//数值显示
	     else 
		  write_Data(dat[i]);//字符直接显示
	    }  
 
 
 }
//**************小数处理**************
void decimal_oprater()
{uchar i,j;

if(flag_node1!=0||flag_node2!=0||num1[0]=='.'||num2[0]=='.')//其中有一个数是小数
   { 
	   if(flag_node1==0&&num1[0]!='.')         //数组1没小数点
	        { for(j=14;j>0;j--)
              num1[j]=num1[j-1];
               num1[0]='.';
		      count1++;
            }
	
	  if(flag_node2==0&&num2[0]!='.')       //数值2没小数点
	       { for(j=14;j>0;j--)
                 num2[j]=num2[j-1];
              num2[0]='.';
		      count2++;
            }
     if(flag_node1>flag_node2)//实现小数位对齐
             {   for(i=0;i<flag_node1-flag_node2;i++)
                   { for(j=14;j>0;j--)
                         num2[j]=num2[j-1];
                      num2[0]=0;
				    }
			     count2+=flag_node1-flag_node2;//小数对齐后，数2的长度
             
             }
	   else 
            { for(i=0;i<(flag_node2-flag_node1);i++)//移位实现两小数对齐
                { for(j=14;j>0;j--)
                     num1[j]=num1[j-1];
                  num1[0]=0;
                  }
              count1+=flag_node2-flag_node1;//小数对齐后，数1的长度
	
            
		    }

	}

}
//*************加法****************
void add(void)
{char i,temp=0;
decimal_oprater();//实现小数点对齐
count1=(count1>count2?count1:count2);//得到最长的计数  
for(i=0;i<count1;i++)//数组从低位到高位不断滚动相加
       {if(num1[i]=='.')i++;//小数点跳过
        num1[i]=num1[i]+num2[i]+temp;
        temp=num1[i]/10;//进位值
        num1[i]=num1[i]%10;//本位值
	    }
    

  while(temp)//最后进位值不为0
         {count1++;
         num1[i]=num1[i]+temp%10;
	     temp=temp/10; 
         i++;
         }
  for(i=0;i<14;i++)
    if(num1[i]=='.')
       {    while(num1[0]==0&&num1[0]!='.')//小数后为0的处理
              {for(i=0;i<14;i++)
                 num1[i]=num1[i+1];
                num1[14]=0;
				count1--;
               }
			   if(num1[0]=='.')
			   {for(i=0;i<14;i++)
                 num1[i]=num1[i+1];
                num1[14]=0;
				count1--;
				}
	     break;
	   }
display(num1,0);//在第一行显示结果

}
//****************减法*************
void sub(void)
 {char i,temp=0,t=0;
   decimal_oprater();//小数对齐处理
     if(count1<count2||(num1[count1-1]<num2[count2-1]&&count1==count2))//让num1为大值
        {  count1=count2; 
		   for(i=0;i<count2;i++)
               {t=num1[i];
		         num1[i]=num2[i];
                 num2[i]=t;
		        }
			t=1;
		 }
      for(i=0;i<count1;i++)
        { if(num1[i]=='.')i++;
	      num1[i]-=temp;
	      if(num1[i]<num2[i])
	            {temp=1;
                 num1[i]=num1[i]+10-num2[i];
		         }
          else  
	            {temp=0;
	             num1[i]=num1[i]-num2[i];
		         }
	      }
  
if(t)
 {i=14;
  while(num1[i-1]==0&&num1[i-2]!='.')i--;
  num1[i]='-';
 }
 for(i=0;i<14;i++)
    if(num1[i]=='.')
       {    while(num1[0]==0&&num1[0]!='.')//小数后为0的处理
              {for(i=0;i<14;i++)
                 num1[i]=num1[i+1];
                num1[14]=0;
				count1--;
               }
			   if(num1[0]=='.')
			   {for(i=0;i<14;i++)
                 num1[i]=num1[i+1];
                num1[14]=0;
				count1--;
				}
	     break;
	   }
display(num1,0);
 

 }
//************乘法******************
void multiply(void)
{xdata char i,j,temp=0,count3=0,num3[20]={0};
 if(flag_node1!=0||num1[0]=='.') 
   {for(i=flag_node1;i<count1-1;i++)
        num1[i]=num1[i+1];
	count1--;	
   }
if(flag_node2!=0||num2[0]=='.') 
 { for(i=flag_node2;i<count2-1;i++)
    num2[i]=num2[i+1];
  count2--;
	}
  
for(i=0;i<count2;i++)
   {  temp=0;
       
      for(j=0;j<count1;j++)
        {
         num3[count3+j]+=num1[j]*num2[i]+temp;
         temp=num3[count3+j]/10;
         num3[count3+j]=num3[count3+j]%10;
        }
       while(temp)
           {
            num3[count3+j]=num3[count3+j]+temp%10;
            temp=temp/10; 
            j++;
            }        
     count3++;
    }
    count1=count3+j-1;
temp=flag_node1+flag_node2;
i=0;
if(temp)//乘积有小数
  {
  while(num3[i]==0)i++;
  if(i>=temp)
  i=temp;
  else 
   {for(j=19;j>temp;j--)
    num3[j]=num3[j-1];
    num3[temp]='.';
	count1++;
	}
count1=count1-i+1;
  }
for(j=0;j<15;j++)
 if(j<count1)
num1[j]=num3[j+i];
else num1[j]=0;   
display(num1,0);
}
//***************除法****************
void divide(void)

{ xdata unsigned long int number1=0,number2=0,temp=0;
  char i,j=0,k=0;
  decimal_oprater();//实现小数点对齐
    for(i=count1-1;i>=0;i--)//抛弃小数点求num1
       {if(num1[i]=='.'){num1[i]=0;i--;}
	   if(i>=0)
        { number1=number1*10+num1[i];
         num1[i]=0;
		}
       }
for(i=count2-1;i>=0;i--)//抛弃小数点求num2
       {if(num2[i]=='.')i--;
	   if(i>=0)
	   number2=number2*10+num2[i];
	    }
temp=number1%number2;  //余数 
number1=number1/number2;//整除结果

i=0;
while(number1)//整数部分写入数组
  {num1[i]=number1%10;
   number1=number1/10;
    i++;//记录写入num1数组的个数
    }
if(temp)//如果有余数
   {for(j=14;j>0;j--)
    num1[j]=num1[j-1];//数组依次移一位
    num1[0]='.';//低位写上小数点
    i++;
    }
while(temp!=0&&k<6)//保留6位小数
{temp=temp*10;
for(j=14;j>0;j--)
    num1[j]=num1[j-1];//每要写入一位，数组依次移一位

    num1[0]=temp/number2;//写入小数部分
    temp=temp%number2;
   i++;//记录写入num1数组的个数
   k++;
}

count1=i;//标记结果实际长度
display(num1,0);//显示结果
}


//***********数值计算*******************
void counter()
{uchar i;
  for(i=0;i<15;i++)//查找数1的小数点位置
   if(num1[i]=='.'){ flag_node1=i;break;}
  for(i=0;i<15;i++)////查找数2的小数点位置
    if(num2[i]=='.'){ flag_node2=i;break;}
 write_CMD(CLR_SCREEN,1);
 write_Data('=');
  switch(flag)
   {case '+':add(); break;
    case '-':sub();break;
    case '*':multiply();break;
	 case '/':divide();break;
    }

for(i=0;i<15;i++)//清空源操作数
num2[i]=0;
flag=0;//运算符标记清零
flag_node1=0;
flag_node2=0;//源操作数小数点标记位清0；
count2=0;//num2数组计数清零
}
/**************按键中断服务函数*****************************/
void server_2(void)
{char keycode,a,b;

keycode=CH452_Read();
a=keycode>>3&0x07;
b=keycode&0x07;
key_num=key_table[3-a][3-b];//根据按键值查表获取按键码

if(key_num<=9&&key_num>=0||key_num=='.')//为数值，或小数点
  { if(!flag)
         { for(a=count1;a>0;a--)
	           num1[a]=num1[a-1];
		    num1[0]=key_num;
		    display(num1,0);
 		    if(count1<14)
			  count1++; //目的操作数实际长度
		  }
    
	else 
     	{ for(a=count2;a>0;a--)
	             num2[a]=num2[a-1]; 
		  num2[0]=key_num;
          display(num2,1);
	      if(count2<14) 
		  count2++;//源操作数实际长度
	
		}
   }
else if(key_num=='='){if(flag)counter();}
else {flag=key_num;write_CMD(0xc0,1); write_Data(key_num);}//标记出运算符，并在第二行开始处显示

}
/**********外部中断1服务程序******/
void Int1(void)interrupt 2 
{
  uchar Int_num;

 Int_num=ADR_8259AO;//获取中断向量号
 Int_num=ADR_8259AO;
 Int_num&=0x07;

 switch(Int_num)
   {
    case 0x02:flag_server2=1;break;//按键中断服务
   
    }

}
main()
{char a;

LCD1602_init();
CH452_Display_Init();
init_INT();
Init8259a ();
write_CMD(0x8f,1); 
write_Data('0');
  while(1)
  {   if(flag_server2){flag_server2=0;server_2();}
      if(!sw5)//清零
         {delay(1);
 		 if(!sw5)
		  {for(a=0;a<15;a++)
		   {num1[a]=0;
		     num2[a]=0;
		    }
		      flag=0;
			  flag_node1=0;
			  flag_node2=0;
              count1=0;
              count2=0;
		      write_CMD(CLR_SCREEN,1);
			  write_CMD(0x8f,1); 
              write_Data('0');
			 }

		 }
	  if(!sw6)//实现退位
	    {delay(50);
	        if(sw6&&count1>=0&&count2>=0)
		       {if(!flag)
				     {if(count1>0)
					  count1--;
					   for(a=0;a<13;a++)
				       num1[a]=num1[a+1];
					   
					  
		              }
                  else
				     {if(count2>=1)
					    count2--;
					    for(a=0;a<13;a++)
				        num2[a]=num2[a+1];
					     
					  
				      }
				
					 write_CMD(CLR_SCREEN,1);
 				   write_CMD(0x8f-count1+1,1);
                    for(a=count1-1;a>=0;a--)
                    if(num1[a]>=0&&num1[a]<=9) write_Data(num1[a]|48);
			        else write_Data(num1[a]);
				   write_CMD(0xcf-count2+1,1);
                     for(a=count2-1;a>=0;a--)
                    if(num2[a]>=0&&num2[a]<=9) write_Data(num2[a]|48);
			         else write_Data(num2[a]);

			    } 
         
		 }
  
	 }

   
}

