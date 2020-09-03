// This file requires at least C99 to compile

/**
 * @file   todolist.c
 * @author Jean-Cédric Chappelier <jean-cedric.chappelier@epfl.ch>
 *
 * @copyright EPFL 2020
**/
/**
 * @section DESCRIPTION
 *
 * Template du second homework du cours CS-207, année 2020.
**/

#include <stdio.h>
#include <stdlib.h>

/* ====================================================================== *
 * Structures de données                                                  *
 * ====================================================================== */

/* Définissez ici les types et macros demandés :
 *    task_t,
 *    NO_TASK,
 *    task_print,
 *    queue_node_t,
 *    queue_t,
 *    priority_t,
 *    priority_print,
 * et priority_queue_t.
 */

/**
 * @brief Types required for normal queues
 * 
 */
typedef const void task_t;
typedef void (*task_print)(const task_t *task);

typedef struct queue_node_t
{
    const task_t *head;
    struct queue_node_t *next;
} queue_node_t;

typedef struct
{
    queue_node_t *first;
    queue_node_t *last;
} queue_t;

/**
 * @brief Types required for priority queues
 * 
 */
typedef size_t priority_t;
typedef struct
{
    priority_t max;
    queue_t *queues;
} priority_queue_t;

/**
 * @brief Macros
 * 
 */
#define priority_print(p) (printf("%zu", p))
#define NO_TASK NULL
#ifndef SIZE_MAX
#define SIZE_MAX (~(size_t)0)
#endif

/* ====================================================================== *
 * Implémentation de queue_t                                              *
 * ====================================================================== */

/**
 * @brief initializes queue q
 * 
 * @param q queue to initialize
 * @return initialized queue
 */
queue_t *queue_init(queue_t *q)
{
    if (q != NULL)
    {
        q->first = NULL;
        q->last = NULL;
    }
    return q;
}

// ----------------------------------------------------------------------
/**
 * @brief checks if the queue is empty
 * 
 * @param q queue to check
 * @return 1 if queue is empty and 0 otherwise
 */
int queue_is_empty(const queue_t *q)
{
    if (q == NULL || q->first == NULL)
    {
        return 1;
    }
    return 0;
}

// ----------------------------------------------------------------------
/**
 * @brief removes oldest task in the queue and returns it
 * 
 * @param q queue to pop the oldest task from
 * @return oldest task in queue q
 */
const task_t *queue_pop(queue_t *q)
{
    if (!queue_is_empty(q))
    {
        const task_t *result = q->first->head;
        queue_node_t *old_node = q->first;
        if (q->first->next != NULL)
        {
            q->first = q->first->next;
        }
        else
        {
            q->first = NULL;
            q->last = NULL;
        }
        free(old_node);
        return result;
    }
    return NO_TASK;
}

// ----------------------------------------------------------------------
/**
 * @brief adds a task into a given queue
 * 
 * @param q queue where we will push the new task
 * @param value task to add to the queue
 * @return queue with given task added
 */
queue_t *queue_push(queue_t *q, const task_t *value)
{
    if (q != NULL)
    {
        queue_node_t *new_node = malloc(sizeof(queue_node_t));
        if (new_node == NULL)
        {
            return q;
        }
        new_node->head = value;
        new_node->next = NULL;
        if (q->first == NULL)
        {
            q->first = new_node;
        }
        else
        {
            q->last->next = new_node;
        }
        q->last = new_node;
    }
    return q;
}

// ----------------------------------------------------------------------
/**
 * @brief shows elements of the queue
 * 
 * @param q queue to print
 * @param print function to print tasks
 */
void queue_print(const queue_t *q, task_print print)
{
    if (!queue_is_empty(q))
    {
        const queue_node_t *node = q->first;
        print(node->head);
        while (node->next != NULL)
        {
            node = node->next;
            printf(", ");
            print(node->head);
        }
    }
    else
    {
        printf("<empty queue>");
    }
}

// ----------------------------------------------------------------------
/**
 * @brief clears a given queue
 * 
 * @param q queue to clear
 */
void queue_clear(queue_t *q)
{
    if (q != NULL)
    {
        while (q->first != NULL)
        {
            queue_node_t *node = q->first;
            q->first = q->first->next;
            free(node);
        }
    }
}

