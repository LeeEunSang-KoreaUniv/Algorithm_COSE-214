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

	// �߽��� : temp1
	// �߽����� ����� �� : temp2
	// temp1, temp2�� ������� ������ ���Ͽ� �񱳵� �� : temp3
	// ���� ax + by - c = 0 ���� a, b, c ���� : line_a, line_b, line_c
	// temp3�� ���Ͽ� ���� ax + by - c �� : check
	// check�� ���� ����� point�� ���� : p
	// check�� ���� ������ point�� ���� : n
	// extreme_point���� ������ ������ ������ t_line �迭 : lines
	t_point temp1, temp2, temp3;
	int line_a, line_b, line_c;
	int check, p, n;

	t_line* lines = (t_line*)malloc(num_point * sizeof(t_line));
	if (lines == NULL) {
		printf("Error, extreme_point�� �޸��Ҵ� ����! \n");
	}
	else {
		for (int i = 0; i < num_point - 1; i++) {

			// �߽��� ����
			temp1 = points[i];

			for (int j = i + 1; j < num_point; j++) {

				// �߽����� �ٸ� �� ���� ���� ������ ������ ���� 
				temp2 = points[j];
				line_a = temp2.y - temp1.y;
				line_b = temp1.x - temp2.x;
				line_c = (temp1.x * temp2.y) - (temp1.y * temp2.x);

				// p, n ���� 0���� ����
				p = 0;
				n = 0;

				for (int e = 0; e < num_point; e++) {

					// ���õ� ������ �񱳵� �� ����
					temp3 = points[e];
					check = line_a * temp3.x + line_b * temp3.y - line_c;

					// ������ ���� positive �����̸� p++ / negative �����̸� n++ / 0�̸� pass
					if (check > 0) {
						p++;
					}
					else if (check < 0) {
						n++;
					}
				}
				// p Ȥ�� n�� 0 �̶��, points[i]�� points[j]�� extreme point�� �߰��ϰ�,
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
		printf("Error, points�� �޸� �Ҵ� ����!");
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

