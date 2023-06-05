# NMEA Parser library (v2.0) Explained

## Update Notice
The previous version of NMEA Parser was a boilerplate which has been converted into a library.
The new libray "NMEA Parser" has a simple interface method `parse_gps_data()`. Usage is explained below.

## Libray Interface
```
parsed* parse_gps_data(char *data);
```
The above method is implemeted in libray and can be used to split NMEA string into an array of tokens of parsed datatype.

##  Example
```
parsed* output;
output = parse_gps_data(char *data);
```
## Caution and Advice
The returned pointer is an address to an internal dynamic array which will be overwritten in the next  call of parse_gps_data() function. To avoid the losing data, deep copy the returned string

Alternatively, add `#define NMEA_COPY` to return a copy of dynamic string (Beware of memory leaks)

## `parsed` datatype
```
typedef struct {
    int type;  
    union _parsed value;  
}parsed;

union _parsed{

    char *str;
    int num;
    float points;
};
```

## Valid values for parsed.type
`parsed.type` can have one of the following values:
```
    0 for string (char*)
    1 for int
    2 for float
    -1 for missing
    -2 to indicate end element (to be used for iteration of array like '\0' in string)
```

## Checksum Verification and data validation
```
bool integrity(char* data);
```
The above function will take the NMEA data as parameter and calculate its checksum.

## Example
An example string `$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\n` is used in the code. It is first verified and then prints all the values of output.

## Special print function for parsed datatype 
To print the output, iterate over all the values of `info` variable and pass them as a parameter to the following function.
```
    void print_parsed (parsed _parsed);

Example:

    while(output[i].type!=-2){
        print_parsed(output[i]);
        i++;
    }
```