/* STATE MACHINE EXAMPLE WITH GPIO

   @Autor José Maurício Alencar Filho

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

/**
 * Brief:
 * This example demonstrates how to use gpio and gpio interrupt to manage the state of a state machine using FreeRTOS.
 *
 * GPIO status:
 * GPIO0:  input, pulled up, interrupt from ANY EDGE
 */

//DEFINES
#define GPIO_INPUT_IO_0     0
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0))
#define ESP_INTR_FLAG_DEFAULT 0

//VARIAVEIS
static xQueueHandle xGpio_evt_queue = NULL;

enum eState_Machine{
    STATE_OFF = 0,
    STATE_ON,
    STATE_PROTECTED    
};

//Vector of ISR to send queue determins menssage do task gerenciation state machine
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t ulGpio_num = (uint32_t) arg;
    xQueueSendFromISR(xGpio_evt_queue, &ulGpio_num, NULL);
}

//TAREFAS
static void prvState_Machine_task(void* arg)
{
    uint32_t ulIo_num;

    //initial state is STATE_OFF
    enum eState_Machine eGPIO_SM = STATE_OFF;

    //aux variable to receive rising edge ISR into GPIO
    uint8_t ucAux_state_change = 0;

    for(;;) {

        //handler of received queue isr
        if(xQueueReceive(xGpio_evt_queue, &ulIo_num, pdMS_TO_TICKS(0))) {

            //checks which gpio signaled the ISR and checks if the state is not protected
            if(ulIo_num == GPIO_INPUT_IO_0 && eGPIO_SM != STATE_PROTECTED)
            {
                //update helper flag to 1 signaling gpio isr
                ucAux_state_change = 1;
            }
        }

        //machine state
        switch (eGPIO_SM)
        {
        case STATE_OFF:
            //verify aux state changed
            if(ucAux_state_change == 1)
            {
                //clean aux state 
                ucAux_state_change = 0;

                //change machine state to STATE_ON
                eGPIO_SM = STATE_ON;
            }
            break;
        case STATE_ON:

            //verify aux state changed
            if(ucAux_state_change == 1)
            {
                //clean aux state 
                ucAux_state_change = 0;

                //change machine state to STATE_PROTECTED
                eGPIO_SM = STATE_PROTECTED;
            }
            break;
        case STATE_PROTECTED:
            //block this task for 10 secs
            vTaskDelay( 1000 * 10 / portTICK_PERIOD_MS);

            //moving state machine for STATE_OFF
            eGPIO_SM = STATE_OFF;
            break;
        default:
            break;
        }

    }

    //code should never get here, but for security vTaskDelete should be used
    vTaskDelete(NULL);
}

//MAIN
void app_main(void)
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    
    //interrupt of ANY EDGE
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    //bit mask of the pins, use GPIO0 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);


    //create a queue to handle gpio event from isr
    xGpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    
    //start state machine task
    xTaskCreate(prvState_Machine_task, "STATE MACHINE TASK", 1024 * 10, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
}
