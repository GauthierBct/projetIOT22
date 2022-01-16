#include <stdio.h>
#include "periph/gpio.h"
#include "xtimer.h"

/* capteur humidité, CO2, température */
#include "scd30.h"
#include "scd30_params.h"
#include "scd30_internal.h"

/* capteur mouvement */
#include "pir.h"
#include "pir_params.h"

/* include lora */
#include "net/loramac.h"
#include "semtech_loramac.h"

/* Add the cayenne_lpp header here */
#include "cayenne_lpp.h"

/* Threading */
#include "thread.h"
static char stack[THREAD_STACKSIZE_MAIN];
kernel_pid_t a = -1;

static long FREQUENCE = 5000;
static gpio_t BUZZER_PIN = GPIO_PIN(1, 5);  // PB5 -> D5

/* Declare globally the loramac descriptor */
extern semtech_loramac_t loramac;

/* Declare globally Cayenne LPP descriptor here */
static cayenne_lpp_t lpp;

/* Device and application informations required for OTAA activation */
static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0x64, 0x05, 0xe0, 0xfd, 0xa2, 0x58, 0x65, 0x82 };
static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0x01, 0x3B, 0xF2, 0x33, 0x0C, 0x6D, 0xA2, 0x03, 0x4D, 0xDE, 0x8E, 0xD0, 0x07, 0xB6, 0x4E, 0x6A };

#define MEASUREMENT_INTERVAL_SECS (10)

scd30_t scd30_dev_scd;
scd30_params_t params = SCD30_PARAMS;
scd30_measurement_t result;

static pir_t dev_pir;

#define PANIC_BUTTON_PIN GPIO_PIN(1, 13) //PA9 -> D0
int flag_panic = 0;


#define NBR_MEAS 2

timex_t  now_time, old_time;
volatile int appui = 1, appui_court = -1, appui_long = -1;

static int flag_sound = 0;

static void sender(scd30_measurement_t result, uint8_t status_pir, uint8_t test)
{
    puts("sending...");
    /* TODO: prepare cayenne lpp payload here */
    cayenne_lpp_add_temperature(&lpp, 0, result.temperature);
    cayenne_lpp_add_analog_input(&lpp, 1, result.co2_concentration/100);
    //cayenne_lpp_add_relative_humidity(&lpp, 2, result.relative_humidity);
    cayenne_lpp_add_presence(&lpp, 2, status_pir);
    cayenne_lpp_add_digital_input(&lpp, 3, test);

    /* send the LoRaWAN message */
    uint8_t ret = semtech_loramac_send(&loramac, lpp.buffer, lpp.cursor);
    
    if (ret != SEMTECH_LORAMAC_TX_DONE) {
        printf("Cannot send lpp message, ret code: %d\n", ret);
    }

    /* TODO: clear buffer once done here */
    cayenne_lpp_reset(&lpp);
    /* this should never be reached */

    return;
}


static void *thread_handler(void *arg)
{
    (void)arg;
    while(1) {
        if (flag_sound == 0) {
            thread_sleep();
        }
        // Boucle pour une seconde
        //float NBR_iter = duration*435;
        for (int i=0; i < 435; i++) {
            // 1 er ton
            gpio_write(BUZZER_PIN, 1);
            xtimer_usleep(1149);
            gpio_clear(BUZZER_PIN);
            xtimer_usleep(1149);

        }
        //NBR_iter = duration*735;
        for (int i=0; i < 735; i++) {
            // 2 eme ton
            gpio_write(BUZZER_PIN, 1);
            xtimer_usleep(683);
            gpio_clear(BUZZER_PIN);
            xtimer_usleep(683);
        }
    } 
    return NULL;
}

void cb_panic_button (void *arg)
{
    (void) arg;

    if (appui == 0) {
        // front montant
        xtimer_now_timex(&old_time);
        appui = 1;
    } else {
        // front descendant
        xtimer_now_timex(&now_time);
        long time_push = now_time.seconds - old_time.seconds;
        if (time_push > 2) {
            appui_long = 1;
            appui_court = 0;
        } else {
            appui_long = 0;
            appui_court = 1;
        }

        appui = 0;
    }

}

int main(void)
{

    /*===================INITIALISATION BUZZER========================*/
    if (gpio_init(BUZZER_PIN, GPIO_OUT)) {
        printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_B, 5);
        return -1;
    }

    /*===================INITIALISATION BUTTON========================*/
    xtimer_now_timex(&old_time);
    if (gpio_init_int(PANIC_BUTTON_PIN, GPIO_IN_PD, GPIO_BOTH, cb_panic_button, (void *)1)) {
        puts("Error on button init");
        return 1;
    }

    /*===================INITIALISATION LORA========================*/
    /* use a fast datarate so we don't use the physical layer too much */
    semtech_loramac_set_dr(&loramac, 5);

    /* set the LoRaWAN keys */
    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);

    /* start the OTAA join procedure */
    puts("Starting join procedure");
    if (semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA) != SEMTECH_LORAMAC_JOIN_SUCCEEDED) {
        puts("Join procedure failed");
        return 1;
    }
    
    puts("Join procedure succeeded");

    /*===================INITIALISATION PIR========================*/
    pir_params_t pir_parametres;
    pir_parametres.gpio = GPIO_PIN(1, 10);
    pir_parametres.active_high = 1;
    if (pir_init(&dev_pir, &pir_parametres) != 0) {
        puts("Error on pir init");
        return 1;
    }


    /*===================INITIALISATION SCD========================*/
    scd30_init(&scd30_dev_scd, &params);
    //scd30_reset(&scd30_dev_scd);
    uint16_t pressure_compensation = SCD30_DEF_PRESSURE;
    scd30_set_param(&scd30_dev_scd, SCD30_INTERVAL, MEASUREMENT_INTERVAL_SECS);
    scd30_set_param(&scd30_dev_scd, SCD30_START, pressure_compensation);

    a = thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_SLEEPING, thread_handler, NULL, "tone_gendarmerie");
    appui_court = 0;

    int nb_tour = 0;
    xtimer_sleep(5);
    uint8_t status_pir;

    while (1) {

        if (nb_tour%100 == 0) {
            scd30_read_triggered(&scd30_dev_scd, &result);
            if (result.co2_concentration > 6500) {
                status_pir = pir_get_status(&dev_pir);
                flag_sound = 1;
                sender(result, status_pir, 0);
            }
        }

        if (appui_long == 1) {
            flag_sound = flag_sound == 1 ? 0 : flag_sound;
            appui_long = 0;
        }
        if (appui_court == 1) {
            appui_court = 0;
            flag_sound = flag_sound == 0 ? 1 : flag_sound;
            scd30_read_triggered(&scd30_dev_scd, &result);
            status_pir = pir_get_status(&dev_pir);
            sender(result, status_pir, 1);
        }

        if (flag_sound == 1) {
            if (thread_is_active(thread_get(a)) == 0)
                thread_wakeup(a);
        }

        nb_tour++;
        xtimer_usleep(1000*200);
    }
    sender(result, status_pir, 0);
    return 0;
}

