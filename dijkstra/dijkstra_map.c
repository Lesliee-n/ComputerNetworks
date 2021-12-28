//Importing standard and needed packages
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

//Defining constant use varibles
#define size_x 21
#define size_y 21
#define number_of_airports 21
#define airport_name_size 3


//Initalizing functions for use
void dijkstra(int matrix[size_x][size_y], int start_node, char array_places[size_x][airport_name_size]);
void printPath(int path_array[], int j, char array_places[size_x][airport_name_size]);
int countHops(int path_array[], int s, int z);



int main()
{
    //Array of all airports given in "canadampa.txt" file to be used to create a cost matrix to feed into dijkstra function
    char array_places[21][3] = {"YYC", "YXE", "YEG", "YQR", "YWG", "YUL", "YYZ", "YVR", "YYJ", "YOW", "YQB", "YHZ", "YQM", "YQX", "YYT", "YFC",  "YZF", "YFB", "YXX", "YXS", "YXY"};

    //Initalizing cost matrix of distances between airports with the array_places index matching the cost matrix indexes
    int matrix_places[number_of_airports][number_of_airports];
    //Setting all values in the 21 x 21 matrix to be zero
    memset(matrix_places, 0, sizeof matrix_places);


    //Credit and help from https://stackoverflow.com/questions/3501338/c-read-file-line-by-line
    //Creatiing buffer for file reading
    FILE* filePointer;
    char buffer[100];    
    //variables to read in values from file
    char* start;
    char* dest;
    char* value;
    //Adding in file to read
    filePointer = fopen("canadamap.txt", "r");

    //reading file line by line
    while(fgets(buffer, 100, filePointer)) {

        //Assigning first airport code in line to the start variable
        start = strtok(buffer, " ");

        //Assigning second airport code in line to destination variable
        dest = strtok(NULL, " ");
        
        //Assigning third string in line to value variable as this is the distance in travel 
        value = strtok(NULL, " ");
        //Converting the number from string to int for use
        int value_int = atoi(value);

        //Filling in the matrix_places
        //first iterate through array_places
        for (int i=0; i<number_of_airports; i++){
            char array_target1[4];
            //Get the the airport code in position i in the array_places and load into the variable array_target1
            snprintf(array_target1, sizeof array_target1, "%.3s", array_places[i]);

            //if the start variable matches the airport code in position i 
            //then check destination value for match
            if(strcmp(start, array_target1) == 0){
                //first iterate through array_places
                for (int j=0; j<number_of_airports; j++){
                    char array_target2[4];
                    //Get the the airport code in position j in the array_places and load into the variable array_target2
                    snprintf(array_target2, sizeof array_target2, "%.3s", array_places[j]);

                    //if the destination value matches the airport code in position j 
                    //then assign the value_int into the matrix at position [x][j] and [j][x]
                    if(strcmp(dest, array_target2) == 0){
                        matrix_places[i][j] = value_int;
                        matrix_places[j][i] = value_int;
                    }
                }
            }
        }

    }
    //Close file after reading 
    fclose(filePointer);

	//PRINT MATRIX
    //int row, columns;
    //for (row=0; row<number_of_airports; row++){
    //    for(columns=0; columns<number_of_airports; columns++) {
    //        printf("[ %d ]", matrix_places[row][columns]);
    //}
    //    printf("\n");
    //}


	printf("\n-----------------------------------------------------------------------------\n");
    //Call dijkstra function giving it the matrix created, the number 0 to indicate where to find paths from, and the array of places to compute pathes
    //the function will compute the paths and print the CAN1,CAN2,CAN3 paths
	dijkstra(matrix_places, 0, array_places);
	printf("\n-----------------------------------------------------------------------------\n\n");

	return 0;
}


//Function takes in amatrix of values indicating cost of travel between their two x and y coordinates, the source to which calculates the paths from, and an array of locations 
//Credit and help from https://www.youtube.com/watch?v=_lHSawdgXpI
//Credit and help from https://www.youtube.com/watch?v=pVfj6mxhdMw
//Credit and help from https://stackoverflow.com/questions/28998597/how-to-save-shortest-path-in-dijkstra-algorithm
//Credit and help from https://www.tutorialspoint.com/dijkstra-s-algorithm-to-compute-the-shortest-path-through-a-graph

