#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DS_PQ_IMPLEMENTATION
#include "../ds.h"

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

void world_free(struct world *w) {
    free(w->map);
}

struct position {
        int x;
        int y;
};

const struct position directions[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
const int num_directions = sizeof(directions) / sizeof(directions[0]);

int manhattan_distance(struct position p1, struct position p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

int position_hash(struct world *w, struct position p) {
    return p.y * w->width + p.x;
}

int position_equals(struct position p1, struct position p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

struct position_node {
        struct position p;
        int f;
};

struct position_node *position_node_new(struct position p, int f) {
    struct position_node *node =
        (struct position_node *)malloc(sizeof(struct position_node));
    node->p = p;
    node->f = f;
    return node;
}

int position_node_compare_min(const void *a, const void *b) {
    return ((struct position_node *)b)->f - ((struct position_node *)a)->f;
}

void position_node_free(struct position_node *p) {
    free(p);
}

struct position_array {
        struct position *items;
        int count;
        int capacity;
};

void position_array_free(struct position_array *p) {
    free(p->items);
}

int reconstruct_path(struct world *w, int *came_from, struct position current,
                     struct position_array *p) {
    ds_da_append(p, current);
    int current_index = position_hash(w, current);

    while (came_from[current_index] != -1) {
        current_index = came_from[current_index];
        struct position current = {current_index % w->width,
                                   current_index / w->width};
        ds_da_append(p, current);
    }

    return 0;
}

int a_star(struct world *w, struct position start, struct position end,
           struct position_array *p) {
    int result = 0;
    int num_nodes = w->width * w->height;

    // The set of discovered nodes that may need to be (re-)expanded.
    // Initially, only the start node is known.
    // This is usually implemented as a min-heap or priority queue rather than a
    // hash-set.
    struct ds_priority_queue open_set;
    ds_priority_queue_init(&open_set, position_node_compare_min);

    struct position_node *start_node =
        position_node_new(start, manhattan_distance(start, end));
    ds_priority_queue_insert(&open_set, start_node);

    // For node n, cameFrom[n] is the node immediately preceding it on the
    // cheapest path from the start to n currently known.
    int *came_from = (int *)malloc(num_nodes * sizeof(int));
    for (int i = 0; i < num_nodes; i++) {
        came_from[i] = -1;
    }

    // For node n, gScore[n] is the cost of the cheapest path from start to n
    // currently known.
    int *g_score = (int *)malloc(num_nodes * sizeof(int));
    for (int i = 0; i < num_nodes; i++) {
        g_score[i] = INT_MAX;
    }
    g_score[position_hash(w, start)] = 0;

    // For node n, fScore[n] := gScore[n] + h(n). fScore[n] represents our
    // current best guess as to how cheap a path could be from start to finish
    // if it goes through n.
    int *f_score = (int *)malloc(num_nodes * sizeof(int));
    for (int i = 0; i < num_nodes; i++) {
        f_score[i] = INT_MAX;
    }
    f_score[position_hash(w, start)] = manhattan_distance(start, end);

    struct position_node *current_node = NULL;
    while (ds_priority_queue_empty(&open_set) == 0) {
        // This operation can occur in O(Log(N)) time if openSet is a min-heap
        // or a priority queue
        current_node = NULL;
        ds_priority_queue_pull(&open_set, (void **)&current_node);
        int current_index = position_hash(w, current_node->p);

        struct position current = current_node->p;

        if (position_equals(current_node->p, end)) {
            reconstruct_path(w, came_from, current_node->p, p);
            return_defer(1);
        }

        for (int i = 0; i < num_directions; i++) {
            struct position neighbor = {current.x + directions[i].x,
                                        current.y + directions[i].y};
            int neighbor_index = position_hash(w, neighbor);

            if (neighbor.x < 0 || neighbor.x >= w->width || neighbor.y < 0 ||
                neighbor.y >= w->height || w->map[neighbor_index] != 0) {
                continue;
            }

            // d(current,neighbor) is the weight of the edge from current to
            // neighbor tentative_gScore is the distance from start to the
            // neighbor through current
            int tentative_g_score = g_score[current_index] + 1;
            if (tentative_g_score < g_score[neighbor_index]) {
                // This path to neighbor is better than any previous one.
                came_from[neighbor_index] = current_index;
                g_score[neighbor_index] = tentative_g_score;
                f_score[neighbor_index] =
                    tentative_g_score + manhattan_distance(neighbor, end);

                int found = 0;
                for (unsigned int j = 0; j < open_set.count; j++) {
                    struct position_node *node =
                        (struct position_node *)open_set.items[j];
                    if (position_equals(node->p, neighbor)) {
                        found = 1;
                        break;
                    }
                }

                if (found == 0) {
                    struct position_node *neighbor_node =
                        position_node_new(neighbor, f_score[neighbor_index]);
                    ds_priority_queue_insert(&open_set, neighbor_node);
                }
            }
        }

        position_node_free(current_node);
    }

defer:
    if (current_node != NULL) {
        position_node_free(current_node);
    }
    while (ds_priority_queue_empty(&open_set) == 0) {
        struct position_node *node = NULL;
        ds_priority_queue_pull(&open_set, (void **)&node);
        position_node_free(node);
    }
    ds_priority_queue_free(&open_set);
    free(came_from);
    free(g_score);
    free(f_score);

    return result;
}

int pathfind(struct position_array *obstacles, int width, int height, struct position start, struct position end, struct position_array *p) {
    struct world w;
    if (world_init(&w, width, height) != 0) {
        return 1;
    }

    for (int i = 0; i < obstacles->count; i++) {
        struct position p = obstacles->items[i];
        if (world_set(&w, p.x, p.y, 1) != 0) {
            return 1;
        }
    }

    int result = a_star(&w, start, end, p);

    world_free(&w);
    return result;
}

/* Specific to the executable */

#define MAX_LINE_LEN 1024

struct line_array {
        char **items;
        int count;
        int capacity;
};

void line_array_free(struct line_array *l) {
    for (int i = 0; i < l->count; i++) {
        free(l->items[i]);
    }
    free(l->items);
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

    struct line_array lines = {0};
    char line[MAX_LINE_LEN];
    int width = 0;
    int height = 0;
    while (fgets(line, MAX_LINE_LEN * sizeof(char), file) != NULL) {
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

    line_array_free(&lines);

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

int main() {
    struct world w;
    if (world_from_file(&w, "examples/world.txt") != 0) {
        return 1;
    }

    world_print(&w);

    struct position_array p = {0};
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
    free(p.items);
    return 0;
}
