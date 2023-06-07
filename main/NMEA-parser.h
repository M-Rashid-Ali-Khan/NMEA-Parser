//------------------------------------------------------------------
//          INTERFACE
//------------------------------------------------------------------

/**
 * * INTERFACE METHOD 2.0:
 * * void parse_gps_data(char *data, nmea *output)
 *  Returns pass by reference struct output which contains individual parameters of GPS data
 * * HOW TO USE:
 * Pass Argument1 as input GPS data string and Argument2 as the address of output variable
 * Make sure that the output variable is of datatype `nmea`
 * To access individual parameters of GPS, pass enum `gps_data` as index to the nmea.data[] object
 * See `main.c` for how for example implememtation or read the README.md file in `main` folder
 * * EXAMPLE:
 * char data[] = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
 * nmea output;
 * parse_gps_data(data, &output);
 * * USING NMEA STRUCT:
 * if (output.data.[gps_quality].type != Missing){
 * printf("GPS Quality: %d \n",output.data.[gps_quality].type);
 * } else {
 *     printf("GPS Quality is missing.\n");
 * }
 * float longitude = output.data.[longitude].value.f;
 * float lattitude = output.data[lattitude].value.f;
 * char* latitude_hemisphere = output.data[lat.hemisphere].value.s;
 * float longitude = output.data[longitude].value.f;
 * float lattitude = output.data[lattitude].value.f;
 * char* latitude_hemisphere = output.data[lat.hemisphere].value.s;
 * * CAUTION for depriciated interface 1.0:
 * The returned pointer is an address to a dynamic array so beware of memeory leaks.
 * Alternatively, use the Interface method 2.0.
 * */
//------------------------------------------------------------------
//          IMPLEMENTATION
//------------------------------------------------------------------

// Max length of string elements of GPS parameters
#define MAX_NMEA_STRING 6

//This enum is passed as index to the nmea.data[] to access GPS parameters
//The naming convention of enums is parameter_format
typedef enum {
    identifier_s,
    utc_d,                  //UTC time status  of position (HHMMSS)
    latitude_f,
    lat_hemisphere_s,
    longitude_f,
    long_hemisphere_s,
    gps_quality_d,
    satelites_d,           //No. of satellites
    hdop_f,                //Horizontal Dilution of Precision
    altitude_f,            //Altitude
    altitude_unit_s,
    undulation_f,
    undulation_unit_s,
    age_of_diff_f,         //age of differential correction GPS data
    stn_id_s               //Diffrential Base station ID
}gps_data;

//This enum provides datatype classification of parameters of GPS in nmea.data[]
typedef enum {
    String,
    Integer,
    Float,
    Missing,              //Indicates missing values
    Undefined
}bucket_type_t;

//Union is used to save unused memory as each data has one datatype
union _shared{
    char   s[MAX_NMEA_STRING];  // String
    int    d;                   // Integer
    float  f;                   // Float
};

//Datatype of individual elements of struct nmea.
//Contains individual parameters of GPS data
typedef struct {
    bucket_type_t type;
    union _shared value;  
}bucket;

//Struct that saves the GPS parameters
//Parameters are accessed through passing `gps_data` enum to the index of of nmea.data[] 
//Inherit 'Missing values' protection is implemented in each struct `bucket`.
typedef struct{
    bucket data[16];
}nmea;

//Static variables declared with underscore in the start to indicate they are for internal use
static char     *_gps_raw    = 0;
static bucket   *_info       = NULL;
static int      _info_size   = 0;
static int      _parse_index = 0;

//print function for individual elements of struct nmea
void print_parsed (bucket _data)
{
    if ( _data.type == String  )  printf("%s", _data.value.s);
    if ( _data.type == Integer )  printf("%d", _data.value.d);
    if ( _data.type == Float   )  printf("%f", _data.value.f);
    if ( _data.type == Missing )  printf("Empty NMEA parameter");
    printf("\n");
}

//Checksum checking function. Returns either true (checksum successful) or false (checksum failed)
bool integrity(char* data)
{
    int i = 0, checksum = 0;
    unsigned char calc = 0;
    char element = data[i]; 
    char *slice;
    if (element == '$' ){
        while (true){
            i++; element = data[i];
            if (element == '\0'){
                printf("Invalid NMEA CODE\n");
                return false;
            }
            if(element == '*'){
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

//Helper function -> used to extract single character input string
char get_char()
{
    _parse_index++;
    return _gps_raw[_parse_index-1];
}

//Helper function -> Reverts the effects of get_char()
void putback()
{
    _parse_index--;
}

// Tokenizes the string between two commas into `bucket` datatype  
bucket comma_parse()
{
    bucket token;
    char* str_alloc = 0;
    char slice[MAX_NMEA_STRING] ="";
    int i = 0;
    bucket_type_t type = String;
    int val = get_char();
    if( val >= '0' && val <= '9' ){
        type = Integer;
    }
    while (true){
        slice[i] = val;
        i++;
        val = get_char();
        if(val == '.') type = Float;
        if(val == ',') break;
    }
    slice[i] = '\0';
    if(type == Float){
        token.type = Float;
        sscanf(slice,"%f", &token.value.f);
    } else if(type == Integer){
        token.type = Integer;
        sscanf(slice,"%d",&token.value.d);
    } else {
        token.type = String;
        int count = 0;
        while(1){
            if(slice[count] == '\0') break;
            count++;
        }
        strcpy(token.value.s, slice);
    }
    return token;
}

// Internal function that appends tokenized parameters into an array
// Input: Raw GPS string data
// Overflow protection: To avoid overflow in array, last element with 
// It saves the array in a global dynamic array
// Memeory managment is implemented to prevent data leaks
void _parsed_data(char *data)
{
    _info_size=0;
    if(_info!=NULL){
        free(_info);
        _info=NULL;
    }
    bucket end;
    end.type = Undefined; //End character for our bucket data
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
            temp.type = Missing; //Shows some data is missing from NMEA format
            _info[_info_size-1] = temp;
            continue;
        }
        putback();
        _info[_info_size-1] = comma_parse(data);
    }
    _info_size++;
    _info = realloc(_info,_info_size*sizeof(bucket));
    _info[_info_size-1] = end;
}

// INTERFACE METHOD:
// Input Arguments: Argument1 (NMEA string code), Argument2 (Output variable passed by refrence)
// Note: Pass enum `gps_data` as index to get your desired value
void parse_gps_data(char *data, nmea *output)
{
    _parsed_data(data);
    int i =0;
    while((_info+i)->type != Undefined){
        output->data[i].type  = (_info+i)->type;
        if((_info+i)->type == String){
            strcpy(output->data[i].value.s, (_info+i)->value.s);
        } else if((_info+i)->type == Integer){
            output->data[i].value.d = (_info+i)->value.d;
        } else if((_info+i)->type == Float){
        output->data[i].value.d = (_info+i)->value.d;
        }
        i++;
    }
    output->data[i].type = _info[i].type;
    if(_info!=NULL){
    free(_info);
    _info=NULL;
    }
}


