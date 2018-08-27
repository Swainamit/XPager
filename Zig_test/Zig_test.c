/*
 * Zig_test.c
 *
 * Created: 22-12-2016 11:18:54
 *  Author: LENOVO
 */ 

/*
 * Mine_Pager.c
 *
 * Created: 18-12-2016 00:10:33
 *  Author: LENOVO
 */ 

#include<avr/io.h>
#define	F_CPU	8000000
#include<util/delay.h>
#include<avr/keypad.h>

#define		KEYPAD_PORT		PORTB
#define		KEYPAD_PIN		PINB

#define		LCD_DATA_PORT		PORTA
#define 	LCD_CONT_PORT		PORTC

#define 	LCD_RS 		PC5
#define 	LCD_RW 		PC6
#define 	LCD_EN 		PC7
#include<usart.h>
#include<avr/lcd.h>
#include<string.h>
#include<stdio.h>
//#include<avr/zigbee.h>
//#include<timercounter2.h>
#include<string.h>
#include<avr/interrupt.h>
volatile uint8_t usart_receive_status;
volatile uint8_t usart_received_data[35]="";
volatile unsigned char count2; 

/*ISR(TIMER2_COMP_vect)  //interrupt service routine
{
	if(count2==13)         //if LED is 'OFF' and timer count is 255
	{
		count2=0;             //reinitialize counter
	}
	else
	count2++;        //otherwise increment counter
	
}*/
ISR(USART_RXC_vect)
{
	usart_received_data[count2]=usart_data_receive();
	//lcd_command_write(0x01);
	/*Clear screen*/
	//lcd_command_write(0x80);
	if(usart_received_data[count2]=='~')
	{
		usart_receive_status=1;
	}
	count2++;
	
}

