#include <iostream>
#include <cstdio>

using namespace std;

class Vector
{
public:
    long long *data;
    long long size;
    long long capacity;

    Vector()
    {
        size = 0;
        capacity = 1;
        data = new long long[capacity];
    }

    ~Vector()
    {
        delete[] data;
    }

    void push_back(long long value)
    {
        if (size == capacity)
        {
            capacity *= 2;
            long long *new_data = new long long[capacity];
            for (long long i = 0; i < size; i++)
            {
                new_data[i] = data[i];
            }
            delete[] data;
            data = new_data;
        }
        data[size++] = value;
    }

    long long &operator[](long long index)
    {
        return data[index];
    }

    const long long &operator[](long long index) const
    {
        return data[index];
    }
};

class Queue
{
public:
    long long *data;
    long long front, rear, size, capacity;

    Queue(long long cap) : capacity(cap)
    {
        data = new long long[capacity];
        front = 0;
        size = 0;
        rear = capacity - 1;
    }

    ~Queue()
    {
        delete[] data;
    }

    bool empty() const
    {
        return size == 0;
    }

    void push(long long value)
    {
        rear = (rear + 1) % capacity;
        data[rear] = value;
        size++;
    }

    long long pop()
    {
        long long value = data[front];
        front = (front + 1) % capacity;
        size--;
        return value;
    }
};

class Graph
{
private:
    long long n; // Количество вершин
    Vector *adj; // Список смежности

public:
    Graph(long long n) : n(n)
    {
        adj = new Vector[n];
    }

    ~Graph()
    {
        delete[] adj;
    }

