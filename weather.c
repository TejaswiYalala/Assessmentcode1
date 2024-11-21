#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define WEATHER_API_KEY "my_api_key" // Replaced with generated openweather api key
#define MAX_CITIES 3
#define BUFFER_SIZE 10000

typedef struct {
    char *content;
    size_t length;
} DataBuffer;

DataBuffer response_buffer = {NULL, 0};

void initialize_buffer(DataBuffer *buf) {
    buf->content = malloc(1);
    buf->length = 0;
}

size_t response_callback(void *data, size_t size, size_t nmemb, void *user_data) {
    size_t total_size = size * nmemb;
    DataBuffer *buf = (DataBuffer *)user_data;

    char *temp = realloc(buf->content, buf->length + total_size + 1);
    if (temp == NULL) {
        fprintf(stderr, "Memory allocation error!\n");
        return 0;
    }

    buf->content = temp;
    memcpy(&(buf->content[buf->length]), data, total_size);
    buf->length += total_size;
    buf->content[buf->length] = 0;

    return total_size;
}

void fetch_weather_data(const char *city_name) {
    CURL *curl;
    CURLcode result;
    char request_url[512];

    // Constructing the URL with parameters
    snprintf(request_url, sizeof(request_url),
             "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s&units=metric",
             city_name, WEATHER_API_KEY);

    initialize_buffer(&response_buffer);
    curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "Failed to initialize CURL.\n");
        return;
    }

    // Setting CURL options in a different order for diversity
    curl_easy_setopt(curl, CURLOPT_URL, request_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "WeatherApp/1.0");

    // Resetting CURL session to demonstrate different approach
    curl_easy_reset(curl);

    // Performing the request
    result = curl_easy_perform(curl);

    // Error handling in a different way
    if (result != CURLE_OK) {
        fprintf(stderr, "Error fetching weather: %s\n", curl_easy_strerror(result));
    } else {
        printf("Weather information for %s:\n%s\n", city_name, response_buffer.content);
    }

    // Cleanup
    curl_easy_cleanup(curl);
    free(response_buffer.content);
}

void add_to_favorites(char cities[MAX_CITIES][50], int *city_count, const char *city_name) {
    if (*city_count >= MAX_CITIES) {
        printf("Favorites list full! Please remove a city before adding another.\n");
        return;
    }

    for (int i = 0; i < *city_count; i++) {
        if (strcmp(cities[i], city_name) == 0) {
            printf("City %s is already in your favorites.\n", city_name);
            return;
        }
    }

    strncpy(cities[*city_count], city_name, 50);
    (*city_count)++;
    printf("Added %s to your favorites.\n", city_name);
}

void display_favorites(char cities[MAX_CITIES][50], int city_count) {
    if (city_count == 0) {
        printf("No favorite cities to display.\n");
        return;
    }

    printf("Your Favorite Cities:\n");
    for (int i = 0; i < city_count; i++) {
        printf("%d. %s\n", i + 1, cities[i]);
        fetch_weather_data(cities[i]);
    }
}

void remove_from_favorites(char cities[MAX_CITIES][50], int *city_count, int index) {
    if (index < 0 || index >= *city_count) {
        printf("Invalid index!\n");
        return;
    }

    // Shift cities down after removal
    for (int i = index; i < *city_count - 1; i++) {
        strcpy(cities[i], cities[i + 1]);
    }

    (*city_count)--;
    printf("Removed city from favorites.\n");
}

int main() {
    char favorite_cities[MAX_CITIES][50];
    int favorite_count = 0;
    int option;

    CURLcode curl_status = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (curl_status != CURLE_OK) {
        fprintf(stderr, "CURL initialization failed: %s\n", curl_easy_strerror(curl_status));
        return 1;
    }

    while (1) {
        printf("\nWeather Application Menu:\n");
        printf("1. Search for Weather Data of a City\n");
        printf("2. Add a City to Favorites\n");
        printf("3. View Favorite Cities\n");
        printf("4. Remove a City from Favorites\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &option);

        if (option == 5) {
            break;
        }

        char city_name[50];
        int city_index;

        switch (option) {
            case 1:
                printf("Enter the city name: ");
                scanf("%s", city_name);
                fetch_weather_data(city_name);
                break;
            case 2:
                printf("Enter the city name to add to favorites: ");
                scanf("%s", city_name);
                add_to_favorites(favorite_cities, &favorite_count, city_name);
                break;
            case 3:
                display_favorites(favorite_cities, favorite_count);
                break;
            case 4:
                printf("Enter the index of the city to remove (1-%d): ", favorite_count);
                scanf("%d", &city_index);
                remove_from_favorites(favorite_cities, &favorite_count, city_index - 1);
                break;
            default:
                printf("Invalid option, please try again.\n");
                break;
        }
    }

    curl_global_cleanup();
    return 0;
}
