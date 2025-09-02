#include <16F877.h>                 
#device ADC=16                     // set ADC resolution to 16 bits

#FUSES NOWDT                     
#FUSES NOBROWNOUT                
#FUSES NOLVP                     

#use delay(crystal=20000000)      

#include <lcd.c>                  

#define PIN_START_STOP PIN_A0
#define PIN_LAP        PIN_A1
#define PIN_UP         PIN_B1
#define PIN_DOWN       PIN_B0
#define PIN_RESET      PIN_C7

unsigned int8 seconds = 0, minutes = 0, centiseconds = 0; 

// counter used for generating stopwatch timing:
// clock frequency = 20 MHz / 4 = 5,000,000 cycles per second
// each instruction cycle = 1 / 5,000,000 = 0.2 µs
// timer0 prescaler = 256, so timer increments every 0.2 µs * 256 = 51.2 µs
// to get a 1 ms interrupt, timer must count approximately 1000 µs / 51.2 µs ˜ 19.5 counts
// since timer counts up from the preset value to 256, preload value = 256 - 19 = 237
unsigned int8 timer_tick_count = 0; 

unsigned int8 is_counting = 0; 

unsigned int8 lap_index = 0; 

unsigned int8 is_stopped = 0; 

unsigned int8 last_lap_minutes = 0, last_lap_seconds = 0, last_lap_centiseconds = 0; 

// arrays to store history of lap times, max 10 laps stored
unsigned int8 lap_minutes_history[10], lap_seconds_history[10], lap_centiseconds_history[10]; 

unsigned int8 total_laps = 0;  

int reset_button_flag = 0;  

#int_timer0                      
void timer_interrupt() {
    // the timer interrupt occurs every ~1 ms, so we increment 'timer_tick_count' each interrupt
    // when timer_tick_count reaches 10, that corresponds to 10 ms (1 centisecond), so we increment centiseconds
    // when centiseconds reaches 100 (1 second), increment seconds and reset centiseconds
    // when seconds reaches 60 (1 minute), increment minutes and reset seconds
    
    set_timer0(237);             // reload timer0 with 237 for next interrupt (to get 1 ms tick)
    timer_tick_count++;          // increment tick counter
    if(timer_tick_count == 10) { // 10 timer interrupts = 10 ms (1 centisecond)
        timer_tick_count = 0;    // reset tick counter
        centiseconds++;          // increment centiseconds
        if(centiseconds == 100) { // if 100 centiseconds (1 second) passed
            centiseconds = 0;    // reset centiseconds
            seconds++;           // increment seconds
            if(seconds == 60) {  // if 60 seconds (1 minute) passed
                seconds = 0;     // reset seconds
                minutes++;       // increment minutes
            }
        }
    }
}

void lcd_start() {
    lcd_init();                
    lcd_gotoxy(1,1);           
    printf(lcd_putc, "STOPWATCH"); 
    lcd_gotoxy(1,2);           
    printf(lcd_putc, "00:00:00");  
}

void stopwatch_start() {
    printf(lcd_putc, "\f");              
    is_counting = 1;                     
    setup_timer_0(RTCC_INTERNAL | RTCC_DIV_256);  
    set_timer0(237);                      
    enable_interrupts(int_timer0);     
    enable_interrupts(global);          
}

void stopwatch_pause() {
    is_counting = 0;                    
    is_stopped = 1;                    
    lcd_gotoxy(1,1);                    
    printf(lcd_putc, "\fPAUSED %02d:%02d:%02d", minutes, seconds, centiseconds);  
    disable_interrupts(int_timer0);     
    disable_interrupts(global);          
}

void stopwatch_resume() {
    is_counting = 1;                    
    is_stopped = 0;                     
    reset_button_flag = 0;               
    lcd_gotoxy(1,1);                     
    printf(lcd_putc, "\fCurr %02d:%02d:%02d", minutes, seconds, centiseconds); 
    setup_timer_0(RTCC_INTERNAL | RTCC_DIV_256); 
    set_timer0(237);                     
    enable_interrupts(int_timer0);      
    enable_interrupts(global);           
}

