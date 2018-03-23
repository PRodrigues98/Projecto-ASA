#include <stdio.h>
#include <stdlib.h>



#define GET_ORIGIN(CON, L) (*(CON + (L) * 2))
#define GET_DEST(CON, L) (*(CON + (L) * 2 + 1))
#define SET_ORIGIN(CON, L, V) {*(CON + (L) * 2) = V;}
#define SET_DEST(CON, L, V) {*(CON + (L) * 2 + 1) = V;}
#define EQUALS(CON, L1, L2) ((GET_ORIGIN(CON, L1) == GET_ORIGIN(CON, L2)) && (GET_DEST(CON, L1) == GET_DEST(CON, L2)))
#define SWITCH(A, B) {int *tmp = A; A = B; B = tmp;}



typedef struct edge Edge;
typedef struct vertex Vertex;
typedef struct connection Connection;

typedef struct vertex {
	int id, d, low, **SCCRoot, inList;
	Edge *head;
} Vertex;

typedef struct edge {
	Vertex *destVertex;
	Edge *next;
} Edge;

typedef struct connection {
	int **originId, destId, children;
	Connection *next;
} Connection;


void add(Vertex *graph, int originId, int destId);
void tarjan(Vertex *graph, int size, Connection **SCCConnects, int *SCCCount);
void tarjanVisit(Vertex *u, Edge **stack, int *visited, Connection **SCCConnects, int *SCCCount);
void push(Edge **stack, Vertex *u);
Edge* pop(Edge **stack);


