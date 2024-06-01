// Author: Lohachov Danylo, 196610
// All rights reserved. Data structures implemented with ChatGPT helping.

#include <iostream>
#include <cstdio>

using namespace std;
#define NEG -1

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
    long long front, end, size, capacity;

    Queue(long long cap) : capacity(cap)
    {
        data = new long long[capacity];
        front = 0;
        size = 0;
        end = capacity - 1;
    }

    ~Queue()
    {
        delete[] data;
    }

    bool empty() const
    {
        if(size == 0) return true; 
        else return false; 
    }

    void push(long long value)
    {
        end = (end + 1) % capacity;
        data[end] = value;
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
    long long n;               // peaks counter
    Vector *adj;               // Lista sąsiedztwa
    long long *crowded;     // Nasycenie wierzchołków
    long long *neighborColors; // Ilość kolorów użytych przez sąsiadów

public:
    Graph(long long peaks) : n(peaks)
    {
        adj = new Vector[n];
        crowded = new long long[n]();     // Inicjalizujemy wszystkie wartości na 0
        neighborColors = new long long[n](); // Inicjalizujemy wszystkie wartości na 0
    }

    ~Graph()
    {
        delete[] adj;
        delete[] crowded;
        delete[] neighborColors;
    }

    void addEdge(long long u, long long v)
    {
        for (long long i = 0; i < adj[u].size; i++)
        {
            if (adj[u][i] == v) return;
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
                addEdge(i, neighbor - 1); // Konwertuj na indeksowanie zerowe
            }
        }
    }

    void printDegrees()
    {
        long long *degrees = new long long[n];
        for (long long i = 0; i < n; i++) { degrees[i] = adj[i].size; }
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
            if (visited[i] == false) { dfs(i, visited); count++; }
        }
        delete[] visited;
        return count;
    }

    bool isBipartite()
    {
        long long *colors = new long long[n];
        for (long long i = 0; i < n; i++) { colors[i] = NEG; }

        for (long long i = 0; i < n; i++)
        {
            if (colors[i] == NEG)
            {
                if (bfsCheckBipartite(i, colors) == false)
                {
                    delete[] colors;
                    return false;
                }
            }
        }
        delete[] colors;
        return true;
    }

    long long countEdges()
    {
        long long edgeCount = 0;
        for (long long i = 0; i < n; i++) { edgeCount += adj[i].size; }
        return edgeCount / 2; // Każda krawędź jest liczona dwukrotnie
    }

    long long countComplementEdges()
    {
        long long totalEdges = (n * (n - 1)) / 2; // Całkowita liczba krawędzi w pełnym grafie
        long long graphEdges = countEdges();
        return totalEdges - graphEdges;
    }

    void colorGraph()
    {
        long long *colors = new long long[n];
        bool *available = new bool[n];
        for (long long i = 0; i < n; i++)
        {
            colors[i] = NEG; // Zainicjuj wszystkie wierzchołki bez koloru
            available[i] = true; // Wszystkie kolory są dostępne
        }

        for (long long i = 0; i < n; i++)
        {
            // Oznacz wszystkie sąsiednie kolory jako niedostępne
            for (long long j = 0; j < adj[i].size; j++)
            {
                long long neighbor = adj[i][j];
                if (colors[neighbor] != NEG) available[colors[neighbor]] = false;
            }

            // Poszukuję pierwszego dostępnego koloru
            long long color;
            for (color = 0; color < n; color++)
            {
                if (available[color] == true) break;
            }

            colors[i] = color;

            // Zwracamy wszystkie dostępne kolory
            for (long long j = 0; j < adj[i].size; j++)
            {
                long long neighbor = adj[i][j];
                if (colors[neighbor] != NEG) available[colors[neighbor]] = true;
            }
        }

        for (long long i = 0; i < n; i++)
        {
            printf("%lld ", colors[i] + 1); // Kolory wyjściowe (zaczynając od 1)
        }
        printf("\n");

        delete[] colors;
        delete[] available;
    }

    void colorGraphLF()
    {
        long long *degrees = new long long[n];
        long long *vertices = new long long[n];
        for (long long i = 0; i < n; i++) { degrees[i] = adj[i].size; vertices[i] = i; }

        // Sortuj wierzchołki w kolejności malejącej, używając sortowania przez scalanie
        mergeSortByDegree(vertices, degrees, 0, n - 1);

        long long *colors = new long long[n];
        bool *available = new bool[n];

        for (long long i = 0; i < n; i++) {
            colors[i] = -1; // Zainicjuj wszystkie wierzchołki bez koloru
            available[i] = true; // Wszystkie kolory są dostępne
        }

        for (long long i = 0; i < n; i++)
        {
            long long vertex = vertices[i];

            // Oznacz wszystkie sąsiednie kolory jako niedostępne
            for (long long j = 0; j < adj[vertex].size; j++)
            {
                long long neighbor = adj[vertex][j];
                if (colors[neighbor] != NEG) available[colors[neighbor]] = false;
            }

            // Poszukuję pierwszego dostępnego koloru
            long long color;
            for (color = 0; color < n; color++)
            {
                if (available[color] == true) break;
            }

            colors[vertex] = color;

            // Zwracamy wszystkie dostępne kolory
            for (long long j = 0; j < adj[vertex].size; j++)
            {
                long long neighbor = adj[vertex][j];
                if (colors[neighbor] != NEG) available[colors[neighbor]] = true;
            }
        }

        for (long long i = 0; i < n; i++)
        {
            printf("%lld ", colors[i] + 1); // Kolory wyjściowe (zaczynając od 1)
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
        for (long long i = 0; i < n; i++) { colors[i] = NEG; }

        for (long long i = 0; i < n; i++)
        {
            long long vertex = getSLFVertex(colors); // Wybór wierzchołka do malowania

            // Najlepsza kolorystyka
            colors[vertex] = getAvailableColor(vertex, colors);

            // Zaktualizuj nasycenie i liczbę kolorów używanych przez sąsiadów
            updateCrowdedAndNeighborColors(vertex, colors);
        }

        for (long long i = 0; i < n; i++)
        {
            printf("%lld ", colors[i] + 1);
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
            if (visited[u] == false) dfs(u, visited);
        }
    }

    bool bfsCheckBipartite(long long start, long long *colors)
    {
        Queue q(n);
        q.push(start);
        colors[start] = 0;

        while (q.empty() == false)
        {
            long long v = q.pop();
            for (long long i = 0; i < adj[v].size; i++)
            {
                long long u = adj[v][i];
                if (colors[u] == NEG) { colors[u] = 1 - colors[v]; q.push(u); }
                else if (colors[u] == colors[v]) return false;
            }
        }
        return true;
    }

    // Metoda do wybierania wierzchołka o największej saturacji do pokolorowania
    long long getSLFVertex(const long long *colors)
    {
        long long maxcrowded = NEG;
        long long vertex = NEG;

        for (long long i = 0; i < n; i++)
        {
            if (colors[i] == NEG) // Sprawdzamy tylko wierzchołki niepokolorowane
            {
                if (crowded[i] > maxcrowded || (crowded[i] == maxcrowded && adj[i].size > adj[vertex].size))
                {
                    maxcrowded = crowded[i];
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
        for (long long i = 0; i < n; i++) { available[i] = true; } // Wszystkie kolory są dostępne na początku

        for (long long i = 0; i < adj[vertex].size; i++)
        {
            long long neighbor = adj[vertex][i];
            if (colors[neighbor] != NEG) available[colors[neighbor]] = false; // Oznaczamy używane kolory jako niedostępne
        }

        // Szukamy pierwszego dostępnego koloru
        long long color;
        for (color = 0; color < n; color++)
        {
            if (available[color] == true) break;
        }

        delete[] available;
        return color;
    }

    // Metoda aktualizacji nasycenia i liczby kolorów używanych przez sąsiadów wierzchołka
    void updateCrowdedAndNeighborColors(long long vertex, const long long *colors)
    {
        for (long long i = 0; i < adj[vertex].size; i++)
        {
            long long neighbor = adj[vertex][i];
            if (colors[neighbor] == NEG)
            {                           // Jeśli sąsiad nie jest pomalowany
                bool isNewColor = true; // Flaga, aby sprawdzić nowy kolor
                for (long long j = 0; j < adj[neighbor].size; j++)
                {
                    long long neighborOfNeighbor = adj[neighbor][j];
                    if (neighborOfNeighbor != vertex && colors[neighborOfNeighbor] == colors[vertex])
                    {
                        isNewColor = false;
                        break;
                    }
                }
                if (isNewColor == true) crowded[neighbor]++;
            }
        }
    }

    void copyToTempArrays(long long arr[], long long L[], long long R[], long long l, long long m, long long n1, long long n2)
    {
        for (int i = 0; i < n1; i++)
            L[i] = arr[l + i];
        for (int j = 0; j < n2; j++)
            R[j] = arr[m + 1 + j];
    }

    void merge(long long arr[], long long l, long long m, long long r)
    {
        long long i, j, k;
        long long n1 = m - l + 1;
        long long n2 = r - m;

        long long *L = new long long[n1];
        long long *R = new long long[n2];

        copyToTempArrays(arr, L, R, l, m, n1, n2);

        i = 0;
        j = 0;
        k = l;
        while (i < n1 && j < n2)
        {
            if (L[i] <= R[j]) { arr[k] = L[i]; i++; }
            else { arr[k] = R[j]; j++; }
            k++;
        }

        while (i < n1) { arr[k] = L[i]; i++; k++; }
        while (j < n2) { arr[k] = L[j]; j++; k++; }

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

        copyToTempArrays(arr, L, R, l, m, n1, n2);

        i = 0;
        j = 0;
        k = l;
        while (i < n1 && j < n2)
        {
            if (L[i] >= R[j]) { arr[k] = L[i]; i++; }
            else { arr[k] = R[j]; j++; }
            k++;
        }

        while (i < n1) { arr[k] = L[i]; i++; k++; }
        while (j < n2) { arr[k] = R[j]; j++; k++; }

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

        for (i = 0; i < n1; i++) { LVertices[i] = vertices[l + i]; LDegrees[i] = degrees[l + i]; }
        for (j = 0; j < n2; j++) { RVertices[j] = vertices[m + 1 + j]; RDegrees[j] = degrees[m + 1 + j]; }

        i = 0;
        j = 0;
        k = l;
        while (i < n1 && j < n2)
        {
            if (LDegrees[i] >= RDegrees[j]) { vertices[k] = LVertices[i]; degrees[k] = LDegrees[i]; i++; }
            else { vertices[k] = RVertices[j]; degrees[k] = RDegrees[j]; j++; }
            k++;
        }

        while (i < n1) { vertices[k] = LVertices[i]; degrees[k] = LDegrees[i]; i++; k++; }
        while (j < n2) { vertices[k] = RVertices[j]; degrees[k] = RDegrees[j]; j++; k++; }

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
        printf("?\n");
        printf("?\n");
        g.colorGraph();
        g.colorGraphLF();
        g.colorGraphSLF();
        printf("?\n");
        printf("%lld\n", g.countComplementEdges());
    }
    return 0;
}
