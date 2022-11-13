#define F_CPU 16000000UL
#define BAUD 9600

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#define LCD_Dir DDRB   /* Define LCD data port direction */
#define LCD_Port PORTB /* Define LCD data port */

#define RS PB0 /* Define Register Select pin */
#define EN PB1 /* Define Enable signal pin */

#define pad PORTA
#define r1 PA0
#define r2 PA1
#define r3 PA2
#define r4 PA3

#define c1 PA4
#define c2 PA5
#define c3 PA6

char key;
char password[4];
char lock[0];
int pos = 0;



void LCD_Init(void) /* LCD Initialize function */
{

    LCD_Dir = 0xFF; /* Make LCD port direction as o/p */
    _delay_ms(20);  /* LCD Power ON delay always >15ms */

    LCD_Command(0x33);
    LCD_Command(0x32); /* Send for 4 bit initialization of LCD  */
    LCD_Command(0x28); /* 2 line, 5*7 matrix in 4-bit mode */
    LCD_Command(0x0c); /* Display on cursor off */
    LCD_Command(0x06); /* Increment cursor (shift cursor to right) */
    LCD_Command(0x01); /* Clear display screen */
}


void LCD_Command(unsigned char cmnd)
{
    LCD_Port = (LCD_Port & 0x0F) | (cmnd & 0xF0); /* Sending upper nibble */
    LCD_Port &= ~(1 << RS);                       /* RS=0, command reg. */
    LCD_Port |= (1 << EN);                        /* Enable pulse */
    _delay_us(1);
    LCD_Port &= ~(1 << EN);
    _delay_us(200);
    LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4); /* Sending lower nibble */
    LCD_Port |= (1 << EN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN);
    _delay_ms(2);
}


void LCD_Char(unsigned char data)
{
    LCD_Port = (LCD_Port & 0x0F) | (data & 0xF0); /* Sending upper nibble */
    LCD_Port |= (1 << RS);                        /* RS=1, data reg. */
    LCD_Port |= (1 << EN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN);
    _delay_us(200);
    LCD_Port = (LCD_Port & 0x0F) | (data << 4); /* Sending lower nibble */
    LCD_Port |= (1 << EN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN);
    _delay_ms(2);
}


void LCD_String(char *str) /* Send string to LCD function */
{
    int i;
    for (i = 0; str[i] != 0; i++) /* Send each char of string till the NULL */
    {
        LCD_Char(str[i]);
    }
}


void Name_Print(char *str,int pos) /* Send string to LCD function */
{
	int i;
	for (i = pos+5; i <=pos+13; i++) /* Send each char of string till the NULL */
	{
		LCD_Char(str[i]);
	}
}


void LCD_Clear()
{
    LCD_Command(0x01); /* Clear display */
    _delay_ms(2);
    LCD_Command(0x80); /* Cursor at home position */
}


unsigned char keypad()
{
    // DDRD = 0xf0;
    pad = 0B11111110;
    // pad &= (0<<r1);
    _delay_us(200);
    if ((PINA & (1 << PINA4)) == 0)
    {
        _delay_ms(200);
        return '1';
    }
    else if ((PINA & (1 << PINA5)) == 0)
    {
        _delay_ms(200);
        return '2';
    }
    else if ((PINA & (1 << PINA6)) == 0)
    {
        _delay_ms(200);
        return '3';
    }

    pad = 0B11111101;
    // pad &= (0 << r2);
    _delay_us(200);
    if ((PINA & (1 << PINA4)) == 0)
    {
        _delay_ms(200);
        return '4';
    }
    else if ((PINA & (1 << PINA5)) == 0)
    {
        _delay_ms(200);
        return '5';
    }
    else if ((PINA & (1 << PINA6)) == 0)
    {
        _delay_ms(200);
        return '6';
    }

    pad = 0B11111011;
    // pad &= (0<<r3);
    _delay_us(200);
    if ((PINA & (1 << PINA4)) == 0)
    {
        _delay_ms(200);
        return '7';
    }
    else if ((PINA & (1 << PINA5)) == 0)
    {
        _delay_ms(200);
        return '8';
    }
    else if ((PINA & (1 << PINA6)) == 0)
    {
        _delay_ms(200);
        return '9';
    }

    pad = 0B11110111;
    // pad &= (0<<r4);
    _delay_us(10);
    if ((PINA & (1 << PINA4)) == 0)
    {
        _delay_ms(200);
        return '*';
    }
    else if ((PINA & (1 << PINA5)) == 0)
    {
        _delay_ms(200);
        return '0';
    }
    else if ((PINA & (1 << PINA6)) == 0)
    {
        _delay_ms(200);
        return '#';
    }
}



