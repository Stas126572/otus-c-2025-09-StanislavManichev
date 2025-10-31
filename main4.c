#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef int64_t INT;

extern void abort(void);
extern int fflush(FILE* stream);
extern int printf(const char* str, ...);
extern int puts(const char* str);
extern void* malloc(size_t size);

const char* empty_str = "";
const char* int_format = "%ld ";
INT data[] = { 4, 8, 15, 16, 23, 42 };
static int data_length = sizeof(data) / sizeof(INT);

typedef struct ListNode {
    INT value;
    struct ListNode* next;
} ListNode;

void print_int(INT value)
{
    printf(int_format, value);
    fflush(NULL);
}

INT p(INT value)
{
    return value & 1;
}

ListNode* add_element(INT value, ListNode* next)
{
    ListNode* node = malloc(sizeof(ListNode));
    if (!node) abort();
    node->value = value;
    node->next = next;
    return node;
}

void map(ListNode* list, void(*func)(INT))
{
    while (list) {
	func(list->value);
    	list = list->next;
    }
}

ListNode* filter(ListNode* list, ListNode* acc, INT(*predicate)(INT))
{
    while (list)
    {
	if (predicate(list->value))
	{
		acc = add_element(list->value, acc);
	}
	list = list->next;
    }

    return acc;
}

int main()
{
    ListNode* list = NULL;

    for (int i = data_length; i > 0; i--) {
        list = add_element(data[i - 1], list);
    }

    ListNode* original_list = list;

    map(list, print_int);
    puts(empty_str);

    ListNode* filtered_list = filter(original_list, NULL, p);

    map(filtered_list, print_int);
    puts(empty_str);

    return 0;
}
