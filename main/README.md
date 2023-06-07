# NMEA Parser library (v2.1) Explained

## Update Notice
The previous version 1.0 of NMEA Parser has been converted into a library.
The new libray "NMEA Parser" has a simple interface method `parse_gps_data()`. Usage is explained below.

## Libray Interface
```
void parse_gps_data(char *data, nmea *output)
```
`Argument1:` Raw GPS data in String format
`Argument2:` Pass by reference output variable of dataype nmea

## Accessing individual elements
Individual parameters of GPS data in `nmea` struct can be acessed by passing the below enums to the index in nmea.data[] object. Enums are listed below: 
```
    identifier_s            (Log header)
    utc_d                   (UTC time status  of position (HHMMSS))
    latitude_f              (Latitude)
    lat_hemisphere_s        (Latitude direction (N = North, S = South))
    longitude_f             (Longitude)
    long_hemisphere_s       (Longitude direction)
    gps_quality_d           (GPS Quality)
    satelites_d             (No. of satellites)
    hdop_f                  (Horizontal Dilution of Precision)
    altitude_f              (Antenna altitude above/below mean sea level)
    altitude_unit_s         (Units of antenna altitude (M = metres))
    undulation_f            (Relationship between the geoid and the WGS84 ellipsoid)
    undulation_unit_s       (Units of undulation (M = metres))
    age_of_diff_f           (age of differential correction GPS data)
    stn_id_s                (Diffrential Base station ID)
```
##  Example
```
char data[] = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
nmea output;
parse_gps_data(data, &output);
if (output.data.[gps_quality].type != Missing){
    printf("GPS Quality: %d \n",output.data.[gps_quality].type);
} else {
    printf("GPS Quality is missing.\n");
}
float longitude = output.data.[longitude].value.f;
float lattitude = output.data[lattitude].value.f;
char* latitude_hemisphere = output.data[lat.hemisphere].value.s;
```

## `nmea` datatype
```
typedef struct{
    bucket data[16];
}nmea;
```

## Valid values for nmea.data[].type
`nmea.data[].type` can have one of the following enum values:
```
    String
    Integer
    Float
    Missing
    Undefined
```
//The naming convention of enums is `parameter_format`.

## Checksum Verification and data validation
```
bool integrity(char* data);
```
The above function will take the NMEA data as parameter and calculate its checksum.

## Example
An example string `$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\n` is used in the code. It is first verified and then prints all the values of output.

## Special print function for bucket datatype 
To print the output, iterate over all the values of `info` variable and pass them as a parameter to the following function.
```
    void print_parsed (bucket _parsed);

Example:

    while(output.data[i].type!=Undefined){
            print_parsed(output.data[i]);
            i++;
    }
```