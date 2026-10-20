
const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 25;
const unsigned char WHITE_ON_BLACK = 0x0F;

volatile unsigned short* const VGA_MEMORY =
    (volatile unsigned short*)0xB8000;

int cursor_pos = 0;

void clear_screen();
void print(const char* message);

extern "C" void kernel_main() {
    clear_screen();

    print("Welcome to HarbourOS!");
    print("\nA tiny OS built from scratch.");
    print("\nConsole initialized successfully.");
}

void clear_screen() {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        VGA_MEMORY[i] = (WHITE_ON_BLACK << 8) | ' ';
    }

    cursor_pos = 0;
}

void print(const char* message) {
    for (int i = 0; message[i] != '\0'; i++) {
        if (message[i] == '\n') {
            cursor_pos =
                ((cursor_pos / SCREEN_WIDTH) + 1) * SCREEN_WIDTH;
        } else {
            VGA_MEMORY[cursor_pos] =
                (WHITE_ON_BLACK << 8) | message[i];

            cursor_pos++;
        }

        if (cursor_pos >= SCREEN_WIDTH * SCREEN_HEIGHT) {
            return;
        }
    }
}