    void addEdge(long long u, long long v)
    {
        for (long long i = 0; i < adj[u].size; i++)
        {
            if (adj[u][i] == v)
                return;
        }
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void readGraph()
    {
        long long num_neighbors, neighbor;
        for (long long i = 0; i < n; i++)
        {
            scanf("%lld", &num_neighbors);
            for (long long j = 0; j < num_neighbors; j++)
            {
                scanf("%lld", &neighbor);
                addEdge(i, neighbor - 1); // Преобразуем к нулевой индексации
            }
        }
    }

    void printDegrees()
    {
        long long *degrees = new long long[n];
        for (long long i = 0; i < n; i++)
        {
            degrees[i] = adj[i].size;
        }
        mergeSort(degrees, 0, n - 1);
        for (long long i = 0; i < n; i++)
        {
            printf("%lld ", degrees[i]);
        }
        printf("\n");
        delete[] degrees;
    }

    long long countComponents()
    {
        long long count = 0;
        bool *visited = new bool[n]();
        for (long long i = 0; i < n; i++)
        {
            if (!visited[i])
            {
                dfs(i, visited);
                count++;
            }
        }
        delete[] visited;
        return count;
    }

    bool isBipartite()
    {
        long long *colors = new long long[n];
        for (long long i = 0; i < n; i++)
        {
            colors[i] = -1;
        }

        for (long long i = 0; i < n; i++)
        {
            if (colors[i] == -1)
            {
                if (!bfsCheckBipartite(i, colors))
                {
                    delete[] colors;
                    return false;
                }
            }
        }
        delete[] colors;
        return true;
    }

    void printEccentricities()
    {
        long long *eccentricities = new long long[n];
        bool *visited = new bool[n]();

        for (long long i = 0; i < n; i++)
        {
            if (!visited[i])
            {
                Vector component = bfsComponent(i, visited);
                for (long long j = 0; j < component.size; j++)
                {
                    long long vertex = component[j];
                    long long farthestVertex = bfsFarthestVertex(vertex);
                    long long *dist = bfsDistances(farthestVertex);
                    for (long long k = 0; k < component.size; k++)
                    {
                        long long u = component[k];
                        if (eccentricities[u] < dist[u])
                        {
                            eccentricities[u] = dist[u];
                        }
                    }
                    delete[] dist;
                }
            }
        }

        for (long long i = 0; i < n; i++)
        {
            printf("%lld ", eccentricities[i]);
        }
        printf("\n");

        delete[] eccentricities;
        delete[] visited;
    }

    long long countEdges()
    {
        long long edgeCount = 0;
        for (long long i = 0; i < n; i++)
        {
            edgeCount += adj[i].size;
        }
        return edgeCount / 2; // Каждое ребро считается дважды
    }

    long long countComplementEdges()
    {
        long long totalEdges = (n * (n - 1)) / 2; // Полное количество рёбер в полном графе
        long long graphEdges = countEdges();
        return totalEdges - graphEdges;
    }

    void colorGraph()
    {
        long long *colors = new long long[n];
        for (long long i = 0; i < n; i++)
        {
            colors[i] = -1; // Инициализируем все вершины без цвета
        }

        bool *available = new bool[n];
        for (long long i = 0; i < n; i++)
        {
            available[i] = true; // Все цвета доступны
        }

        for (long long i = 0; i < n; i++)
        {
            // Отмечаем все цвета соседей как недоступные
            for (long long j = 0; j < adj[i].size; j++)
            {
                long long neighbor = adj[i][j];
                if (colors[neighbor] != -1)
                {
                    available[colors[neighbor]] = false;
                }
            }

            // Ищем первый доступный цвет
            long long color;
            for (color = 0; color < n; color++)
            {
                if (available[color])
                {
                    break;
                }
            }

            colors[i] = color; //// Возвращаем все цвета как доступные
            for (long long j = 0; j < adj[i].size; j++)
            {
                long long neighbor = adj[i][j];
                if (colors[neighbor] != -1)
                {
                    available[colors[neighbor]] = true;
                }
            }
        }

        for (long long i = 0; i < n; i++)
        {
            printf("%lld ", colors[i] + 1); // Выводим цвета (начиная с 1)
        }
        printf("\n");

        delete[] colors;
        delete[] available;
    }

private:
    void dfs(long long v, bool *visited)
    {
        visited[v] = true;
        for (long long i = 0; i < adj[v].size; i++)
        {
            long long u = adj[v][i];
            if (!visited[u])
            {
                dfs(u, visited);
            }
        }
    }
    bool bfsCheckBipartite(long long s, long long *colors)
    {
        Queue q(n);
        q.push(s);
        colors[s] = 0;

        while (!q.empty())
        {
            long long u = q.pop();
            for (long long i = 0; i < adj[u].size; i++)
            {
                long long v = adj[u][i];
                if (colors[v] == -1)
                {
                    colors[v] = 1 - colors[u];
                    q.push(v);
                }
                else if (colors[v] == colors[u])
                {
                    return false;
                }
            }
        }
        return true;
    }

    long long *bfsDistances(long long start)
    {
        long long *dist = new long long[n];
        for (long long i = 0; i < n; i++)
        {
            dist[i] = n; // Используем n как "бесконечность"
        }
        Queue q(n);
        q.push(start);
        dist[start] = 0;

        while (!q.empty())
        {
            long long v = q.pop();
            for (long long i = 0; i < adj[v].size; i++)
            {
                long long neighbor = adj[v][i];
                if (dist[neighbor] == n)
                {
                    dist[neighbor] = dist[v] + 1;
                    q.push(neighbor);
                }
            }
        }

        return dist;
    }

    long long bfsFarthestVertex(long long start)
    {
        long long *dist = new long long[n];
        for (long long i = 0; i < n; i++)
        {
            dist[i] = n; // Используем n как "бесконечность"
        }
        Queue q(n);
        q.push(start);
        dist[start] = 0;

        long long farthestVertex = start;

        while (!q.empty())
        {
            long long v = q.pop();
            for (long long i = 0; i < adj[v].size; i++)
            {
                long long neighbor = adj[v][i];
                if (dist[neighbor] == n)
                {
                    dist[neighbor] = dist[v] + 1;
                    q.push(neighbor);
                    if (dist[neighbor] > dist[farthestVertex])
                    {
                        farthestVertex = neighbor;
                    }
                }
            }
        }

        delete[] dist;
        return farthestVertex;
    }

    Vector bfsComponent(long long start, bool *visited)
    {
        Vector component;
        Queue q(n);
        q.push(start);
        visited[start] = true;
        component.push_back(start);

        while (!q.empty())
        {
            long long v = q.pop();
            for (long long i = 0; i < adj[v].size; i++)
            {
                long long neighbor = adj[v][i];
                if (!visited[neighbor])
                {
                    visited[neighbor] = true;
                    q.push(neighbor);
                    component.push_back(neighbor);
                }
            }
        }

        return component;
    }

    void mergeSort(long long *array, long long left, long long right)
    {
        if (left < right)
        {
            long long mid = left + (right - left) / 2;
            mergeSort(array, left, mid);
            mergeSort(array, mid + 1, right);
            merge(array, left, mid, right);
        }
    }

    void merge(long long *array, long long left, long long mid, long long right)
    {
        long long n1 = mid - left + 1;
        long long n2 = right - mid;

        long long *L = new long long[n1];
        long long *R = new long long[n2];

        for (long long i = 0; i < n1; i++)
        {
            L[i] = array[left + i];
        }
        for (long long j = 0; j < n2; j++)
        {
            R[j] = array[mid + 1 + j];
        }

        long long i = 0, j = 0, k = left;

        while (i < n1 && j < n2)
        {
            if (L[i] >= R[j])
            {
                array[k] = L[i];
                i++;
            }
            else
            {
                array[k] = R[j];
                j++;
            }
            k++;
        }

        while (i < n1)
        {
            array[k] = L[i];
            i++;
            k++;
        }

        while (j < n2)
        {
            array[k] = R[j];
            j++;
            k++;
        }

        delete[] L;
        delete[] R;
    }
};

int main()
{
    long long k;
    scanf("%lld", &k);
    while (k--)
    {
        long long n;
        scanf("%lld", &n);
        Graph g(n);
        g.readGraph();
        g.printDegrees();
        printf("%lld\n", g.countComponents());
        printf("%s\n", g.isBipartite() ? "T" : "F");
        // g.printEccentricities(); // Выводим эксцентриситеты
        printf("?\n");
        printf("?\n");  // Планарность
        g.colorGraph(); // Цвета вершин, методы раскраски
        printf("?\n");  // Планарность
        printf("?\n");
        printf("?\n");
        printf("%lld\n", g.countComplementEdges());
    }
    return 0;
}