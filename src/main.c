#include "stc8g.h"

#include "gpio.h"

#include "settings.h"

#define SECOND_COUNTER 100
#define SECOND_TIMER 65536 - (F_CPU / 12 / SECOND_COUNTER)

unsigned int seconds = 0;
unsigned long milliseconds = 0;

void toggleSRCLK()
{
    P5 |= (1 << 4);
    P5 &= ~(1 << 4);
}

void writeSER(unsigned char data)
{
    if (data)
    {
        P5 |= (1 << 5);
    }
    else
    {
        P5 &= ~(1 << 5);
    }
}

void toggleRCLK()
{
    P3 |= (1 << 2);
    P3 &= ~(1 << 2);
}

void writeByte(unsigned char data)
{
    for (int i = 0; i < 8; i++)
    {
        if (data & (1 << (7 - i)))
        {
            writeSER(1);
        }
        else
        {
            writeSER(0);
        }
        toggleSRCLK();
    }

    toggleRCLK();
}

void setup()
{
    // Port 3 pin 2 as output
    P3M0 |= (1 << 2);
    P3M1 = 0x00;

    // Port 3 pin 3 as high-Z input
    P3M0 &= ~(1 << 3);
    P3M1 |= (1 << 3);

    // Port 5 pin 4, 5 as output
    P5M0 |= (1 << 5) | (1 << 4);
    P5M1 = 0x00;

    P3 = 0;
    P5 = 0;

    IT1 = 1; // Enable INT1 falling edge interrupt
    EX1 = 1; // Enable INT1 interrupt

    TMOD = 0; // timer 0 in mode 0

    TL0 = SECOND_TIMER & 0xFF;
    TH0 = (SECOND_TIMER >> 8) & 0xFF;

    TR0 = 1; // timer 0 enable
    ET0 = 1; // timer 0 interrupt
    EA = 1;  // global interrupt
}

void wait(unsigned int duration)
{
    unsigned int start = seconds;
    while (seconds - start < duration)
        ;
}

void waitMs(unsigned long duration)
{
    unsigned long start = milliseconds;
    while (milliseconds - start < duration)
        ;
}

void loop()
{
    unsigned char i = 1;
    do
    {
        unsigned char v = i;
        unsigned int start = seconds;
        while (seconds - start < TOOTH_DURATION)
        {
            writeByte(v);
            waitMs(500);
            v ^= i;
        }

        i <<= 1;
    } while (i < 0b01000000);
}

void powerDown()
{
    writeByte(0x00);
    seconds = 0;
    milliseconds = 0;
    PCON |= PD;
}

int main()
{
    setup();

    while (1)
    {
        loop();
        powerDown();
    }

    return 0;
}

unsigned char counter = 0;

void TM0_Isr() __interrupt(TIMER0_INTERRUPT)
{
    milliseconds += 10;
    counter++;

    if (counter == SECOND_COUNTER)
    {
        seconds++;
        counter = 0;
    }
}

void INT1_Isr() __interrupt(EXTINT1_INTERRUPT)
{
    // powerDown();
}