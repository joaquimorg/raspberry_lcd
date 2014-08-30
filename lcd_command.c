//
// Low level access function to the LCD
//
  
#include <unistd.h>

#include "gpio.h" 
//#include "lcd_register.h" 



// LCD Control constants
#define WR_Pin   14   //  ~WR
#define RD_Pin   15   //  ~RD
#define CE_Pin   17   //  ~CE
#define CD_Pin   18   //  Command/Data
#define RS_Pin   27   //  ~Reset

#define D0_Pin   7    //  D0
#define D1_Pin   8    //  D1
#define D2_Pin   9    //  D2
#define D3_Pin   10   //  D3
#define D4_Pin   22   //  D4
#define D5_Pin   23   //  D5
#define D6_Pin   24   //  D6
#define D7_Pin   25   //  D7


#define WR   1<<WR_Pin
#define RD   1<<RD_Pin
#define CE   1<<CE_Pin
#define CD   1<<CD_Pin
#define RS   1<<RS_Pin

#define D0   1<<D0_Pin
#define D1   1<<D1_Pin
#define D2   1<<D2_Pin
#define D3   1<<D3_Pin
#define D4   1<<D4_Pin
#define D5   1<<D5_Pin
#define D6   1<<D6_Pin
#define D7   1<<D7_Pin




// Send a full byte (8 bits) into LCD
void reg_data(short byte) { 

  if( byte &   1 ) gpio_set( D0 ); else gpio_clr( D0 );
  if( byte &   2 ) gpio_set( D1 ); else gpio_clr( D1 );
  if( byte &   4 ) gpio_set( D2 ); else gpio_clr( D2 );
  if( byte &   8 ) gpio_set( D3 ); else gpio_clr( D3 );
  if( byte &  16 ) gpio_set( D4 ); else gpio_clr( D4 );
  if( byte &  32 ) gpio_set( D5 ); else gpio_clr( D5 );
  if( byte &  64 ) gpio_set( D6 ); else gpio_clr( D6 );
  if( byte & 128 ) gpio_set( D7 ); else gpio_clr( D7 );

}

//
// Set up the LCD
//
void LCD_setup()
{
  //reg_setup();
  // Set useful GPIO pins to output
  gpio_output_pin(WR_Pin);
  gpio_output_pin(RD_Pin);
  gpio_output_pin(CE_Pin);
  gpio_output_pin(CD_Pin);
  gpio_output_pin(RS_Pin);
  
  gpio_output_pin(D0_Pin);
  gpio_output_pin(D1_Pin);
  gpio_output_pin(D2_Pin);
  gpio_output_pin(D3_Pin);
  gpio_output_pin(D4_Pin);
  gpio_output_pin(D5_Pin);
  gpio_output_pin(D6_Pin);
  gpio_output_pin(D7_Pin);

  usleep(1); // ! Important

  gpio_clr( RS );
  usleep(1); // ! Important
  gpio_set( RS );

  gpio_clr( WR );
  gpio_set( RD );
  gpio_set( CE );
}

void LCD_command(short command) {
  int i;
  reg_data(command);
  gpio_set( CD );

  gpio_clr( CE );
  //usleep(1); // ! Important
  for(i=1;i<1000;i++);
  gpio_set( CE );
}

void LCD_data(short data) {
  int i;
  reg_data(data);
  gpio_clr( CD );

  gpio_clr( CE );
  //usleep(1); // ! Important
  for(i=1;i<1000;i++);
  gpio_set( CE );
}

void LCD_command_1D(short command, short data) {
  LCD_data(data);
  LCD_command(command);
}

void LCD_command_2D(short command, int data) {
  LCD_data(data & 0xFF);
  LCD_data((data >> 8) & 0xFF);
  LCD_command(command);
}


