#include <stdio.h>
#include <stdlib.h>

#define PEASANT 0x08
#define WOLF	0x04
#define GOAT	0x02
#define CABBAGE	0x01

// 주어진 상태 state에서 농부, 늑대, 염소, 양배추의 상태를 각각 추출하여 p, w, g, c에 저장
// 예) state가 7(0111)일 때, p = 0, w = 1, g = 1, c = 1
static void get_pwgc( int state, int *p, int *w, int *g, int *c){

	*c = state % 2; state /= 2;
	*g = state % 2; state /= 2;
	*w = state % 2; state /= 2;
	*p = state % 2; 

}

// 주어진 상태 state의 이름(마지막 4비트)을 화면에 출력
// 예) state가 7(0111)일 때, "<0111>"을 출력
static void print_statename(int state){

	int p,w,g,c;
	get_pwgc(state, &p, &w, &g, &c);
	printf("<%d%d%d%d> \n", p,w,g,c);

}

// 허용되지 않는 상태인지 검사
// 예) 농부없이 늑대와 염소가 같이 있는 경우 / 농부없이 염소와 양배추가 같이 있는 경우
// return value: 1 허용되지 않는 상태인 경우, 0 허용되는 상태인 경우
static int is_dead_end( int state){

	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);

	if ((w == g && w != p) || (g == c && g != p)){
		return 1;
	}
	else{
		return 0;
	}

}

// state1 상태에서 state2 상태로의 전이 가능성 점검
// 농부 또는 농부와 다른 하나의 아이템이 강 반대편으로 이동할 수 있는 상태만 허용
// 허용되지 않는 상태(dead-end)로의 전이인지 검사
// return value: 1 전이 가능한 경우, 0 전이 불이가능한 경우 
static int is_possible_transition( int state1,	int state2){
	if(is_dead_end(state2) == 1){
		return 0;
	}
	else{
		int p,w,g,c;
		get_pwgc(state1, &p, &w, &g, &c);
		int X = state1 ^ state2;

		// 가능한 이동은 농부 혼자 가거나, 농부가 하나를 데리고 가는 경우
		// X = 1000, 1001, 1010, 1100
		// 이 때, 농부가 하나를 데리고 가려면 state1에서 농부과 그 대상이 같은 곳에 있어야 한다.
		if (X==8 || (X==9 && p==c) || (X==10 && p==g) || (X==12 && p==w)){
			return 1;
		}
		else{
			return 0;
		}
	}
}

// 상태 변경: 농부 이동
// return value : 새로운 상태
static int changeP( int state){
	int p,w,g,c;
	get_pwgc(state, &p, &w, &g, &c);
	p = 1-p;
	return p*8 + w*4 + g*2 + c*1;
}

// 상태 변경: 농부, 늑대 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePW( int state){
	int p,w,g,c;
	get_pwgc(state, &p, &w, &g, &c);
	if (p == w){
		p = 1-p;
		w = 1-w;
		return p*8 + w*4 + g*2 + c*1;
	}
	else{
		return -1;
	}
}

// 상태 변경: 농부, 염소 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePG( int state){
	int p,w,g,c;
	get_pwgc(state, &p, &w, &g, &c);
	if (p == g){
		p = 1-p;
		g = 1-g;
		return p*8 + w*4 + g*2 + c*1;
	}
	else{
		return -1;
	}
}

// 상태 변경: 농부, 양배추 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1 
static int changePC( int state){
	int p,w,g,c;
	get_pwgc(state, &p, &w, &g, &c);
	if (p == c){
		p = 1-p;
		c = 1-c;
		return p*8 + w*4 + g*2 + c*1;
	}
	else{
		return -1;
	}
}

// 주어진 state가 이미 방문한 상태인지 검사
// return value : 1 visited, 0 not visited
static int is_visited( int visited[], int depth, int state){
	for(int i=0; i<depth; i++){
		if(visited[i] == state){
			return 1;
		}
	}
	return 0;
}

// 방문한 경로(상태들)을 차례로 화면에 출력
static void print_path( int visited[], int depth){
	for(int i=0; i<=depth; i++){
		print_statename(visited[i]);
	}
}

