  
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <assert.h>
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


char meteo_obs[100];
char meteo_ico[5];
char meteo_min[5];
char meteo_max[5];

struct url_data {
    size_t size;
    char* data;
};

size_t write_data(void *ptr, size_t size, size_t nmemb, struct url_data *data) {
    size_t index = data->size;
    size_t n = (size * nmemb);
    char* tmp;

    data->size += (size * nmemb);

#ifdef DEBUG
    fprintf(stderr, "data at %p size=%ld nmemb=%ld\n", ptr, size, nmemb);
#endif
    tmp = realloc(data->data, data->size + 1); /* +1 for '\0' */

    if(tmp) {
        data->data = tmp;
    } else {
        if(data->data) {
            free(data->data);
        }
        fprintf(stderr, "Failed to allocate memory.\n");
        return 0;
    }

    memcpy((data->data + index), ptr, n);
    data->data[data->size] = '\0';

    return size * nmemb;
}

char *handle_url(char* url) {
    CURL *curl;

    struct url_data data;
    data.size = 0;
    data.data = malloc(4096); /* reasonable size initial buffer */
    if(NULL == data.data) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return NULL;
    }

    data.data[0] = '\0';

    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n",  
                        curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);

    }
    return data.data;
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}


void GetWeather(void) {
  
  char* data;
  char** tokens;

  data = handle_url("http://home.joaquim.org/tempo/tempolcd.php");

    if(data) {
        printf(" Meteo : %s\n", data);
        tokens = str_split(data, '|');

        sprintf(meteo_obs, "%s", *(tokens + 0)); 
        sprintf(meteo_ico, "%s", *(tokens + 1)); 
        sprintf(meteo_min, "%s", *(tokens + 2)); 
        sprintf(meteo_max, "%s", *(tokens + 3)); 

        free(data);
        free(tokens);
    }
  
}


int main(int argc, char **argv) {
	
	// Compute the size of the screen
	int pixelX = 320;
	int pixelY = 256;
	
	int fontSize = 1; // 0=6x8   1=8x8
	
	GetWeather();

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
	sprintf(buffer,"%s/%s", meteo_min, meteo_max);
	WriteText(50, 55, buffer, 1);
	WriteText(85, 49, "*", 3);

	WriteText(1, 75, meteo_obs, 1);
	
	return 0;		

} // main

