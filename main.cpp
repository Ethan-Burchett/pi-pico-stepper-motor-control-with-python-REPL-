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

extern "C"
{ // put c libraries here so compiler doesn't get confused
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "pico/multicore.h"
}

using namespace std;

#define steps_per_rev 200

#define STEP_PULSE_WIDTH_US 10
#define STEP_PIN 21
#define DIR_PIN 20
// #define ENABLE_PIN 36 // enable pin toggles if the driver gets power or not - not currently wired though

// pico to DRV8825 pin connections
// 38: GND
// 36: 3V3 OUT
// 27: STEP
// 26: DIR

// initializing stepper driver
void init_stepper()
{

    gpio_init(STEP_PIN);
    gpio_set_dir(STEP_PIN, GPIO_OUT); // GPIO_OUT sets step_pin as output

    gpio_init(DIR_PIN);
    gpio_put(DIR_PIN, true);
    gpio_set_dir(DIR_PIN, GPIO_OUT);
}

// globals for stepper motor:
volatile int step_delay_us = 10000;
volatile bool motor_running = false;
volatile bool motor_direction = false;
volatile int steps_remaining = 0;

void continuous_stepper()
{
    while (true)
    {
        if (motor_running)
        {
            if (steps_remaining > 0)
            {
                gpio_put(DIR_PIN, motor_direction);
                // start one motor step
                gpio_put(STEP_PIN, 1);
                sleep_us(STEP_PULSE_WIDTH_US);
                gpio_put(STEP_PIN, 0);
                sleep_us(step_delay_us - STEP_PULSE_WIDTH_US);
                // end of one motor step
                steps_remaining--;

                if (steps_remaining == 0)
                {
                    motor_running = false;
                }
            }
        }
        else
        {
            sleep_ms(2); // chill when not stepping
        }
    }
}

void step_motor(int steps, bool dir, int delay_us)
{
    motor_direction = dir;
    step_delay_us = delay_us;
    steps_remaining = steps;
    motor_running = true;
}

// input: seconds per revolution
// output: step delay in us
int calculate_step_delay_from_secs_per_rev(float seconds)
{
    float microseconds = seconds * 1000000;
    int delay_us = microseconds / 200;
    return delay_us; // returns how many microseconds the step delay should be to get the desired seconds per revolution
}

// function to turn motor by number of revolution
void rotate_motor(float revolutions, bool dir, float seconds_per_rev)
{
    int steps = revolutions * 200; // steps_per_rev = 200

    int step_delay = calculate_step_delay_from_secs_per_rev(seconds_per_rev);
    step_motor(steps, dir, step_delay); // call the step_motor function
}

// function to calculate number of revolutions from extrusion time and seconds per revolution
float calculate_revolutions_from_time_and_speed(float extrusion_time, float seconds)
{
    float revolutions = extrusion_time / seconds;
    return revolutions;
    
}

// function to rotate motor based on extrusion time and seconds per revolution
void rotate_motor_time(float extrusion_time, bool dir, float seconds_per_rev)
{
    float revolutions = calculate_revolutions_from_time_and_speed(extrusion_time, seconds_per_rev);
    rotate_motor(revolutions, dir, seconds_per_rev);
}

// function to rotate motor at different rates based on impedance
void rotate_motor_impedance(float impedance, float extrusion_time)
{
    bool dir = true;
    float seconds_per_rev = 0;

    if (impedance < 117)
    {
        seconds_per_rev = 10;
    }
    else if (impedance >= 117 && impedance <= 137)
    {
        seconds_per_rev = 30;
    }
    else if (impedance > 137)
    {
        seconds_per_rev = 50;
    }
    rotate_motor_time(extrusion_time, dir, seconds_per_rev);
}

void init_everything()
{
    init_stepper();
    gpio_init(25);              // Initialize GPIO 25 (onboard LED)
    gpio_set_dir(25, GPIO_OUT); // Set it as an output
    stdio_init_all();
}
void process_command(string line) // this is where the commands that can be entered into the terminal are defined
{
    // break cmd into arguments
    string cmd;
    istringstream iss(line);
    iss >> cmd;

    if (cmd == "r") // rotate by revolutions and seconds
    {
        float revolutions;
        float seconds;
        string direction;
        bool up;

        iss >> seconds >> revolutions >> direction;

        if (direction == "cw" || direction == "CW")
        {
            up = false;
        }
        else if (direction == "ccw" || direction == "CCW")
        {
            up = true;
        }
        else
        {
            up = true;
        }
        rotate_motor(revolutions, up, seconds);
    }
    else if (cmd == "rt") // rotate for time
    {
        float extrusion_time;
        float seconds_per_rev;
        string direction;
        bool up;

        iss >> extrusion_time >> seconds_per_rev >> direction;

        if (direction == "cw" || direction == "CW")
        {
            up = false;
        }
        else if (direction == "ccw" || direction == "CCW")
        {
            up = true;
        }
        else
        {
            up = true;
        }

        rotate_motor_time(extrusion_time, up, seconds_per_rev);
    }
    else if (cmd == "ri")
    {
        float extrusion_time;
        float impedance;

        iss >> extrusion_time >> impedance;

        rotate_motor_impedance(impedance, extrusion_time);
    }
    else if (cmd == "start")
    {
        motor_running = true;
    }
    else if (cmd == "stop" || cmd == "s")
    {
        motor_running = false;
    }
    else if (cmd == "help")
    {
        cout << "stop or s                                                -- stops motor" << endl
             << "start                                                    -- resumes last command" << endl
             << "r (extrusion_time) (number of revolutions) (direction)   -- rotate           -- ex: r 3 3 cw" << endl
             << "rt (extrusion_time) (seconds_per_revolution) (direction) -- rotate_time      -- ex: rt 3 3 cw" << endl
             << "ri (extrusion_time) (impedance)                          -- rotate_impedance -- ex: ri 3 3 " << endl;
    }
    else
    {
        cout << "use 'help' command for help" << endl;
    }
}

string get_command()
{
    string line;
    getline(cin, line);
    return line;
}

int main()
{
    init_everything();

    multicore_launch_core1(continuous_stepper);

    while (1)
    {
        process_command(get_command());
    }
}