/* ====================================================================== *
 * Implémentation de priority_queue_t                                     *
 * ====================================================================== */

/**
 * @brief initializes a priority queue
 * 
 * @param pq priority queue to initialize
 * @return priority queue initialized
 */
priority_queue_t *pri_queue_init(priority_queue_t *pq)
{
    if (pq != NULL)
    {
        pq->max = 0;
        pq->queues = malloc(sizeof(queue_t));
        if (pq->queues == NULL)
        {
            return NULL;
        }
        queue_init(pq->queues);
    }
    return pq;
}

// ----------------------------------------------------------------------
/**
 * @brief removes oldest task in the queue with highest priority and returns it
 * 
 * @param pq priority queue to pop the oldest task from
 * @return oldest task of the queue with highest priority in pq
 */
const task_t *pri_queue_pop(priority_queue_t *pq)
{
    if (pq == NULL || pq->queues == NULL)
    {
        return NO_TASK;
    }
    const task_t *result = queue_pop(&pq->queues[pq->max]);
    int maxChanged = 0;
    while (pq->max > 0 && queue_is_empty(&pq->queues[pq->max]))
    {
        pq->max--;
        maxChanged = 1;
    }
    if (maxChanged && ((pq->max + 1) * sizeof(queue_t) > SIZE_MAX ||
                       (pq->queues = realloc(pq->queues, (pq->max + 1) * sizeof(queue_t))) == NULL))
    {
        return NO_TASK;
    }
    return result;
}

// ----------------------------------------------------------------------
/**
 * @brief adds a task with given priority in a priority queue
 * 
 * @param pq priority queue where to add the task
 * @param task task to add
 * @param priority priority of the task to add
 * @return priority queue with new task added
 */
priority_queue_t *pri_queue_push(priority_queue_t *pq, const task_t *task,
                                 const priority_t priority)
{
    if (pq != NULL && pq->queues != NULL)
    {
        if (priority <= pq->max)
        {
            queue_push(&pq->queues[priority], task);
        }
        else
        {
            if ((priority + 1) > SIZE_MAX / sizeof(queue_t) ||
                (pq->queues = realloc(pq->queues, (priority + 1) * sizeof(queue_t))) == NULL)
            {
                return NULL;
            }
            for (priority_t i = pq->max + 1; i < priority + 1; i++)
            {
                queue_init(&pq->queues[i]);
            }
            queue_push(&pq->queues[priority], task);
            pq->max = priority;
        }
    }
    return pq;
}

// ----------------------------------------------------------------------
/**
 * @brief checks if a priority queue is empty
 * 
 * @param pq priority queue to check
 * @return 1 if priority queue is empty and 0 otherwise
 */
int pri_queue_is_empty(const priority_queue_t *pq)
{
    if (pq == NULL || pq->queues == NULL)
    {
        return 1;
    }
    for (priority_t i = 0; i < pq->max + 1; i++)
    {
        if (!queue_is_empty(&pq->queues[i]))
        {
            return 0;
        }
    }
    return 1;
}

// ----------------------------------------------------------------------
// provided
static void print_subqueue(const priority_t p, const queue_t *q, task_print print_f)
{
    priority_print(p);
    fputs(" : ", stdout);
    queue_print(q, print_f);
    putchar('\n');
}

// ----------------------------------------------------------------------
/**
 * @brief shows elements of a priority queue with decreasing priority
 * 
 * @param pq priority queue to print
 * @param print_func function to print tasks
 */
void pri_queue_print(const priority_queue_t *pq, task_print print_func)
{
    if (pq != NULL && pq->queues != NULL)
    {
        for (priority_t i = 0; i < pq->max + 1; i++)
        {
            print_subqueue(pq->max - i, &pq->queues[pq->max - i], print_func);
        }
    }
}

// ----------------------------------------------------------------------
/**
 * @brief clears a priority queue
 * 
 * @param pq priority queue to clear
 */
void pri_queue_clear(priority_queue_t *pq)
{
    if (pq != NULL && pq->queues != NULL)
    {
        pq->queues = realloc(pq->queues, sizeof(queue_t));
        if (pq->queues == NULL)
        {
            return;
        }
        pq->max = 0;
        queue_clear(pq->queues);
    }
}

