#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define DS_PQ_IMPLEMENTATION
#include "../ds.h"

#define LINE_MAX 1024

struct lines {
    char **items;
    int count;
    int capacity;
};

struct world {
    int width;
    int height;
    unsigned char *map;
};

int world_init(struct world *w, int width, int height) {
    w->width = width;
    w->height = height;
    w->map = (unsigned char *)malloc(width * height * sizeof(unsigned char));
    if (w->map == NULL) {
        DS_LOG_ERROR("Failed to allocate memory for world map");
        return 1;
    }
    return 0;
}

int world_set(struct world *w, int x, int y, unsigned char value) {
    if (x < 0 || x >= w->width || y < 0 || y >= w->height) {
        DS_LOG_ERROR("Invalid coordinates");
        return 1;
    }
    w->map[y * w->width + x] = value;
    return 0;
}

int world_get(struct world *w, int x, int y, unsigned char *value) {
    if (x < 0 || x >= w->width || y < 0 || y >= w->height) {
        DS_LOG_ERROR("Invalid coordinates");
        return 1;
    }

    *value = w->map[y * w->width + x];
    return 0;
}

int world_from_file(struct world *w, const char *filename) {
    FILE *file = NULL;
    if (filename == NULL) {
        file = stdin;
    } else {
        file = fopen(filename, "r");
        if (file == NULL) {
            DS_LOG_ERROR("Failed to open file %s", filename);
            return 1;
        }
    }

    struct lines lines = {0};
    char line[LINE_MAX];
    int width = 0;
    int height = 0;
    while (fgets(line, LINE_MAX * sizeof(char), file) != NULL) {
        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        len = len - 1;

        if (width == 0) {
            width = len;
        } else if (len != width) {
            DS_LOG_ERROR("Invalid map width");
            return 1;
        }
        height++;

        char *line_copy = (char *)malloc((len + 1) * sizeof(char));
        if (line_copy == NULL) {
            DS_LOG_ERROR("Failed to allocate memory for line");
            return 1;
        }

        strcpy(line_copy, line);
        ds_da_append(&lines, line_copy);
    }

    if (filename != NULL) {
        fclose(file);
    }

    if (world_init(w, width, height) != 0) {
        return 1;
    }

    for (int y = 0; y < height; y++) {
        char *line = lines.items[y];
        for (int x = 0; x < width; x++) {
            char c = line[x];
            if (c == '.') {
                world_set(w, x, y, 0);
            } else if (c == '#') {
                world_set(w, x, y, 1);
            } else {
                DS_LOG_ERROR("Invalid character in map: %d", c);
                return 1;
            }
        }
    }

    for (int i = 0; i < lines.count; i++) {
        free(lines.items[i]);
    }
    free(lines.items);

    return 0;
}

int world_print(struct world *w) {
    for (int y = 0; y < w->height; y++) {
        for (int x = 0; x < w->width; x++) {
            printf("%c", w->map[y * w->width + x] == 0 ? '.' : '#');
        }
        printf("\n");
    }
    return 0;
}

int world_free(struct world *w) {
    free(w->map);
    return 0;
}

struct position {
    int x;
    int y;
};

