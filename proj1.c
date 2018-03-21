#include <stdio.h>
#include <stdlib.h>



typedef struct edge Edge;

typedef struct vertex {
	int id, d, low;
	char inList;
	Edge *head;
} Vertex;

typedef struct edge {
	Vertex *destVertex;
	Edge *next;
} Edge;


void add(Vertex *graph, int originId, int destId);
void tarjan(Vertex *graph, int size);
void tarjanVisit(Vertex *u, Edge **stack, int *visited);
void push(Edge **stack, Vertex *u);
Edge* pop(Edge **stack);


int main(){
	int V, E, i, originId, destId;


	scanf("%d", &V);
	scanf("%d", &E);

	Vertex *graph = (Vertex*)malloc(sizeof(Vertex) * V);

	for(i = 0; i < V; i++){
		graph[i].id = i + 1;
		graph[i].d = -1;
		graph[i].low = -1;
		graph[i].inList = 0;

		graph[i].head = NULL;
	}

	for(i = 0; i < E; i++){
		scanf("%d %d", &originId, &destId);

		add(graph, originId, destId);
	}

	tarjan(graph, V);
}



void add(Vertex *graph, int originId, int destId){
	Edge *e = (Edge*)malloc(sizeof(Edge));
	e->destVertex = &graph[destId - 1];

	if(graph[originId - 1].head == NULL){
		e->next = NULL;
		graph[originId - 1].head = e;
	}
	else{
		e->next = graph[originId - 1].head;
		graph[originId - 1].head = e;
	}

}


void tarjan(Vertex *graph, int size){
	int visited = 0, i;

	Edge *stack = (Edge*)malloc(sizeof(Edge));
	stack->destVertex = NULL;
	stack->next = NULL;

	for(i = 0; i < size; i++){
		if(graph[i].d == -1){
			tarjanVisit(&graph[i], &stack, &visited);
		}
	}

}


void tarjanVisit(Vertex *u, Edge **stack, int *visited){

	u->d = u->low = *visited;
	*visited += 1;

	push(stack, u);

	Edge *v = u->head;

	while(v != NULL){
		if(v->destVertex->d == -1 || v->destVertex->inList){
			if(v->destVertex->d == -1){
				tarjanVisit(v->destVertex, stack, visited);
			}

			if(u->low >= v->destVertex->low){
				u->low = v->destVertex->low;
			}
		}

		if(!(v->destVertex->inList)){
			printf("%d %d\n", u->id, v->destVertex->id);
		}

		v = v->next;
	}

	if(u->d == u->low){
		printf("\n");
		do{
			v = pop(stack);
			printf("%d\n", v->destVertex->id);
		} while(v->destVertex->id != u->id);
		printf("\n");
	}
}


void push(Edge **stack, Vertex *u){

	u->inList = 1; // Internal function, so no need to verify if its null

	if((*stack)->destVertex == NULL){
		(*stack)->destVertex = u;
	}
	else{
		Edge *new = (Edge*)malloc(sizeof(Edge));
		new->destVertex = u;
		new->next = *stack;
		*stack = new;
	}

}


Edge* pop(Edge **stack){

	Edge *popped = *stack;

	if(popped->next == NULL){
		Edge *new = (Edge*)malloc(sizeof(Edge));
		new->destVertex = NULL;
		new->next = NULL;
		*stack = new;
	}
	else{
		*stack = popped->next;
	}

	popped->destVertex->inList = 0;

	return popped;

}

