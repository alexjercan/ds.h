#include <signal.h>
#define DS_LL_IMPLEMENTATION
#define DS_SS_IMPLEMENTATION
#include "../ds.h"
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define LINE_MAX 1024
#define PORT 8080

struct task {
        int id;
        int kind;
};

struct status {
        int position;
};

struct mpsc {
        ds_linked_list queue;
        pthread_mutex_t lock;
};

void mpsc_init(struct mpsc *c, unsigned int item_size) {
    ds_linked_list_init(&c->queue, item_size);
    pthread_mutex_init(&c->lock, NULL);
}

void mpsc_enqueue(struct mpsc *c, struct task *item) {
    pthread_mutex_lock(&c->lock);
    ds_linked_list_push_back(&c->queue, item);
    pthread_mutex_unlock(&c->lock);
}

void mpsc_dequeue(struct mpsc *c, struct task *item) {
    pthread_mutex_lock(&c->lock);
    ds_linked_list_pop_front(&c->queue, item);
    pthread_mutex_unlock(&c->lock);
}

void mpsc_status(struct mpsc *c, int id, struct status *s) {
    pthread_mutex_lock(&c->lock);

    int position = 0;
    struct ds_linked_list_node *n = c->queue.head;
    while (n) {
        if (((struct task *)n->item)->id == id) {
            s->position = position;
            break;
        }
        position++;
        n = n->next;
    }

    pthread_mutex_unlock(&c->lock);
}

int mpsc_is_empty(struct mpsc *c) {
    int result = 0;
    pthread_mutex_lock(&c->lock);
    result = ds_linked_list_empty(&c->queue);
    pthread_mutex_unlock(&c->lock);
    return result;
}

void mpsc_free(struct mpsc *c) {
    ds_linked_list_free(&c->queue);
    pthread_mutex_destroy(&c->lock);
}

int keep_running = 1;
int task_id_iota = 0;

void handle_shutdown(int sig) {
    DS_LOG_INFO("Shutting down");
    keep_running = 0;
}

void *worker_thread(void *args) {
    struct mpsc *c = (struct mpsc *)args;
    struct task t;

    while (keep_running) {
        if (mpsc_is_empty(c)) {
            sleep(1);
            continue;
        }

        mpsc_dequeue(c, &t);

        DS_LOG_INFO("Processing task %d", t.id);

        sleep(5);
    }

    return NULL;
}

int mk_task_id() { return task_id_iota++; }

int handle_connection(int s_fd, struct mpsc *c) {
    int result = 0;
    char buff[LINE_MAX];
    char *tmp = NULL;
    ds_string_slice s, tok;

    int c_fd = accept(s_fd, (struct sockaddr *)NULL, NULL);
    if (c_fd < 0) {
        DS_LOG_ERROR("Unable to accept connection: %s", strerror(errno));
        return_defer(1);
    }

    memset(buff, 0, LINE_MAX);
    read(c_fd, buff, LINE_MAX);
    ds_string_slice_init(&s, buff, strlen(buff));
    ds_string_slice_tokenize(&s, ' ', &tok);

    ds_string_slice_to_owned(&tok, &tmp);
    if (strcmp(tmp, "q") == 0) {
        int task_id = mk_task_id();

        ds_string_slice_to_owned(&s, &tmp);
        int task_kind = atoi(tmp);

        struct task t = {task_id, task_kind};

        mpsc_enqueue(c, &t);

        DS_LOG_INFO("Enqueued task %d", task_id);

        memset(buff, 0, LINE_MAX);
        snprintf(buff, LINE_MAX, "%d", task_id);
        write(c_fd, buff, strlen(buff));
    } else if (strcmp(tmp, "s") == 0) {
        ds_string_slice_to_owned(&s, &tmp);
        int task_id = atoi(tmp);

        struct status s = {0};
        mpsc_status(c, task_id, &s);

        memset(buff, 0, LINE_MAX);
        snprintf(buff, LINE_MAX, "%d", s.position);
        write(c_fd, buff, strlen(buff));
    }

defer:
    if (c_fd >= 0) {
        close(c_fd);
    }
    return result;
}

int main() {
    int s_fd = -1;
    struct sockaddr_in s_addr;
    struct mpsc c;
    pthread_t worker;

    int result = 0;

    signal(SIGINT, handle_shutdown);
    signal(SIGTERM, handle_shutdown);

    mpsc_init(&c, sizeof(struct task));

    pthread_create(&worker, NULL, worker_thread, &c);

    s_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s_fd < 0) {
        DS_LOG_ERROR("Unable to create socket: %s", strerror(errno));
        return_defer(1);
    }

    memset(&s_addr, 0, sizeof(struct sockaddr_in));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(PORT);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(s_fd, (struct sockaddr *)&s_addr, sizeof(struct sockaddr_in)) <
        0) {
        DS_LOG_ERROR("Unable to bind socket: %s", strerror(errno));
        return_defer(1);
    }

    if (listen(s_fd, 10) < 0) {
        DS_LOG_ERROR("Unable to listen on socket: %s", strerror(errno));
        return_defer(1);
    }

    DS_LOG_INFO("Listening on port %d", PORT);
    while (keep_running) {
        handle_connection(s_fd, &c);
    }

defer:
    pthread_join(worker, NULL);

    if (s_fd >= 0) {
        shutdown(s_fd, SHUT_RDWR);
        close(s_fd);
    }

    mpsc_free(&c);

    return result;
}