void dijkstra(int matrix[size_x][size_y], int start_node, char array_places[21][3])
{
    //varivle of infinity to which initaialize paths and variables with
    int infinity = 9999;
	
    //Initialize a path array to which will hold the indexes of airport codes from array_places to travel from to get to destination 
	int path_array[number_of_airports];
    //Set the first value to -1 in order to indicate the end of the path
	path_array[0] = -1;

    //Initialize a cost array that will hold the distance traveled from start t destination
    int cost_array[number_of_airports];
    //Set all values in the array to infinity 
	memset(cost_array, infinity, sizeof cost_array);
    //Set the cost of travel from the source YYC to the source YYC to 0
    cost_array[start_node] = 0;

    //Initalize a bool array to keep track of the vertexs that at in the shorest path
    bool status_array[number_of_airports];
    //Set all values in the array to false
	memset(status_array, false, sizeof status_array);

	//Find the vertex position with the minimum distance
    //First interate through the possible airport codes
	for (int c = 0; c< number_of_airports - 1; c++) {

    	int min = infinity;
        int u;
        //Iterate through the vertexs
	    for (int i = 0; i < number_of_airports; i++) {
            //if the vertex is not already in the path so that it is false and the cost value in the cost array is less than or equal to min
            //then update min and remenber the index number into u
		    if (status_array[i] == false && cost_array[i] <= min) {
                min = cost_array[i];
                u = i;
            }
        }

        //Update true for index u so that it is true that the value is in the path to print later
		status_array[u] = true;

        //Update the distance cost in matrix of the vertex i
		for (int v = 0; v < number_of_airports; v++) {
            //if status os i is false and the cost of i is less than or equal to infinity 
            //then update the value in the cost array
			if (!status_array[v] && matrix[u][v] && cost_array[u] + matrix[u][v] < cost_array[v]) {
				path_array[v] = u;
				cost_array[v] = cost_array[u] + matrix[u][v];
			}
        }
	}


    //Pinting final table for the cost, number of hops, and path to destinations CAN1,CAN2,CAN3
	printf("DESTINATION\t\t COST\t\t HOPS\t\t SHORTEST PATH");

	for (int i = 1; i < number_of_airports; i++) {
        char dest_airport[4];
        snprintf(dest_airport, sizeof dest_airport, "%.3s", array_places[i]);
        //for CAN1 the location is Winnipeg Manitoba
        //the closest airport is YWG
        if (strcmp("YWG",dest_airport) ==0) {
            //calling countHops to return the number of hops in the path
            printf("\nYYC to %s\t|\t %d\t|\t %d\t|\t YYC ", dest_airport, cost_array[i], countHops(path_array,i,0));
            //Calling print path to print the aireports between the start to the destination
            printPath(path_array, i, array_places);        
        }
        //for CAN2 the location is Toronto Ontario
        //the closest airport is YYZ
        else if (strcmp("YYZ", dest_airport) ==0) {
            //calling countHops to return the number of hops in the path
            printf("\nYYC to %s\t|\t %d\t|\t %d\t|\t YYC ", dest_airport, cost_array[i], countHops(path_array,i,0));
            //Calling print path to print the aireports between the start to the destination
            printPath(path_array, i, array_places);
        }
        //for CAN1 the location is Saint-Armand Quebec
        //the closest airport is YUL
        else if (strcmp("YUL", dest_airport) ==0) {
            //calling countHops to return the number of hops in the path
            printf("\nYYC to %s\t|\t %d\t|\t %d\t|\t YYC ", dest_airport, cost_array[i], countHops(path_array,i,0));
            //Calling print path to print the aireports between the start to the destination
            printPath(path_array, i, array_places);
        }
	}
}

//Function takes in the array containing the indexes of the airports in a path from start to destination that match the indexs of the array_places from start node to j and array of places for strings to print
void printPath(int path_array[], int j, char array_places[size_x][airport_name_size])
{
	if (path_array[j] == - 1) {
		return;
    }
	printPath(path_array, path_array[j], array_places);

    char airport_code[4];
    //Get the the airport code in position j in the array_places and load into the variable airport_code
    snprintf(airport_code, sizeof airport_code, "%.3s", array_places[j]);
	printf("-> %s ", airport_code);
}

//Function takes in the array containing the indexes of the airports in a path from start to destination that match the indexs of the array_places from start node to s, and the value z as a counter to return
//returns the number of hops in the shortest path
int countHops(int path_array[], int s, int z)
{
    //Increment the z cunter
    z++;
    if (path_array[s] == - 1) {
		return z-1;
    }

	countHops(path_array, path_array[s], z);
}
