# Assessmentcode1

Problem statement: To create a command line application that is able to interact with openweather API to fetch weather details of cities, along with being to able to add, modify and remove favourite cities(allowed upto 3 cities).
In this code, different libraries are used :
       stdio.h is for standard input/output functions (like printf and scanf).
       stdlib.h is for memory allocation and other utility functions (like malloc, free).
       string.h provides string manipulation functions (like strcmp, strncpy).
       curl/curl.h is the header for the cURL library, which is used to fetch data from the 
       internet (here, weather data from the OpenWeather API).

For installing curl library in linux, the following command is used: install libcurl4-openssl-dev
gcc is used to complie the code by running the following command in terminal: 
           gcc -o weather weather.c -lcurl
           
