#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

typedef struct {
    char **sentences;
    size_t count;
} SentenceArray;

char *read_text() {
    size_t size = 2048, length = 0;
    char *text = malloc(size);
    if (!text) return NULL;

    int c;
    while ((c = getchar()) != EOF) {
        if (length + 1 >= size) {
            size *= 2;
            char *new_text = realloc(text, size);
            if (!new_text) {
                free(text);
                return NULL;
            }
            text = new_text;
        }
        text[length++] = c;
        if (length >= 2 && text[length - 1] == '\n' && text[length - 2] == '\n') {
            break;
        }
    }
    text[length] = '\0';
    return text;
}

SentenceArray parse_text(char *text) {
    SentenceArray array = {NULL, 0};
    char *sentence = strtok(text, ".");
    while (sentence) {
        while (isspace((unsigned char)*sentence)) sentence++;
        if (*sentence) {
            size_t len = strlen(sentence) + 2;
            char *sentence_with_dot = malloc(len);
            if (sentence_with_dot) {
                strcpy(sentence_with_dot, sentence);
                sentence_with_dot[len - 2] = '.';
                sentence_with_dot[len - 1] = '\0';

                array.sentences = realloc(array.sentences, sizeof(char *) * (array.count + 1));
                array.sentences[array.count++] = sentence_with_dot;
            }
        }
        sentence = strtok(NULL, ".");
    }
    return array;
}

void free_sentence_array(SentenceArray *array) {
    for (size_t i = 0; i < array->count; i++) {
        free(array->sentences[i]);
    }
    free(array->sentences);
}

void remove_duplicate_sentences(SentenceArray *array) {
    for (size_t i = 0; i < array->count; i++) {
        for (size_t j = i + 1; j < array->count; ) {
            if (strcasecmp(array->sentences[i], array->sentences[j]) == 0) {
                free(array->sentences[j]);
                for (size_t k = j; k < array->count - 1; k++) {
                    array->sentences[k] = array->sentences[k + 1];
                }
                array->count--;
            } else {
                j++;
            }
        }
    }
}

void function_list() {
    puts("Справка о функциях:");
    puts("0 - ");
    puts("1 - ");
    puts("2 - ");
    puts("3 - ");
    puts("4 - ");
    puts("5 - ");
}

int main() {
    setlocale(LC_ALL, "en_US.UTF-8");
    printf("Course work for option 4.19, created by Egor Omelyash.\n");

    int command;
    if (scanf("%d", &command) != 1) {
        fprintf(stderr, "Error: invalid input\n");
        return 1;
    }
    getchar();

    if (command == 5) {
        function_list();
        return 0;
    }

    char word[100];
    if (command == 1) {
        if (scanf("%99s", word) != 1) {
            fprintf(stderr, "Error: invalid word input\n");
            return 1;
        }
    }

    char *text = read_text();
    if (!text) {
        fprintf(stderr, "Error: unable to read text\n");
        return 1;
    }

    SentenceArray array = parse_text(text);
    free(text);
    if (array.count == 0) {
        fprintf(stderr, "Error: no sentences found\n");
        return 1;
    }

    remove_duplicate_sentences(&array);

    switch (command) {
        case 0:
            for (size_t i = 0; i < array.count; i++) {
                printf("%s\n", array.sentences[i]);
            }
            break;
        case 1:

            break;
        case 2:

            break;
        case 3:

            break;
        case 4:

            break;
        case 9:

            break;
        default:
            fprintf(stderr, "Error: unknown command\n");
            break;
    }

    free_sentence_array(&array);
    return 0;
}
