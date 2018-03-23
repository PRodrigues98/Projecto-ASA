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
	int id, d, low, inList;
	Edge *head;
} Vertex;

typedef struct edge {
	Vertex *destVertex;
	Edge *next;
} Edge;

typedef struct connection {
	int originId, destId;
	Connection *next;
} Connection;


int *SCCRoot;
int connectCount = 0;
int SCCCount = 0;


void add(Vertex *graph, int originId, int destId);
void tarjan(Vertex *graph, int size, Connection **SCCConnects);
void tarjanVisit(Vertex *u, Edge **stack, int *visited, Connection **SCCConnects);
void push(Edge **stack, Vertex *u);
int pop(Edge **stack);


int main(){
	int V, E, i, originId, destId;

	scanf("%d", &V);
	scanf("%d", &E);

	Vertex *graph = (Vertex*)malloc(sizeof(Vertex) * V);

	SCCRoot = (int*)malloc(sizeof(int) * (V + 1));

	/* 
	   Vectores que guardarao o numero de vezes que cada valor possivel 
	   para o id de um vertice surge nas conexoes encontradas
	   permitindo assim fazer um counting sort
	*/
	int *countOrigin = (int*)malloc(sizeof(int) * (V + 1));
	int *countDest = (int*)malloc(sizeof(int) * (V + 1));



	/* Inicializacao da Connection List */
	Connection **connects = (Connection**)malloc(sizeof(Connection*));
	*connects = NULL;

	for(i = 0; i < V; i++){
		graph[i].id = i + 1;
		graph[i].d = -1;
		graph[i].low = -1;
		graph[i].inList = 0;

		graph[i].head = NULL;

		countOrigin[i] = 0;
		countDest[i] = 0;
	}


	for(i = 0; i < E; i++){
		scanf("%d %d", &originId, &destId);

		add(graph, originId, destId);
	}

	tarjan(graph, V, connects);

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
		int realConnectCount = 0, j;

		int *connectionsToOrder = (int*)malloc(sizeof(int)*connectCount*2);

		for(i = 0; i < connectCount; i++){

			int o, d;

			if(SCCRoot[(*connects)->originId] < 0){ /* Caso seja o id mais pequeno do SCC nao sendo raiz */
				o = (*connects)->originId;
			}
			else if(SCCRoot[SCCRoot[(*connects)->originId]] == -1 * (*connects)->originId){ /* Caso seja a raiz */
				o = SCCRoot[(*connects)->originId];
			}
			else{ /* Caso nao seja nem o id mais pequeno do SCC nem raiz */
				o = SCCRoot[SCCRoot[(*connects)->originId]];
			}


			if(SCCRoot[(*connects)->destId] < 0){ /* Caso seja o id mais pequeno do SCC nao sendo raiz */
				d = (*connects)->destId;
			}
			else if(SCCRoot[SCCRoot[(*connects)->destId]] == -1 * (*connects)->destId){ /* Caso seja a raiz */
				d = SCCRoot[(*connects)->destId];
			}
			else{ /* Caso nao seja nem o id mais pequeno do SCC nem raiz */
				d = SCCRoot[SCCRoot[(*connects)->destId]];
			}

			SET_ORIGIN(connectionsToOrder, i, o);
			SET_DEST(connectionsToOrder, i, d);

			countOrigin[o]++;
			countDest[d]++;

			Connection *garbage = *connects;

			*connects = (*connects)->next;

			free(garbage);
		}

		free(connects);
		free(SCCRoot);

		int *orderedConnections = (int*)malloc(sizeof(int)*connectCount*2);


		for(i = 1; i < V; i++){
			countOrigin[i] += countOrigin[i - 1];
			countDest[i] += countDest[i - 1];
		}


		/* Ordenar a segunda coluna (vertices destino) */
		for(i = 0; i < connectCount; i++){
			SET_ORIGIN(orderedConnections, countDest[GET_DEST(connectionsToOrder, i) - 1], GET_ORIGIN(connectionsToOrder, i));
			SET_DEST(orderedConnections, countDest[GET_DEST(connectionsToOrder, i) - 1], GET_DEST(connectionsToOrder, i));
			countDest[GET_DEST(connectionsToOrder, i) - 1]++;
		}

		free(countDest);


		/* Trocar os apontadores para ordenar a primeira coluna */
		SWITCH(orderedConnections, connectionsToOrder);


		/* Ordenar a primeira coluna (vertices origem) */
		for(i = 0; i < connectCount; i++){
			SET_ORIGIN(orderedConnections, countOrigin[GET_ORIGIN(connectionsToOrder, i) - 1], GET_ORIGIN(connectionsToOrder, i));
			SET_DEST(orderedConnections, countOrigin[GET_ORIGIN(connectionsToOrder, i) - 1], GET_DEST(connectionsToOrder, i));	
			countOrigin[GET_ORIGIN(connectionsToOrder, i) - 1]++;
		}

		free(countOrigin);
		free(connectionsToOrder);


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

		free(orderedConnections);

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


void tarjan(Vertex *graph, int size, Connection **SCCConnects){
	int visited = 0, i;

	Edge *stack = (Edge*)malloc(sizeof(Edge));
	stack->destVertex = NULL;
	stack->next = NULL;

	for(i = 0; i < size; i++){
		if(graph[i].d == -1){
			tarjanVisit(&graph[i], &stack, &visited, SCCConnects);
		}
	}

	free(stack);

}


void tarjanVisit(Vertex *u, Edge **stack, int *visited, Connection **SCCConnects){

	u->d = u->low = *visited;
	(*visited)++;

	push(stack, u);

	Edge *v = u->head;

	while(v != NULL){
		if(v->destVertex->d == -1 || v->destVertex->inList){
			if(v->destVertex->d == -1){
				tarjanVisit(v->destVertex, stack, visited, SCCConnects);
			}

			if(u->low >= v->destVertex->low){
				u->low = v->destVertex->low;
			}
		}

		/* Apenas e possivel que v nao esteja no stack caso seja parte de um SCC */
		if(!(v->destVertex->inList)){
			Connection *newConnect = (Connection*)malloc(sizeof(Connection));

			newConnect->originId = u->id;
			newConnect->destId = v->destVertex->id;
			newConnect->next = *SCCConnects;
			*SCCConnects = newConnect;

			connectCount++;
		}

		v = v->next;
	}

	if(u->d == u->low){
		SCCRoot[u->id] = u->id;
		SCCCount++;

		int id = pop(stack);

		while(id != u->id){
			
			SCCRoot[id] = u->id;

			/* Caso o id deste vertice seja inferior ao considerado root, trocar para o mais pequeno */
			if(id < SCCRoot[u->id]){
				SCCRoot[u->id] = id;
			}
			
			id = pop(stack);
		}

		if(SCCRoot[u->id] != u->id){
			SCCRoot[SCCRoot[u->id]] *= -1;
		}
	}
}


void push(Edge **stack, Vertex *u){

	u->inList = 1; /* Funcao interna, logo nao e necessario verificar se e null */

	Edge *new = (Edge*)malloc(sizeof(Edge));
	new->destVertex = u;
	new->next = *stack;
	*stack = new;

}


int pop(Edge **stack){

	Edge *popped = *stack;
	
	*stack = popped->next;

	popped->destVertex->inList = 0;

	int id = popped->destVertex->id;

	free(popped);

	return id;
}