int main(){
	int V, E, i, originId, destId, SCCCount = 0;

	scanf("%d", &V);
	scanf("%d", &E);

	Vertex *graph = (Vertex*)malloc(sizeof(Vertex) * V);

	/* Inicializacao da Connection List */
	Connection **connects = (Connection**)malloc(sizeof(Connection*));
	*connects = NULL;

	for(i = 0; i < V; i++){
		graph[i].id = i + 1;
		graph[i].d = -1;
		graph[i].low = -1;
		graph[i].inList = 0;
		graph[i].SCCRoot = (int**)malloc(sizeof(int*));
		*(graph[i].SCCRoot) = NULL;

		graph[i].head = NULL;
	}


	for(i = 0; i < E; i++){
		scanf("%d %d", &originId, &destId);

		add(graph, originId, destId);
	}

	tarjan(graph, V, connects, &SCCCount);

	for(i = 0; i < V; i++){
		while(graph[i].head != NULL){
			Edge *tmp = graph[i].head;
			graph[i].head = (graph[i].head)->next;
			free(tmp);
		}
	}

	free(graph);

	printf("%d\n", SCCCount);

	if(connects != NULL){
		int connectCount = (*connects)->children, realConnectCount = 0, j;

		int *connectionsToOrder = (int*)malloc(sizeof(int)*connectCount*2);


		/* 
		   Vectores que guardarao o numero de vezes que cada valor possivel 
		   para o id de um vertice surge nas conexoes encontradas
		   permitindo assim fazer um counting sort
		*/
		int *count = (int*)malloc(sizeof(int)*(V + 1));


		for(i = 0; i < V + 1; i++){
			count[i] = 0;
		}

		for(i = 0; i < connectCount; i++){
			SET_ORIGIN(connectionsToOrder, i, *(*((*connects)->originId)));
			SET_DEST(connectionsToOrder, i, (*connects)->destId);

			count[(*connects)->destId] += 1;

			Connection *garbage = *connects;

			free((*connects)->originId);

			*connects = (*connects)->next;

			free(garbage);
		}

		int *orderedConnections = (int*)malloc(sizeof(int)*connectCount*2);


		for(i = 1; i < V; i++){
			count[i] += count[i - 1];
		}


		/* Ordenar a segunda coluna (vertices destino) */
		for(i = 0; i < connectCount; i++){
			SET_ORIGIN(orderedConnections, count[GET_DEST(connectionsToOrder, i) - 1], GET_ORIGIN(connectionsToOrder, i));
			SET_DEST(orderedConnections, count[GET_DEST(connectionsToOrder, i) - 1], GET_DEST(connectionsToOrder, i));
			count[GET_DEST(connectionsToOrder, i) - 1]++;
		}


		/* Trocar os apontadores para ordenar a primeira coluna */
		SWITCH(orderedConnections, connectionsToOrder);


		for(i = 0; i < V + 1; i++){
			count[i] = 0;
		}


		for(i = 0; i < connectCount; i++){
			count[GET_ORIGIN(connectionsToOrder, i)]++;
		}


		for(i = 1; i < V; i++){
			count[i] += count[i - 1];
		}

		
		/* Ordenar a primeira coluna (vertices origem) */
		for(i = 0; i < connectCount; i++){
			SET_ORIGIN(orderedConnections, count[GET_ORIGIN(connectionsToOrder, i) - 1], GET_ORIGIN(connectionsToOrder, i));
			SET_DEST(orderedConnections, count[GET_ORIGIN(connectionsToOrder, i) - 1], GET_DEST(connectionsToOrder, i));	
			count[GET_ORIGIN(connectionsToOrder, i) - 1]++;
		}

		/* Contagem do numero de conexoes sem duplicados */
		for(i = 0; i < connectCount; i++){
			realConnectCount++;
			for(j = 1; j < connectCount - i && EQUALS(orderedConnections, i, i + j); j++);
			i += j - 1;
		}

		printf("%d\n", realConnectCount);

		/* Print das conexoes encontradas removendo duplicados */
		for(i = 0; i < connectCount; i++){
			printf("%d %d\n", GET_ORIGIN(orderedConnections, i), GET_DEST(orderedConnections, i));
			for(j = 1; j < connectCount - i && EQUALS(orderedConnections, i, i + j); j++);
			i += j - 1;
		}

		free(count);
		free(orderedConnections);
		free(connectionsToOrder);

	}

	return 0;
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


void tarjan(Vertex *graph, int size, Connection **SCCConnects, int *SCCCount){
	int visited = 0, i;

	Edge *stack = (Edge*)malloc(sizeof(Edge));
	stack->destVertex = NULL;
	stack->next = NULL;

	for(i = 0; i < size; i++){
		if(graph[i].d == -1){
			tarjanVisit(&graph[i], &stack, &visited, SCCConnects, SCCCount);
		}
	}

}


void tarjanVisit(Vertex *u, Edge **stack, int *visited, Connection **SCCConnects, int *SCCCount){

	u->d = u->low = *visited;
	*visited += 1;

	push(stack, u);

	Edge *v = u->head;

	while(v != NULL){
		if(v->destVertex->d == -1 || v->destVertex->inList){
			if(v->destVertex->d == -1){
				tarjanVisit(v->destVertex, stack, visited, SCCConnects, SCCCount);
			}

			if(u->low >= v->destVertex->low){
				u->low = v->destVertex->low;
			}
		}

		/* Apenas e possivel que v nao esteja no stack caso seja parte de um SCC */
		if(!(v->destVertex->inList)){
			Connection *newConnect = (Connection*)malloc(sizeof(Connection));

			newConnect->originId = u->SCCRoot;
			newConnect->destId = *(*(v->destVertex->SCCRoot));
			newConnect->children = (*SCCConnects == NULL) ? 1 : (*SCCConnects)->children + 1;
			newConnect->next = *SCCConnects;
			*SCCConnects = newConnect;
		}

		v = v->next;
	}

	if(u->d == u->low){
		*(u->SCCRoot) = (int*)malloc(sizeof(int));
		*(*(u->SCCRoot)) = u->id;
		*SCCCount += 1;

		v = pop(stack);

		while(v->destVertex->id != u->id){
			*(v->destVertex->SCCRoot) = *(u->SCCRoot);

			/* Caso o id deste vertice seja inferior ao considerado root, trocar para o mais pequeno */
			if(v->destVertex->id < *(*(u->SCCRoot))){ 
				*(*(u->SCCRoot)) = v->destVertex->id;
			}

			free(v);

			v = pop(stack);
		}

		free(v);
	}
}


void push(Edge **stack, Vertex *u){

	u->inList = 1; /* Funcao interna, logo nao e necessario verificar se e null */

	Edge *new = (Edge*)malloc(sizeof(Edge));
	new->destVertex = u;

	if(*stack == NULL){
		new->next = NULL;
		*stack = new;
	}
	else{
		new->next = *stack;
		*stack = new;
	}

}


Edge* pop(Edge **stack){

	Edge *popped = *stack;
	
	*stack = popped->next;

	popped->destVertex->inList = 0;

	return popped;

}

