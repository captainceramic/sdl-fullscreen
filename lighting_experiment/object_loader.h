#include <stdbool.h>
#include "cube.h"

// C header-only library containing functions to load up TRIANGLE based OBJ files.
bool loadOBJ(const char* path, Cube* cubePtr) {

  /* I need to assign some memory for the arrays.
     However I don't know how much I will need!
     Therefore, I will find the length of the file     
     https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
  */

  FILE *fp;
  fp = fopen(path, "r");
  if( fp == NULL ) {
    printf("Impossible to open file: %s", path);
    return false;
  }

  fseek(fp, 0L, SEEK_END);
  uint file_size = ftell(fp);
  printf("file is %d bytes long\n", file_size);
  rewind(fp);

  uint max_length = file_size / 8;
  printf("therefore, cannot require more than %d vertices\n", max_length);
  
  /* Next step: allocate memory for the temp arrays
     see: https://www.geeksforgeeks.org/dynamically-allocate-2d-array-c/
  */
  float **temp_vertices = (float **) calloc(max_length, sizeof(float*));
  float **temp_uvs = (float **) calloc(max_length, sizeof(float*));
  float **temp_normals = (float **) calloc(max_length, sizeof(float*));
  for(int i = 0; i<max_length; i++) {
    temp_vertices[i] = (float *) calloc(3, sizeof(float));
    temp_uvs[i] = (float *) calloc(2, sizeof(float));
    temp_normals[i] = (float *) calloc(3, sizeof(float));
  }

  /* array to store the index positions for the output (a set of 5) */
  int *output_indices = (int *) calloc(9*max_length, sizeof(int));

  /* Now we start parsing the file
     I think this bit is from the learn openGL tutorial
  */
  uint vert_ix = 0;
  uint uv_ix = 0;
  uint normal_ix = 0;
  uint output_ix = 0;
  
  while( 1 ) {
    char lineHeader[128];
    /* read the first word of the line */
    int res = fscanf(fp, "%s", lineHeader);
    if (res == EOF)
      break;

    /*  vertices, uvs and normals first */
    if ( strcmp( lineHeader, "v" ) == 0) {

      fscanf(fp, "%f %f %f\n",
	     &temp_vertices[vert_ix][0],
	     &temp_vertices[vert_ix][1],
	     &temp_vertices[vert_ix][2]);

      vert_ix += 1;
      
    } else if ( strcmp( lineHeader, "vt" ) == 0 ){
      fscanf(fp, "%f %f\n",
    	     &temp_uvs[uv_ix][0],
    	     &temp_uvs[uv_ix][1] );
      uv_ix += 1;
    } else if ( strcmp( lineHeader, "vn" ) == 0) {
      fscanf(fp, "%f %f %f\n",
    	     &temp_normals[normal_ix][0],
    	     &temp_normals[normal_ix][1],
    	     &temp_normals[normal_ix][2]);
      normal_ix += 1;
    } else if (strcmp( lineHeader, "f" ) == 0 ) { 

      /* Now the indexes of each point
	 3 sets, vertex, uv, normal  */
      int matches = fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
			   &output_indices[output_ix],
			   &output_indices[output_ix+1],
			   &output_indices[output_ix+2],
			   &output_indices[output_ix+3],
			   &output_indices[output_ix+4],
			   &output_indices[output_ix+5],
			   &output_indices[output_ix+6],
			   &output_indices[output_ix+7],
			   &output_indices[output_ix+8]);
      output_ix += 9;
      if (matches != 9) {
	printf("File can't be read!");
	return false;
      }
    }
  }
  
  printf("file contains: %d vertices, %d uvs and %d normals\n",
	 vert_ix, uv_ix, normal_ix);
  
  uint output_length = output_ix / 9;
  printf("output length must be %d\n", output_length);
  
  /* Now set up the outputs - straightforward arrays */
  float *out_vertices = (float *) calloc(3*3*output_length, sizeof(float));
  float *out_uvs =(float *) calloc(3*2*output_length, sizeof(float));
  float *out_normals = (float *) calloc(3*3*output_length, sizeof(float));
  
  for(int i=0; i<output_length; i++) {

    /* OBJ files index from 1, so need to subtract one from the indices */
    int vertex_ix_1 = output_indices[(9*i)]-1 ;
    int uv_ix_1 = output_indices[(9*i)+1]-1;	
    int normal_ix_1 = output_indices[(9*i)+2]-1;

    int vertex_ix_2 = output_indices[(9*i)+3]-1;
    int uv_ix_2 = output_indices[(9*i)+4]-1;	
    int normal_ix_2 = output_indices[(9*i)+5]-1;
    
    int vertex_ix_3 = output_indices[(9*i)+6]-1;
    int uv_ix_3 = output_indices[(9*i)+7]-1;	
    int normal_ix_3 = output_indices[(9*i)+8]-1;

    /* Now write to the output arrays */
    out_vertices[(9*i)] = temp_vertices[vertex_ix_1][0];
    out_vertices[(9*i)+1] = temp_vertices[vertex_ix_1][1];
    out_vertices[(9*i)+2] = temp_vertices[vertex_ix_1][2];
    out_vertices[(9*i)+3] = temp_vertices[vertex_ix_2][0];
    out_vertices[(9*i)+4] = temp_vertices[vertex_ix_2][1];
    out_vertices[(9*i)+5] = temp_vertices[vertex_ix_2][2];
    out_vertices[(9*i)+6] = temp_vertices[vertex_ix_3][0];
    out_vertices[(9*i)+7] = temp_vertices[vertex_ix_3][1];
    out_vertices[(9*i)+8] = temp_vertices[vertex_ix_3][2];
    
    out_uvs[(6*i)] = temp_vertices[uv_ix_1][0];
    out_uvs[(6*i)+1] = temp_vertices[uv_ix_1][0];
    out_uvs[(6*i)+2] = temp_vertices[uv_ix_2][1];
    out_uvs[(6*i)+3] = temp_vertices[uv_ix_2][1];
    out_uvs[(6*i)+4] = temp_vertices[uv_ix_3][2];
    out_uvs[(6*i)+5] = temp_vertices[uv_ix_3][2];

    out_normals[(9*i)] = temp_normals[normal_ix_1][0];
    out_normals[(9*i)+1] = temp_normals[normal_ix_1][1];
    out_normals[(9*i)+2] = temp_normals[normal_ix_1][2];
    out_normals[(9*i)+3] = temp_normals[normal_ix_2][0];
    out_normals[(9*i)+4] = temp_normals[normal_ix_2][1];
    out_normals[(9*i)+5] = temp_normals[normal_ix_2][2];
    out_normals[(9*i)+6] = temp_normals[normal_ix_3][0];
    out_normals[(9*i)+7] = temp_normals[normal_ix_3][1];
    out_normals[(9*i)+8] = temp_normals[normal_ix_3][2];
  }

  /* Free all our memory */
  for(int i = 0; i<max_length; i++) {
    free(temp_vertices[i]);
    free(temp_normals[i]);
    free(temp_uvs[i]);
  }
  free(output_indices);
  fclose(fp);

  /* Update the output pointers. */
  cubePtr->num_triangles = output_length;

  cubePtr->vertices = out_vertices;
  cubePtr->uvs = out_uvs;
  cubePtr->normals = out_normals;

  return true;
}
