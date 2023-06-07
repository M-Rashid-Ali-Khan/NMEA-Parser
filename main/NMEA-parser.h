
//------------------------------------------------------------------
//          INTERFACE
//------------------------------------------------------------------

/**
 * * The function is declared as:
 * * bucket* parse_gps_data(char *data);
 * 
 * * HOW TO USE:
 * Call the above function.
 * Input parameter is string of NMEA code (starting from $ and ending with *xx)`
 * It returns a pointer to the array of bucket datatype.
 * See `main.c` for how to read the array or read the README.md file in `main` folder
 * * CAUTION:
 * The returned pointer is an address to an internal dynamic array which will be overwritten in the next call of parse_gps_data() function
 * To avoid the losing data, deep copy the returned string
 * Or add #define NMEA_COPY to return a copy of dynamic string (Beware of memory leaks)
*/


//------------------------------------------------------------------
//          IMPLEMENTATION
//------------------------------------------------------------------
union _shared{
    char *str;
    int   num;
    float points;
};

typedef struct {
    int           type;
    union _shared value;  
}bucket;

char     *_gps_raw    = 0;
bucket   *_info       = NULL;
int      _info_size   = 0;
int      _parse_index = 0;

void print_parsed (bucket _parsed)
{
    if ( _parsed.type == 0 )  printf("%s", _parsed.value.str);
    if ( _parsed.type == 1 )  printf("%d", _parsed.value.num);
    if ( _parsed.type == 2 )  printf("%f", _parsed.value.points);
    if ( _parsed.type == -1 ) printf("Empty NMEA parameter");
    printf("\n");
}

bool integrity(char* data)
{
    int  i=0, checksum;
    unsigned char calc = 0; 
    char element = data[i]; 
    char *slice;
    if (element == '$' ){
        while (1){
            i++; element = data[i];
            if (element =='\0'){
                printf("Invalid NMEA CODE\n");
                return false;
            }
            if(element =='*'){
                i++; slice = (data+i);
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

char get_char()
{
    _parse_index++;
    return _gps_raw[_parse_index-1];
}

void putback()
{
    _parse_index--;
}

bucket comma_parse()
{
    bucket value;
    char* str_alloc = 0;
    char slice[15] ="";
    int i = 0;
    int type = 0 ; //0 for string, 1 for int, 2 for float
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

bucket* parse_gps_data(char *data)
{
    _info_size=0;
    if(!_info){
        free(_info);
        _info=0;
    }
    bucket end;
    end.type = -2; //End character for our bucket data
    size_t length = strlen(data);
    _gps_raw = malloc((length+1)*sizeof(char));
    strcpy(_gps_raw, data);
    while(get_char()!='$'){}
    while(get_char()!='*'){
        putback();
        _info_size++;
        _info = realloc(_info,_info_size*sizeof(bucket));
        if(get_char()==',') {
            bucket temp;
            temp.type = -1; //Shows some data is missing from NMEA format
            _info[_info_size-1] = temp;
            continue;
        }
        putback();
        _info[_info_size-1] = comma_parse(data);
    }
    _info_size++;
    _info = realloc(_info,_info_size*sizeof(bucket));
    _info[_info_size-1] = end;

    #ifndef NMEA_COPY
    return _info;
    #else
    bucket* copy = malloc(_info_size*sizeof(bucket));
    return copy;
    #endif
}
