# Ray-Tracing-HeightFields-using-Posix-Threads

  /***************Ray Tracing Algorithm***************/
 /**Authors:Pascal Talom & Thorben Pajunk**/
/***************************************************/

**********************README***********************

1. Import the project into any c++ IDE

2. import the Libraries GL, GLU, glut and pthread

3. Import any bmp image(8, 16 or 24 bit/pixel) into the project folder.
   note: for our test we generated images with the Gimp2.0 image editor
   
4. in 'main.cpp' line 94 under the loadTerrain function:
    -line 23-24: Set Screen Width and Height.
    -line 19: Set the number of threads
    -line 94: specify the heightmap file and path to be rendered.
    
5. Compile code and run(notice it runs much more faster under the release mode).

6. Use keys A=left, D=Right, W=Closer, S=Farder

7.Optional: You can edit the initial position of the heightmap in 
            main.cpp line 255(topleftX, topleftY, topleftZ positions)
