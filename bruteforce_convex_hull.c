#include <stdlib.h> // atoi, rand, malloc, realloc
#include <stdio.h>
#include <time.h> //time

#define RANGE 10000

typedef struct
{
	int x;
	int y;
} t_point;

typedef struct
{
	t_point from;
	t_point to;
} t_line;

////////////////////////////////////////////////////////////////////////////////
void print_header( char *filename)
{
	printf( "#! /usr/bin/env Rscript\n");
	printf( "png(\"%s\", width=700, height=700)\n", filename);
	
	printf( "plot(1:%d, 1:%d, type=\"n\")\n", RANGE, RANGE);
}
////////////////////////////////////////////////////////////////////////////////
void print_footer( void)
{
	printf( "dev.off()\n");
}

////////////////////////////////////////////////////////////////////////////////
/*
#points
points(2247,7459)
points(616,2904)
points(5976,6539)
points(1246,8191)
*/
void print_points(t_point* points, int num_point) {
	printf("\n#points \n");
	for (int i = 0; i < num_point; i++) {
		printf("points(%d, %d) \n", points[i].x, points[i].y);
	}
}

/*
#line segments
segments(7107,2909,7107,2909)
segments(43,8,5,38)
segments(43,8,329,2)
segments(5047,8014,5047,8014)
*/
void print_line_segments(t_line* lines, int num_line) {
	printf("\n#line segments \n");
	for (int i = 0; i < num_line; i++) {
		printf("segments(%d, %d, %d, %d) \n", lines[i].from.x, lines[i].from.y, lines[i].to.x, lines[i].to.y);
	}
}

// [input] points : set of points
// [input] num_point : number of points
// [output] num_line : number of line segments that forms the convex hull
// return value : set of line segments that forms the convex hull
t_line* convex_hull(t_point* points, int num_point, int* num_line) {

	// 중심점 : temp1
	// 중심점과 연결될 점 : temp2
	// temp1, temp2로 만들어진 직선에 대하여 비교될 점 : temp3
	// 직선 ax + by - c = 0 에서 a, b, c 성분 : line_a, line_b, line_c
	// temp3에 대하여 계산된 ax + by - c 값 : check
	// check의 값이 양수인 point의 개수 : p
	// check의 값이 음수인 point의 개수 : n
	// extreme_point으로 선정된 점들을 저장할 t_line 배열 : lines
	t_point temp1, temp2, temp3;
	int line_a, line_b, line_c;
	int check, p, n;

	t_line* lines = (t_line*)malloc(num_point * sizeof(t_line));
	if (lines == NULL) {
		printf("Error, extreme_point에 메모리할당 실패! \n");
	}
	else {
		for (int i = 0; i < num_point - 1; i++) {

			// 중심점 세팅
			temp1 = points[i];

			for (int j = i + 1; j < num_point; j++) {

				// 중심점과 다른 한 점을 이은 직선의 방정식 세팅 
				temp2 = points[j];
				line_a = temp2.y - temp1.y;
				line_b = temp1.x - temp2.x;
				line_c = (temp1.x * temp2.y) - (temp1.y * temp2.x);

				// p, n 값을 0으로 리셋
				p = 0;
				n = 0;

				for (int e = 0; e < num_point; e++) {

					// 세팅된 직선과 비교될 점 세팅
					temp3 = points[e];
					check = line_a * temp3.x + line_b * temp3.y - line_c;

					// 직선에 대해 positive 방향이면 p++ / negative 방향이면 n++ / 0이면 pass
					if (check > 0) {
						p++;
					}
					else if (check < 0) {
						n++;
					}
				}
				// p 혹은 n이 0 이라면, points[i]와 points[j]를 extreme point로 추가하고,
				// num_line -> +1
				if (p == 0 || n == 0) {
					lines[*num_line].from = points[i];
					lines[*num_line].to = points[j];
					(*num_line)++;
				}
			}
		}
		return lines;
	}
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int x, y;
	int num_point; // number of points
	int num_line = 0; // number of lines
	
	if (argc != 2)
	{
		printf( "%s number_of_points\n", argv[0]);
		return 0;
	}

	num_point = atoi( argv[1]);
	if (num_point <= 0)
	{
		printf( "The number of points should be a positive integer!\n");
		return 0;
	}

	t_point *points = (t_point *) malloc( num_point * sizeof( t_point));
	if (points == NULL) {
		printf("Error, points에 메모리 할당 실패!");
		return 0;
	}
	else {
		t_line* lines;

		// making n points

		srand(time(NULL));
		for (int i = 0; i < num_point; i++)
		{
			x = rand() % RANGE + 1; // 1 ~ RANGE random number
			y = rand() % RANGE + 1;

			points[i].x = x;
			points[i].y = y;
		}

		fprintf(stderr, "%d points created!\n", num_point);

		print_header("convex.png");

		print_points( points, num_point);

		lines = convex_hull( points, num_point, &num_line);

		fprintf( stderr, "%d lines created!\n", num_line);

		print_line_segments( lines, num_line);

		print_footer();

		free(points);
		free( lines);

		return 0;
	}
}