// recursive function
static void dfs_main( int initial_state, int goal_state, int depth, int visited[]){

	int current_state = initial_state;
	int Cp,Cw,Cg,Cc;
	get_pwgc(current_state, &Cp, &Cw, &Cg, &Cc);
	printf("Current State is <%d%d%d%d> (Depth %d) \n", Cp,Cw,Cg,Cc,depth);	

	if (current_state == goal_state){
		printf("\n\nGoal State found!! \n");
		print_path(visited, depth);
		printf("\n\n");
		return;
	}

	int P, Pw, Pg, Pc;
	P = changeP(current_state); 
	Pw = changePW(current_state); 
	Pg = changePG(current_state); 
	Pc = changePC(current_state); 

	int p,w,g,c;
	// 농부만 이동하는 경우
	if (P != -1){
		get_pwgc(P, &p, &w, &g, &c);
		if(is_dead_end(P) == 1){ 
			printf("		Next state <%d%d%d%d> is dead-end! \n", p,w,g,c);
		}
		else if(is_visited(visited, depth, P) == 1){
			printf("		Next state <%d%d%d%d> has been visited! \n", p,w,g,c);
		}
		else if(is_possible_transition(current_state, P) == 1){
			visited[depth+1] = P;
			dfs_main(P, goal_state, depth+1, visited);
			printf("Back to <%d%d%d%d> (Depth %d) \n", Cp,Cw,Cg,Cc, depth);
		}
	}
	
	

	if (Pw != -1){
		get_pwgc(Pw, &p, &w, &g, &c);
		if(is_dead_end(Pw) == 1){ 
			printf("		Next state <%d%d%d%d> is dead-end! \n", p,w,g,c);
		}
		else if(is_visited(visited, depth, Pw) == 1){
			printf("		Next state <%d%d%d%d> has been visited! \n", p,w,g,c);
		}
		else if(is_possible_transition(current_state, Pw) == 1){
			visited[depth+1] = Pw;
			dfs_main(Pw, goal_state, depth+1, visited);
			printf("Back to <%d%d%d%d> (Depth %d) \n", Cp,Cw,Cg,Cc, depth);
		}
	}


	
	if (Pg != -1){
		get_pwgc(Pg, &p, &w, &g, &c);
		if(is_dead_end(Pg) == 1){ 
			printf("		Next state <%d%d%d%d> is dead-end! \n", p,w,g,c);
		}
		else if(is_visited(visited, depth, Pg) == 1){
			printf("		Next state <%d%d%d%d> has been visited! \n", p,w,g,c);
		}
		else if(is_possible_transition(current_state, Pg) == 1){
			visited[depth+1] = Pg;
			dfs_main(Pg, goal_state, depth+1, visited);
			printf("Back to <%d%d%d%d> (Depth %d) \n", Cp,Cw,Cg,Cc, depth);
		}
	}


	
	if (Pc != -1){
		get_pwgc(Pc, &p, &w, &g, &c);
		if(is_dead_end(Pc) == 1){ 
			printf("		Next state <%d%d%d%d> is dead-end! \n", p,w,g,c);
		}
		else if(is_visited(visited, depth, Pc) == 1){
			printf("		Next state <%d%d%d%d> has been visited! \n", p,w,g,c);
		}
		else if(is_possible_transition(current_state, Pc) == 1){
			visited[depth+1] = Pc;
			dfs_main(Pc, goal_state, depth+1, visited);
			printf("Back to <%d%d%d%d> (Depth %d) \n", Cp,Cw,Cg,Cc, depth);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// 상태들의 인접 행렬을 구하여 graph에 저장
// 상태간 전이 가능성 점검
// 허용되지 않는 상태인지 점검 
void make_adjacency_matrix( int graph[][16]){
	for(int i=0; i<16; i++){
		for(int j=0; j<16; j++){
			if ((is_dead_end(i) == 0) && (is_possible_transition(i, j) == 1)){
				graph[i][j] = 1;
			}
			else{
				graph[i][j] = 0;
			}
		}
	}
}

// 인접행렬로 표현된 graph를 화면에 출력
void print_graph( int graph[][16], int num){
	for(int i=0; i<16; i++){
		for(int j=0; j<16; j++){
		printf("%d ", graph[i][j]);
		}
		printf("\n");
	}
}

// 주어진 그래프(graph)를 .net 파일로 저장
// pgwc.net 참조
void save_graph( char *filename, int graph[][16], int num){
   
   FILE *fp = fopen(filename, "w");
   int p,w,g,c;
   
   fprintf(fp, "*Vertices %d \n", num);
   for(int i = 0; i < num; i++){
      fprintf(fp, "%d ", i+1);
      get_pwgc(i, &p, &w, &g, &c);
	  fprintf(fp, "\"<%d%d%d%d>\" \n", p,w,g,c);
   }
   
   fprintf(fp, "*Edges \n");
   for (int i = 0; i<8; i++){      //edges 앞에는 state, 옆에는 연결 된 state
      for(int j = 0; j < num; j++){
         if(graph[i][j] == 1){
            fprintf(fp, "%d  %d\n", i+1, j+1);
         }
      }
   } 
}

////////////////////////////////////////////////////////////////////////////////
// 깊이 우선 탐색 (초기 상태 -> 목적 상태)
void depth_first_search( int initial_state, int goal_state)
{
	int depth = 0;
	int visited[16] = {0,}; // 방문한 정점을 저장
	
	dfs_main( initial_state, goal_state, depth, visited); 
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int graph[16][16] = {0, };
	
	// 인접 행렬 만들기
	make_adjacency_matrix( graph);

	// 인접 행렬 출력 (only for debugging)
	// print_graph( graph, 16);
	
	// .net 파일 만들기
	save_graph( "pwgc.net", graph, 16);

	// 깊이 우선 탐색
	depth_first_search( 0, 15); // initial state, goal state
	
	return 0;
}

