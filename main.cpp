/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include <string.h>
#include <string> 
#include <sstream>
#include <iostream> 

extern "C" {
#include "pico/stdlib.h"
#include "hardware/uart.h"
}

using namespace std;

#define STEP_PIN 21
#define DIR_PIN 20
//#define ENABLE_PIN 36 // enable pin toggles if the driver gets power or not - not currently wired though

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

void blink_led(int times){
    if(led_on == true){
        toggle_led(); // turn off to start
    }
    for(int i = 0; i < times; i++){
        toggle_led();
        sleep_ms(100);
        toggle_led();
    }
}

void init_stepper(){

    gpio_init(STEP_PIN);
    gpio_set_dir(STEP_PIN, GPIO_OUT);

    gpio_init(DIR_PIN);
    gpio_put(DIR_PIN, true);
    gpio_set_dir(DIR_PIN, GPIO_OUT);

    //gpio_init(ENABLE_PIN);
    //gpio_set_dir(ENABLE_PIN, GPIO_OUT);
    // gpio_put(ENABLE_PIN, 1); // disable by default
   // gpio_put(ENABLE_PIN, 0); // enable driver - does nothing for now - torque always on
}

void step_motor(int steps, bool dir, int delay_us = 10000)
{
    gpio_put(DIR_PIN, dir);  // true = CW, false = CCW
   // gpio_put(ENABLE_PIN, 0); // Enable driver (active low)

    for (int i = 0; i < steps; i++)
    {
        gpio_put(STEP_PIN, 1);
        sleep_us(10); // Minimum HIGH pulse: 1.9us, so 10us is safe
        gpio_put(STEP_PIN, 0);
        sleep_us(delay_us - 10); // Adjust for total step delay
    }

    //gpio_put(ENABLE_PIN, 0); // Disable if you want to cut power
    
}

void init_everything(){
    init_stepper();
    gpio_init(25);              // Initialize GPIO 25 (onboard LED)
    gpio_set_dir(25, GPIO_OUT); // Set it as an output
    stdio_init_all();
    // move motor back and forth a little to check if alive
    step_motor(20, true, 10000);
    sleep_ms(100);
    step_motor(20, false, 10000);
}
void process_command(string line){
    // break cmd into arguments
    string cmd;
    istringstream iss(line);
    iss >> cmd;

    if(cmd == "step"){
        int steps;
        string dir;
        int delay;
        iss >> steps >> dir >> delay;
        bool clockwise = (dir == "cw" || dir == "CW");
        step_motor(steps,clockwise,delay = 10000); // delay should have a default to be 10000
        cout<<"motor moved: "<<steps<<" steps "<<dir<<endl;
    }
    else if(cmd == "led"){
        string led_arg; 
        int blink_times;
        iss>>led_arg;
        if(led_arg == "toggle"){
            toggle_led();
            cout<<"--toggled--"<<endl;
        }
        if(led_arg == "blink"){
            iss >> blink_times;
            blink_led(blink_times);
            cout<<"blinking "<<blink_times<<" times"<<endl;
        }
    }
}

string get_command(){
    string line;
    getline(cin, line);
    return line;
}

int main() {
    init_everything();


    while(1){
        process_command(get_command());
    }


    // int i = 0;
    // char buf[64];
    // while (true) {



    //     if(fgets(buf, sizeof(buf), stdin));{
    //         buf[strcspn(buf, "\r\n")] = 0; // Strip newline

    //         if (strcmp(buf, "led toggle") == 0){
    //             toggle_led();
    //             printf("--toggled--\n");
    //         }
    //         if (strcmp(buf, "step") == 0)
    //         {
    //             printf("stepping x\n");
    //             step_motor(200, true, 10000); // 200 steps CW
    //             sleep_ms(1000);
    //         }

    //         if (strcmp(buf, "led blink") == 0)
    //         {
    //             printf("--blink--\n");
    //             while(true){
    //                 toggle_led();
    //                 sleep_ms(200);
    //             }
    //         }

    //         else
    //         {
    //             printf("ERR: unknown command\n");
    //         }
    //     }
    // }
}

