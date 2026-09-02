#include <stdint.h>

extern uint32_t __bss_link_start __asm("__bss_link_start");
extern uint32_t __bss_link_end __asm("__bss_link_end");

extern uint32_t __ctors_link_start __asm("__ctors_link_start");
extern uint32_t __ctors_link_end __asm("__ctors_link_end");

extern uint32_t __data_link_start __asm("__data_link_start");
extern uint32_t __data_link_end __asm("__data_link_end");
extern uint32_t __data_load_start __asm("__data_load_start");

typedef void(init_t)(void);

void crt_init(void)
{
	uint32_t * start;
	uint32_t * end;
	uint32_t * load;

	start = &__bss_link_start;
	end = &__bss_link_end;
	while (start < end) {
		*start++ = 0;
	}

	start = &__data_link_start;
	end = &__data_link_end;
	load = &__data_load_start;
	while (start < end) {
		*start++ = *load++;
	};

	start = &__ctors_link_start;
	end = &__ctors_link_end;
	while (start < end) {
		((init_t*)(*start++))();
	}
}
