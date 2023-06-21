#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "NMEA-parser.h"
#include "unity.h"

#define C_DEBUG
#define ARRAY_SIZE 5
void parsing_test(void){
    const bucket_type_t *expected_val = {String,Float,Integer};
    const char* data[ARRAY_SIZE] = {
        "$GPGGA,123519.00,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47",
        "$GPGGA,091530.00,3723.2475,N,12158.3416,W,1,8,1.0,10.0,M,,,,*14",
        "$GPGGA,124652.00,5103.743,N,11400.769,W,1,04,1.5,100.0,M,,,,*2B",
        "$GPGGA,080756.00,3343.5423,S,15110.3963,E,1,06,2.3,21.9,M,,,,*14",
        "$GPGGA,153456.00,2400.2356,N,05422.6704,E,1,05,0.7,135.7,M,,,,*3A"
    };
    printf("DEVICE STARTED\n\n");
    for(int i = 0; i < ARRAY_SIZE;i++){
        nmea output;
        int verified; int i = 0;
        verified = integrity(data[i]);
        if(verified!=0){
            printf("VERIFIED\n");        
            parse_gps_data(data[i],&output);
            while(output.data[i].type != Undefined){
                if(output.data[i].type!=Missing)
                    TEST_ASSERT_EQUAL(expected_val[i],output.data[i].type);
                else 
                    TEST_ASSERT_EQUAL(Missing,output.data[i].type);
                i++;
            }
        } else{
            printf("\nNOT VERIFIED\n");
        }
    }
}

#ifdef C_DEBUG
void main() //For C compiler
#else
void app_main(void) //For - ESP32
#endif
{
    UNITY_BEGIN();     
    RUN_TEST(parsing_test);
    UNITY_END();

    

    //Demo to access individual elements
    printf("\nlongitude:%f %s, latitude:%f %s",
    output.data[longitude_f].value.f,
    output.data[long_hemisphere_s].value.s,
    output.data[latitude_f].value.f,
    output.data[lat_hemisphere_s].value.s
    );
}