int manhattan_distance(struct position p1, struct position p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

int position_hash(struct world *w, struct position p) {
    return p.y * w->width + p.x;
}

int position_equals(struct position p1, struct position p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

struct neighbors {
    struct position *items;
    int count;
    int capacity;
};

int neighbors_get(struct world *w, struct position *p, struct neighbors *n) {
    if (p->x > 0 && w->map[p->y * w->width + p->x - 1] == 0) {
        struct position left = {p->x - 1, p->y};
        ds_da_append(n, left);
    }

    if (p->x < w->width - 1 && w->map[p->y * w->width + p->x + 1] == 0) {
        struct position right = {p->x + 1, p->y};
        ds_da_append(n, right);
    }

    if (p->y > 0 && w->map[(p->y - 1) * w->width + p->x] == 0) {
        struct position up = {p->x, p->y - 1};
        ds_da_append(n, up);
    }

    if (p->y < w->height - 1 && w->map[(p->y + 1) * w->width + p->x] == 0) {
        struct position down = {p->x, p->y + 1};
        ds_da_append(n, down);
    }

    return 0;
}

struct position_node {
    struct position p;
    int f;
};

int simple_node_compare_min(const void *a, const void *b) {
    return ((struct position_node *)b)->f - ((struct position_node *)a)->f;
}

struct path {
    struct position *items;
    int count;
    int capacity;
};

int reconstruct_path(struct world *w, int *came_from, struct position current, struct path *p) {
    ds_da_append(p, current);
    int current_index = position_hash(w, current);

    while (came_from[current_index] != -1) {
        current_index = came_from[current_index];
        struct position current = {current_index % w->width, current_index / w->width};
        ds_da_append(p, current);
    }

    return 0;
}

int a_star(struct world *w, struct position start, struct position end, struct path *p) {
    struct ds_priority_queue open_set;
    struct position_node *start_node = (struct position_node *)malloc(sizeof(struct position_node));
    start_node->p = start;
    start_node->f = manhattan_distance(start, end);
    ds_priority_queue_init(&open_set, simple_node_compare_min);
    ds_priority_queue_insert(&open_set, start_node);

    int num_nodes = w->width * w->height;

    int *came_from = (int *)malloc(num_nodes * sizeof(int));
    for (int i = 0; i < num_nodes; i++) {
        came_from[i] = -1;
    }

    int *g_score = (int *)malloc(num_nodes * sizeof(int));
    for (int i = 0; i < num_nodes; i++) {
        g_score[i] = INT_MAX;
    }
    g_score[position_hash(w, start)] = 0;

    int *f_score = (int *)malloc(num_nodes * sizeof(int));
    for (int i = 0; i < num_nodes; i++) {
        f_score[i] = INT_MAX;
    }
    f_score[position_hash(w, start)] = manhattan_distance(start, end);

    int result = 0;
    while (ds_priority_queue_empty(&open_set) == 0) {
        struct position_node *current_node = NULL;
        ds_priority_queue_pull(&open_set, (void **)&current_node);

        if (position_equals(current_node->p, end)) {
            result = 1;
            reconstruct_path(w, came_from, current_node->p, p);
            free(current_node);
            break;
        }

        struct neighbors n = {0};
        neighbors_get(w, &current_node->p, &n);
        int current_index = position_hash(w, current_node->p);
        for (int i = 0; i < n.count; i++) {
            int neighbor_index = position_hash(w, n.items[i]);
            int tentative_g_score = g_score[current_index] + 1;
            if (tentative_g_score < g_score[neighbor_index]) {
                came_from[neighbor_index] = current_index;
                g_score[neighbor_index] = tentative_g_score;
                f_score[neighbor_index] = tentative_g_score + manhattan_distance(n.items[i], end);

                int found = 0;
                for (unsigned int j = 0; j < open_set.count; j++) {
                    struct position_node *node = (struct position_node *)open_set.items[j];
                    if (position_equals(node->p, n.items[i])) {
                        found = 1;
                        break;
                    }
                }

                if (found == 0) {
                    struct position_node *neighbor_node = (struct position_node *)malloc(sizeof(struct position_node));
                    neighbor_node->p = n.items[i];
                    neighbor_node->f = f_score[neighbor_index];
                    ds_priority_queue_insert(&open_set, neighbor_node);
                }
            }
        }

        free(n.items);
        free(current_node);
    }

    while (ds_priority_queue_empty(&open_set) == 0) {
        struct position_node *node = NULL;
        ds_priority_queue_pull(&open_set, (void **)&node);
        free(node);
    }
    ds_priority_queue_free(&open_set);
    free(came_from);
    free(g_score);
    free(f_score);

    return result;
}

int main() {
    struct world w;
    if (world_from_file(&w, "world.txt") != 0) {
        return 1;
    }

    world_print(&w);

    struct path p = {0};
    struct position start = {0, 0};
    struct position end = {w.width - 1, w.height - 1};

    if (a_star(&w, start, end, &p) == 0) {
        DS_LOG_ERROR("No path found");
    } else {
        for (int i = p.count - 1; i >= 0; i--) {
            printf("%d, %d\n", p.items[i].x, p.items[i].y);
        }
    }

    world_free(&w);
    return 0;
}
