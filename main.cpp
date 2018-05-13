//Yanting Zhang 1207766 A2
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <math.h> 

//global variables & intial mode options state
int iterations, grid;
int lightControl = 0;
int switchON = 1;
int light = 1;
int wMode = 0;
int wireMode = 0;
int colourT = 0;
int shadeMode = 0; 

float maxH = 0.0;
float minH = 0.0;
float rotateY=0.0;
float rotateX=0.0;

float eye[] = {300, 300, 300};
float camPos[] = {800, 800, 800};
float maxheight = (float)grid/2;
float origin[] = {0,0,0,1};
float m_dif[] = {0.28, 0.27, 0.11, 1.0};
float m_amb[] = {0.3, 0.6, 0.03, 1.0};
float m_amb1[] ={0.5, 0.5, 0.0, 1.0};
float m_spec[] = {0.4, 0.5, 0.0, 1.0};
float shiny = 17;

//arrays
float height[300][300]; 
float faceN[300][300][3];
float vertexN[300][300][3];


// light0
float amb0[4]  = {0.4,0.6,0.3,1};
float diff0[4] = {1,0,0,1};
float spec0[4] = {0.3,0,1,1};

// light1
float amb1[4]  = {0.5,0.5,0.5, 1};
float diff1[4] = {0.8,0.8,0.8, 1};
float spec1[4] = {0,0,0,1};


float light_pos1[] = {300,-300, -300,1};
float light_pos0[] = {-300,-300, -300,1};


//fo quads..
void calNormal(){

	// calculating face normals
	for (int i = 0; i < grid; i++){
		for (int j = 0; j < grid; j++){	
	
			//calculating normal vector for faces
			float v1x = 0;
			float v1y = height[i][j+1]-height[i][j];
			float v1z = 1;

			float v2x = 1;
			float v2y = height[i+1][j]-height[i][j];
			float v2z = 0;

			float vx = v1y * v2z - v2y * v1z;
			float vy = -(v1x * v2z - v2x * v1z);
			float vz = v1x * v2y - v2x * v1y;

			float length = sqrt(pow(vx,2)+pow(vy,2)+pow(vz,2));

			//store the face normal into the array faceN[][][]
			faceN[i][j][0]=vx/length;
			faceN[i][j][1]=vy/length;
			faceN[i][j][2]=vz/length;
		} 
	}

	// calculating vertex normals
	for (int i = 1; i < grid-1; ++i){
		for (int j = 1; j < grid-1; ++j){	
			// vertex normal's x , y and z
			vertexN[i][j][0]=(faceN[i-1][j-1][0]+faceN[i][j-1][0]+faceN[i-1][j][0]+faceN[i][j][0])/4;
			vertexN[i][j][1]=(faceN[i-1][j-1][1]+faceN[i][j-1][1]+faceN[i-1][j][1]+faceN[i][j][1])/4;
			vertexN[i][j][2]=(faceN[i-1][j-1][2]+faceN[i][j-1][2]+faceN[i-1][j][2]+faceN[i][j][2])/4;
			// normalize the vector
			float lengthN = sqrt(pow(vertexN[i][j][0],2)+pow(vertexN[i][j][1],2)+pow(vertexN[i][j][2],2));
			vertexN[i][j][0] = vertexN[i][j][0]/lengthN;
			vertexN[i][j][1] = vertexN[i][j][1]/lengthN;
			vertexN[i][j][2] = vertexN[i][j][2]/lengthN;
		}
	}
}

//http://www.lighthouse3d.com/opengl/terrain/index.php?fault
void circleAlgorithm(){
	for(int i=0; i<iterations; i++){
		//random center point for circle
		float cx = rand() % grid; 
		float cy = rand() % grid; 

		float cr = grid/8;

		for (int i = 0; i < grid; ++i){
			for (int j = 0; j < grid; ++j){	
				float dis = sqrt(pow((i-cx),2)+pow((j-cy),2));
				float pd = dis/cr;
				if (fabs(pd)<=1.0){	
					height[i][j]+= cr/2 + cos(pd*3.14)*cr/2;
					if (height[i][j] > maxH){
						maxH = height[i][j];
					}
				}
			}
		}
	}
}

