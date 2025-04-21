/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

    bool led_on = false;

void toggle_led(){
    if(led_on == false){
        gpio_put(25, 1);
        led_on = true;
    } else {
        gpio_put(25, 0);
        led_on = false;
    }
}


void init_everything(){
    gpio_init(25);              // Initialize GPIO 25 (onboard LED)
    gpio_set_dir(25, GPIO_OUT); // Set it as an output
    stdio_init_all();
}

int main() {
    init_everything();

    int i = 0;
    char buf[64];
    while (true) {
        //toggle_led();

        if(fgets(buf, sizeof(buf), stdin));{
            buf[strcspn(buf, "\r\n")] = 0; // Strip newline

            if (strcmp(buf, "led toggle") == 0){
                toggle_led();
                printf("--toggled--\n");
            }
            if (strcmp(buf, "led blink") == 0)
            {
                printf("--blink--\n");
                while(true){
                    toggle_led();
                    sleep_ms(200);
                }
            }

            else
            {
                printf("ERR: unknown command\n");
            }
        }
    }
}

