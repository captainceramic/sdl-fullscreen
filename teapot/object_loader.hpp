// C++ header file containing functions to load up QUAD and TRIANGLE based OBJ files.
bool loadTriangleOBJ(
	     const char* path,
	     std::vector < glm::vec3 > & out_vertices,
	     std::vector < glm::vec2 > & out_uvs,
	     std::vector < glm::vec3 > & out_normals
	     ) {

  // set up temporary variables
  std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
  std::vector< glm::vec3 > temp_vertices;
  std::vector< glm::vec2 > temp_uvs;
  std::vector< glm::vec3 > temp_normals;

  // open up the object file
  FILE * file = fopen(path, "r");
  if( file == NULL ) {
    std::cout << "Impossible to open file!" << std::endl;
    return false;
  }

  while( 1 ) {
    char lineHeader[128];
    // read the first word of the line
    int res = fscanf(file, "%s", lineHeader);

    if (res == EOF)
      break;

    // vertice, uvs and normals first
    if ( strcmp( lineHeader, "v" ) == 0) {
      glm::vec3 vertex;
      fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
      temp_vertices.push_back(vertex);
    } else if ( strcmp( lineHeader, "vt" ) == 0 ){
      glm::vec2 uv;
      fscanf(file, "%f %f\n", &uv.x, &uv.y );
      temp_uvs.push_back(uv);
    } else if ( strcmp( lineHeader, "vn" ) == 0) {
      glm::vec3 normal;
      fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
      temp_normals.push_back(normal);
    } else if (strcmp( lineHeader, "f" ) == 0 ) {
      // Now the indexes of each point
      std::string vertex1, vertex2, vertex3;
      unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

      int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
			   &vertexIndex[0], &uvIndex[0], &normalIndex[0],
			   &vertexIndex[1], &uvIndex[1], &normalIndex[1],
			   &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

      if (matches != 9) {
	std::cout << "File can't be read! line is:\n" << file << std::endl;
	return false;
      }

      // Add the indices of each point to the input vectors
      vertexIndices.push_back(vertexIndex[0]);
      vertexIndices.push_back(vertexIndex[1]);
      vertexIndices.push_back(vertexIndex[2]);

      uvIndices.push_back(uvIndex[0]);
      uvIndices.push_back(uvIndex[1]);
      uvIndices.push_back(uvIndex[2]);

      normalIndices.push_back(normalIndex[0]);
      normalIndices.push_back(normalIndex[1]);
      normalIndices.push_back(normalIndex[2]);
      
    }
  }

  std::cout << "We need to store "
	    << vertexIndices.size()
	    << " vertices." << std::endl;
  
  // Now we replace the indices with the actual values
  for( unsigned int i=0; i<vertexIndices.size(); i++) {
    unsigned int vertexIndex = vertexIndices[i];
    
    // OBJ file indexes from 1 not 0;
    glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
    out_vertices.push_back(vertex);
    
    glm::vec2 uv = temp_uvs[ vertexIndex-1 ];
    out_uvs.push_back(uv);
    
    glm::vec3 normal = temp_normals[ vertexIndex-1];
    out_normals.push_back(normal);
    
  }
  
  return true;
};


