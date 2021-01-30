/*
 * queue.h
 *
 *  Created on: 30 gen 2021
 *      Author: gigi-g
 *
 *	Description:
 *		A queue is defined in this file that will be used to manage reservations.
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdio.h>

#ifndef NULL
#define NULL ((void *) 0)
#endif

struct node {
	uint8_t data;
	struct node *next;
};
typedef struct node node;

struct queue {
	int count;
	node *head;
	node *tail;
};
typedef struct queue queue;

void initialize(queue *q);

uint8_t isempty(queue *q);

uint8_t search(node *head, uint8_t data);

uint8_t enqueue(queue *q, uint8_t value);

uint8_t dequeue(queue *q);

#endif /* QUEUE_H_ */
