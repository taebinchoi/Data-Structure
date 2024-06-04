#include <stdio.h>
#include <stdlib.h>
#include <limits.h> // 필요한 헤더파일 불러오기

#define INF INT_MAX
#define NIL -1

typedef struct{
    int vertex;
    int dist;
    int pred;
} MinHeapNode;

typedef struct{
    int size;
    int capacity;
    int *pos;
    MinHeapNode **array;
} MinHeap;

typedef struct{
    int vertex;
    int weight;
    struct AdjListNode* next;
} AdjListNode;

typedef struct{
    int num_vertices;
    int num_edges;
    int source;
    AdjListNode** adjList;
} Graph; // 각 구조체 선언

AdjListNode* newAdjListNode(int vertex, int weight){ // 새로운 인접 ListNode를 생성하고 초기화, 시간복잡도 = O(1)
    AdjListNode* node = (AdjListNode*) malloc(sizeof(AdjListNode));
    node->vertex = vertex;
    node->weight = weight;
    node->next = NULL;
    return node;
}

Graph* createGraph(int num_vertices, int num_edges, int source){ // 그래프 생성 관련, 시간복잡도 = O(num_vertices)
    Graph* graph = (Graph*) malloc(sizeof(Graph));
    graph->num_vertices = num_vertices;
    graph->num_edges = num_edges;
    graph->source = source;
    graph->adjList = (AdjListNode**) malloc(num_vertices * sizeof(AdjListNode*));
    for (int i = 0; i < num_vertices; ++i){
        graph->adjList[i] = NULL;
    }
    return graph;
}

void addEdge(Graph* graph, int src, int dest, int weight){ // addEdge 함수, 시간복잡도 = O(1)
    AdjListNode* node = newAdjListNode(dest, weight);
    node->next = graph->adjList[src];
    graph->adjList[src] = node;
}

MinHeapNode* newMinHeapNode(int vertex, int dist){ // 새로운 HeapNode 생성하고 초기화, 시간복잡도 = O(1)
    MinHeapNode* minHeapNode = (MinHeapNode*) malloc(sizeof(MinHeapNode));
    minHeapNode->vertex = vertex;
    minHeapNode->dist = dist;
    minHeapNode->pred = NIL;
    return minHeapNode;
}

MinHeap* createMinHeap(int capacity){ // minHeap을 생성하고 초기화, 시간복잡도 = O(1)
    MinHeap* minHeap = (MinHeap*) malloc(sizeof(MinHeap));
    minHeap->pos = (int*) malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**) malloc(capacity * sizeof(MinHeapNode*));
    return minHeap;
}

void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b){ // 두 포인터가 가리키는 노드 교환
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(MinHeap* minHeap, int idx){ // 문제조건 minHeapify 함수, 시간복잡도 O(log N)
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->dist < minHeap->array[smallest]->dist)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->dist < minHeap->array[smallest]->dist)
        smallest = right;

    if (smallest != idx){
        MinHeapNode* smallestNode = minHeap->array[smallest];
        MinHeapNode* idxNode = minHeap->array[idx];

        minHeap->pos[smallestNode->vertex] = idx;
        minHeap->pos[idxNode->vertex] = smallest;

        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);

        minHeapify(minHeap, smallest);
    }
} // 기존 Max-Heapify 함수의 변형

int isEmpty(MinHeap* minHeap){ // 비어있는 경우 처리
    return minHeap->size == 0;
}

MinHeapNode* extractMin(MinHeap* minHeap){ // 문제조건 extractMin 함수, 시간복잡도 O(log N)
    if (isEmpty(minHeap))
        return NULL;

    MinHeapNode* root = minHeap->array[0];
    MinHeapNode* lastNode = minHeap->array[minHeap->size-1];
    minHeap->array[0] = lastNode;

    minHeap->pos[root->vertex] = minHeap->size-1;
    minHeap->pos[lastNode->vertex] = 0;

    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
} // 우선순위 큐에서 최소값을 추출하고, 힙의 구조를 유지하기 위해 나머지 요소들을 재조정

