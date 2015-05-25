/* useful reference: www.linuxselfhelp.com/gnu/ld/html_chapter/ld_toc.html */
/* Written by Frank Duignan */

HEAP_SIZE   	    = 4096;
THREAD_STACK_SIZE   = 4096; 
MAIN_STACK_SIZE     = 124;

MEMORY
{
    flash : org = 0x10001000, len = 64k
    ram : org = 0x20000000, len = 16k
}
  
SECTIONS
{

  . = ORIGIN(flash);
  .text : {		  
    *(.vectors); /* The interrupt vectors */
    *(.text);
    *(.rodata);
  } >flash

  . = ORIGIN(ram);
  .data : {	  
    __data_flash = LOADADDR(.data);
    __data_ram = .;
    *(.remapped_vectors);
    *(.data);
    __data_ram_end = .;
  } >ram AT>flash
  __data_size = SIZEOF(.data);

  __bss_start = .;
  __bss_start__ = .;
  .bss : {
    *(.bss);
  } > ram
  _end = .;
  _bss_end__ = . ; __bss_end__ = . ; __end__ = . ;
  PROVIDE (end = .);
  __bss_size = SIZEOF(.bss);
  
  __heap = .;
  .heap : {
    . = ALIGN (4);
    . += HEAP_SIZE;
  } > ram
  __heap_end = .;
  __heap_size = SIZEOF(.heap);


  __stack = .;
  __stack_start = .;
  .stack : {
    . = ALIGN (4);

    . += THREAD_STACK_SIZE;
    . = ALIGN (4);
    THREAD_STACK_TOP = . ;

    . += MAIN_STACK_SIZE;
    . = ALIGN (4);
    MAIN_STACK_TOP = . ;
  } > ram
  __stack_end = .;
  __stack_size = SIZEOF(.stack);

}
