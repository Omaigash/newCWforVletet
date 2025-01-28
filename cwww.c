#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

typedef struct {
    char **sentences;
    size_t count;
} SentenceArray;

// Функция для чтения текста из стандартного ввода
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

// Функция для разбиения текста на предложения
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

// Функция для освобождения памяти, выделенной под массив предложений
void free_sentence_array(SentenceArray *array) {
    for (size_t i = 0; i < array->count; i++) {
        free(array->sentences[i]);
    }
    free(array->sentences);
}

// Функция для удаления дубликатов предложений
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

// Функция для вывода справки по функциям
void function_list() {
    puts("Справка о функциях:");
    puts("0 - Вывести все предложения");
    puts("1 - В каждом предложении найти индексы первого и последнего вхождения символа ‘$’ и удалить все символы до первого индекса и после последнего индекса. Если символ ‘$’ единственный, то необходимо удалить этот символ");
    puts("2 - Отсортировать все предложения по уменьшению количества слов в предложении");
    puts("3 - ");
    puts("4 - ");
    puts("5 - Вывести справку");
    puts("8 - Дублировать каждое предложение N раз");
}

// Функция для дублирования предложений N раз
void duplicate_sentences(SentenceArray *array, int n) {
    for (size_t i = 0; i < array->count; i++) {
        char *original_sentence = array->sentences[i];
        size_t len = strlen(original_sentence);

        //Удалим точку в конце предложения
        if (original_sentence[len - 1] == '.') {
            original_sentence[len - 1] = '\0';
            len--;
        }

        // Создадим новую строку для дублированного предложения
        char *duplicated_sentence = malloc(len * n + n + 1); // +n для пробелов и +1 для \0

        if (!duplicated_sentence) {
            fprintf(stderr, "Memory allocation error\n");
            exit(1);
        }

        char *p = duplicated_sentence;
        for (int j = 0; j < n; j++) {
            strcpy(p, original_sentence);
            p += len;
            if (j < n - 1) {
                *p = ' ';
                p++;
            }
        }
        *p = '.';
        p++;
        *p = '\0';

        free(array->sentences[i]);
        array->sentences[i] = duplicated_sentence;
    }
}

// Функция для обработки предложений (удаление символов до первого и после последнего вхождения '$')
void process_sentences(SentenceArray *array) {
    for (size_t i = 0; i < array->count; i++) {
        char *sentence = array->sentences[i];
        char *first = strchr(sentence, '$');
        char *last = strrchr(sentence, '$');

        if (first == NULL) continue;

        if (first == last) {

            char *temp = malloc(strlen(sentence));
            if (!temp) {
                fprintf(stderr, "Memory allocation error\n");
                exit(1);
            }
            strncpy(temp, sentence, first - sentence);
            temp[first - sentence] = '\0';
            strcat(temp, first + 1);
            free(array->sentences[i]);
            array->sentences[i] = temp;

        } else {

            char *temp = malloc(last - first + 2);
            if (!temp) {
                fprintf(stderr, "Memory allocation error\n");
                exit(1);
            }
            strncpy(temp, first, last - first + 1);
            temp[last - first + 1] = '\0';
            if (temp[strlen(temp)-1] != '.') {

                char *temp2 = malloc(strlen(temp) + 2);
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

// Функция для подсчета количества слов в предложении
int count_words(const char *sentence) {
    int count = 0;
    char *copy = strdup(sentence);
    char *word = strtok(copy, " ");
    while (word) {
        count++;
        word = strtok(NULL, " ");
    }
    free(copy);
    return count;
}

// Функция сравнения для qsort (сортировка по убыванию количества слов)
int compare_sentences(const void *a, const void *b) {
    const char *sentence_a = *(const char **)a;
    const char *sentence_b = *(const char **)b;
    return count_words(sentence_b) - count_words(sentence_a);
}

// Функция для сортировки предложений по убыванию количества слов
void sort_sentences_by_word_count(SentenceArray *array) {
    qsort(array->sentences, array->count, sizeof(char *), compare_sentences);
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

    int n = 0;
    if (command == 8) {

        char temp;
        if (scanf("%d%c", &n, &temp) != 2 || temp != '\n') {
            fprintf(stderr, "Error: invalid N input\n");
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
            process_sentences(&array);
            for (size_t i = 0; i < array.count; i++) {
                printf("%s ", array.sentences[i]);
            }
            break;
        case 2:
            sort_sentences_by_word_count(&array);
            for (size_t i = 0; i < array.count; i++) {
                printf("%s\n", array.sentences[i]);
            }
            break;
        case 3:

            break;
        case 4:

            break;
        case 8:
            duplicate_sentences(&array, n);
            for (size_t i = 0; i < array.count; i++) {
                printf("%s\n", array.sentences[i]);
            }
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
