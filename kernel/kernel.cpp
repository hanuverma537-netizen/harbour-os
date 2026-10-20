
const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 25;
const unsigned char WHITE_ON_BLACK = 0x0F;

volatile unsigned short* const VGA_MEMORY =
    (volatile unsigned short*)0xB8000;

int cursor_pos = 0;

void clear_screen();
void print(const char* message);


unsigned char inb(unsigned short port) {
    unsigned char result;

    asm volatile (
        "inb %1, %0"
        : "=a"(result)
        : "Nd"(port)
    );

    return result;
}

char read_key() {
    while (true) {
        // Wait until the keyboard has data
        if (inb(0x64) & 1) {
            unsigned char scancode = inb(0x60);

            // Ignore key releases
            if (scancode & 0x80) {
                continue;
            }

            // Convert common keyboard scancodes to characters
            switch (scancode) {
                case 0x1E: return 'a';
                case 0x30: return 'b';
                case 0x2E: return 'c';
                case 0x20: return 'd';
                case 0x12: return 'e';
                case 0x21: return 'f';
                case 0x22: return 'g';
                case 0x23: return 'h';
                case 0x17: return 'i';
                case 0x24: return 'j';
                case 0x25: return 'k';
                case 0x26: return 'l';
                case 0x32: return 'm';
                case 0x31: return 'n';
                case 0x18: return 'o';
                case 0x19: return 'p';
                case 0x10: return 'q';
                case 0x13: return 'r';
                case 0x1F: return 's';
                case 0x14: return 't';
                case 0x16: return 'u';
                case 0x2F: return 'v';
                case 0x11: return 'w';
                case 0x2D: return 'x';
                case 0x15: return 'y';
                case 0x2C: return 'z';
                case 0x39: return ' ';
                case 0x1C: return '\n';
                default:   continue;
            }
        }
    }
}


extern "C" void kernel_main() {
    clear_screen();

    print("Welcome to HarbourOS!");
    print("\nA tiny OS built from scratch.");
    print("\nConsole initialized successfully.");

    while (true) {
        char key = read_key();

        if (key == '\n') {
            print("\n");
        } else {
            char text[2] = {key, '\0'};
            print(text);
        }
    }
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
