
void clear_screen();
void print(const char* message);


extern "C" void kernel_main() {
    clear_screen();

    print("Welcome to HarbourOS!");
}

volatile unsigned short* const VGA_MEMORY =
    (volatile unsigned short*)0xB8000;

const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 25;
const unsigned char WHITE_ON_BLACK = 0x0F;

void clear_screen() {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        VGA_MEMORY[i] = (WHITE_ON_BLACK << 8) | ' ';
    }
}

void print(const char* message) {
    for (int i = 0; message[i] != '\0'; i++) {
        VGA_MEMORY[i] = (WHITE_ON_BLACK << 8) | message[i];
    }
}
