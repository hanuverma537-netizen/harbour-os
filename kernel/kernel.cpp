extern "C" void kernel_main() {
    volatile unsigned short* vga = (volatile unsigned short*) 0xB8000;

    const char* message = "Welcome to Harbor OS";

    for(int i=0; message[i] != '\0'; i++) {
        vga[i] = (0x0F << 8) | message[i];
    }

    while(true) {
        asm volatile("hlt");
    }
}