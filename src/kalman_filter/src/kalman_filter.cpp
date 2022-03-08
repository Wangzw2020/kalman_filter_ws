#include <iostream>
#include <fstream>

#include <GL/glut.h>
#include <vector>
#include <string>
#include <fstream>
#include "kalman.h"
#include "tools.h"
#include "target.h"
#include "data.h"



using namespace std;

string target_txt = "/home/wzw/workspace/kalman_filter_ws/src/kalman_filter/src/data/ped0.txt";
string save_txt = "/home/wzw/workspace/kalman_filter_ws/src/kalman_filter/src/data/ped2.txt";

GLsizei winWidth = 1600;
GLsizei winHeight = 900;
float fps = 0;
bool act = false;
Target target;
fstream txt;

int flag = 0;
int data_num = 0;
std::vector<double> data_time;
Data All_measurement_data;

void init();
void loadData();

void display();
void drawTarget();
void drawPed();
void drawCircle(float x, float y, float z, float r, Color color, int slices = 90);

void showInformation();
void drawText(float x, float y, char text[]);
void reshape(int width, int height);
void normalKey(unsigned char key, int xMousePos, int yMousePos);
void update();
void computeFPS();

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Social Force Model");
	init();
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(normalKey);
	glutIdleFunc(update);
	glutMainLoop();
	return 1;
}

void init()				//初始化opengl
{
	GLfloat gnrlAmbient[] = { 0.8F, 0.8F, 0.8F, 1.0 };	//一般光强度
	GLfloat lghtDiffuse[] = { 0.7F, 0.7F, 0.7F, 1.0 };	//物体光强度
	GLfloat lghtPosition[] = { 4.0, -4.0, 4.0, 0.0 };	//灯光位置

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glShadeModel(GL_SMOOTH);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gnrlAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lghtDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lghtPosition);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);

	//读取文件
	loadData();
	
	ofstream new_txt(save_txt,ios_base::out);
	new_txt.close();
	txt.open(save_txt);
	if (!txt)
		cout << "open " << save_txt << " failed!" << endl;
}

void loadData()
{
	ifstream data_file;
	string line;

	data_file.open(target_txt.c_str());
	if(!data_file)
		cout<<"open target file failed!"<<endl;
	while(data_file.good())
	{
		double t,x,y;
		Info info;
		All_measurement_data.setType(0);
		getline(data_file, line);
		if (line.length() == 0)
			break;
		std::stringstream ss(line);

		ss >> t >> x >> y;
		info.time = t;
		info.x = x;
		info.y = y;
		All_measurement_data.addData(info);
		data_time.push_back(t);
	}
	data_file.close();
	
//	target.tracker.init(0.1, 0.0, 0.0, 0.0, 0.0, SIGMA_AX, SIGMA_AY, SIGMA_OX, SIGMA_OY);
	target.tracker.init(0.1, All_measurement_data.getData(0).x + gaussian_noise(0, 0.1), 0.0, All_measurement_data.getData(0).y + gaussian_noise(0, 0.1), 0.0, SIGMA_AX, SIGMA_AY, SIGMA_OX, SIGMA_OY);
	
	target.x = target.tracker.get_state()(0, 0);
	target.y = target.tracker.get_state()(2, 0);
	
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(0.0, 0.0, 15.0,		//相机位置
			  0.0, 0.0, 0.0,		//相机镜头方向对准物体在世界坐标位置
			  0.0, 1.0, 0.0);		//镜头向上方向在世界坐标的方向

	glPushMatrix();
	glScalef(1.0, 1.0, 1.0);
	
	//画图

	drawTarget();
	drawPed();
	
	glPopMatrix();
	showInformation();
	glutSwapBuffers();
}

