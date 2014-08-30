  
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include <curl/curl.h>

#include "lcd_controller.h" 
#include "lcd_screen.h" 
#include "lcd_graphics.h" 

const char * weekday[] = { 	"Domingo", "Segunda",
                            "Terca", "Quarta",
                            "Quinta", "Sexta", "Sabado"};

const char * month[] = { 	"Jan", "Fev",
                            "Mar", "Abr",
                            "Mai", "Jun", "Jul",
                        	"Ago", "Set", "Out",
                        	"Nov", "Dez"};



void GetWeather(void)
{
  CURL *curl;
  CURLcode res;
 
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://home.joaquim.org/tempo/tempolcd.php");    
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
 
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);
  }
  
}


int main(int argc, char **argv)
{
	GetWeather();
	
	// Compute the size of the screen
	int pixelX = 320;
	int pixelY = 256;
	
	int fontSize = 1; // 0=6x8   1=8x8
	
	LCD_screen_init(pixelX,pixelY, fontSize);
	
	// MODE SET
	LCD_mode( MODE_OR ); 
	
	// DISPLAY MODE
	//LCD_display_mode( DM_TEXT + DM_GRAPHICS );
	LCD_display_mode( DM_GRAPHICS );

	int i;
	
	// Erase graphic screen
	LCD_set_address_pointer( LCD_getBaseGraphic() );
	LCD_auto_write_start();
	for (i=0;i<LCD_getGraphicScreenSize();i++) {
		LCD_auto_write( 0x00 ); 
	}
	LCD_auto_write_stop();
	LCD_set_address_pointer( LCD_getBaseText() );
	
	char buffer[256];

	Puts(1,122, "Raspberry Pi LCD - by joaquim.org");

	// current date/time based on current system
   	
   	time_t now = time(0);
   	struct tm *tm_struct = localtime(&now);

   	sprintf(buffer,"%02d:%02d",tm_struct->tm_hour, tm_struct->tm_min);

	WriteText(45, 20, buffer, 2);

	sprintf(buffer,"%s %d %s %04d",weekday[tm_struct->tm_wday], tm_struct->tm_mday, month[tm_struct->tm_mon], 1900 + tm_struct->tm_year);

	WriteText(5, 2, buffer, 1);

	WriteText(5, 50, "3", 3);	
	WriteText(30, 50, "'", 3);
	WriteText(50, 55, "12/33", 1);
	WriteText(85, 49, "*", 3);

	WriteText(1, 75, "Ceu com periodos de muito nublado", 1);
	
	return 0;		

} // main

