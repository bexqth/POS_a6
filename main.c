#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct buffer_t {
    int *array;
    int capacity;
    int index;
    int pocetDokazovPrvy;
    int pocetDokazovDruhy;
    int pocetDokazovTreti;
} buffer_t;

void buffer_ini(buffer_t *buff, int capacity) {
    buff->array = malloc(sizeof(int) * capacity);
    buff->capacity = capacity;
    buff->index = 0;
    buff->pocetDokazovPrvy = 0;
    buff->pocetDokazovDruhy = 0;
    buff->pocetDokazovTreti = 0;
}

void buffer_destroy(buffer_t *buff) {
    free(buff->array);
}

bool buffer_push(buffer_t *buff, int data) {
    if(buff->index < buff->capacity) {
        buff->array[buff->index] = data;
        buff->index++;
        return true;
    }
    return false;
}

int buffer_pull(buffer_t *buff) {
    if(buff->index > 0) {
        return buff->array[--buff->index];
    }
    return -1;
}

typedef struct thread_data_t {
    pthread_mutex_t mutex;
    pthread_cond_t policia;
    pthread_cond_t poirot;
    buffer_t buff;

} thread_data_t;

void thread_data_init(thread_data_t *data, int capacity) {
    pthread_mutex_init(&data->mutex, NULL);
    pthread_cond_init(&data->policia, NULL);
    pthread_cond_init(&data->poirot, NULL);
    buffer_ini(&data->buff, capacity);
}

void thread_data_destroy(thread_data_t *data) {
    pthread_mutex_destroy(&data->mutex);
    pthread_cond_destroy(&data->policia);
    pthread_cond_destroy(&data->poirot);
    buffer_destroy(&data->buff);
}

int pridajDokaz(int min, int max) {
    int rozsah = max - min + 1;
    int cislo = rand() % rozsah;
    //printf("cislo je %d \n", min + cislo);
    return min + cislo;


}

void* policia_fun(void* data) {
    thread_data_t* data_t = (thread_data_t*) data;

    //while(true) {
    for(int i = 0; i < 10; i++) {
        pthread_mutex_lock(&data_t->mutex);
        int dokaz = pridajDokaz(1,3);

        while(!buffer_push(&data_t->buff, dokaz)) {
            pthread_cond_wait(&data_t->policia, &data_t->mutex);
        }

        //printf("Policie vložila do bufferu: %d\n", dokaz);
        pthread_mutex_unlock(&data_t->mutex);
        pthread_cond_signal(&data_t->poirot);
    }

}

void vyhodnotDokaz(buffer_t *buff, int dokaz) {
    if(dokaz == 1) {
        buff->pocetDokazovPrvy++;
    } else if(dokaz == 2) {
        buff->pocetDokazovDruhy++;
    } else if(dokaz == 3){
        buff->pocetDokazovTreti++;
    }
    printf("pocet dokazkov prveho %d \n", buff->pocetDokazovPrvy);
    printf("pocet dokazkov druheho %d \n", buff->pocetDokazovDruhy);
    printf("pocet dokazkov tretieho %d \n", buff->pocetDokazovTreti);
}

void* poirot_fun(void* data) {
    thread_data_t* data_t = (thread_data_t*) data;

    //while(true) {
    for(int i = 0; i < 10; i++) {
        pthread_mutex_lock(&data_t->mutex);
        while(data_t->buff.index == 0) {
            pthread_cond_wait(&data_t->poirot, &data_t->mutex);
        }

        int dokaz = buffer_pull(&data_t->buff);
        vyhodnotDokaz(&data_t->buff, dokaz);
        //printf("Poirot vyjmul z bufferu: %d\n", dokaz);
        pthread_mutex_unlock(&data_t->mutex);
        pthread_cond_signal(&data_t->policia);
    }

    if(data_t->buff.pocetDokazovPrvy >= data_t->buff.pocetDokazovDruhy && data_t->buff.pocetDokazovPrvy >= data_t->buff.pocetDokazovTreti) {
        printf("Vinik je podozrivy cislo jedna");
    } else if (data_t->buff.pocetDokazovDruhy >= data_t->buff.pocetDokazovPrvy && data_t->buff.pocetDokazovDruhy >= data_t->buff.pocetDokazovTreti) {
        printf("Vinik je podozrivy cislo dva");
    } else if (data_t->buff.pocetDokazovTreti >= data_t->buff.pocetDokazovPrvy && data_t->buff.pocetDokazovTreti >= data_t->buff.pocetDokazovDruhy) {
        printf("Vinik je podozrivy cislo tri");
    }

}


int main() {
    srand(time(NULL));

    thread_data_t data;
    thread_data_init(&data, 10);

    pthread_t policia, poirot;
    pthread_create(&policia, NULL, policia_fun, &data);
    pthread_create(&poirot, NULL, poirot_fun, &data);

    pthread_join(policia, NULL);
    pthread_join(poirot, NULL);

    thread_data_destroy(&data);

    return 0;
}