void decreaseKey(MinHeap* minHeap, int vertex, int dist, int pred){ // 문제조건 decreaseKey 함수, 시간복잡도 O(log N)
    int i = minHeap->pos[vertex];
    minHeap->array[i]->dist = dist;
    minHeap->array[i]->pred = pred;

    while (i && minHeap->array[i]->dist < minHeap->array[(i-1)/2]->dist){
        minHeap->pos[minHeap->array[i]->vertex] = (i - 1) / 2;
        minHeap->pos[minHeap->array[(i-1)/2]->vertex] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i-1)/2]);

        i = (i-1)/2;
    }
} // 키 값을 감소시킨 후, 이를 적절한 위치에 배치하기 위해 힙 구조를 재조정하는 과정

void dijkstra(Graph* graph){ // 위의 함수를 바탕으로 구현되는 다익스트라 알고리즘, 시간복잡도 O((V+E)logV)
    int num_vertices = graph->num_vertices;
    int src = graph->source;

    int* dist = (int*) malloc(num_vertices * sizeof(int));
    int* pred = (int*) malloc(num_vertices * sizeof(int));
    MinHeap* minHeap = createMinHeap(num_vertices);

    for (int v = 0; v < num_vertices; ++v){ // Insertion() 문제조건 관련 반영 부분
        dist[v] = INF;
        pred[v] = NIL;
        minHeap->array[v] = newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
    }

    minHeap->array[src] = newMinHeapNode(src, dist[src]);
    minHeap->pos[src] = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src], NIL);
    minHeap->size = num_vertices;

    while (!isEmpty(minHeap)){
        MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->vertex;

        AdjListNode* pCrawl = graph->adjList[u];
        while (pCrawl != NULL){
            int v = pCrawl->vertex;

            if (minHeap->pos[v] < minHeap->size && dist[u] != INF && pCrawl->weight + dist[u] < dist[v]) {
                dist[v] = dist[u] + pCrawl->weight;
                pred[v] = u;
                decreaseKey(minHeap, v, dist[v], u);
            }
            pCrawl = pCrawl->next;
        }
    }

    FILE *ptr_output = fopen("output.txt", "w");
    for (int i = 0; i < num_vertices; ++i){
        if (i == src)
            fprintf(ptr_output, "%d\t%d\tNIL\n", i, dist[i]);
        else
            fprintf(ptr_output, "%d\t%d\t%d\n", i, dist[i], pred[i]);
    }
    fclose(ptr_output);

    free(minHeap->pos);
    free(minHeap->array);
    free(minHeap);
    free(dist);
    free(pred);
}

int main(int argc, char *argv[]){ // main 함수, 시간복잡도 O((V+E)logV) : 다익스트라 알고리즘과 동일
    if (argc != 3) { // 예외 처리
        printf("Usage: %s input.txt output.txt\n", argv[0]);
        return 1;
    }

    FILE *ptr_input = fopen(argv[1], "r");
    FILE *ptr_output = fopen(argv[2], "w");
    if (ptr_input == NULL || ptr_output == NULL){
        printf("Error opening files.\n");
        return 1;
    }

    int num_vertices, num_edges, source;
    fscanf(ptr_input, "%d\t%d\t%d", &num_vertices, &num_edges, &source);

    Graph* graph = createGraph(num_vertices, num_edges, source);

    for (int i = 0; i < num_edges; ++i){
        int src, dest, weight;
        fscanf(ptr_input, "%d\t%d\t%d", &src, &dest, &weight);
        addEdge(graph, src, dest, weight);
    }

    fclose(ptr_input);

    dijkstra(graph);

    for (int i = 0; i < num_vertices; ++i){
        AdjListNode* pCrawl = graph->adjList[i];
        while (pCrawl){
            AdjListNode* temp = pCrawl;
            pCrawl = pCrawl->next;
            free(temp);
        }
    }
    free(graph->adjList);
    free(graph);

    fclose(ptr_input); // 입출력파일 닫기
    fclose(ptr_output);

    return 0;
}