//http://www.lighthouse3d.com/opengl/terrain/index.php?fault
void faultAlgorithm(){
	//iterations*4 looks more prettier
	for(int i=0; i<iterations*4; i++){

        float v = rand();
        float a = sinf(v);
        float b = cosf(v);
        float d = sqrtf(2)*grid;
        float c = ((float) rand() / RAND_MAX) * d - d/2.0;

		for ( int i=0; i<grid; i++){
			for ( int j =0 ; j<grid; j++){
				if ( a*i + b*j - c > 0){
					height[i][j] += 4;
				}
				else{
					height[i][j] -= 4;
				}
			}
		}
	}
}

void colorTerrian(int i, int j){

	float percent = (float)grid/50.0;
	if (wMode==2){
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb1);
	}

	else if (height[i][j]<=percent){
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb);
	}

	else if (height[i][j]>= (maxH*0.85)){
		float m_ambo[4] = {0.2,0.2,0.2,1.0};
		float m_difo[4] = {0.5, 0.5, 0.5, 1.0};
		float m_speco[4] = {0.3, 0.3, 0.3, 1.0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_ambo);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_difo);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_speco);
	}

	else{
		float m_ambo[4] = {0.65,0.2,0.1,1.0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_ambo);
	}
}

void drawVertex(int i, int j){
		// asign the normal color according to the height if not drawing wireframe over solid
		if(wMode!=2){ 
		colorTerrian(i,j);
		glColor3f(0.38+0.5*height[i][j]/maxheight,0.33,0.44+0.4*height[i][j]/maxheight);
		}
		// assigned the ambient material feature if drawing wirefram over mesh
		else{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb1);
		glColor3f(1,1,0);
		}
		// don'apply vertext normals if it's flat shading
		if (shadeMode==0){
			colorTerrian(i,j);
			glNormal3f(vertexN[i][j][0],vertexN[i][j][1],vertexN[i][j][2]);
		}
		//define the color into gray if we are drawing lines
		glColor3f(0.5,0.5,0.5);
		glVertex3f(i,height[i][j],j);
}

void drawTerrain(){
	// calculate the normal first
	calNormal();
	for (int i = 0; i < grid-1; ++i){
		for (int j = 0; j < grid-1; ++j){	
			if ( switchON == 0){
				// drawing triangles
				glBegin(GL_TRIANGLES);
				// signing the face normal if using flat shading
				if (shadeMode==1){
					glNormal3f(faceN[i][j][0],faceN[i][j][1],faceN[i][j][2]);
				}
				drawVertex(i,j);
				drawVertex(i+1,j);
				drawVertex(i+1,j+1);
				
				drawVertex(i+1,j+1);
				drawVertex(i,j+1);
				drawVertex(i,j);
				glEnd();
			}

			if ( switchON == 1){
				// drawing Quads
				glBegin(GL_QUADS);
				// signing the face normal if using flat shading
				if (shadeMode==1){
					glNormal3f(faceN[i][j][0],faceN[i][j][1],faceN[i][j][2]);
				}
				drawVertex(i,j);
				drawVertex(i+1,j);
				drawVertex(i+1,j+1);
				drawVertex(i,j+1);
				glEnd();
			}
		}
	}
}

void display(void)
{
	

	//clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2], 0, 0, 0, 0, 1, 0);
	glPushMatrix();
	glRotatef(rotateY,0.0,1.0,0.0);
	glRotatef(rotateX,-0.5,0.0,0.5);
	glTranslatef(-grid/2,0,-grid/2);

	//Mesh
	if (wMode==0){
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		drawTerrain();
	}

	//Wire
	else if (wMode==1){
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		drawTerrain();
	}

	//Mesh + Wire
	else if (wMode==2){	
		wMode=0;
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		drawTerrain();
		wMode=2;
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		drawTerrain();
	}
	glPopMatrix();
	glutSwapBuffers();
	glutPostRedisplay();
}


// function that reset all the values in face&vertex normals array and height map
void reset(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	maxheight = 0;
	for (int i = 0; i < grid; ++i){
		for (int j = 0; j < grid; ++j){	
		faceN[i][j][0]=0;
		faceN[i][j][1]=0;
		faceN[i][j][2]=0;
		vertexN[i][j][0]=0;
		vertexN[i][j][1]=0;
		vertexN[i][j][2]=0;
		height[i][j]=0;
		}
	}
}

