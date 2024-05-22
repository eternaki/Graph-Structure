#include <iostream>
#include <cstdio>

using namespace std;

class Vector {
public:
    int *data;
    int size;
    int capacity;

    Vector() {
        size = 0;
        capacity = 1;
        data = new int[capacity];
    }

    ~Vector() {
        delete[] data;
    }

    void push_back(int value) {
        if (size == capacity) {
            capacity *= 2;
            int *new_data = new int[capacity];
            for (int i = 0; i < size; i++) {
                new_data[i] = data[i];
            }
            delete[] data;
            data = new_data;
        }
        data[size++] = value;
    }

    int& operator[](int index) {
        return data[index];
    }

    const int& operator[](int index) const {
        return data[index];
    }
};

class Queue {
public:
    int *data;
    int front, rear, size, capacity;

    Queue(int cap) : capacity(cap) {
        data = new int[capacity];
        front = 0;
        size = 0;
        rear = capacity - 1;
    }

    ~Queue() {
        delete[] data;
    }

    bool empty() const {
        return size == 0;
    }

    void push(int value) {
        rear = (rear + 1) % capacity;
        data[rear] = value;
        size++;
    }

    int pop() {
        int value = data[front];
        front = (front + 1) % capacity;
        size--;
        return value;
    }
};

class Graph {
private:
    int n; // Количество вершин
    Vector *adj; // Список смежности

public:
    Graph(int n) : n(n) {
        adj = new Vector[n];
    }

    ~Graph() {
        delete[] adj;
    }

    void addEdge(int u, int v) {
        for (int i = 0; i < adj[u].size; i++) {
            if (adj[u][i] == v) return;
        }
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void readGraph() {
        int num_neighbors, neighbor;
        for (int i = 0; i < n; i++) {
            scanf("%d", &num_neighbors);
            for (int j = 0; j < num_neighbors; j++) {
                scanf("%d", &neighbor);
                addEdge(i, neighbor - 1); // Преобразуем к нулевой индексации
            }
        }
    }

    void printDegrees() {
        int *degrees = new int[n];
        for (int i = 0; i < n; i++) {
            degrees[i] = adj[i].size;
        }
        quickSort(degrees, 0, n - 1);
        for (int i = 0; i < n; i++) {
            printf("%d ", degrees[i]);
        }
        printf("\n");
        delete[] degrees;
    }

    int countComponents() {
        int count = 0;
        bool *visited = new bool[n]();
        for (int i = 0; i < n; i++) {
            if (!visited[i]) {
                dfs(i, visited);
                count++;
            }
        }
        delete[] visited;
        return count;
    }

    bool isBipartite() {
        int *colors = new int[n];
        for (int i = 0; i < n; i++) {
            colors[i] = -1;
        }

        for (int i = 0; i < n; i++) {
            if (colors[i] == -1) {
                if (!bfsCheckBipartite(i, colors)) {
                    delete[] colors;
                    return false;
                }
            }
        }
        delete[] colors;
        return true;
    }

    void printEccentricities() {
        int *eccentricities = new int[n];
        bool *visited = new bool[n]();

        for (int i = 0; i < n; i++) {
            if (!visited[i]) {
                Vector component = bfsComponent(i, visited);
                for (int j = 0; j < component.size; j++) {
                    int vertex = component[j];
                    int farthestVertex = bfsFarthestVertex(vertex);
                    int *dist = bfsDistances(farthestVertex);
                    for (int k = 0; k < component.size; k++) {
                        int u = component[k];
                        if (eccentricities[u] < dist[u]) {
                            eccentricities[u] = dist[u];
                        }
                    }
                    delete[] dist;
                }
            }
        }

        for (int i = 0; i < n; i++) {
            printf("%d ", eccentricities[i]);
        }
        printf("\n");

        delete[] eccentricities;
        delete[] visited;
    }

    int countEdges() {
        int edgeCount = 0;
        for (int i = 0; i < n; i++) {
            edgeCount += adj[i].size;
        }
        return edgeCount / 2; // Каждое ребро считается дважды
    }

    int countComplementEdges() {
        int totalEdges = (n * (n - 1)) / 2; // Полное количество рёбер в полном графе
        int graphEdges = countEdges();
        return totalEdges - graphEdges;
    }

    void colorGraph() {
        int *colors = new int[n];
        for (int i = 0; i < n; i++) {
            colors[i] = -1; // Инициализируем все вершины без цвета
        }

        bool *available = new bool[n];
        for (int i = 0; i < n; i++) {
            available[i] = true; // Все цвета доступны
        }

        for (int i = 0; i < n; i++) {
            // Отмечаем все цвета соседей как недоступные
            for (int j = 0; j < adj[i].size; j++) {
                int neighbor = adj[i][j];
                if (colors[neighbor] != -1) {
                    available[colors[neighbor]] = false;
                }
            }

            // Ищем первый доступный цвет
            int color;
            for (color = 0; color < n; color++) {
                if (available[color]) {
                    break;
                }
            }

            colors[i] = color; // Назначаем вершине найденный цвет

            // Возвращаем все цвета как доступные
            for (int j = 0; j < adj[i].size; j++) {
                int neighbor = adj[i][j];
                if (colors[neighbor] != -1) {
                    available[colors[neighbor]] = true;
                }
            }
        }

        for (int i = 0; i < n; i++) {
            printf("%d ", colors[i] + 1); // Выводим цвета (начиная с 1)
        }
        printf("\n");

        delete[] colors;
        delete[] available;
    }

private:
    void dfs(int v, bool *visited) {
        visited[v] = true;
        for (int i = 0; i < adj[v].size; i++) {
            int u = adj[v][i];
            if (!visited[u]) {
                dfs(u, visited);
            }
        }
    }

    bool bfsCheckBipartite(int s, int *colors) {
        Queue q(n);
        q.push(s);
        colors[s] = 0;

        while (!q.empty()) {
            int u = q.pop();
            for (int i = 0; i < adj[u].size; i++) {
                int v = adj[u][i];
                if (colors[v] == -1) {
                    colors[v] = 1 - colors[u];
                    q.push(v);
                } else if (colors[v] == colors[u]) {
                    return false;
                }
            }
        }
        return true;
    }

    int* bfsDistances(int start) {
        int *dist = new int[n];
        for (int i = 0; i < n; i++) {
            dist[i] = n; // Используем n как "бесконечность"
        }
        Queue q(n);
        q.push(start);
        dist[start] = 0;

        while (!q.empty()) {
            int v = q.pop();
            for (int i = 0; i < adj[v].size; i++) {
                int neighbor = adj[v][i];
                if (dist[neighbor] == n) {
                    dist[neighbor] = dist[v] + 1;
                    q.push(neighbor);
                }
            }
        }

        return dist;
    }

    int bfsFarthestVertex(int start) {
        int *dist = new int[n];
        for (int i = 0; i < n; i++) {
            dist[i] = n; // Используем n как "бесконечность"
        }
        Queue q(n);
        q.push(start);
        dist[start] = 0;

        int farthestVertex = start;

        while (!q.empty()) {
            int v = q.pop();
            for (int i = 0; i < adj[v].size; i++) {
                int neighbor = adj[v][i];
                if (dist[neighbor] == n) {
                    dist[neighbor] = dist[v] + 1;
                    q.push(neighbor);
                    if (dist[neighbor] > dist[farthestVertex]) {
                        farthestVertex = neighbor;
                    }
                }
            }
        }

        delete[] dist;
        return farthestVertex;
    }

    Vector bfsComponent(int start, bool *visited) {
        Vector component;
        Queue q(n);
        q.push(start);
        visited[start] = true;
        component.push_back(start);

        while (!q.empty()) {
            int v = q.pop();
            for (int i = 0; i < adj[v].size; i++) {
                int neighbor = adj[v][i];
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                    component.push_back(neighbor);
                }
            }
        }

        return component;
    }

