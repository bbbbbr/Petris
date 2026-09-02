#pragma once

void serial_begin(int baud);
void serial_write(char c);
void serial_print(char *s);
int serial_available();
char serial_read();

void serial_isrRxI0();
void serial_isrERI0();
