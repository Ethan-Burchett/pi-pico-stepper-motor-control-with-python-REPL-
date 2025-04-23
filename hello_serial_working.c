/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define STEP_PIN 21
#define DIR_PIN 20
#define ENABLE_PIN 36 // enable pin toggles if the driver gets power or not

// 38 is ground
// 36 is 3v3 out
// 27 is something - GP21 - step?
// 26 is something - GP20 - dir?

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

void init_stepper(){

    gpio_init(STEP_PIN);
    gpio_set_dir(STEP_PIN, GPIO_OUT);

    gpio_init(DIR_PIN);
    gpio_put(DIR_PIN, true);
    gpio_set_dir(DIR_PIN, GPIO_OUT);

    gpio_init(ENABLE_PIN);
    gpio_set_dir(ENABLE_PIN, GPIO_OUT);
    // gpio_put(ENABLE_PIN, 1); // disable by default
    gpio_put(ENABLE_PIN, 0); // enable driver
}

void init_everything(){
    init_stepper();
    gpio_init(25);              // Initialize GPIO 25 (onboard LED)
    gpio_set_dir(25, GPIO_OUT); // Set it as an output
    stdio_init_all();
}

void step_motor(int steps, bool dir, int delay_us)
{
    gpio_put(DIR_PIN, dir);  // true = CW, false = CCW
    gpio_put(ENABLE_PIN, 0); // Enable driver (active low)

    for (int i = 0; i < steps; i++)
    {
        gpio_put(STEP_PIN, 1);
        sleep_us(10); // Minimum HIGH pulse: 1.9us, so 10us is safe
        gpio_put(STEP_PIN, 0);
        sleep_us(delay_us - 10); // Adjust for total step delay
    }

    gpio_put(ENABLE_PIN, 1); // Disable if you want to cut power
}

int main() {
    init_everything();

    int i = 0;
    char buf[64];
    while (true) {

        if(fgets(buf, sizeof(buf), stdin));{
            buf[strcspn(buf, "\r\n")] = 0; // Strip newline

            if (strcmp(buf, "led toggle") == 0){
                toggle_led();
                printf("--toggled--\n");
            }
            if (strcmp(buf, "step") == 0)
            {
                printf("stepping x\n");
                step_motor(200, true, 10000); // 200 steps CW
                sleep_ms(1000);
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

