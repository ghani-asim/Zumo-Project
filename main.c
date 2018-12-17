/**
* @mainpage ZumoBot Project
* @brief    You can make your own ZumoBot with various sensors.
* @details  <br><br>
    <p>
    <B>General</B><br>
    You will use Pololu Zumo Shields for your robot project with CY8CKIT-059(PSoC 5LP) from Cypress semiconductor.This
    library has basic methods of various sensors and communications so that you can make what you want with them. <br>
    <br><br>
    </p>

    <p>
    <B>Sensors</B><br>
    &nbsp;Included: <br>
        &nbsp;&nbsp;&nbsp;&nbsp;LSM303D: Accelerometer & Magnetometer<br>
        &nbsp;&nbsp;&nbsp;&nbsp;L3GD20H: Gyroscope<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Reflectance sensor<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Motors
    &nbsp;Wii nunchuck<br>
    &nbsp;TSOP-2236: IR Receiver<br>
    &nbsp;HC-SR04: Ultrasonic sensor<br>
    &nbsp;APDS-9301: Ambient light sensor<br>
    &nbsp;IR LED <br><br><br>
    </p>

    <p>
    <B>Communication</B><br>
    I2C, UART, Serial<br>
    </p>
*/

#include <project.h>
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "LSM303D.h"
#include "IR.h"
#include "Beep.h"
#include "mqtt_sender.h"
#include <time.h>
#include <sys/time.h>
#include "serial1.h"
#include <unistd.h>
/**
 * @file    main.c
 * @brief
 * @details  ** Enable global interrupt since Zumo library uses interrupts. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/

#if 0
/* Maze */
void zmain(void)
{
    
    
    struct sensors_ dig;
    TickType_t start, finish;
    IR_Start();
    Ultra_Start();                        
    uint8_t stop = 0;
    int x = 0, y = -1, dir = 0, line = 0, i = 0;  
     
    reflectance_start();
    motor_start();
        
        // When button pressed, robot will move upto the 1st line and wait for IR
        while(SW1_Read() == 1);
        vTaskDelay(1000);
        do{
            
            motor_forward(100, 0);
            reflectance_digital(&dig);
            
            if(dig.l3 == 1 && dig.r3 == 1){
                motor_forward(0, 0);
                stop = 1;
            }
        }while(!stop);
        
        BatteryLed_Write(0);
        print_mqtt("Zumo034","Waiting for IR");
        IR_wait();
        start = xTaskGetTickCount();
       
        print_mqtt("Zumo034/start","%d", start);
        BatteryLed_Write(1);
        motor_forward(100, 100);
        
        do {
            reflectance_digital(&dig);
            int d = Ultra_GetDistance();
           
        if((dig.l1 == 1 && dig.r1 == 1))        //line follow loop
        {
            motor_forward(100,1);
        } else if(dig.l1 == 1 && dig.r1 == 0)
        {
            motor_turn(0,65,1);
        } else if(dig.l1 == 0 && dig.r1 == 1)
        {
            motor_turn(65,0,1);
        } 
         
        if(dig.l3 == 1 && dig.r3 == 1)      
        {
            i = 1;
            d = Ultra_GetDistance();
            
        if(line == 2 && dir == 1)      
        {
            motor_turn(0,165,550);
            line = 0;
            dir--;
        } else if(line == 2 && dir == -1)      
        {
            motor_turn(165,0,575);
            line = 0;
            dir++;
        } 
        if(d<22 && dir == 1)        //sees object facing right
        {
            motor_turn(0,165,550);
            line = 0;
            dir--;
        } else if(d<22 && dir == -1)        //sees object facing left
        {
            motor_turn(165,0,575);
            line = 0;
            dir++;
        } else if (d<22 && dir == 0)        //sees object facing forward
            {
                if(x == 3 || x == 2 || x == 1)      //and is on the right side of grid
                {
                    motor_turn(0,165,550);
                    line = 0;
                    dir--;
                } else if(x == -3 || x == -2 || x == -1 || x == 0)  //and on left side of grid or centre
                {
                    motor_turn(165,0,575);
                    line = 0;
                    dir++;
                }
                 
                
            } 
        } else if(dig.l3 == 0 && dig.r3 == 0 && i == 1)         //counts lines and x and y dir
        {
            line++;
            if(dir == 1)
            {
                x++;
            } else if(dir == -1)
            {
                x--;
            } else if(dir == 0)
            {
                y++;
            }
            i = 0;
        }
        if(dig.l1 == 0 && dig.r1 == 0 && dir == 1)      //reaches right end side of grid
        {
            motor_turn(0,165,550);
            dir++;
        }else if(dig.l1 == 0 && dig.r1 == 0 && dir == -1)       //reaches left end side of grid
        {
            motor_turn(165,0,575);
            dir++;
        }else if(dig.l3==0 && dig.l2==0 && dig.l1==0 && dig.r1==0 && dig.r2==0 && dig.r3==0){ 
            motor_forward(0,0);
            break;
        }
             
        } while(y<14);
        
        finish = xTaskGetTickCount();
        print_mqtt("Zumo034/stop","%d", finish);
    
        int z = finish - start;
        print_mqtt("Zumo034/time","%d", z);
        motor_stop(0,0);
       
    
    }
    
