#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define STAT_MAX      10U
#define LOW_WARNING    3U
#define NAME_LEN      12U

/* mood bits */
#define BIT_ASLEEP     0U
#define BIT_HUNGRY     1U
#define BIT_SAD        2U
#define BIT_SICK       3U

#define SET_BIT(reg, n)    ((reg) |=  (uint8_t)(1U << (n)))
#define CLR_BIT(reg, n)    ((reg) &= (uint8_t)~(1U << (n)))
#define READ_BIT(reg, n)   ((uint8_t)(((reg) >> (n)) & 1U))

typedef struct {
    char     name[NAME_LEN];
    uint8_t  food;     
    uint8_t  fun;      
    uint8_t  energy;   
    uint8_t  mood;      
    uint16_t hours;     
} Pet_t;

static Pet_t fluffy;

static void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

static int read_int(const char *prompt, int *val) {
    printf("%s", prompt);
    if (scanf("%d", val) != 1) {
        clear_input_buffer();
        return 0;
    }
    clear_input_buffer();
    return 1;
}

static void drawStat(const char *label, uint8_t value) {
    printf("%-8s [", label);
    for (uint8_t i = 0; i < STAT_MAX; ++i) {
        putchar(i < value ? '#' : ' ');
    }
    printf("] %u/%u\n", (unsigned int)value, (unsigned int)STAT_MAX);
}

static void updateMood(void) {
    if (fluffy.food <= LOW_WARNING) {
        SET_BIT(fluffy.mood, BIT_HUNGRY);
    } else {
        CLR_BIT(fluffy.mood, BIT_HUNGRY);
    }

    if (fluffy.fun <= LOW_WARNING) {
        SET_BIT(fluffy.mood, BIT_SAD);
    } else {
        CLR_BIT(fluffy.mood, BIT_SAD);
    }

    if (fluffy.energy == 0U) {
        SET_BIT(fluffy.mood, BIT_ASLEEP);
    }

    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        CLR_BIT(fluffy.mood, BIT_SICK);
    }
}

static void hatchPet(void) {
    printf("Enter a name for your pet: ");
    if (fgets(fluffy.name, NAME_LEN, stdin) != NULL) {
        size_t len = strlen(fluffy.name);
        if (len > 0 && fluffy.name[len - 1] == '\n') {
            fluffy.name[len - 1] = '\0';
        }
    }
    if (strlen(fluffy.name) == 0) {
        strncpy(fluffy.name, "Fluffy", NAME_LEN);
    }
    fluffy.food = STAT_MAX;
    fluffy.fun = STAT_MAX;
    fluffy.energy = STAT_MAX;
    CLR_BIT(fluffy.mood, BIT_ASLEEP);
    CLR_BIT(fluffy.mood, BIT_HUNGRY);
    CLR_BIT(fluffy.mood, BIT_SAD);
    CLR_BIT(fluffy.mood, BIT_SICK);
    fluffy.hours = 0U;
    printf("\n%s has hatched!\n", fluffy.name);
}

static void drawFace(void) {
    printf("\n  +-----------+\n");
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        printf("  |   (- -)   |  zzZ\n");
        printf("  |    . .    |\n");
    } else if (READ_BIT(fluffy.mood, BIT_SICK)) {
        printf("  |   (x x)   |  sick...\n");
        printf("  |    ~~~    |\n");
    } else if (READ_BIT(fluffy.mood, BIT_SAD) || READ_BIT(fluffy.mood, BIT_HUNGRY)) {
        printf("  |   (. .)   |  sad/hungry\n");
        printf("  |    (n)    |\n");
    } else {
        printf("  |   (^ ^)   |  happy!\n");
        printf("  |    \\_/    |\n");
    }
    printf("  +-----------+\n");
}

static void feed(void) {
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        printf("%s is asleep! Wake %s up first.\n", fluffy.name, fluffy.name);
        return;
    }
    if (fluffy.food >= STAT_MAX) {
        SET_BIT(fluffy.mood, BIT_SICK);
        printf("You overfed %s! %s is now sick!\n", fluffy.name, fluffy.name);
    } else {
        fluffy.food = (fluffy.food + 3U > STAT_MAX) ? STAT_MAX : (fluffy.food + 3U);
        printf("You fed %s.\n", fluffy.name);
    }
    updateMood();
}