// ----------------------------------------------------------------------
/**
 * @brief deletes a priority queue
 * 
 * @param pq priority queue to delete
 */
void pri_queue_delete(priority_queue_t *pq)
{
    if (pq != NULL && pq->queues != NULL)
    {
        pri_queue_clear(pq);
        free(pq->queues);
        pq->queues = NULL;
    }
}

/*  ====================================================================== *
 *  ====================================================================== *
 *  Code fourni                                                            *
 *  ====================================================================== *
 *  ====================================================================== */

#define array_size(T) (sizeof(T) / sizeof((T)[0]))
#define min2(x, y) ((x) > (y) ? (y) : (x))
#define push_task(i)    \
    if (nb_tasks > (i)) \
    queue_push(&q, (const char *)tasks + (i)*task_size)

void test(const task_t *tasks, size_t nb_tasks, size_t task_size, task_print print)
{
    /* -------------------------------------------------- *
     * Tests queue_t                                      *
     * -------------------------------------------------- */
    puts("======\nQueue example:");

    queue_t q;
    queue_t *check = queue_init(&q);
    if (check != &q)
        puts("Houlàlà !");
    push_task(0);
    push_task(1);
    push_task(2);
    queue_print(&q, print);
    putchar('\n');
    while (!queue_is_empty(&q))
    {
        print(queue_pop(&q));
        puts(" <-- pop");
    }
    push_task(0);
    push_task(1);
    queue_clear(&q);
    puts("---");
    queue_print(&q, print);

    /* -------------------------------------------------- *
     * Tests priority_queue_t                             *
     * -------------------------------------------------- */

    puts("\n-----\nPriority queue example:");

    priority_queue_t pq;
    pri_queue_init(&pq);

    const priority_t priors[] = {
        5, 4, 2, 7, 1, 2, 5, 4, 3, 2, 6, 1, 4, 3};

    const size_t end = min2(nb_tasks, array_size(priors));
    for (size_t i = 0; i < end; ++i)
    {
        pri_queue_push(&pq, (const char *)tasks + i * task_size, priors[i]);
    }

    pri_queue_print(&pq, print);

    while (!pri_queue_is_empty(&pq))
    {
        print(pri_queue_pop(&pq));
        printf(" <-- pop (max p = ");
        priority_print(pq.max);
        puts(")");
    }

    if (nb_tasks > 1)
    {
        pri_queue_push(&pq, tasks, 3);
        pri_queue_push(&pq, (const char *)tasks + task_size, 1);
        pri_queue_push(&pq, (const char *)tasks + task_size, 3);
        puts("---");
        pri_queue_print(&pq, print);
        pri_queue_clear(&pq);
        puts("---");
        pri_queue_print(&pq, print);
    }

    if (nb_tasks > 1)
    {
        pri_queue_push(&pq, tasks, 5);
        pri_queue_push(&pq, (const char *)tasks + task_size, 0);
        pri_queue_push(&pq, (const char *)tasks + task_size, 3);
        puts("---");
        pri_queue_print(&pq, print);
    }
    pri_queue_delete(&pq);
    puts("---");
    pri_queue_print(&pq, print);
}

// ----------------------------------------------------------------------
void print_string(const char **p_s)
{
    if (p_s != NULL)
    {
        fputs(*p_s, stdout);
    }
    else
    {
        fputs("<NULL>", stdout);
    }
}

// ----------------------------------------------------------------------
void print_int(const int *p_i)
{
    if (p_i != NULL)
    {
        printf("%d", *p_i);
    }
    else
    {
        fputs("<NULL>", stdout);
    }
}

#define TEST(T, Func) \
    test(T, array_size(T), sizeof((T)[0]), (task_print)Func)

// ----------------------------------------------------------------------
int main()
{
    const char *const s_tasks[] = {
        "pain", "confiture", "lessive", "beurre", "savon", "sel",
        "noix", "miel"};
    TEST(s_tasks, print_string);

    const int i_tasks[] = {
        111, 112, 113, 114, 115, 116, 117, 118};
    TEST(i_tasks, print_int);

    return 0;
}
