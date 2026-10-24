
const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 25;
const unsigned char WHITE_ON_BLACK = 0x0F;

volatile unsigned short* const VGA_MEMORY =
    (volatile unsigned short*)0xB8000;

int cursor_pos = 0;

void clear_screen();
void print(const char* message);
void scroll_screen();

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
        if (inb(0x64) & 1) {
            unsigned char scancode = inb(0x60);

            // Ignore key releases.
            if (scancode & 0x80) {
                continue;
            }

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
                case 0x0E: return '\b';
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

    print("\n\nHarbourOS shell");
    print("\nType help to see available commands.");
    print("\n> ");

    char command[64];
    int length = 0;
    command[0] = '\0';

    while (true) {
        char key = read_key();

        if (key == '\b') {
            if (length > 0) {
                length--;
                command[length] = '\0';

                if (cursor_pos > 0) {
                    cursor_pos--;
                    VGA_MEMORY[cursor_pos] =
                        (WHITE_ON_BLACK << 8) | ' ';
                }
            }
        } else if (key == '\n') {
            print("\n");

            if (length == 0) {
                // Ignore empty commands.
            } else if (
                command[0] == 'h' &&
                command[1] == 'e' &&
                command[2] == 'l' &&
                command[3] == 'p' &&
                command[4] == '\0'
            ) {
                print("Commands: help, about, version, clear");
            } else if (
                command[0] == 'a' &&
                command[1] == 'b' &&
                command[2] == 'o' &&
                command[3] == 'u' &&
                command[4] == 't' &&
                command[5] == '\0'
            ) {
                print("HarbourOS: a tiny operating system built from scratch.");
            } else if (
                command[0] == 'v' &&
                command[1] == 'e' &&
                command[2] == 'r' &&
                command[3] == 's' &&
                command[4] == 'i' &&
                command[5] == 'o' &&
                command[6] == 'n' &&
                command[7] == '\0'
            ) {
                print("HarbourOS v0.1");
                print("\n32-bit x86 kernel");
            } else if (
                command[0] == 'c' &&
                command[1] == 'l' &&
                command[2] == 'e' &&
                command[3] == 'a' &&
                command[4] == 'r' &&
                command[5] == '\0'
            ) {
                clear_screen();
                print("HarbourOS shell");
                print("\nType help to see available commands.");
            } else {
                print("Unknown command.");
            }

            print("\n> ");

            length = 0;
            command[0] = '\0';
        } else if (length < 63) {
            char text[2] = {key, '\0'};
            print(text);

            command[length] = key;
            length++;
            command[length] = '\0';
        }
    }
}

void clear_screen() {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        VGA_MEMORY[i] = (WHITE_ON_BLACK << 8) | ' ';
    }

    cursor_pos = 0;
}

void scroll_screen() {
    // Move every row up by one.
    for (int i = 0; i < SCREEN_WIDTH * (SCREEN_HEIGHT - 1); i++) {
        VGA_MEMORY[i] = VGA_MEMORY[i + SCREEN_WIDTH];
    }

    // Clear the bottom row.
    for (int i = SCREEN_WIDTH * (SCREEN_HEIGHT - 1);
         i < SCREEN_WIDTH * SCREEN_HEIGHT;
         i++) {
        VGA_MEMORY[i] = (WHITE_ON_BLACK << 8) | ' ';
    }

    // Move the cursor up by one row.
    cursor_pos -= SCREEN_WIDTH;
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
            scroll_screen();
        }
    }
}