    void quickSort(int *array, int left, int right) {
        if (left < right) {
            int pivot = partition(array, left, right);
            quickSort(array, left, pivot - 1);
            quickSort(array, pivot + 1, right);
        }
    }

    int partition(int *array, int left, int right) {
        int pivot = array[right];
        int i = left - 1;

        for (int j = left; j < right; j++) {
            if (array[j] >= pivot) {
                i++;
                swap(array[i], array[j]);
            }
        }
        swap(array[i + 1], array[right]);
        return i + 1;
    }

    void sortDescending(int *array, int size) {
        for (int i = 0; i < size - 1; i++) {
            for (int j = 0; j < size - i - 1; j++) {
                if (array[j] < array[j + 1]) {
                    int temp = array[j];
                    array[j] = array[j + 1];
                    array[j + 1] = temp;
                }
            }
        }
    }
};

int main() {
    int k;
    scanf("%d", &k);
    while (k--) {
        int n;
        scanf("%d", &n);
        Graph g(n);
        g.readGraph();
        g.printDegrees();
        printf("%d\n", g.countComponents());
        printf("%s\n", g.isBipartite() ? "T" : "F");
        // g.printEccentricities(); // Выводим эксцентриситеты
        printf("?\n");
        printf("?\n"); // Планарность
        g.colorGraph(); // Цвета вершин, методы раскраски
        printf("?\n"); // Планарность
        printf("?\n");
        printf("?\n"); // Число разных подграфов C4
        printf("%d\n", g.countComplementEdges()); // Число рёбер дополнения графа

    }
    return 0;
}