void mark_lap() {
    int diff_centiseconds = centiseconds - last_lap_centiseconds;  
    int diff_seconds = seconds - last_lap_seconds;  
    int diff_minutes = minutes - last_lap_minutes;  

    if(centiseconds < last_lap_centiseconds) {               
        diff_centiseconds += 100;             
        diff_seconds--;                       
    }
    if(seconds < last_lap_seconds) {               
        diff_seconds += 60;                   
        diff_minutes--;                     
    }

    // check if lap storage is full (max 10 laps)
    if (total_laps < 10) {          
        lap_index = total_laps;       
        total_laps++;              
    } else {                       
        lap_index = 0;             
        total_laps = 1;              
    }

    lap_centiseconds_history[lap_index] = diff_centiseconds;  
    lap_seconds_history[lap_index] = diff_seconds;  
    lap_minutes_history[lap_index] = diff_minutes;  

    last_lap_centiseconds = centiseconds;  
    last_lap_seconds = seconds; 
    last_lap_minutes = minutes;
}

void display_time() {
    lcd_gotoxy(1,1);                 
    printf(lcd_putc, "Curr %02d:%02d:%02d", minutes, seconds, centiseconds);  
}

void display_lap() {
    if(total_laps > 0 && lap_index < total_laps) {  
        lcd_gotoxy(1,2);                            
        printf(lcd_putc, "LAP%d %02d:%02d:%02d", lap_index+1, lap_minutes_history[lap_index], lap_seconds_history[lap_index], lap_centiseconds_history[lap_index]);  // show lap time
    }
}

void show_stopped_message() {
    lcd_gotoxy(1,1);                       
    printf(lcd_putc, "\fSTOPPED %02d:%02d:%02d", minutes, seconds, centiseconds);  
}

void stopwatch_reset() {
    printf(lcd_putc, "\fRESET");   
    delay_ms(2000);                
    seconds = 0, minutes = 0, centiseconds = 0;        
    timer_tick_count = 0, is_counting = 0, lap_index = 0, is_stopped = 0;  
    last_lap_minutes = 0, last_lap_seconds = 0, last_lap_centiseconds = 0;  
    total_laps = 0;                
    reset_button_flag = 0;         
    printf(lcd_putc, "\f");       
    lcd_start();             
}

// main program loop
void main() {
    lcd_start();                   
    set_tris_a(0x03);                  
    set_tris_b(0x03);                  
    set_tris_c(0x80);                  

    while(TRUE) {                     
        if(input(PIN_START_STOP)) {   
            delay_ms(50);             
            while(input(PIN_START_STOP)) {}  
            if(is_counting == 0 && is_stopped == 0) {   
                stopwatch_start();  
            } else if(is_counting == 1) {               
                stopwatch_pause();  
            } else if(is_stopped == 1) {                 
                stopwatch_resume();  
            }
        }

        if(input(PIN_LAP)) {         
            while(input(PIN_LAP)) {} 
            mark_lap();           
        }

        if(input(PIN_UP)) {          
            while(input(PIN_UP)) {} 
            if(lap_index < total_laps - 1) {  
                lap_index++;  
            }
        }

        if(input(PIN_DOWN)) {         
            while(input(PIN_DOWN)) {} 
            if(lap_index > 0) {      
                lap_index--;  
            }
        }

        if(input(PIN_RESET)) {       
            while(input(PIN_RESET)) {}          
            if(reset_button_flag == 0 && is_stopped == 1) {  
                show_stopped_message();       
                reset_button_flag = 1;         
            } else if (reset_button_flag == 1 && is_stopped == 1) {  
                stopwatch_reset();            
            }
        }

        if(is_counting == 1) {      
            display_time();   
        }
        display_lap();                
    }
}

