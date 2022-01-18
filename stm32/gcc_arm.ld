MEMORY
{
    FLASH(rx) : ORIGIN = 0x08000000, LENGTH = 0x100000
    RAM(xrw) : ORIGIN = 0x20010000, LENGTH = 0x40000
}

ENTRY(Reset_Handler)

_minimum_stack_size = 0x1000;

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
    } >FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text*)
        . = ALIGN(4);
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } >FLASH

    .init_array :
    {
	. = ALIGN(4);
        __init_array_start = .;
        KEEP (*(.init_array*))
    	. = ALIGN(4);
        __init_array_end = .;
    } >FLASH

    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT> FLASH

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = .;
        *(.bss*)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = .;
    } >RAM

    .stack :
    {
        . = ALIGN(4);
        . = . + _minimum_stack_size;
        . = ALIGN(4);
    } >RAM

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