bool loadQuadOBJ(
	     const char* path,
	     std::vector < glm::vec3 > & out_vertices,
	     std::vector < glm::vec2 > & out_uvs,
	     std::vector < glm::vec3 > & out_normals
	     ) {

  // set up temporary variables
  std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
  std::vector< glm::vec3 > temp_vertices;
  std::vector< glm::vec2 > temp_uvs;
  std::vector< glm::vec3 > temp_normals;

  // open up the object file
  FILE * file = fopen(path, "r");
  if( file == NULL ) {
    std::cout << "Impossible to open file!" << std::endl;
    return false;
  }

  while( 1 ) {
    char lineHeader[128];
    // read the first word of the line
    int res = fscanf(file, "%s", lineHeader);

    if (res == EOF)
      break;

    // vertice, uvs and normals first
    if ( strcmp( lineHeader, "v" ) == 0) {
      glm::vec3 vertex;
      fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
      temp_vertices.push_back(vertex);
    } else if ( strcmp( lineHeader, "vt" ) == 0 ){
      glm::vec2 uv;
      fscanf(file, "%f %f\n", &uv.x, &uv.y );
      temp_uvs.push_back(uv);
    } else if ( strcmp( lineHeader, "vn" ) == 0) {
      glm::vec3 normal;
      fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
      temp_normals.push_back(normal);
    } else if (strcmp( lineHeader, "f" ) == 0 ) {
      // Now the indexes of each point
      std::string vertex1, vertex2, vertex3;
      unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];

      int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
			   &vertexIndex[0], &uvIndex[0], &normalIndex[0],
			   &vertexIndex[1], &uvIndex[1], &normalIndex[1],
			   &vertexIndex[2], &uvIndex[2], &normalIndex[2],
			   &vertexIndex[3], &uvIndex[3], &normalIndex[3]);

      if (matches != 12) {
	std::cout << "Matches is: " << matches << std::endl;
	std::cout << "File can't be read! line is:\n" << file << std::endl;
	return false;
      }

      // Add the indices of each point to the input vectors
      vertexIndices.push_back(vertexIndex[0]);
      vertexIndices.push_back(vertexIndex[1]);
      vertexIndices.push_back(vertexIndex[2]);
      vertexIndices.push_back(vertexIndex[3]);

      uvIndices.push_back(uvIndex[0]);
      uvIndices.push_back(uvIndex[1]);
      uvIndices.push_back(uvIndex[2]);
      uvIndices.push_back(uvIndex[3]);
      

      normalIndices.push_back(normalIndex[0]);
      normalIndices.push_back(normalIndex[1]);
      normalIndices.push_back(normalIndex[2]);
      normalIndices.push_back(normalIndex[3]);      
      
    }
  }

  std::cout << "We need to store "
	    << vertexIndices.size()
	    << " vertices." << std::endl;
  
  // Now we replace the indices with the actual values
  // But we store them as two triangles rather than a single quad.

  // Loop through quads.
  for( unsigned int i=0; i<(vertexIndices.size() / 4); i++) {

    unsigned int j = i * 4;
    
    unsigned int qIndex1 = vertexIndices[j] - 1;
    unsigned int qIndex2 = vertexIndices[j+1] - 1;
    unsigned int qIndex3 = vertexIndices[j+2] - 1;
    unsigned int qIndex4 = vertexIndices[j+3] - 1;

    // OBJ file indexes from 1 not 0
    glm::vec3 vertex_1 = temp_vertices[ qIndex1 ];
    glm::vec3 vertex_2 = temp_vertices[ qIndex2 ];
    glm::vec3 vertex_3 = temp_vertices[ qIndex3 ];
    glm::vec3 vertex_4 = temp_vertices[ qIndex4 ];

    glm::vec2 uv_1 = temp_uvs[ qIndex1 ];
    glm::vec2 uv_2 = temp_uvs[ qIndex2 ];
    glm::vec2 uv_3 = temp_uvs[ qIndex3 ];
    glm::vec2 uv_4 = temp_uvs[ qIndex4 ];

    glm::vec3 normal_1 = temp_normals[ qIndex1 ];
    glm::vec3 normal_2 = temp_normals[ qIndex2 ];
    glm::vec3 normal_3 = temp_normals[ qIndex3 ];
    glm::vec3 normal_4 = temp_normals[ qIndex4 ];
    
    // Triangle 1
    out_vertices.push_back(vertex_1);
    out_vertices.push_back(vertex_2);
    out_vertices.push_back(vertex_3);

    out_uvs.push_back(uv_1);
    out_uvs.push_back(uv_2);
    out_uvs.push_back(uv_2);

    out_normals.push_back(normal_1);
    out_normals.push_back(normal_2);
    out_normals.push_back(normal_3);
    
    // Triangle 2
    // see: https://stackoverflow.com/questions/23723993/converting-quadriladerals-in-an-obj-file-into-triangles
    out_vertices.push_back(vertex_1);
    out_vertices.push_back(vertex_3);
    out_vertices.push_back(vertex_4);

    out_uvs.push_back(uv_1);
    out_uvs.push_back(uv_3);
    out_uvs.push_back(uv_4);

    out_normals.push_back(normal_1);
    out_normals.push_back(normal_3);
    out_normals.push_back(normal_4);

  }
  
  return true;
};


