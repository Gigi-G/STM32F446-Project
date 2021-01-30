/*
 * queue.c
 *
 *  Created on: 30 gen 2021
 *      Author: gigi-g
 */

#include "queue.h"
#include <stdlib.h>

/**
 * Initialize a queue.
 *
 * @param	q is a pointer to a struct queue
 */
void initialize(queue *q) {
	q->count = 0;
	q->head = NULL;
	q->tail = NULL;
}

/**
 * Check if the queue is empty.
 *
 * @param	q is a pointer to a struct queue
 * @return	a value that is 1 if the queue is empty, otherwise it is 0
 */
uint8_t isempty(queue *q) {
	return (q->count == 0);
}

/**
 * Check if a value was already put in the queue.
 *
 * @param	head is a pointer to a struct node and it is the first element of the queue
 * @param	data is the value to search inside the queue
 * @return	1 if the data is found, 0 otherwise
 */
uint8_t search(node *head, uint8_t data) {
	if(head == NULL) return 0;
	if(head->data == data) return 1;
	return search(head->next, data);
}

/**
 * An element will be insert inside the queue, if it hasn't already exist.
 *
 * @param	q is a pointer to a struct queue
 * @param	value is the value to insert
 * @return	1 if enqueue is done, 0 otherwise.
 */
uint8_t enqueue(queue *q, uint8_t value) {
	uint8_t find = search(q->head, value);
	if(find) return 0;
	node *tmp;
	tmp = malloc(sizeof(node));
	tmp->data = value;
	tmp->next = NULL;
	if(!isempty(q)) {
		q->tail->next = tmp;
		q->tail = tmp;
	}
	else {
		q->head = q->tail = tmp;
	}
	q->count++;
	return 1;
}

/**
 * The first element of the queue is extracted.
 *
 * @param	q is a pointer to a struct queue
 * @return	n is the value of the first element of the queue
 */
uint8_t dequeue(queue *q){
	node *tmp;
	uint8_t n = q->head->data;
	tmp = q->head;
	q->head = q->head->next;
	q->count--;
	free(tmp);
	return n;
}

uint8_t readTail(queue *q) {
	return q->tail->data;
}
