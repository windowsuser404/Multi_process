#include <stdio.h>
#include<omp.h>
#include <stdlib.h>
#include<limits.h>
#include<stdbool.h>
int numvertex;
//define numvertex 3072627
//#define numvertex 11548846
//#define numvertex 1965206
//#define numvertex 1632803
//#define numvertex 10000
//define numvertex 7
typedef struct Node {
    int data;
    int distance;
    bool change;
} Node;

typedef struct LinkedList {
    Node** array;  // Array of pointers to the nodes
    int size;
} LinkedList;

typedef struct Graph{
	int numVertices;
	struct LinkedList** adjLists;
}Graph;

Node* createNode(int value);

Node** createListofNodes(){	
int i;
Node** nodearray = malloc(sizeof(Node*)*numvertex);
for(i=0;i<numvertex;i++){
	nodearray[i]=createNode(i);
}
return nodearray;
}

LinkedList* createLinkedList() {
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    list->array = NULL;
    list->size = 0;
    return list;
}

Graph* createAGraph(int vertices){
	Graph* graph = malloc(sizeof( Graph));
	graph->numVertices = vertices;
	graph->adjLists = malloc(vertices*sizeof(LinkedList*));
	int i;
	for(i=0;i<vertices;i++)
		graph->adjLists[i]=NULL;
	return graph;
}

Node* createNode(int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = value;
    newNode->distance = INT_MAX;
    newNode->change = true;
    return newNode;
}


void insertAtBeginning(LinkedList* list, int value, Node** nodearray) {
    // Update the array of pointers
    list->array = realloc(list->array, (list->size + 1) * sizeof(Node*));
    list->array[list->size] = nodearray[value];
    list->size++;
}

Node* getNodeByIndex(LinkedList* list, int index) {
    if (index >= 0 && index < list->size) {
        return list->array[index];
    }
    return NULL;
}


void addEdge(Graph* graph, int s, int d,Node** nodearray){
	if(graph->adjLists[s]==NULL){
		graph->adjLists[s] = createLinkedList();
	}
	if(graph->adjLists[d]==NULL){
		graph->adjLists[d] = createLinkedList();
	}
	insertAtBeginning(graph->adjLists[s],d,nodearray);
	insertAtBeginning(graph->adjLists[d],s,nodearray);
}	

void Bellman_ford(Graph* graph, int source,Node** nodearray){
	int i,j,node,k=0;
	bool changed=true;
	nodearray[source]->distance = 0;
	while(changed && k<numvertex )	{
		changed = false;
//	printf("\nexecuting %d time\n",k);
	k++;
//	printf("number thread:%d\n",omp_get_num_threads());
	for(node=0;node<numvertex;node++){
		i = (node+source)%numvertex;
//		printf("\nadjusting%d \n",i);
		if(graph->adjLists[i]!=NULL)
		if(nodearray[i]->change){
	//	printf("enter node %d\n",i);
		nodearray[i]->change = false;
		if((nodearray[i]->distance)!=INT_MAX){
#pragma omp parallel for 
		for(j=0;j<graph->adjLists[i]->size;j++){
//			printf("executed by:%d\n",omp_get_thread_num());
			if(graph->adjLists[i]->array[j]->distance==INT_MAX || (((graph->adjLists[i]->array[j]->distance)-1)>(nodearray[i]->distance))){
//	printf("\nChanging %d distance from %d to %d",(graph->adjLists[i]->array[j]->data),(graph->adjLists[i]->array[j]->distance),(nodearray[i]->distance+1));
				graph->adjLists[i]->array[j]->distance=nodearray[i]->distance+1;
				changed = true;
				graph->adjLists[i]->array[j]->change = true;
			}

			}
		}
		}
			}
		}			
	}	

int main(int argc, char* argv[]) {
	numvertex = atoi(argv[1])+1;
	int source=atoi(argv[2]);
    Node** nodearray;
    nodearray = createListofNodes();
    int i,j;
    int src,dest;
    Graph* graph = createAGraph(numvertex);
    FILE* file;
    file = fopen(argv[3],"r");
    if(file==NULL){
	    printf("Nope");
	    exit(0);
    }
   // printf("Adding edges\n");
    while(fscanf(file, "%d %d", &src, &dest) == 2){
	    addEdge(graph,src,dest,nodearray);
 //   printf("\nadding %d and %d",src,dest);
    }
   // printf("Added all edges\n");
/*    printf("\n");
	for(i=0;i<numvertex;i++){
		printf("%d, has:",i);
		for(j=0;j<graph->adjLists[i]->size;j++){
			printf("%d, ",graph->adjLists[i]->array[j]->data);
		}	
		printf("\n");
	}
	printf("Printed all\n");
	for(i=0;i<numvertex;i++){
		printf("%d has:\n",i);
		for(j=0;j<graph->adjLists[i]->size;j++){
			printf("%d at %d\n",graph->adjLists[i]->array[j]->data,graph->adjLists[i]->array[j]->distance);
			graph->adjLists[i]->array[j]->distance = i;
		}
	}*/
  //  	printf("\nStarting algo\n");
	float time1,time2;
	omp_set_num_threads(4);
//	printf("Max threads be%d",omp_get_max_threads());
//	printf("Before starting algo threads be:%d",omp_get_num_threads());
	time1 = omp_get_wtime();
	Bellman_ford(graph,source,nodearray);
	time2 = omp_get_wtime();
//	printf("\nWorked till here\n");
//	printf("\n");
	/*
	for(i=0;i<numvertex;i++){
		if(nodearray[i]!=NULL){
		printf("Vertex %d at %d\n",i,nodearray[i]->distance);
		}
	}
	*/
	printf("time taken by inner for is:%f\n",time2-time1);
    return 0;
}
