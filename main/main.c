#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "NMEA-parser.h"
#include "unity.h"

// #define C_DEBUG

#define ARRAY_SIZE 5
void parsing_test(void){
    bucket_type_t expected_val[] = 
    {String,Float,Float,String,Float,String, Integer, Integer, Float, Float, 
    String, Float, String, Float, String, Undefined};
    char* data[ARRAY_SIZE] = {
        "$GPGGA,123519.00,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47",
        "$GPGGA,091530.00,3723.2475,N,12158.3416,W,1,8,1.0,10.0,M,,,,*14",
        "$GPGGA,124652.00,5103.743,N,11400.769,W,1,04,1.5,100.0,M,,,,*2B",
        "$GPGGA,080756.00,3343.5423,S,15110.3963,E,1,06,2.3,21.9,M,,,,*14",
        "$GPGGA,153456.00,2400.2356,N,05422.6704,E,1,05,0.7,135.7,M,,,,*3A"
    };
    printf("DEVICE STARTED\n\n");
    for(int batch = 0; batch < ARRAY_SIZE;batch++){
        nmea output;
        int ele_num = 0;
        parse_gps_data(data[batch],&output);
        while(output.data[ele_num].type != Undefined){
            if(output.data[ele_num].type!=Missing)
            {
                printf("%d , %d \n",expected_val[ele_num],output.data[ele_num].type);
                TEST_ASSERT_EQUAL_INT(expected_val[ele_num],output.data[ele_num].type);
            }
            else 
                TEST_ASSERT_EQUAL_INT(Missing,output.data[ele_num].type);
            ele_num++;
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
}