static void play(void) {
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        printf("%s is asleep and cannot play!\n", fluffy.name);
        return;
    }
    fluffy.fun = (fluffy.fun + 3U > STAT_MAX) ? STAT_MAX : (fluffy.fun + 3U);
    if (fluffy.energy >= 2U) {
        fluffy.energy -= 2U;
    } else {
        fluffy.energy = 0U;
    }
    printf("You played with %s.\n", fluffy.name);
    updateMood();
}

static void sleepPet(void) {
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        CLR_BIT(fluffy.mood, BIT_ASLEEP);
        printf("%s woke up!\n", fluffy.name);
    } else {
        SET_BIT(fluffy.mood, BIT_ASLEEP);
        printf("%s went to sleep.\n", fluffy.name);
    }
    updateMood();
}

static void hourPasses(void) {
    fluffy.hours++;
    if (fluffy.food > 0U) {
        fluffy.food--;
    }
    if (fluffy.fun > 0U) {
        fluffy.fun--;
    }

    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        fluffy.energy = (fluffy.energy + 2U > STAT_MAX) ? STAT_MAX : (fluffy.energy + 2U);
    } else {
        if (fluffy.energy > 0U) {
            fluffy.energy--;
        }
    }
    updateMood();
}

static uint8_t isHappy(void) {
    if (fluffy.food > LOW_WARNING &&
        fluffy.fun > LOW_WARNING &&
        fluffy.energy > LOW_WARNING &&
        fluffy.mood == 0U) {
        return 1U;
    }
    return 0U;
}

static void petReport(void) {
    printf("\n=== %s'S CARE CARD ===\n", fluffy.name);
    printf("Hours Alive: %u\n", (unsigned int)fluffy.hours);
    drawStat("Food", fluffy.food);
    drawStat("Fun", fluffy.fun);
    drawStat("Energy", fluffy.energy);

    printf("Mood Byte (Bin): ");
    for (int i = 7; i >= 0; i--) {
        putchar(READ_BIT(fluffy.mood, (uint8_t)i) ? '1' : '0');
    }
    putchar('\n');

    printf("Status: ");
    if (READ_BIT(fluffy.mood, BIT_SICK)) {
        printf("%s is sick and needs rest!\n", fluffy.name);
    } else if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        printf("%s is sleeping peacefully.\n", fluffy.name);
    } else if (READ_BIT(fluffy.mood, BIT_HUNGRY)) {
        printf("%s needs food!\n", fluffy.name);
    } else if (READ_BIT(fluffy.mood, BIT_SAD)) {
        printf("%s needs to play!\n", fluffy.name);
    } else if (isHappy()) {
        printf("%s is super happy and content!\n", fluffy.name);
    } else {
        printf("%s is doing okay.\n", fluffy.name);
    }
}

static void passMultipleHours(void) {
    int count = 0;
    if (!read_int("Enter hours to pass (1-50): ", &count) || count <= 0 || count > 50) {
        printf("Invalid hours.\n");
        return;
    }
    for (int i = 0; i < count; ++i) {
        hourPasses();
    }
    printf("%d hour(s) passed in %s's world.\n", count, fluffy.name);
}

static void print_menu(void) {
    printf("\n--- PET ROBOT MENU ---\n");
    printf("1. Feed Fluffy\n");
    printf("2. Play with Fluffy\n");
    printf("3. Toggle Sleep/Wake\n");
    printf("4. Pass 1 Hour\n");
    printf("5. Pass N Hours\n");
    printf("6. Draw Face\n");
    printf("7. Pet Care Report\n");
    printf("8. Re-hatch Pet\n");
    printf("9. Exit\n");
}

static int handle_menu(void) {
    print_menu();
    int choice = 0;
    if (!read_int("Select choice: ", &choice)) {
        printf("Invalid choice.\n");
        return 1;
    }

    switch (choice) {
        case 1: feed(); drawFace(); break;
        case 2: play(); drawFace(); break;
        case 3: sleepPet(); drawFace(); break;
        case 4: hourPasses(); drawFace(); break;
        case 5: passMultipleHours(); drawFace(); break;
        case 6: drawFace(); break;
        case 7: petReport(); break;
        case 8: hatchPet(); drawFace(); break;
        case 9: return 0;
        default: printf("Invalid choice.\n"); break;
    }
    return 1;
}

static void run_app(void) {
    hatchPet();
    drawFace();
    int running = 1;
    do {
        running = handle_menu();
    } while (running);
}

int main(void) {
    run_app();
    return 0;
}