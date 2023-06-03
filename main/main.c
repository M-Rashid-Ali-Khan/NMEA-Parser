#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
struct gps_data{
    char* system;
    int hour;
    int minute;
    int second;
    float latitude;
    float longitude;
    int quality;
    int satelites;
    float altitude;
};
union _parsed{

    char *str;
    int num;
    float points;
};
typedef struct {
    int type;
    union _parsed value;  
}parsed;

int parse_index = 0;

char gps_raw[] = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\n";
void print_parsed (parsed _parsed){
    if(_parsed.type==0) printf("%s",_parsed.value.str);
    if(_parsed.type==1) printf("%d",_parsed.value.num);
    if(_parsed.type==2) printf("%f",_parsed.value.points);
    if(_parsed.type==-1) printf("Empty NMEA parameter");
    printf("\n");
}
bool integrity(char* data){
    int i=0,checksum; unsigned char calc = 0; char element = gps_raw[i]; char *slice;
    if (element=='$'){
        while(1){
            i++; element = gps_raw[i];
            if(element =='*'){
                i++; slice = (gps_raw+i);
                sscanf(slice, "%x",&checksum);
                break;
            } else {
                    calc ^= element;
            }
        }
    } else {
        printf("Invalid NMEA CODE\n");
    }
    if(calc == checksum){
        return true;
    } else {
        return false;
    }
}
char get_char(){
    parse_index++;
    return gps_raw[parse_index-1];
}
void putback(){
    parse_index--;
}

parsed comma_parse(){
    parsed value; char* str_alloc =0;
    char slice[15] =""; int i = 0; int type = 0 ; //0 for string, 1 for int, 2 for float
    int val = get_char();
    if( val >= '0' && val <= '9' ){
        type = 1;
    }
    while (i<15)
    {
        slice[i] = val;
        i++;
        val = get_char();
        if(val == '.') type = 2;
        if(val == ',') break;
    }
    slice[i] = '\0';
    if(type == 2){
        value.type = 2;
        sscanf(slice,"%f", &value.value.points);
    } else if(type == 1){
        value.type = 1;
        sscanf(slice,"%d",&value.value.num);
    } else {
        value.type = 0;
        int count = 0;
        while(1){
            if(slice[count] == '\0') break;
            count++;
        }
        str_alloc = malloc((count+1)*sizeof(char));
        strcpy(str_alloc, slice);
        value.value.str = str_alloc;
    }
    return value;
}
parsed *info=NULL; int info_size=0;
void parsing(char *data){
    parsed end; end.type = -2; //End character for our parsed data
    while(get_char()!='$'){}
    while(get_char()!='*'){
        putback();
        info_size++;
        info = realloc(info,info_size*sizeof(parsed));
        if(get_char()==',') {
            parsed temp;
            temp.type = -1; //Shows some data is missing from NMEA format
            info[info_size-1] = temp;
            continue;
        }
        putback();
        info[info_size-1] = comma_parse(data);
        // print_parsed(info[info_size-1]);
    }
    info_size++;
    info = realloc(info,info_size*sizeof(parsed));
    info[info_size-1] = end;
}

void app_main(void)
// void main()
{
    // struct parsed *string = malloc(2*sizeof(struct parsed));
    // string[1].type = -2;
    // string[1].value.str = "hiii";
    // string = realloc(string,3*sizeof(struct parsed));
    // string[2].type = -3;
    // string[2].value.str = "heee";
    // printf("%s\n",string[2].value.str);

    int verified; int i = 0;
    verified = integrity(gps_raw);
    if(verified!=0){
        printf("VERIFIED\n");        
        parsing(gps_raw);
        while(info[i].type!=-2){
            print_parsed(info[i]);
            i++;
        }
    } else{
        printf("\nNOT VERIFIED\n");
    }
}