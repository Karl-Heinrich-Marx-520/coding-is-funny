#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct Node
{
	int date;
	Node* next;
}Node;

void insert_at_head(Node* head, int date)
{
	Node* new_node = (Node*)malloc(sizeof(Node));
	if (new_node == NULL)
	{
		printf("failure\n");
		return;
	}
	new_node->date = date;
	new_node->next = head->next;
	head->next = new_node;
}

void traverse_list(Node* head)
{
	if (head == NULL)
	{
		printf("failure\n");
		return;
	}
	Node* current = head->next;
	while (current != NULL)
	{
		printf("%d->", current->date);
		current = current->next;
	}
	printf("NULL");
}

void free_list(Node* head)
{
	Node* current = head;
	while (current != NULL)
	{
		Node* temp = current;
		current = temp->next;
		free(temp);
	}
}

void delete_node(Node* head, int target)
{
	Node* current = head;
	while (current->next != NULL && current->next->date != target)
	{
		current = current->next;
	}
	if (current->next != NULL || current->date == target)
	{
		Node* temp = current->next;
		current->next = temp->next;
		free(temp);
	}
	else
	{
		printf("failure\n");
	}
}

int main()
{
	Node* head = (Node*)malloc(sizeof(Node));
	if (head == NULL)
	{
		printf("failure\n");
		return 1;
	}
	head->date = 0;
	head->next = NULL;

	insert_at_head(head, 10);
	insert_at_head(head, 20);
	insert_at_head(head, 30);
	traverse_list(head);
	delete_node(head, 20);
	printf("\n");
	traverse_list(head);
	free_list(head);

}