void calLight(int lightControl, int coords, int IorD){
	// controlling light 0
	if (lightControl == 0 ){ 

		if (coords == 0){// x axis
			if (IorD ==1&&light_pos0[0]<=grid){ // x++
				light_pos0[0]+=grid/10; 
				glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
				glutPostRedisplay();
			}
			else if(IorD ==0 && light_pos0[0]>=-grid){ // x--
				light_pos0[0]-=grid/10;
				glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
				glutPostRedisplay();
			}
			printf("Light0 x position: %f \n" ,light_pos0[0]);
		}

	    else if(coords == 1){
			if (IorD ==1&&light_pos0[1]<=grid){ // y++
				light_pos0[1]+=grid/10; 
				glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
				glutPostRedisplay();
			}
			else if(IorD ==0 && light_pos0[1]>=-grid){ // y--
				light_pos0[1]-=grid/10;
				glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
				glutPostRedisplay();
			}
			printf("Light0 y position: %f \n" ,light_pos0[1]);
		}

		else if(coords == 2){
		  	if (IorD ==1&&light_pos0[2]<=grid){ // z++
				light_pos0[2]+=grid/10; 
				glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
				glutPostRedisplay();
			}
			else if(IorD ==0 && light_pos0[2]>=-grid){ // z--
				light_pos0[2]-=grid/10;
				glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
				glutPostRedisplay();
			}
			printf("Light0 z position: %f \n" ,light_pos0[2]);
		}
	}

	else if (lightControl==1){
		if (coords == 0){// x axis
			if (IorD ==1&&light_pos1[0]<=grid){ // x++
				light_pos1[0]+=grid/10; 
				glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
				glutPostRedisplay();
			}
			else if(IorD ==0 && light_pos1[0]>=-grid){ // x--
			light_pos1[0]-=grid/10;
			glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
			glutPostRedisplay();
			}
			printf("Light1 x position: %f \n" ,light_pos1[0]);
		}

		else if(coords == 1){
		  	if (IorD ==1&&light_pos1[1]<=grid){ // y++
				light_pos1[1]+=grid/10; 
				glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
				glutPostRedisplay();
			}
			else if(IorD ==0 && light_pos1[1]>=-grid){ // y--
			light_pos1[1]-=grid/10;
			glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
			glutPostRedisplay();
			}
			printf("Light1 y position: %f \n" ,light_pos1[1]);
		}

		else if(coords == 2){
		  	if (IorD ==1&&light_pos1[2]<=grid){ // z++
				light_pos1[2]+=grid/10; 
				glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
				glutPostRedisplay();
			}
			else if(IorD ==0 && light_pos1[2]>=-grid){ // z--
				light_pos1[2]-=grid/10;
				glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
				glutPostRedisplay();
			}
			printf("Light1 z position: %f \n" ,light_pos1[2]);
		}
	}
}


void kbd(unsigned char key, int x, int y)
{	
	// if press Q, escapse the window
	switch(key){
		case 'Q':
		case 'q':
			exit(0);
			break;
		case 'l':
		case 'L':
			if (light==0){	
				glEnable(GL_LIGHTING);
				light=1;
				drawTerrain();
			}
			else{
				glDisable(GL_LIGHTING);
				light=0;
				drawTerrain();
			}
				break;
		case 'w':
		case 'W':
			if (wireMode==0){
				wMode=1;
				wireMode=1;
			}
			else if(wireMode==1){
				wMode=2;
				wireMode=2;
			}
			else if(wireMode==2){
				wMode=0;
				wireMode=0;
			}
			break;
		case 's':
		case 'S':
			// turn on and off the shading flat mode
			if (shadeMode==0){
				shadeMode=1;
				glShadeModel(GL_FLAT);
			}
			else{
				shadeMode=0;
				glShadeModel(GL_SMOOTH);
			}
			break;
		case 'r':
			reset();
			circleAlgorithm();
			display();
			glutPostRedisplay();
			break;
		case 'R':
			reset();
			faultAlgorithm();
			display();
			glutPostRedisplay();
			break;
		case '0': // controlling light0
			lightControl=0;
			break;
		case '1':
			lightControl=1;
			break;
		case 'u':
		case 'U': // light x++ 
			calLight(lightControl,0,1);
			break;
		case 'h': 
		case 'H': // light x--
			calLight(lightControl,0,0);
			break;
		case 'i':
		case 'I': // light z++
			calLight(lightControl,1,1);
			break;
		case 'j': 
		case 'J': // light z--
			calLight(lightControl,1,0);
			break;
		case 'o':
		case 'O': // light y++ 
			calLight(lightControl,2,1);
			break;
		case 'k': 
		case 'K': // light y--
			calLight(lightControl,2,0);
			break;

		case 't':
		case 'T': // draw in tri
			switchON = 0;
			break;

		case 'y':
		case 'Y': // draw in quads
			switchON = 1;
			break; 

	}
}


