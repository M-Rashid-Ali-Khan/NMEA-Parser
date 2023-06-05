#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "NMEA-parser.h"

#ifdef C_DEBUG
void main() //For C compiler
#else
void app_main(void) //For - ESP32
#endif

{
    char nmea[] = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    printf("DEVICE STARTED\n\n");  
    parsed *output=0;
    int verified; int i = 0;
    verified = integrity(nmea);
    if(verified!=0){
        printf("VERIFIED\n");        
        output = parsing(nmea);
        while(output[i].type!=-2){
            print_parsed(output[i]);
            i++;
        }
    } else{
        printf("\nNOT VERIFIED\n");
    }
}