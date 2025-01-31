#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

typedef struct {
    char** sentences;
    size_t count;
} SentenceArray;

char* my_strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* copy = malloc(len);
    if (copy) {
        memcpy(copy, s, len);
    }
    return copy;
}

int my_strcasecmp(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (diff != 0) return diff;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

char* read_text() {
    size_t size = 2048, length = 0;
    char* text = malloc(size);
    if (!text) return NULL;
    int c;
    while ((c = getchar()) != EOF) {
        if (length + 1 >= size) {
            size *= 2;
            char* new_text = realloc(text, size);
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

SentenceArray parse_text(char* text) {
    SentenceArray array = { NULL, 0 };
    char* sentence = strtok(text, ".");
    while (sentence) {
        while (isspace((unsigned char)*sentence)) sentence++;
        if (*sentence) {
            size_t len = strlen(sentence) + 2;
            char* sentence_with_dot = malloc(len);
            if (sentence_with_dot) {
                strcpy(sentence_with_dot, sentence);
                sentence_with_dot[len - 2] = '.';
                sentence_with_dot[len - 1] = '\0';
                array.sentences = realloc(array.sentences, sizeof(char*) * (array.count + 1));
                array.sentences[array.count++] = sentence_with_dot;
            }
        }
        sentence = strtok(NULL, ".");
    }
    return array;
}

void free_sentence_array(SentenceArray* array) {
    for (size_t i = 0; i < array->count; i++) {
        free(array->sentences[i]);
    }
    free(array->sentences);
}

void remove_duplicate_sentences(SentenceArray* array) {
    for (size_t i = 0; i < array->count; i++) {
        for (size_t j = i + 1; j < array->count; ) {
            if (my_strcasecmp(array->sentences[i], array->sentences[j]) == 0) {
                free(array->sentences[j]);
                for (size_t k = j; k < array->count - 1; k++) {
                    array->sentences[k] = array->sentences[k + 1];
                }
                array->count--;
            }
            else {
                j++;
            }
        }
    }
}

void function_list() {
    puts("��������� �������:\n");
    puts("0 - ������� ������������ �����.\n");
    puts("1 - ����� � ������� ����� ������, ���������� �� ������� '$'.\n");
    puts("2 - ������������� ����������� �� �������� ���������� ����.\n");
    puts("3 - ��������� � ���������� ������� ������� �������.\n");
    puts("4 - ������� ����������� � ������� ���������.\n");
    puts("5 - ����� ������� (��� ���������).\n");
    puts("9 - ������������� ����������� �� ����������� ���������� ����.\n");
}

void process_sentences(SentenceArray* array) {
    for (size_t i = 0; i < array->count; i++) {
        char* sentence = array->sentences[i];
        char* first = strchr(sentence, '$');
        char* last = strrchr(sentence, '$');
        if (first == NULL) continue;
        if (first == last) {
            char* temp = malloc(strlen(sentence));
            if (!temp) {
                fprintf(stderr, "Memory allocation error\n");
                exit(1);
            }
            strncpy(temp, sentence, first - sentence);
            temp[first - sentence] = '\0';
            strcat(temp, first + 1);
            free(array->sentences[i]);
            array->sentences[i] = temp;
        }
        else {
            char* temp = malloc(last - first + 2);
            if (!temp) {
                fprintf(stderr, "Memory allocation error\n");
                exit(1);
            }
            strncpy(temp, first, last - first + 1);
            temp[last - first + 1] = '\0';
            if (temp[strlen(temp) - 1] != '.') {
                char* temp2 = malloc(strlen(temp) + 2);
                if (!temp2) {
                    fprintf(stderr, "Memory allocation error\n");
                    exit(1);
                }
                strcpy(temp2, temp);
                temp2[strlen(temp)] = '.';
                temp2[strlen(temp) + 1] = '\0';
                free(temp);
                temp = temp2;
            }
            free(array->sentences[i]);
            array->sentences[i] = temp;
        }
    }
}

int count_words(const char* sentence) {
    int count = 0;
    char* copy = my_strdup(sentence);
    char* word = strtok(copy, " ");
    while (word) {
        count++;
        word = strtok(NULL, " ");
    }
    free(copy);
    return count;
}

int compare_sentences(const void* a, const void* b) {
    const char* sentence_a = *(const char**)a;
    const char* sentence_b = *(const char**)b;
    return count_words(sentence_b) - count_words(sentence_a);
}

void sort_sentences_by_word_count(SentenceArray* array) {
    qsort(array->sentences, array->count, sizeof(char*), compare_sentences);
}

void count_and_print_chars(const SentenceArray* array) {
    char chars[256] = { 0 };
    int counts[256] = { 0 };
    for (size_t i = 0; i < array->count; i++) {
        const char* sentence = array->sentences[i];
        for (size_t j = 0; sentence[j] != '\0'; j++) {
            unsigned char c = (unsigned char)sentence[j];
            if (!isspace(c)) {
                if (chars[c] == 0) {
                    chars[c] = 1;
                }
                counts[c]++;
            }
        }
    }
    for (size_t i = 0; i < array->count; i++) {
        const char* sentence = array->sentences[i];
        for (size_t j = 0; sentence[j] != '\0'; j++) {
            unsigned char c = (unsigned char)sentence[j];
            if (!isspace(c)) {
                if (chars[c] == 1) {
                    printf("'%c': %d\n", c, counts[c]);
                    chars[c] = 2;
                }
            }
        }
    }
}

void remove_even_index_sentences(SentenceArray* array) {
    SentenceArray new_array = { NULL, 0 };
    for (size_t i = 0; i < array->count; i++) {
        if (i % 2 == 0) {
            new_array.sentences = realloc(new_array.sentences, sizeof(char*) * (new_array.count + 1));
            if (new_array.sentences == NULL) {
                fprintf(stderr, "Memory allocation error\n");
                exit(1);
            }
            new_array.sentences[new_array.count] = my_strdup(array->sentences[i]);
            if (new_array.sentences[new_array.count] == NULL) {
                fprintf(stderr, "Memory allocation error\n");
                exit(1);
            }
            new_array.count++;
        }
    }
    free_sentence_array(array);
    array->sentences = new_array.sentences;
    array->count = new_array.count;
}

int compare_sentences_ascending(const void* a, const void* b) {
    const char* sentence_a = *(const char**)a;
    const char* sentence_b = *(const char**)b;

    int words_a = count_words(sentence_a);
    int words_b = count_words(sentence_b);

    if (words_a != words_b) {
        return words_a - words_b;
    }

    return strlen(sentence_a) - strlen(sentence_b);
}

void sort_sentences_ascending(SentenceArray* array) {
    qsort(array->sentences, array->count, sizeof(char*), compare_sentences_ascending);
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
    char* text = read_text();
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
        process_sentences(&array);
        for (size_t i = 0; i < array.count; i++) {
            printf("%s\n", array.sentences[i]);
        }
        break;
    case 2:
        sort_sentences_by_word_count(&array);
        for (size_t i = 0; i < array.count; i++) {
            printf("%s\n", array.sentences[i]);
        }
        break;
    case 3:
        count_and_print_chars(&array);
        break;
    case 4:
        remove_even_index_sentences(&array);
        for (size_t i = 0; i < array.count; i++) {
            printf("%s\n", array.sentences[i]);
        }
        break;
    case 9:
        sort_sentences_ascending(&array);
        for (size_t i = 0; i < array.count; i++) {
            printf("%s\n", array.sentences[i]);
        }
        break;
    default:
        fprintf(stderr, "Error: unknown command\n");
        break;
    }
    free_sentence_array(&array);
    return 0;
}