void special(int key, int x, int y){
	switch(key){
    
    case GLUT_KEY_LEFT:
     	rotateY += 15;
       	break;
    case GLUT_KEY_RIGHT:
    	rotateY -= 15;
        break;
    case GLUT_KEY_DOWN:
    	if(rotateX>=-15){
       	rotateX -= 15;
    }
        break;
    case GLUT_KEY_UP:
        if(rotateX<=15){
        	rotateX += 15;}
        break;
	}
}



void init(){

	/* LIGHTING */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	// setting up lights features
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, diff1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, amb1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_dif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
}

void overveiwDisplay(){

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(-120,-75, 300, 300);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glPointSize(5);//iterate through our height map
    //iterate through our height map
    //draw a point, and depending on the height of the point, set a color
    for (int i = 0; i < grid; i ++){
    	for (int j = 0; j < grid; j++){
    		glBegin(GL_POINTS);
    		glColor3f(height[i][j]/100, height[i][j]/100, height[i][j]/100);
    		glVertex2f(i*0.02, j*0.02);
    		glEnd();
    	}
    }
    glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc, char** argv)
{


	printf("Select grid between 50 to 300: \n");
	scanf("%d", &grid);
	while ((grid<50) | (grid>300)){	// if true then print ask user input again
		printf("Select grid between 50 to 300: \n");
		scanf("%d", &grid);
	}
	printf("Select iterations between 1 to 100 \n");
	scanf("%d", &iterations);


	printf("\n\n");
	printf("           README             \n");
	printf("+----------------------------+\n");
	printf("|  w  : Toogle Wireframe Mode|\n");
	printf("|-----+----------------------|\n");
	printf("|  l  : Toggle Light Mode    |\n");
	printf("|-----+----------------------|\n");
	printf("|  s  : Toggle Flat Shadding |\n");
	printf("|-----+----------------------|\n");
	printf("|  r  : Random using Circle  |\n");
	printf("|-----+----------------------|\n");
	printf("|  R  : Random using Fault   |\n");
	printf("|-----+----------------------|\n");
	printf("|  t  : Draw in Triangle     |\n");
	printf("|----------------------------|\n");
	printf("|  y  : Draw in Quads        |\n");
	printf("|----------------------------|\n");
	printf("|  0  : Switch to Light 0    |\n");
	printf("|----------------------------|\n");
	printf("|  1  : Switch to Light 1    |\n");
	printf("|----------------------------|\n");
	printf("| uio : Control Light Up     |\n");
	printf("|-----+----------------------|\n");
	printf("| hjk : Control Light Down   |\n");
 	printf("|-----+----------------------|\n");
	printf("|  q  : Quit Program         |\n");
	printf("+----------------------------+\n");
	printf("\n");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(300, 300);

	glutCreateWindow("2D View");	//creates the window
	glutDisplayFunc(overveiwDisplay); 
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Terrain");

 	init();

	glutKeyboardFunc(kbd);
	glutDisplayFunc(display);
	glutSpecialFunc(special);




    //set clear colour to white
	glClearColor(0.99, 0.99, 0.99, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,1,1,1000);

	circleAlgorithm();


	glutMainLoop();

	return 0;
}
