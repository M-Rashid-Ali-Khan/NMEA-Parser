#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "NMEA-parser.h"
// #define C_DEBUG

#ifdef C_DEBUG
void main() //For C compiler
#else
void app_main(void) //For - ESP32
#endif

{
    char data[] = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    printf("DEVICE STARTED\n\n");  
    nmea output;
    int verified; int i = 0;
    verified = integrity(data);
    if(verified!=0){
        printf("VERIFIED\n");        
        parse_gps_data(data,&output);
        while(output.data[i].type != Undefined){
            print_parsed(output.data[i]);
            i++;
        }
    } else{
        printf("\nNOT VERIFIED\n");
    }

    //Demo to access individual elements
    printf("\nlongitude:%f %s, latitude:%f %s",
    output.data[longitude_f].value.f,
    output.data[long_hemisphere_s].value.s,
    output.data[latitude_f].value.f,
    output.data[lat_hemisphere_s].value.s
    );
}