int main(void)
{
	DDRA=0xff;
	/*All the 8 pins of PortB are declared output (data pins of LCD are connected)*/

	DDRC=0xff;
	
	DDRD=0xf0;
	/*PC0, PC1 and PC2 pins of PortD are declared output (control pins of LCD are connected)*/

	DDRB=0x0f;
	/*PortC's upper 4 bits are declared input and lower 4 bits are declared output(4x4 Keypad is connected)*/

	PORTB=0xff;
	/*PortC's lower 4 bits are given high value and pull-up are enabled for upper 4 bits*/

	unsigned char count=0, cursor_count=0, password_status=0, keypad_value;
	count2=0;

	char password_set[5]="1111", password_entered[5];
	char input_array[34]="";
	char id_count[5]="";char CONT_ID[5] ="0001";
	
	unsigned char string_index=0;   const char s[2] = ",";
	char *token;//string_index2,string_index3,string_index4,
	//string_index5,string_index6,string_index7,string_index8,string_index9=0;
	
	char String2[5]={'a','b','c','-','\0'},String3[5]={'d','e','f','&','\0'},String4[5]={'g','h','i','%','\0'},
	String5[5]={'j','k','l','(','\0'},String6[5]={'m','n','o',')','\0'},String7[5]={'p','q','r','s','\0'},
	String8[5]={'t','u','v','?','\0'},String9[5]={'w','x','y','z','\0'},String1[5]={'.',',','!','_','\0'};
		
	char cString2[5]={97,98,99,45,'\0'},cString3[5]={100,101,102,38,'\0'},cString4[5]={103,104,105,37,'\0'},
	cString5[5]={106,107,108,40,'\0'},cString6[5]={109,110,111,41,'\0'},cString7[5]={112,113,114,115,'\0'},
	cString8[5]={116,117,118,63,'\0'},cString9[5]={119,120,121,122,'\0'},cString1[5]={46,44,33,95,'\0'},cString0[5]={32,32,32,32,'\0'};
		
	unsigned char input_status=0; // 0-numeric and 1-alphabet
	
	usart_init();
	lcd_init();
	
	UCSRB |= (1 << RXCIE); // Enable the USART Recieve Complete interrupt (USART_RXC)

	
	lcd_string_write("Wireless Pager");
	
	lcd_command_write(0xc0);
	/*Cursor moves to 2nd row 1st column of LCD*/

	lcd_string_write("System Ready");
	/*String display in 2nd row of LCD*/
	
	_delay_ms(500);
	_delay_ms(500);
	_delay_ms(500);
	_delay_ms(500);

	lcd_command_write(0x01);
	/*Clear screen*/

	lcd_string_write("Enter Password");
	/*String display in 1st row of LCD*/

	/*While loop for password entry and checking*/
	while(password_status==0x00)
	{
		lcd_command_write(0xc0);
		/*Cursor moves to 2nd row 1st column of LCD*/

		/*While loop for 4 digit password entry*/
		while(count<4)
		{
			keypad_value=read_keypad();
			/*Scans 4X4 keypad and returns pressed key value or default value*/

			/*Checking if any key is pressed or not*/
			if(keypad_value != 0xff)
			{
				password_entered[count]=keypad_value+48;
				/*Storing the pressed key value of 4X4 keypad in ASCII format*/

				count++;
				/*Counter increment*/

				lcd_data_write('*');
				/* Star(*) is displayed in 2nd row of LCD*/
			}
			else
			{
				;
				/*Null statement*/
			}

			_delay_ms(300);
			/*300ms delay as guard time between two consecutive pressing of key*/
		}
		count=0;
		/*Counter reset to 0*/

		password_entered[4]=0;
		/*Null character at the last of array(password_entered) to convert it to a string*/

		lcd_command_write(0x01);
		/*Clear screen*/

		/*Password Comparison*/
		if(!(strcmp(password_set,password_entered)))
		{
			lcd_string_write("Correct Password");
			/*String display in 1st row of LCD*/

			password_status=1;
			/*Changing the Password Status to Correct Password*/
		}
		else
		{
			lcd_string_write("Wrong Password");
			/*String display in 1st row of LCD*/
			
			_delay_ms(500);
			_delay_ms(500);
			_delay_ms(500);
			_delay_ms(500);
			/*Display stays for 2 second*/

			lcd_command_write(0x01);
			/*Clear screen*/

			lcd_string_write("Reenter Password");
			/*String display in 1st row of LCD*/
		}
	}

	_delay_ms(500);
	_delay_ms(500);
	_delay_ms(500);
	_delay_ms(500);
	/*Display stays for 2 second*/
	
	lcd_command_write(0x01);
	/*Clear Screen*/
	
	/*set_timercounter2_mode(2);
	set_timercounter2_prescaler(5);
	set_timercounter2_output_mode(2);
	enable_timercounter2_interrupt(1);
	set_timercounter2_compare_value(255);
	sei();            //enable Global Interrupt*/
	
	sei();
	/*Start of infinite loop*/
	while(1)
	{
		lcd_command_write(0x06);
		
		keypad_value=read_keypad();
		/*Scans 4X4 keypad and returns pressed key value or default value*/
		
		if (usart_receive_status==1)
		{
			lcd_command_write(0x01);
			lcd_command_write(0x80);
			cursor_count=0;
			keypad_value=0xff;
			usart_receive_status=0;
			token = strtok(usart_received_data, s);
			token = strtok(NULL, s);
			if ((!(strcmp(token,CONT_ID))||!(strcmp(token,"0000"))))
			{
				lcd_command_write(0x01);
				lcd_command_write(0x80);
				for (int i=0;i<=((count2)-8);i++)
				{
					lcd_data_write(usart_received_data[i]);
					cursor_count++;
					if (cursor_count==16)
					{
						lcd_command_write(0xc0);
					}
				}
				for (int a=0;a<3;a++)
				{
					PORTD=0b01000000;
					_delay_ms(50);
					PORTD=0b00000000;
					_delay_ms(50);
					PORTD=0b01000000;
					_delay_ms(50);
					PORTD=0b00000000;
					_delay_ms(150);
				}
			}
				
			count2=0;	
			//lcd_command_write(0x14);
		}
		
		if (keypad_value == 15)
		{
			if (input_status==0)
			{
				input_status=1;
			}
			else if (input_status==1)
			{
				input_status=0;
			}
		}
		if (keypad_value == 12)
		{
			lcd_command_write(0x01);
					/*Clear screen*/
					
					lcd_command_write(0x80);
					
					for(int i=0;i<(cursor_count-4);i++)
					{
						usart_data_transmit(input_array[i]);
					}
					usart_string_transmit(",");
					for(int i=0;i<=3;i++)
					{
						usart_data_transmit(id_count[i]);
					}
					usart_string_transmit(",");
					usart_string_transmit("~");
					//usart_string_transmit("     ID: 0001");
					//usart_data_transmit(0x0d);
					usart_data_transmit(0x0a);
					lcd_string_write("Page sent");
					_delay_ms(200);
					lcd_command_write(0x01);
					lcd_command_write(0x80);
					/*for (int i=0;i<33;i++)
					{
						if (i==17)
						{
							lcd_command_write(0xc0);
						}
					
						lcd_data_write(input_array[i]);		
					}*/
		}
		
		/*Checking if any key is pressed or not*/
		if(keypad_value!=0xff)
		{
			if (keypad_value!=10 && keypad_value!=11 && keypad_value!=12 && keypad_value!=13 && keypad_value!=14 && keypad_value!=15)
			{
				if (input_status==0)
				{
					input_array[cursor_count]=48+keypad_value;
					cursor_count++;
				}
				//else if(input_status==1)
				//{
					//input_array[cursor_count]=;
					
				//}	
			}
			
			if (cursor_count==17)
			{
				lcd_command_write(0xc0);
			}
			
			
			//if(keypad_value == 12||keypad_value == 15)
			//{
			//	keypad_value=" ";
			//}
			
			switch(keypad_value)
			{
				case 0:
				{
					if (input_status==0)
					{
						lcd_data_write('0');
						/*Displays 1 in 2nd row of LCD*/
					}
					else
					{
						lcd_command_write(0x14);
						input_array[cursor_count]=cString0[string_index];
						cursor_count++;
					}
				};
				
				break;
				/*Break statement*/

				case 1:
				{
					if (input_status==0)
					{
						lcd_data_write('1');
						/*Displays 1 in 2nd row of LCD*/
					} 
					else 
					{
						//string_index=0;
						lcd_data_write(String1[string_index]);
						input_array[cursor_count]=cString1[string_index];
						cursor_count++;		
					}
				};
	
				break;
				/*Break statement*/

				case 2:
				{
					if (input_status==0)
					{
						lcd_data_write('2');
						/*Displays 1 in 2nd row of LCD*/
					}
					else 
					{
						//string_index=0;
						lcd_data_write(String2[string_index]);
						input_array[cursor_count]=cString2[string_index];
						cursor_count++;
					}
				};
				
				break;
				/*Break statement*/
				
				case 3:
				{
					if (input_status==0)
					{
						lcd_data_write('3');
						/*Displays 1 in 2nd row of LCD*/
					}
					else 
					{
						//string_index=0;
						lcd_data_write(String3[string_index]);
						input_array[cursor_count]=cString3[string_index];
						cursor_count++;
					}
				};
				
				break;
				/*Break statement*/

				case 4:
				{
					if (input_status==0)
					{
						lcd_data_write('4');
						/*Displays 1 in 2nd row of LCD*/
					}
					else 
					{
						//string_index=0;
						lcd_data_write(String4[string_index]);
						input_array[cursor_count]=cString4[string_index];
						cursor_count++;
					}
				};
				
				break;
				/*Break statement*/

				case 5:
				{
					if (input_status==0)
					{
						lcd_data_write('5');
						/*Displays 1 in 2nd row of LCD*/
					}
					else 
					{
						//string_index=0;
						lcd_data_write(String5[string_index]);
						input_array[cursor_count]=cString5[string_index];
						cursor_count++;
					}
				};
				
				break;
				/*Break statement*/

				case 6:
				{
					if (input_status==0)
					{
						lcd_data_write('6');
						/*Displays 1 in 2nd row of LCD*/
					}
					else
					{
						//string_index=0;
						lcd_data_write(String6[string_index]);
						input_array[cursor_count]=cString6[string_index];
						cursor_count++;
					}
				};

				break;
				/*Break statement*/

				case 7:
				{
					if (input_status==0)
					{
						lcd_data_write('7');
						/*Displays 1 in 2nd row of LCD*/
					}
					else if(input_status==1)
					{
						//string_index=0;
						lcd_data_write(String7[string_index]);
						input_array[cursor_count]=cString7[string_index];
						cursor_count++;
					}
				};	

				break;
				/*Break statement*/

				case 8:
				{
					if (input_status==0)
					{
						lcd_data_write('8');
						/*Displays 1 in 2nd row of LCD*/
					}
					else if(input_status==1)
					{
						//string_index=0;
						lcd_data_write(String8[string_index]);
						input_array[cursor_count]=cString8[string_index];
						cursor_count++;
					}
				};
				

				break;
				/*Break statement*/

				case 9:
				{
					if (input_status==0)
					{
						lcd_data_write('9');
						/*Displays 1 in 2nd row of LCD*/
					}
					else if(input_status==1)
					{
						//string_index=0;
						lcd_data_write(String9[string_index]);
						input_array[cursor_count]=cString9[string_index];
						cursor_count++;
					}	
				};

				break;
				/*Break statement*/

				case 10:
				{
				 lcd_command_write(0x10);
				 cursor_count--;
				 input_array[cursor_count]="";
				 lcd_string_write(" ");
				 lcd_command_write(0x10);
				};

				break;
				/*Break statement*/

				case 11:
				{
				string_index++;
				if (string_index>=4)
				{
					string_index=0;
				}
				//lcd_data_write('#');
				/*Displays # in 2nd row of LCD*/
				//keypad_value=" ";	
				};
				
				break;
				/*Break statement*/

				case 12:
				{
					//lcd_command_write(0x01);
					/*Clear screen*/
					
					/*lcd_command_write(0x80);
					
					for(int i=0;i<33;i++)
					{
						usart_data_transmit(input_array[i]);
					}
					usart_string_transmit("ID: 0001");
					usart_data_transmit(0x0d);
					usart_data_transmit(0x0a);
					lcd_data_write('y');
					
					/*for (int i=0;i<33;i++)
					{
						if (i==17)
						{
							lcd_command_write(0xc0);
						}
					
						lcd_data_write(input_array[i]);		
					}*/
				};
				
				break;
				/*Break statement*/

				case 13:
				{
				lcd_command_write(0x01);
				lcd_command_write(0x80);
				lcd_string_write("ID: ");
				for (int i=((cursor_count)-4),j=0;i<cursor_count && j<=3;i++,j++)
				{
					id_count[j]=input_array[i];
					lcd_data_write(id_count[j]);
				}
				};										

				break;
				/*Break statement*/

				case 14:
				{
				//lcd_data_write('C');
				
				lcd_command_write(0x01);
				for (int i=0;i<=cursor_count;i++)
				{
					input_array[i]="";
				}
				cursor_count=0;
				/*Displays C in 2nd row of LCD*/
				};
				
				break;
				/*Break statement*/

				case 15:
				{
				 
				};				

				break;
				/*Break statement*/ 
			}
		}
		else
		{
			
		}
		_delay_ms(200);
		/*200ms delay as guard time between two consecutive pressing of key*/
	}
}


