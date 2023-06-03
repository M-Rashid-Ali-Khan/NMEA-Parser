# -NMEA-Parser

##  Inputs and Outputs
This folder contains the c code for the parsing boilerplate.
It converts an NMEA code in string format to a (dynamic) array of datatype parsed.
The parsed datatype is given as follows:
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
## type element of datatype parsed 

`parsed.type` can have one of the following values:
```
    0 for string (char*)
    1 for int
    2 for float
    -1 for missing
    -2 to indicate end element (to be used for iteration of array like '\0' in string)
```
## Boilerplate Usage
Update the NMEA string in the global variable `char gps_raw[]`. Call `parsing(gps_raw)` and the output will be saved to the global variable `char parsed *info`.

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

    while(info[i].type!=-2){
        print_parsed(info[i]);
        i++;
    }
```