void drawCircle(float x, float y, float z, float r, Color color, int slices)
{
	float sliceAngle;
	Point current, next;

	glPushMatrix();
	glColor3f(color.r,color.g,color.b);
	glTranslatef(x, y, z); 
	sliceAngle = static_cast<float>(360.0 / slices);
	current.x = r;
	current.y = 0;
	current.z = 0.0f;
	next.z = 0.0f;
	for (float angle = sliceAngle; angle <= 360; angle += sliceAngle) {
		next.x = r * cos(angle * PI / 180);
		next.y = r * sin(angle * PI / 180);

		glBegin(GL_TRIANGLES);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(current.x, current.y, current.z);
			glVertex3f(next.x, next.y, next.z);
		glEnd();

		current = next;
	}
	glPopMatrix();
}


void drawTarget()
{
	Color target_color = fb_Color(1.0, 0.0, 0.0);
	drawCircle(target.x, target.y, 0.0, 0.2, target_color);
}

void drawPed()
{
	Color ped_color = fb_Color(0.0, 0.0, 0.0);
	
	Info ped_i = All_measurement_data.getData(flag);
	drawCircle(ped_i.x, ped_i.y, 0.0, 0.3, ped_color);
}

void showInformation()
{
	Point margin;
	char totalCrowdsStr[5] = "\0", fpsStr[8] = "\0", frctnStr[6] = "\0", totalCarsStr[5] = "\0";
	margin.x = static_cast<float>(-winWidth) / 50;
	margin.y = static_cast<float>(winHeight) / 50 - 0.75F;
	
}

void drawText(float x, float y, char text[])
{
	glDisable(GL_LIGHTING);	
	glDisable(GL_DEPTH_TEST);

	glPushMatrix();
		glTranslatef(x, y, 0.0);
		glScalef(0.0045F, 0.0045F, 0.0);
		glLineWidth(1.4F);

		int idx = 0;
		while (text[idx] != '\0')
			glutStrokeCharacter(GLUT_STROKE_ROMAN, text[idx++]);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

void reshape(int width, int height) {
	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.0, static_cast<GLfloat>(width) / height, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW);

	winWidth = width;  
	winHeight = height;
}

void normalKey(unsigned char key, int xMousePos, int yMousePos) {
	switch (key) {
	case 'a':
		act = (!act) ? true : false;
		break;
	case 'b':
		break;
	case 27:
		exit(0);
		break;
	}
}

void update() {
	int currTime, frameTime;
	static int prevTime;
	static int actTime = 0;
	static int i = 0;
	
	currTime = glutGet(GLUT_ELAPSED_TIME);
	frameTime = currTime - prevTime;
	prevTime = currTime;
	
	if (act) { 
		actTime+=frameTime;
		if(actTime >= i * 100)
		{
			for(int k=0; k<data_time.size(); ++k)
			{
				if(actTime <= data_time[k] * 1000)
				{
					flag = k;
					break;
				}
			}
			
			
			target.tracker.predict();
			target.tracker.update(All_measurement_data.getData(flag).x + gaussian_noise(0, 0.1), All_measurement_data.getData(flag).y + gaussian_noise(0, 0.1));

			target.x = target.tracker.get_state()(0, 0);
			target.y = target.tracker.get_state()(2, 0);
			txt << data_time[flag] << " " << target.x << " " << target.y << " "
				<< target.tracker.get_state()(1, 0) 
				<< target.tracker.get_state()(3, 0) << endl;
			
			++i;
			cout << "step: " << i << endl;
			if (actTime/1000 >= data_time[data_time.size()-1])
			{
				flag = 0;
				i = 0;
				actTime = 0;
			}
		}
		

	}
		
	computeFPS();
	glutPostRedisplay();
	glutIdleFunc(update);
}

void computeFPS() {
	static int frameCount = 0;
	int currTime, frameTime;
	static int prevTime;

	frameCount++;
	currTime = glutGet(GLUT_ELAPSED_TIME); 
	frameTime = currTime - prevTime;

	if (frameTime > 1000) {
		fps = frameCount / (static_cast<float>(frameTime) / 1000);
		prevTime = currTime;
		frameCount = 0;
	}
}
