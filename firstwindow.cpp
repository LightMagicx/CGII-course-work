#include <GLFW/glfw3.h>
#include "mylib.hpp"
#include "file.hpp"
#include <Eigen/Dense>

using namespace Eigen;



int main(void)
{


    GLFWwindow* window;

    //iniatialize MVP matrix
    initMat(matMVP);

    //define view matrices
    GLfloat Pref[3] = { 0,0,0 };
    GLfloat Camera[3] = { 100,100,100 };
    GLfloat Vup[3] = { 0,1,0 };

    //calculate matrices
    GLfloat* View = viewMat(Pref, Camera, Vup);
    GLfloat* Model = matI();
    GLfloat* Pers = persMat(1, 10, 120, 5);
    //GLfloat* Pers = matI();          

    generateMVP(Pers, View, Model);
    Matrix4f mvp;
    mvp << matMVP[0], matMVP[1], matMVP[2], matMVP[3],
        matMVP[4], matMVP[5], matMVP[6], matMVP[7],
        matMVP[8], matMVP[9], matMVP[10], matMVP[11],
        matMVP[12], matMVP[13], matMVP[14], matMVP[15];

    Matrix4f ivsmvp = mvp.inverse();
    //should inverse VP, not MVP, but it works here
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            ivsMVP[4 * i + j] = ivsmvp(i, j);
        }
        
    }
    
    
    
    //read model file
    Mesh mesh = openFile("Z:\\house.d.txt");

    //calculate model
    vector<GLfloat*> polyTable =setPolygon(mesh);

    vector<vector<GLfloat*>> poly = deployPoly(mesh, polyTable);
    mesh.imptPersMesh(poly);

    backCulling(mesh,poly,Pref,Camera);

    
    bufferSet tex;
    tex.texture = readBMP("Z:\\bliss.bmp");


    //disable backculling
    /*
    for (int i = 0; i < mesh.culling.size(); i++) {
        mesh.culling[i] = true;
    }

    */
    

    //new for lab2
    
    //mesh2
    Mesh mesh2 = openFile("Z:\\bench.d.txt");

    //calculate model
    vector<GLfloat*> polyTable2 = setPolygon(mesh2);
    vector<vector<GLfloat*>> poly2 = deployPoly(mesh2, polyTable2);
    backCulling(mesh2, poly2, Pref, Camera);

    
    vector<vector<GLfloat*>> orgPoly = inversePoly(poly);
    mesh.imptWorldMesh(orgPoly);
    mesh.facetNormal = generateFacetNormal(mesh.worldMehsVertex);
    mesh.polyTableNormal = generatePolyNormal(mesh);
    mesh.generateVAL();
    mesh.generateVertexNormal();


    Ver light(5,-5,3);
    Ver view(100,100,70);

    //new for lab3
    //mesh.facetShading = constShading(light, view, mesh.facetNormal, mesh);
    //mesh.vertexShading = GourandShading(light, view, mesh.vertexNormal, mesh);
    

    

    
    /*
    vector<vector<GLfloat*>> orgPoly2 = inversePoly(poly2);
    mesh2.imptWorldMesh(orgPoly2);
    mesh2.facetNormal = generateFacetNormal(mesh2.worldMehsVertex);
    mesh2.facetShading = constShading(light, view, mesh2.facetNormal, mesh2);
    */
    

    
    //pixel
    //mesh1
    vector<vector<Ver>> pixelizedPoly = pixelize(mesh, poly);
    vector<vector<edgeTable>> MeshList=generateMeshList(mesh, pixelizedPoly);
    bufferSet imgbuffer = polyZBuff(MeshList,mesh);
    imgbuffer = PhongShading(light, view, imgbuffer);

    /*
    //mesh2
    vector<vector<Ver>> pixelizedPoly2 = pixelize(mesh2, poly2);
    vector<vector<edgeTable>> MeshList2 = generateMeshList(mesh2, pixelizedPoly2);
    bufferSet imgbuffer2 = polyZBuff(MeshList2,mesh2);

    //refresh zbuffer with mesh1 and mesh2
    bufferSet zbuffer = updateZBuffer(imgbuffer, imgbuffer2);
    */
    
    
    imgbuffer = applyShadingMap(imgbuffer);
    imgbuffer = textureMapping(imgbuffer, tex);

    
    cout << endl;
    
    

    
    
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1920, 1080, "Computer Graphics Lab3", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        
        drawPixel(imgbuffer.imgBuffer);
        //drawPoly(mesh, poly);
        
        

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        
    }

    glfwTerminate();
    return 0;
}