char scankey()
{
    unsigned char value;
    char key = 'a';
    while (key == 'a')
    {
        PORTA = 0xF0;      // set all the input to one
        value = PINA;      // get the PORTD value in variable “value”
        if (value != 0xf0) // if any key is pressed value changed
        {
            key = keypad();
        }
    }
    return key;
}



void _unlock(void)
{
    PORTC = 0x02;
    _delay_ms(3000);
    PORTC = 0x00;
}



void _lock(void)
{
    PORTC = 0x01;
    _delay_ms(3000);
    PORTC = 0x00;
}



int main(void)
{

    DDRC = 0xff; // motor int
    DDRA = 0x0F;
    pad = 0xf0;

    LCD_Init(); /* Initialization of LCD*/
    LCD_Clear();

    char R_array[43];
    char W_array[43] = {"7944:Maneesha!,1234:Rohan...!,5678:Damian..!"};
    memset(R_array, 0, 15);
    eeprom_busy_wait();
    eeprom_write_block(W_array, 0, strlen(W_array)); /* Write W_array
                             from EEPROM address 0 */

    // LCD_String(R_array);
    // LCD_String(R_array[0]);
    // LCD_Char(R_array[0]);
    _delay_ms(1000);

    LCD_String("Please Enter Pin!"); /* Write string on 1st line of LCD*/
    LCD_Command(0xC0);               /* Go to 2nd line*/
    _delay_ms(500);

    //     while (1)
    //     {
    //          PORTA = 0xF0;      // set all the input to one
    //          value = PINA;      // get the PORTD value in variable “value”
    //          if (value != 0xf0) // if any key is pressed value changed
    //          {
    //
    // 	         LCD_Char(keypad());
    //          }
    //     }

    for (int i = 0; i <= 3; i++)
    {
        key = scankey();
        _delay_ms(200);
        LCD_Char(key);
        password[i] = key;
        _delay_ms(200);
    }
    LCD_Clear();

    _delay_ms(600);
    LCD_Clear();

    eeprom_read_block(R_array, 0, strlen(W_array)); /* Read EEPROM
                        from address 0 */
    for (int y = 0; y <= 2; y++)
    {
        // int pos=0;
        if (y == 0)
        {
            pos = 0;
        }
        else if (y == 1)
        {
            pos = 15;
        }
        else if (y == 2)
        {
            pos = 30;
        }
        if ((password[0] == R_array[pos + 0]) && (password[1] == R_array[pos + 1]) && password[2] == R_array[pos + 2] && password[3] == R_array[pos + 3])
        {
            LCD_String("Welcome ");
            LCD_Command(0xC0);
			Name_Print(R_array,pos);
           // LCD_String("Maneesha");
            _unlock();
            _delay_ms(3000);
            LCD_Clear();
            LCD_String("Lock?Press 1");
            LCD_Command(0xC0);
            key = scankey();

            LCD_Char(key);
            _delay_ms(100);
            lock[0] = key;
            if (lock[0] == '1')
            {
                _lock();
                LCD_String("Locked!");
                _delay_ms(1000);
                return main();
            }
        }
    }
    LCD_String("WRONG PASSWORD!");
    _delay_ms(1000);
    return main();

    return 0;
}
