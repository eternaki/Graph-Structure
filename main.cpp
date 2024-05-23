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
    long long n;               // Количество вершин
    Vector *adj;               // Список смежности
    long long *saturation;     // Nasycenie wierzchołków
    long long *neighborColors; // Ilość kolorów użytych przez sąsiadów

public:
    Graph(long long n) : n(n)
    {
        adj = new Vector[n];
        saturation = new long long[n]();     // Inicjalizujemy wszystkie wartości na 0
        neighborColors = new long long[n](); // Inicjalizujemy wszystkie wartości na 0
    }

    ~Graph()
    {
        delete[] adj;
        delete[] saturation;
        delete[] neighborColors;
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
        mergeSortDescending(degrees, 0, n - 1);
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

            colors[i] = color;

            // Возвращаем все цвета как доступные
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

    void colorGraphLF()
    {
        long long *degrees = new long long[n];
        long long *vertices = new long long[n];
        for (long long i = 0; i < n; i++)
        {
            degrees[i] = adj[i].size;
            vertices[i] = i;
        }

        // Сортируем вершины по убыванию степени с использованием сортировки слиянием
        mergeSortByDegree(vertices, degrees, 0, n - 1);

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
            long long vertex = vertices[i];

            // Отмечаем все цвета соседей как недоступные
            for (long long j = 0; j < adj[vertex].size; j++)
            {
                long long neighbor = adj[vertex][j];
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

            colors[vertex] = color;

            // Возвращаем все цвета как доступные
            for (long long j = 0; j < adj[vertex].size; j++)
            {
                long long neighbor = adj[vertex][j];
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
        delete[] degrees;
        delete[] vertices;
        delete[] available;
    }

    // Metoda do kolorowania wierzchołków za pomocą algorytmu SLF
    void colorGraphSLF()
    {
        long long *colors = new long long[n];
        for (long long i = 0; i < n; i++)
        {
            colors[i] = -1; // Inicjalizujemy wszystkie wierzchołki jako bez koloru
        }

        for (long long i = 0; i < n; i++)
        {
            long long vertex = getSLFVertex(colors); // Wybieramy wierzchołek do pokolorowania

            // Pokolorowani wierzchołka
            colors[vertex] = getAvailableColor(vertex, colors);

            // Aktualizacja nasycenia i ilości kolorów użytych przez sąsiadów
            updateSaturationAndNeighborColors(vertex, colors);
        }

        // Wypisujemy kolory wierzchołków
        for (long long i = 0; i < n; i++)
        {
            printf("%lld ", colors[i] + 1); // Wypisujemy kolory (zaczynając od 1)
        }
        printf("\n");

        delete[] colors;
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

    bool bfsCheckBipartite(long long start, long long *colors)
    {
        Queue q(n);
        q.push(start);
        colors[start] = 0;

        while (!q.empty())
        {
            long long v = q.pop();
            for (long long i = 0; i < adj[v].size; i++)
            {
                long long u = adj[v][i];
                if (colors[u] == -1)
                {
                    colors[u] = 1 - colors[v];
                    q.push(u);
                }
                else if (colors[u] == colors[v])
                {
                    return false;
                }
            }
        }
        return true;
    }

    long long bfsFarthestVertex(long long start)
    {
        Queue q(n);
        q.push(start);

        bool *visited = new bool[n]();
        visited[start] = true;

        long long farthestVertex = start;

        while (!q.empty())
        {
            long long v = q.pop();
            farthestVertex = v;
            for (long long i = 0; i < adj[v].size; i++)
            {
                long long u = adj[v][i];
                if (!visited[u])
                {
                    visited[u] = true;
                    q.push(u);
                }
            }
        }

        delete[] visited;
        return farthestVertex;
    }

    long long *bfsDistances(long long start)
    {
        long long *dist = new long long[n];
        for (long long i = 0; i < n; i++)
        {
            dist[i] = -1;
        }
        dist[start] = 0;

        Queue q(n);
        q.push(start);

        while (!q.empty())
        {
            long long v = q.pop();
            for (long long i = 0; i < adj[v].size; i++)
            {
                long long u = adj[v][i];
                if (dist[u] == -1)
                {
                    dist[u] = dist[v] + 1;
                    q.push(u);
                }
            }
        }

        return dist;
    }

    Vector bfsComponent(long long start, bool *visited)
    {
        Vector component;
        Queue q(n);
        q.push(start);
        visited[start] = true;

        while (!q.empty())
        {
            long long v = q.pop();
            component.push_back(v);
            for (long long i = 0; i < adj[v].size; i++)
            {
                long long u = adj[v][i];
                if (!visited[u])
                {
                    visited[u] = true;
                    q.push(u);
                }
            }
        }

        return component;
    }

    // Metoda do wybierania wierzchołka o największej saturacji do pokolorowania
    long long getSLFVertex(const long long *colors)
    {
        long long maxSaturation = -1;
        long long vertex = -1;

        for (long long i = 0; i < n; i++)
        {
            if (colors[i] == -1) // Sprawdzamy tylko wierzchołki niepokolorowane
            {
                if (saturation[i] > maxSaturation || (saturation[i] == maxSaturation && adj[i].size > adj[vertex].size))
                {
                    maxSaturation = saturation[i];
                    vertex = i;
                }
            }
        }

        return vertex;
    }

    // Metoda do znalezienia dostępnego koloru dla wierzchołka
    long long getAvailableColor(long long vertex, const long long *colors)
    {
        bool *available = new bool[n];
        for (long long i = 0; i < n; i++)
        {
            available[i] = true; // Wszystkie kolory są dostępne na początku
        }

        for (long long i = 0; i < adj[vertex].size; i++)
        {
            long long neighbor = adj[vertex][i];
            if (colors[neighbor] != -1)
            {
                available[colors[neighbor]] = false; // Oznaczamy używane kolory jako niedostępne
            }
        }

        // Szukamy pierwszego dostępnego koloru
        long long color;
        for (color = 0; color < n; color++)
        {
            if (available[color])
            {
                break;
            }
        }

        delete[] available;
        return color;
    }

    // Metoda do aktualizowania nasycenia i ilości kolorów użytych przez sąsiadów wierzchołka
    // Metoda do aktualizowania nasycenia i ilości kolorów użytych przez sąsiadów wierzchołka
    // Metoda do aktualizowania nasycenia i ilości kolorów użytych przez sąsiadów wierzchołka
    void updateSaturationAndNeighborColors(long long vertex, const long long *colors)
    {
        for (long long i = 0; i < adj[vertex].size; i++)
        {
            long long neighbor = adj[vertex][i];
            if (colors[neighbor] == -1) // Jeśli sąsiad nie jest pokolorowany
            {
                // Zwiększamy nasycenie tylko dla niepokolorowanych sąsiadów
                saturation[neighbor]++;

                // Zwiększamy ilość kolorów użytych przez sąsiada tylko dla niepokolorowanych sąsiadów
                for (long long j = 0; j < adj[neighbor].size; j++)
                {
                    long long neighborOfNeighbor = adj[neighbor][j];
                    if (colors[neighborOfNeighbor] != -1)
                    {
                        neighborColors[neighbor]++;
                        break; // Przerywamy pętlę, jeśli znaleziono pokolorowanego sąsiada
                    }
                }
            }
        }
    }

    void merge(long long arr[], long long l, long long m, long long r)
    {
        long long i, j, k;
        long long n1 = m - l + 1;
        long long n2 = r - m;

        long long *L = new long long[n1];
        long long *R = new long long[n2];

        for (i = 0; i < n1; i++)
            L[i] = arr[l + i];
        for (j = 0; j < n2; j++)
            R[j] = arr[m + 1 + j];

        i = 0;
        j = 0;
        k = l;
        while (i < n1 && j < n2)
        {
            if (L[i] <= R[j])
            {
                arr[k] = L[i];
                i++;
            }
            else
            {
                arr[k] = R[j];
                j++;
            }
            k++;
        }

        while (i < n1)
        {
            arr[k] = L[i];
            i++;
            k++;
        }

        while (j < n2)
        {
            arr[k] = R[j];
            j++;
            k++;
        }

        delete[] L;
        delete[] R;
    }

    void mergeDescending(long long arr[], long long l, long long m, long long r)
    {
        long long i, j, k;
        long long n1 = m - l + 1;
        long long n2 = r - m;

        long long *L = new long long[n1];
        long long *R = new long long[n2];

        for (i = 0; i < n1; i++)
            L[i] = arr[l + i];
        for (j = 0; j < n2; j++)
            R[j] = arr[m + 1 + j];

        i = 0;
        j = 0;
        k = l;
        while (i < n1 && j < n2)
        {
            if (L[i] >= R[j])
            {
                arr[k] = L[i];
                i++;
            }
            else
            {
                arr[k] = R[j];
                j++;
            }
            k++;
        }

        while (i < n1)
        {
            arr[k] = L[i];
            i++;
            k++;
        }

        while (j < n2)
        {
            arr[k] = R[j];
            j++;
            k++;
        }

        delete[] L;
        delete[] R;
    }

    void mergeSort(long long arr[], long long l, long long r)
    {
        if (l < r)
        {
            long long m = l + (r - l) / 2;

            mergeSort(arr, l, m);
            mergeSort(arr, m + 1, r);

            merge(arr, l, m, r);
        }
    }

    void mergeSortDescending(long long arr[], long long l, long long r)
    {
        if (l < r)
        {
            long long m = l + (r - l) / 2;

            mergeSortDescending(arr, l, m);
            mergeSortDescending(arr, m + 1, r);

            mergeDescending(arr, l, m, r);
        }
    }

    void mergeSortByDegree(long long vertices[], long long degrees[], long long l, long long r)
    {
        if (l < r)
        {
            long long m = l + (r - l) / 2;

            mergeSortByDegree(vertices, degrees, l, m);
            mergeSortByDegree(vertices, degrees, m + 1, r);

            mergeByDegree(vertices, degrees, l, m, r);
        }
    }

    void mergeByDegree(long long vertices[], long long degrees[], long long l, long long m, long long r)
    {
        long long i, j, k;
        long long n1 = m - l + 1;
        long long n2 = r - m;

        long long *LVertices = new long long[n1];
        long long *RVertices = new long long[n2];
        long long *LDegrees = new long long[n1];
        long long *RDegrees = new long long[n2];

        for (i = 0; i < n1; i++)
        {
            LVertices[i] = vertices[l + i];
            LDegrees[i] = degrees[l + i];
        }
        for (j = 0; j < n2; j++)
        {
            RVertices[j] = vertices[m + 1 + j];
            RDegrees[j] = degrees[m + 1 + j];
        }

        i = 0;
        j = 0;
        k = l;
        while (i < n1 && j < n2)
        {
            if (LDegrees[i] >= RDegrees[j])
            {
                vertices[k] = LVertices[i];
                degrees[k] = LDegrees[i];
                i++;
            }
            else
            {
                vertices[k] = RVertices[j];
                degrees[k] = RDegrees[j];
                j++;
            }
            k++;
        }

        while (i < n1)
        {
            vertices[k] = LVertices[i];
            degrees[k] = LDegrees[i];
            i++;
            k++;
        }

        while (j < n2)
        {
            vertices[k] = RVertices[j];
            degrees[k] = RDegrees[j];
            j++;
            k++;
        }

        delete[] LVertices;
        delete[] RVertices;
        delete[] LDegrees;
        delete[] RDegrees;
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
        printf("?\n");     // Планарность
        g.colorGraph();    // Цвета вершин, обычный метод раскраски
        g.colorGraphLF();  // Цвета вершин, метод раскраски LF
        // g.colorGraphSLF(); // Цвета вершин, метод раскраски SLF
        printf("?\n");
        printf("?\n");
        printf("%lld\n", g.countComplementEdges());
    }
    return 0;
}