#endif

#if 0
/* Zumo Wrestling */
void zmain(void)
{
    CyGlobalIntEnable;
        struct sensors_ dig;
        TickType_t x, y;
        int z = 0;
        Ultra_Start();
        IR_Start();
        bool sent = false;
        
        uint8_t stop = 0;
        reflectance_start();
        reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
        
        motor_start();
        
        // When button pressed, robot will move upto the 1st line and wait for IR
        while(SW1_Read() == 1);
        vTaskDelay(1000);
        do{
            
            motor_forward(100, 0);
            reflectance_digital(&dig);
            
            if(dig.l3 == 1 && dig.r3 == 1){
                motor_forward(0, 0);
                stop = 1;
            }
        }while(!stop);
        
        BatteryLed_Write(0);
        print_mqtt("Zumo034","Waiting for IR");
        IR_wait();
        x = xTaskGetTickCount();
       
        print_mqtt("Zumo034/start","%d", x);
        BatteryLed_Write(1);
        motor_forward(200, 100);
        
        // This loop is for simply if any of the sensor detects black line 
        for(;;){
            reflectance_digital(&dig);
            
            int d = Ultra_GetDistance();
        
            if(dig.l3==1 || dig.l2==1 || dig.l1==1 || dig.r1==1 || dig.r2==1 || dig.r3==1){
                motor_backward(100,0);
                motor_turn(0, 200, 700);
            }
            else{
                motor_forward(150, 0);
            }
            
            // detects and send hits
            if(d < 8 && sent == false){
                TickType_t h = xTaskGetTickCount();
                print_mqtt("Zumo034/hit","%d", h);
                sent = true;
            }else {
                motor_forward(150, 0);
            }
            
            if(d > 15) {sent = false;}
            
            if(SW1_Read() == 0){
                motor_forward(0, 0);
                break;
            }
            
    
        }
        
        y = xTaskGetTickCount();
        print_mqtt("Zumo034/stop","%d", y);
    
        z = y - x;
        print_mqtt("Zumo034/time","%d", z);
}
#endif

#if 0
/* Line following */
void zmain(void)
{
    CyGlobalIntEnable;
        struct sensors_ dig;
        //uint8_t count = 0;
        
        IR_Start();
        TickType_t x, y;
        int z = 0;
        uint8_t stop = 0;
        reflectance_start();
        reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
        
        motor_start();
        
        // When button pressed, robot will move upto the 1st line and wait for IR
        while(SW1_Read() == 1);
        vTaskDelay(1000);
        do{
            
            motor_forward(100, 0);
            reflectance_digital(&dig);
            
            if(dig.l3 == 1 && dig.r3 == 1){
                motor_forward(0, 0);
                stop = 1;
            }
        }while(!stop);
        
        BatteryLed_Write(0);
        print_mqtt("Zumo034","Waiting for IR");
        IR_wait();
        x = xTaskGetTickCount();
       
        print_mqtt("Zumo034/start","%d", x);
        BatteryLed_Write(1);
        motor_forward(150, 100);
        
    for(;;)
    {
       
        reflectance_digital(&dig); 
        
        if(dig.l1==1&& dig.r1==1 && dig.l3==0 && dig.l2==0 && dig.r2==0&& dig.r3==0){
            motor_forward(150, 0);
        }
        else if(dig.l1==0&& dig.r1==1 && dig.l3==0 && dig.l2==0 && dig.r2==1&& dig.r3==0){
            motor_turn(150,30,0);
        }
        else if(dig.l1==0&& dig.r1==0 && dig.l3==0 && dig.l2==0 && dig.r2==1&& dig.r3==1){
        motor_turn(188,23,0);
        }
         else if(dig.l1==0&& dig.r1==0 && dig.l3==0 && dig.l2==0 && dig.r2==0&& dig.r3==1){
        motor_turn(170,0,0);
        }
        else if(dig.l1==1&& dig.r1==0 && dig.l3==0 && dig.l2==1 && dig.r2==0&& dig.r3==0){
            motor_turn(30,150,0);
        }
        else if(dig.l1==0&& dig.r1==0 && dig.l3==1 && dig.l2==1 && dig.r2==0&& dig.r3==0){
            motor_turn(20,160,0);
        }
        else if(dig.l1==0&& dig.r1==0 && dig.l3==1 && dig.l2==0 && dig.r2==0&& dig.r3==0){
            motor_turn(0,190,0);
        }
        
        else if(dig.l3==1 && dig.r3==1){
            break;
        }
       
    }
    
        motor_forward(150, 200);
        
    // This loop will make the robot stop on the 2nd line
    for(;;)
    {
       
        reflectance_digital(&dig);
        
         if(dig.l3==1 && dig.r3==1){
            motor_forward(0, 0);
            break;
            printf("Error6\n");
        }else {
            motor_forward(150,0);
        }
    }
        y = xTaskGetTickCount();
        print_mqtt("Zumo034/stop","%d", y);
    
        z = y - x;
        print_mqtt("Zumo034/time","%d", z);
}   
#endif

// [] END OF FILE */
