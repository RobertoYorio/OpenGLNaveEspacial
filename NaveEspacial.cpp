
#define PROYECTO "Nave Espacial"

#include <iostream>	
#include <codebase.h>
#include <GL\freeglut.h>
#include <sstream> // Biblioteca de manejo de strings
#include <cmath> // Biblioteca matematica de C
#include <vector>

using namespace cb;
using namespace std;

static const int tasaFPS = 40; // tasa que se quiere como maximo
static GLuint tex[25];
//****camaraa****//
static const float z0 = 2;
Vec3 u(1, 0, 0);
Vec3 v(0, 0, 1);
Vec3 w(0, -1, 0);
Vec3 o(0, 0, z0);
Sistema3d sist(u, v, w, o);
static const float camReach = 500;
static float camAngle = 45;
static float razon = 1;

Sistema3d insideSist;
static bool down = false;
static bool zoomed = false;
static bool inside = false;
static GLubyte objeto[1];

static bool jumping = false;

static const int numAst = 150;
int nave = 2;

void deleteAst(int i);
void deleteDust(int i);
void initMenu();
void ViewSphere(float rad, int sectors, int stacks, int repeatX, int repeatY, float offsetX, float offsetY);
void BackgroudSphere(float rad, int sectors, int stacks, int repeat);
float getAlignment(Vec3 initVec);
void drawMalla();

class Asteroid {
public:
	float angularVel;
	float traslaVel;
	Vec3 rotaVec;
	Vec3 trasVec;

	GLuint asteroide;
	Vec3 pos;
	Vec3 tempPos;
	Vec3 rotPos;
	float id; //blue color
	float angRot = 0;
	float timeMaxAst = 0.1;
	float timeAst = 0;
	float astScaleMax = 1.25;
	float astScale = 1;

	GLuint dust;
	Vec3 posDust;
	int dustLayer;
	bool alive;
	float angRotDust = 0;
	float alphaDust = 0.25;
	float timeMaxDust = 0.3;
	float timeDust = 0;
	float dustScaleMax = 3;
	float dustScale = 1;

	Asteroid(float blue = 0.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f, int dl = 5) :
		angularVel(random(0.05, 0.1)),
		traslaVel(random(0.05, 0.1)),
		pos(Vec3(x, y, z)),
		posDust(Vec3(x, y, z)),
		rotaVec(Vec3(random(-1, 1), random(-1, 1), random(-1, 1))),
		trasVec(Vec3(random(-1, 1), random(-1, 1), random(-1, 1))),
		asteroide(glGenLists(1)),
		dust(glGenLists(1)),
		alive(false),
		dustLayer(dl),
		id(blue)
	{
		//	cout << "creating" << endl;
	};

	// este codigo esta modificado a partir de http://www.songho.ca/opengl/gl_sphere.html
	static void AsteroideObject(float rad, int sectors, int stacks) {
		float x, y, z;
		float secStep = 2 * PI / sectors;
		float staStep = PI / stacks;
		float secAngle, staAngle;
		float numIndices = 0;

		vector<float> vertices;
		vector<float>normals;
		vector<float>texCoords;
		float lengthInv = 1.0f / rad;
		float alpha;
		float delta;

#pragma region vertices
		//for sta == 0
		alpha = random(0, 0.125);
		delta = random(0, 0.125);

		x = random(0 - staStep * delta, 0 + staStep * delta);
		y = random(0 - secStep * delta, 0 + secStep * delta);
		z = random(rad - rad * alpha, rad + rad * alpha) *
			sin(random(PI / 2 - staStep * delta, PI / 2 + staStep * delta));

		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(z);

		normals.push_back(x * lengthInv);
		normals.push_back(y * lengthInv);
		normals.push_back(z * lengthInv);

		numIndices += 1;

		//for 1 <= sta <= stacks-1
		for (int sta = 1; sta < stacks; sta++) {
			staAngle = PI / 2 - sta * staStep;

			for (int sec = 0; sec < sectors; sec++)
			{
				secAngle = 1.0 * sec * secStep;

				alpha = random(0, 0.125);
				delta = random(0, 0.125);

				/*
				float radx = random(rad - rad * alpha, rad + rad * alpha);
				float rady = random(rad - rad * alpha, rad + rad * alpha);
				float radz = random(rad - rad * alpha, rad + rad * alpha);
				*/

				x = random(rad - rad * alpha, rad + rad * alpha) *
					cos(random(staAngle - staStep * delta, staAngle + staStep * delta)) *
					cos(random(secAngle - secStep * delta, secAngle + secStep * delta));
				y = random(rad - rad * alpha, rad + rad * alpha) *
					cos(random(staAngle - staStep * delta, staAngle + staStep * delta)) *
					sin(random(secAngle - secStep * delta, secAngle + secAngle * delta));
				z = random(rad - rad * alpha, rad + rad * alpha) *
					sin(random(staAngle - staStep * delta, staAngle + staStep * delta));
				/*
				y = rad * cos(staAngle) * sin(secAngle);             // r * cos(u)
				x = rad * cos(staAngle) * cos(secAngle);             // r * cos(u)
				z = rad * sin(staAngle);
				*/

				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);

				normals.push_back(x * lengthInv);
				normals.push_back(y * lengthInv);
				normals.push_back(z * lengthInv);

				numIndices += 1;
			}
		}

		//for sta == stacks
		alpha = random(0, 0.125);
		delta = random(0, 0.125);

		x = random(0 - staStep * delta, 0 + staStep * delta);
		y = random(0 - secStep * delta, 0 + secStep * delta);
		z = random(rad - rad * alpha, rad + rad * alpha) *
			sin(random(-PI / 2 - staStep * delta, -PI / 2 + staStep * delta));

		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(z);

		normals.push_back(x * lengthInv);
		normals.push_back(y * lengthInv);
		normals.push_back(z * lengthInv);

		numIndices += 1;

#pragma endregion

		vector<int> indices;
#pragma region indices

		indices.push_back(0);
		for (int sec = 1; sec <= sectors + 1; sec++) {
			indices.push_back(((sec - 1) % sectors) + 1);
		}

		for (int sta = 0; sta < stacks - 2; sta++) {
			for (int sec = 0; sec <= sectors; sec++) {
				indices.push_back(1 + (sta * sectors) + (sec % sectors));
				indices.push_back(1 + ((sta + 1) * sectors) + (sec % sectors));
			}
		}

		indices.push_back(numIndices - 1);
		for (int sec = 1; sec <= sectors + 1; sec++) {
			indices.push_back(((stacks - 2) * sectors) + ((sec - 1) % sectors) + 1);
		}
#pragma endregion

		int i;
#pragma region draw
		glPushMatrix();
		glBegin(GL_QUAD_STRIP);
		for (i = 1; i <= sectors + 1; i++) {
			glTexCoord2f(i, 0);
			glNormal3f(
				normals[3 * indices[0] + 0],
				normals[3 * indices[0] + 1],
				normals[3 * indices[0] + 2]
			);
			glVertex3f(
				vertices[3 * indices[0] + 0],
				vertices[3 * indices[0] + 1],
				vertices[3 * indices[0] + 2]
			);
			glNormal3f(
				normals[3 * indices[i] + 0],
				normals[3 * indices[i] + 1],
				normals[3 * indices[i] + 2]
			);
			glTexCoord2f(i, 1);
			glVertex3f(
				vertices[3 * indices[i] + 0],
				vertices[3 * indices[i] + 1],
				vertices[3 * indices[i] + 2]
			);

		}
		glEnd();

		i = sectors + 2;
		while (i < indices.size() - 1 - (sectors + 1)) {
			glBegin(GL_QUAD_STRIP);
			for (int sec = 0; sec <= sectors; sec++) {
				glTexCoord2f(sec, 0);
				glNormal3f(
					normals[3 * indices[i] + 0],
					normals[3 * indices[i] + 1],
					normals[3 * indices[i] + 2]
				);
				glVertex3f(
					vertices[3 * indices[i] + 0],
					vertices[3 * indices[i] + 1],
					vertices[3 * indices[i] + 2]
				);
				glTexCoord2f(sec, 1);
				i += 1;
				glNormal3f(
					normals[3 * indices[i] + 0],
					normals[3 * indices[i] + 1],
					normals[3 * indices[i] + 2]
				);
				glVertex3f(
					vertices[3 * indices[i] + 0],
					vertices[3 * indices[i] + 1],
					vertices[3 * indices[i] + 2]
				);
				i += 1;
			}
			glEnd();
		}

		glBegin(GL_QUAD_STRIP);
		glNormal3f(
			normals[3 * indices[indices.size() - 1 - sectors - 1] + 0],
			normals[3 * indices[indices.size() - 1 - sectors - 1] + 1],
			normals[3 * indices[indices.size() - 1 - sectors - 1] + 2]
		);
		for (int i = indices.size() - 1 - sectors - 1; i < indices.size() - 1; i++) {
			glTexCoord2f(i, 0);
			glVertex3f(
				vertices[3 * indices[i + 1] + 0],
				vertices[3 * indices[i + 1] + 1],
				vertices[3 * indices[i + 1] + 2]
			);
			glTexCoord2f(i, 1);
			if (i == indices.size() - 1 - sectors - 1) {
			}
			glNormal3f(
				normals[3 * indices[indices.size() - 1 - sectors - 1] + 0],
				normals[3 * indices[indices.size() - 1 - sectors - 1] + 1],
				normals[3 * indices[indices.size() - 1 - sectors - 1] + 2]
			);
			glVertex3f(
				vertices[3 * indices[indices.size() - 1 - sectors - 1] + 0],
				vertices[3 * indices[indices.size() - 1 - sectors - 1] + 1],
				vertices[3 * indices[indices.size() - 1 - sectors - 1] + 2]
			);

		}
		glEnd();
		glPopMatrix();
#pragma endregion

		/*
		cout << "------------------" << numIndices << "------------------" << endl;
		cout << "=============" << endl;
		for (int i = 0; i < indices.size(); i++) {
			cout << indices[i] << endl;
		}

		cout << "=============" << endl;
		for (int i = 0; i < vertices.size(); i+= 3) {
			cout << "(" << vertices[i + 0] << ", " << vertices[i + 1]
				<< ", " << vertices[i + 2] << ")\n" << endl;
		}
		*/

	}

	void create() {
		Vec3 sca(random(0.5, 3), random(0.5, 3), random(0.5, 3));

#pragma region asteroid
		glNewList(asteroide, GL_COMPILE);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();

		float tempTexture = random(1, 5);
		glBindTexture(GL_TEXTURE_2D, tex[(int)tempTexture]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		rotPos = (Vec3(random(0, 180), random(0, 180), random(0, 180)));

		glRotatef(rotPos.x, 1, 0, 0);
		glRotatef(rotPos.y, 0, 1, 0);
		glRotatef(rotPos.z, 0, 0, 1);
		glScalef(sca.x, sca.y, sca.z);
		glColor3f(0, 0, id);
		AsteroideObject(4, 8, 8);

		glPopMatrix();
		glPopAttrib();
		glEndList();
#pragma endregion

#pragma region dust
		glNewList(dust, GL_COMPILE);
		glPushAttrib(GL_ALL_ATTRIB_BITS);


		for (int i = 0; i < dustLayer; i++) {
			glPushMatrix();
			//glBindTexture(GL_TEXTURE_2D, tex[(int)random(9, 13)]);
			glBindTexture(GL_TEXTURE_2D, tex[9]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glRotatef(rotPos.x + random(-10, 10), 1, 0, 0);
			glRotatef(rotPos.y + random(-10, 10), 0, 1, 0);
			glRotatef(rotPos.z + random(-10, 10), 0, 0, 1);
			glScalef(sca.x, sca.y, sca.z);
			AsteroideObject(random(0.5, 5), 8, 8);
			glPopMatrix();
		}

		glPopAttrib();
		glEndList();
#pragma endregion

	};

	void drawAst() {
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();

		GLfloat Dm_TV1[] = { 0  ,0  ,1  ,1 };
		GLfloat Sm_TV1[] = { 0.1,0.1,0.1,1 };
		GLfloat s_TV1 = 3;
		glMaterialfv(GL_BACK, GL_DIFFUSE, Dm_TV1);
		glMaterialfv(GL_BACK, GL_SPECULAR, Sm_TV1);
		glMaterialf(GL_BACK, GL_SHININESS, s_TV1);

		glTranslatef(pos.x, pos.y, pos.z);
		glRotatef(angRot, rotaVec.x, rotaVec.y, rotaVec.z);
		glScalef(astScale, astScale, astScale);
		glCallList(asteroide);
		glPopMatrix();
		glPopAttrib();
		angRot += angularVel;
		pos = pos + trasVec * traslaVel;
	};

	void drawDust() {
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();

		GLfloat Dm_TV[] = { 0.5,0.5,0.5,alphaDust };
		GLfloat Sm_TV[] = { 0.1,0.1,0.1,alphaDust };
		GLfloat s_TV = 2;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, Dm_TV);
		glMaterialfv(GL_FRONT, GL_SPECULAR, Sm_TV);
		glMaterialf(GL_FRONT, GL_SHININESS, s_TV);

		glTranslatef(posDust.x, posDust.y, posDust.z);
		glRotatef(angRot, rotaVec.x, rotaVec.y, rotaVec.z);

		glPushMatrix();
		glTranslatef(random(-0.25, 0.25), random(-0.25, 0.25), random(-0.25, 0.25));
		glScalef(dustScale, dustScale, dustScale);
		glCallList(dust);
		glPopMatrix();

		glPopMatrix();
		glPopAttrib();
	}

	void detect() {
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		glRotatef(angRot, rotaVec.x, rotaVec.y, rotaVec.z);
		glCallList(asteroide);
		glPopMatrix();
	};

	void changePos(Vec3 newPos) {
		pos = newPos;
	}

	void destroy(Vec3 newPos) {
		posDust = pos;
		tempPos = newPos;
		alive = true;
	}
};

class AsteroidField {
public:
	Vec3 fieldDim;
	Vec3 origin;
	Asteroid ast[numAst];

	AsteroidField(Vec3 fd, Vec3 o) :
		fieldDim(fd),
		origin(o)
	{
	};

	void createField() {
		for (int i = 0; i < numAst; i++) {
			ast[i] = Asteroid(
				1.0 * i / numAst,
				random(origin.x - (fieldDim.x) / 2.0, origin.x + (fieldDim.x) / 2.0),
				random(origin.y - (fieldDim.y) / 2.0, origin.y + (fieldDim.y) / 2.0),
				random(origin.z - (fieldDim.z) / 2.0, origin.z + (fieldDim.z) / 2.0)
			);
			ast[i].create();
		}
	}

	void drawAst() {
		for (int i = 0; i < numAst; i++) {
			Vec3 astPos = ast[i].pos;
			if ((astPos - sist.geto()).normalize().dot(sist.getw() * -1) > 0.80) {
				ast[i].drawAst();
			}
			if (!isInside(astPos) && !jumping) {
				Vec3 dir = (origin - astPos);
				Vec3 newPos = astPos + (dir + dir) * .9;
				ast[i].changePos(newPos);
			}
		}
	}

	void drawDust() {
		for (int i = 0; i < numAst; i++) {
			if (ast[i].alive) {
				ast[i].drawDust();
			}
		}
	}

	void detect() {
		for (int i = 0; i < numAst; i++) {
			ast[i].detect();
		}
	}

	bool isInside(Vec3 obj) {
		if (obj.x < origin.x - (fieldDim.x) / 2.0) { return false; }
		else if (obj.x > origin.x + (fieldDim.x) / 2.0) { return false; }
		else if (obj.y < origin.y - (fieldDim.y) / 2.0) { return false; }
		else if (obj.y > origin.y + (fieldDim.y) / 2.0) { return false; }
		else if (obj.z < origin.z - (fieldDim.z) / 2.0) { return false; }
		else if (obj.z > origin.z + (fieldDim.z) / 2.0) { return false; }
		else { return true; }
	}

	void destroyAsteroid(float id) {
		for (int i = 0; i < numAst; i++) {
			if (ast[i].id > id - (1.0 / (numAst * 2.1)) && ast[i].id < id + (1.0 / (numAst * 2.1))) {
				Vec3 astPos = ast[i].pos;
				Vec3 dir = (origin - astPos);
				Vec3 newPos = astPos + (dir + dir);
				ast[i].destroy(newPos);
				glutTimerFunc(1000 / tasaFPS, deleteAst, i);
				glutTimerFunc(1000 / tasaFPS, deleteDust, i);
			}
		}
	}

	void changeFieldPos(Vec3 newPos) {
		origin = newPos;
	}

	void teletransport(Vec3 newPos) {
		origin = newPos;
		//for (int i = 0; i < numAst; i++) {
		//	ast[i].changePos(ast[i].pos + newPos);
		//}
		//createField();
	}
};

class OrbitObject {
public:
	Vec3 pos;
	bool orbit;
	GLuint id;

	float rotx;
	float roty;
	float rotz;
	float incx;
	float incy;
	float incz;

	float alignment;
	float obScaleBig;
	float obScaleSmall;
	float size;
	int texture;
	float fact;

	OrbitObject(Vec3 in, bool b, float scaBig, float scaSmall, int t, float inx, float iny, float inz, float si, float f) :
		pos(in), orbit(b), id(glGenLists(1)), rotx(0), roty(0), rotz(0), fact(f),
		incx(inx), incy(iny), incz(inz), alignment(0), obScaleBig(scaBig* f), obScaleSmall(scaSmall), texture(t), size(si)
	{
		//cout << "create" << endl;
	}

	virtual void create() {
		glNewList(id, GL_COMPILE);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();
		//cout << "created: " << texture << endl;
		glBindTexture(GL_TEXTURE_2D, tex[texture]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glRotatef(180, 1, 0, 0);
		BackgroudSphere(size, 50, 50, 1);

		glPopMatrix();
		glPopAttrib();
		glEndList();
	}

	virtual void draw() {
		rotx += incx;
		roty += incy;
		rotz += incz;
		if (orbit) {
			o = sist.geto();
			alignment = getAlignment(pos);
			//alignment = 1;
			float scaleSmall = obScaleSmall * alignment * alignment * alignment * alignment;

			glPushMatrix();
			glTranslatef(o.x, o.y, o.z);
			glPushMatrix();
			glTranslatef(pos.x, pos.y, pos.z);
			glRotatef(rotz, 0, 0, 1);
			glRotatef(roty, 0, 1, 0);
			glRotatef(rotx, 1, 0, 0);
			glScalef(
				scaleSmall,
				scaleSmall,
				scaleSmall
			);
			glCallList(id);
			glPopMatrix();
			glPopMatrix();
		}
		else {
			glPushMatrix();
			glRotatef(rotz, 0, 0, 1);
			glRotatef(roty, 0, 1, 0);
			glRotatef(rotx, 1, 0, 0);
			glScalef(obScaleSmall, obScaleSmall, obScaleSmall);
			glCallList(id);
			glPopMatrix();
		}
	}

	void drawMoonSmall() {
		rotx += incx;
		roty += incy;
		rotz += incz;
		glPushMatrix();
		glRotatef(rotz, 0, 0, 1);
		glRotatef(roty, 0, 1, 0);
		glRotatef(rotx, 1, 0, 0);
		glCallList(id);
		glPopMatrix();
	}

	void drawMoonBig() {
		rotx += incx;
		roty += incy;
		rotz += incz;
		glPushMatrix();
		glRotatef(rotz, 0, 0, 1);
		glRotatef(roty, 0, 1, 0);
		glRotatef(rotx, 1, 0, 0);
		glCallList(id);
		glPopMatrix();
	}

	void drawEarthSmall() {
		rotx += incx;
		roty += incy;
		rotz += incz;
		glPushMatrix();
		glRotatef(rotz, 0, 0, 1);
		glRotatef(sin(roty), 0, 1, 0);
		glRotatef(cos(roty), 1, 0, 0);
		glCallList(id);
		glPopMatrix();
	}

	void drawEarthBig() {
		rotx += incx;
		roty += incy;
		rotz += incz;
		glPushMatrix();
		glRotatef(rotz, 0, 0, 1);
		glRotatef(sin(roty), 0, 1, 0);
		glRotatef(cos(roty), 1, 0, 0);
		glCallList(id);
		glPopMatrix();
	}
};

static AsteroidField astField(Vec3(camReach * 0.7, camReach * 0.7, camReach * 0.7), o);
static vector<OrbitObject*> orb;

class Shots {
public:
	GLuint shots[4];
	Vec3 pos[4];
	Vec3 dir[4];
	Vec3 lightPos;
	Vec3 lightDir;
	Vec3 target;
	float targetId;
	float speed;
	int alive;

	Shots(float sp = 1.0) :
		speed(sp),
		alive(false)
	{
		cout << "created " << numAst << endl;
	}

	void create(int num, Vec3 t, float tarId) {
		nave = num;
		target = t;
		targetId = tarId;
		glEnable(GL_LIGHT3);
		if (inside) {
			lightPos = insideSist.geto();
		}
		else {
			lightPos = sist.geto();
		}
		lightDir = (target - lightPos).normalize();
		if (nave == 4) {
			Vec3 tempo;
			Vec3 tempu;
			Vec3 tempv;
			if (inside) {
				tempo = insideSist.geto();
				tempu = insideSist.getu();//x
				tempv = insideSist.getv();//y
			}
			else {
				tempo = sist.geto();
				tempu = sist.getu();//x
				tempv = sist.getv();//y
			}
			pos[0] = tempo - tempu + (tempv * 0.5);
			pos[1] = tempo + tempu + (tempv * 0.5);
			pos[2] = tempo + tempu - (tempv * 0.5);
			pos[3] = tempo - tempu - (tempv * 0.5);
		}
		else if (nave == 3) {
			Vec3 tempo;
			Vec3 tempu;
			Vec3 tempv;
			if (inside) {
				tempo = insideSist.geto();
				tempu = insideSist.getu();//x
				tempv = insideSist.getv();//y
			}
			else {
				tempo = sist.geto();
				tempu = sist.getu();//x
				tempv = sist.getv();//y
			}
			pos[0] = tempo + (tempv * 1);
			pos[1] = tempo - (tempu * 1) - (tempv * 1);
			pos[2] = tempo + (tempu * 1) - (tempv * 1);
		}
		else if (nave == 2) {
			Vec3 tempo;
			Vec3 tempu;
			Vec3 tempv;
			if (inside) {
				tempo = insideSist.geto();
				tempu = insideSist.getu();//x
				tempv = insideSist.getv();//y
			}
			else {
				tempo = sist.geto();
				tempu = sist.getu();//x
				tempv = sist.getv();//y
			}
			pos[0] = tempo - (tempu * 0.5) - tempv;
			pos[1] = tempo + (tempu * 0.5) - tempv;
		}

		for (int i = 0; i < nave; i++) {
			Vec3 tempTar = target - pos[i];
			dir[i] = tempTar.normalize();
		}

		for (int i = 0; i < nave; i++) {

			shots[i] = glGenLists(1);
			glNewList(shots[i], GL_COMPILE);
			glPushAttrib(GL_ALL_ATTRIB_BITS);

			GLfloat Em[] = { 0.5,0,0,1.0 };
			GLfloat Dm_TV[] = { 0.5,0,0,1.0 };
			GLfloat Sm_TV[] = { 0.5,0,0,1.0 };
			GLfloat s_TV = 5;
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Em);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Dm_TV);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Sm_TV);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, s_TV);
			glPushMatrix();

			glLineWidth(5);
			glBegin(GL_LINES);
			glVertex3f(0, 0, 0);
			glVertex3f((dir[i].x * 5), (dir[i].y * 5), (dir[i].z * 5));
			glEnd();

			glPopMatrix();
			glPopAttrib();
			glEndList();
		}
		alive = true;
	}

	void draw() {
		GLfloat PL[] = { lightPos.x,lightPos.y, lightPos.z, 1 };
		glLightfv(GL_LIGHT3, GL_POSITION, PL);
		lightPos = lightPos + (lightDir * speed);
		Vec3 tempo;
		if (inside) {
			tempo = insideSist.geto();
		}
		else {
			tempo = sist.geto();
		}
		if (abs(lightPos.x) > abs(target.x) - (abs(lightDir.x) * speed) && abs(lightPos.x) < abs(target.x) + (abs(lightDir.x) * speed) &&
			abs(lightPos.y) > abs(target.y) - (abs(lightDir.y) * speed) && abs(lightPos.y) < abs(target.y) + (abs(lightDir.y) * speed) &&
			abs(lightPos.z) > abs(target.z) - (abs(lightDir.z) * speed) && abs(lightPos.z) < abs(target.z) + (abs(lightDir.z) * speed)
			) {
			glDisable(GL_LIGHT3);
			alive = false;
			if (targetId != 1) {
				astField.destroyAsteroid(targetId);
			}
			//cout << "destroyed" << endl;
		}
		else {
			for (int i = 0; i < nave; i++) {
				glPushMatrix();
				glTranslatef(pos[i].x, pos[i].y, pos[i].z);
				glCallList(shots[i]);
				glPopMatrix();
				pos[i] = pos[i] + (dir[i] * speed);
			}
		}
	}
};

static int screenWidth = 800;
static int screenHeight = 600;

static const int DIM_ESPACIO = 50;
static const int NUM_X = 100;
static const int NUM_Y = 100;

static GLuint malla, fondo, cabina1, cabina2, cabina3, cabina1Inside, cabina2Inside, cabina3Inside;

static Vec3 posMalla = Vec3(0, 0, 0);


//----------------lights----------------//
static bool lights = true;
static bool parked = true;
static float cutoff = 15;

//-----------------anim-----------------//
static const float incVelocidadNave = 0.5f;
static const float factor = 0.1f;
static float velocidadNave = 0;
static float maxVelocidadNave = 20;

static const float pxToDe = 0.0001;
static float windCenterX;
static float windCenterY;
static float mousePosX;
static float mousePosY;
static float rollAngle = 90;

static int planetSelected = -1;

static bool map = false;
static bool movimiento = false;

static float xrot = 0;
static float zrot = 0;

static float alignment;
static float maxAlignment;


//****asteroids****//
static Shots shots(maxVelocidadNave);

static float insideRotx = 0;
static float insideRoty = 0;

static float tempx = 0;
static float tempy = 0;
static float tempz = 0;

double log_a_to_base_b(double a, double b) {
	return log(a) / log(b);
}

void zoom(int i) {
	float incStep = log_a_to_base_b(camAngle + 5, (1000.0 / tasaFPS));//log_{1000/tasaFPS}(45
	float dec = 45 / (2000.0 / tasaFPS);
	static float inc = incStep;
	//cout << camAngle << " " << inc << " " << dec << endl;
	if (i == 0) {
		if (camAngle < 1) {
			camAngle = 1;
			glutTimerFunc(1000 / tasaFPS, zoom, 1);
		}
		else {
			camAngle -= inc;
			inc *= incStep;
			glutTimerFunc(1000 / tasaFPS, zoom, 0);
		}
	}
	else if (i == 1) {
		orb[planetSelected]->orbit = false;
		sist.seto(orb[planetSelected]->pos * -1);
		astField.teletransport(orb[planetSelected]->pos * -1);
		astField.createField();
		glutTimerFunc(1000, zoom, 2);
	}
	else if (i == 2) {
		camAngle += dec;
		if (camAngle > 45) {
			camAngle = 45;
			jumping = false;
		}
		else {
			glutTimerFunc(1000 / tasaFPS, zoom, 2);
		}
	}
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camAngle, razon, 1, 1000);
	glutPostRedisplay();
}

void zoomACasa(int i) {
	float incStep = log_a_to_base_b(camAngle + 5, (1000.0 / tasaFPS));//log_{1000/tasaFPS}(45
	float dec = 45 / (2000.0 / tasaFPS);
	static float inc = incStep;
	//cout << camAngle << " " << inc << " " << dec << endl;
	if (i == 0) {
		if (camAngle < 1) {
			camAngle = 1;
			glutTimerFunc(1000 / tasaFPS, zoomACasa, 1);
		}
		else {
			camAngle -= inc;
			inc *= incStep;
			glutTimerFunc(1000 / tasaFPS, zoomACasa, 0);
		}
	}
	else if (i == 1) {
		orb[planetSelected]->orbit = true;
		Vec3 tempW = (sist.getw() * -1);
		posMalla = sist.geto() + tempW * camReach;
		planetSelected = -1;
		glutTimerFunc(250, zoomACasa, 2);
	}
	else if (i == 2) {
		camAngle += dec;
		if (camAngle > 45) {
			camAngle = 45;
			jumping = false;
		}
		else {
			glutTimerFunc(1000 / tasaFPS, zoomACasa, 2);
		}
	}
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camAngle, razon, 1, 1000);
	glutPostRedisplay();
}

void aterizar(int i) {
	float incz = 20.0 / tasaFPS;
	float incv = maxVelocidadNave / (0.15 * tasaFPS);

	if (i == 0) {
		//cout << "0 " << velocidadNave << " " << 0 << endl;

		if (velocidadNave < 0) {
			velocidadNave = 0;
			glutTimerFunc(1000 / tasaFPS, aterizar, 1);
		}
		else {
			velocidadNave -= incv;
			glutTimerFunc(1000 / tasaFPS, aterizar, 0);
		}
	}
	else {
		//cout << "1 " << sist.geto().z << " " << incz << endl;
		if (sist.geto().z < posMalla.z + 2) {
			sist.seto(Vec3(sist.geto().x, sist.geto().y, posMalla.z + 2));
			movimiento = false;
		}
		else {
			sist.seto(sist.geto() - Vec3(0, 0, incz));
			glutTimerFunc(1000 / tasaFPS, aterizar, 1);
			glutPostRedisplay();
		}
	}
	glutPostRedisplay();
}

void despegar(int i) {

	float incStep = log_a_to_base_b(21, (1000.0 / tasaFPS));
	float incv = maxVelocidadNave / (0.75 * tasaFPS);
	static float incz = incStep;
	if (i == 0) {
		if (sist.geto().z > posMalla.z + 15) {
			glutTimerFunc(1000 / tasaFPS, despegar, 1);

		}
		else {
			sist.seto(sist.geto() + Vec3(0, 0, incz));
			incz *= incStep;
			glutTimerFunc(1000 / tasaFPS, despegar, 0);

		}
	}
	else {
		if (velocidadNave > maxVelocidadNave) {
			velocidadNave = 20;
			incz = incStep;
			movimiento = false;
		}
		else {
			velocidadNave += incv;
			glutTimerFunc(1000 / tasaFPS, despegar, 1);
		}
	}
	glutPostRedisplay();
}

void deleteDust(int i) {
	static float inc = 1.0 / tasaFPS;
	static float alphaInc = 0.25 / (tasaFPS * astField.ast[i].timeMaxDust);
	static float scaleInc = (astField.ast[i].dustScaleMax - 1) / (tasaFPS * astField.ast[i].timeMaxDust);

	if (astField.ast[i].timeDust >= astField.ast[i].timeMaxDust) {
		astField.ast[i].alive = false;
		astField.ast[i].timeDust = 0;
		astField.ast[i].alphaDust = 0.5;
		astField.ast[i].dustScale = 0.25;
	}
	else {
		astField.ast[i].timeDust += inc;
		astField.ast[i].alphaDust -= alphaInc;
		astField.ast[i].dustScale += scaleInc;
		glutTimerFunc(1000 / tasaFPS, deleteDust, i);
	}
	glutPostRedisplay();

}

void deleteAst(int i) {
	static float inc = 1.0 / tasaFPS;
	static float scaleInc = (astField.ast[i].astScaleMax - 1) / (tasaFPS * astField.ast[i].timeMaxAst);


	if (astField.ast[i].timeAst >= astField.ast[i].timeMaxAst) {
		astField.ast[i].timeAst = 0;
		astField.ast[i].astScale = 1;
		astField.ast[i].changePos(astField.ast[i].tempPos);
	}
	else {
		astField.ast[i].timeAst += inc;
		astField.ast[i].astScale += scaleInc;
		glutTimerFunc(1000 / tasaFPS, deleteAst, i);
	}
	glutPostRedisplay();

}

void onPassiveMotion(int x, int y) {
	mousePosX = x;
	mousePosY = y;
	glutPostRedisplay();

}

void apuntarNave(int k) {
	static float rotY = 0;
	float safeMarginX = windCenterX * 0.075;
	float safeMarginY = windCenterY * 0.075;
	float changeX = (windCenterX - mousePosX);
	float changeY = (windCenterY - mousePosY);
	float rotXDegrees = (changeX - safeMarginX) * pxToDe;
	float rotYDegrees = (changeY - safeMarginY) * pxToDe;
	if (!jumping) {

		if (inside) {
			Vec3 tempU = insideSist.getu();
			Vec3 tempV = insideSist.getv();
			if (changeX >= -safeMarginX && changeX <= safeMarginX) {}
			else {
				if (changeX < 0 && insideRotx > -110) {
					insideSist.rotar(rotXDegrees, Vec3(0, 0, 1));
					insideRotx += deg(rotXDegrees);
				}
				else if (changeX > 0 && insideRotx < 110) {
					insideSist.rotar(rotXDegrees, Vec3(0, 0, 1));
					insideRotx += deg(rotXDegrees);
				}
				//cout << tempW.x << " " << tempW.y << " " << tempW.z << endl;
			}
			if (changeY >= -safeMarginY && changeY <= safeMarginY) {}
			else {
				if (changeY < 0 && insideRoty > -35) {
					insideSist.rotar(rotYDegrees, tempU);
					insideRoty += deg(rotYDegrees);
				}
				else if (changeY > 0 && insideRoty < 35) {
					insideSist.rotar(rotYDegrees, tempU);
					insideRoty += deg(rotYDegrees);
				}
			}
			//cout << insideRotx << " " << insideRoty << endl;
		}
		else {
			Vec3 tempU = sist.getu();
			Vec3 tempV = sist.getv();
			Vec3 tempW = sist.getw();
			if (changeX >= -safeMarginX && changeX <= safeMarginX) { rollAngle = 90; }
			else {
				sist.rotar(rotXDegrees, Vec3(0, 0, 1));
				xrot += deg(rotXDegrees);
				float per = 0;
				if (changeX < 0) {
					per = (-changeX - safeMarginX) / (windCenterX - safeMarginX);
				}
				else {
					per = (1 - (changeX - safeMarginX)) / (windCenterX - safeMarginX);
				}
				rollAngle = 90 - 20 * per;
				//cout << tempW.x << " " << tempW.y << " " << tempW.z << endl;
			}
			if (changeY >= -safeMarginY && changeY <= safeMarginY) {}
			else {
				if (changeY < 0 && tempW.z < 0.90) {
					sist.rotar(rotYDegrees, tempU);
					zrot += deg(rotYDegrees);
				}
				else if (changeY > 0 && tempW.z > -0.90) {
					sist.rotar(rotYDegrees, tempU);
					zrot += deg(rotYDegrees);
				}
				//cout << tempV.x << " " << tempV.y << " " << tempV.z << endl;
			}
		}
	}

	glutTimerFunc(1000 / tasaFPS, apuntarNave, 0);
	glutPostRedisplay();
}

void desplazarNave(int m) {
	static int t = 0;
	t++;
	Vec3 aux = (sist.geto() + (sist.getw() * -1)) - sist.geto();
	Vec3 temp = aux * velocidadNave * factor;
	sist.seto(sist.geto() + temp);
	insideSist.seto(sist.geto() + temp);
	glutTimerFunc(1000 / tasaFPS, desplazarNave, 0);
	glutPostRedisplay();
}

void moveFoward() {
	float aux = velocidadNave + incVelocidadNave;
	if (aux < maxVelocidadNave) {
		velocidadNave += incVelocidadNave;
	}
	else {
		velocidadNave = maxVelocidadNave;
	}

	//cout << "front:	" << velocidadNave << endl;
	glutPostRedisplay();
}

void moveBackward() {
	float aux = velocidadNave - incVelocidadNave;
	if (0 < velocidadNave) {
		velocidadNave -= incVelocidadNave;
	}
	else {
		velocidadNave = 0;
	}
	//cout << "back: 	" << velocidadNave << endl;
	glutPostRedisplay();

}

void loadTexture() {
	glGenTextures(25, tex);

	char im[50] = "./Imagenes/metalFloorTile.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[0]);
	loadImageFile(im);

	char im1[50] = "./Imagenes/asteroid1.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[1]);
	loadImageFile(im1);

	char im2[50] = "./Imagenes/asteroid2.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[2]);
	loadImageFile(im2);

	char im3[50] = "./Imagenes/asteroid3.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[3]);
	loadImageFile(im3);

	char im4[50] = "./Imagenes/asteroid4.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[4]);
	loadImageFile(im4);

	char im5[50] = "./Imagenes/space.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[5]);
	loadImageFile(im5);

	char im6[50] = "./Imagenes/cabina1.png";
	glBindTexture(GL_TEXTURE_2D, tex[6]);
	loadImageFile(im6);

	char im7[50] = "./Imagenes/cabina2.png";
	glBindTexture(GL_TEXTURE_2D, tex[7]);
	loadImageFile(im7);

	char im8[50] = "./Imagenes/cabina3.png";
	glBindTexture(GL_TEXTURE_2D, tex[8]);
	loadImageFile(im8);

	char im9[50] = "./Imagenes/dust.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[9]);
	loadImageFile(im9);

	char im10[50] = "./Imagenes/PEarth.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[10]);
	loadImageFile(im10);

	char im11[50] = "./Imagenes/PJupiter.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[11]);
	loadImageFile(im11);

	char im12[50] = "./Imagenes/PMars.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[12]);
	loadImageFile(im12);

	char im13[50] = "./Imagenes/PMercury.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[13]);
	loadImageFile(im13);

	char im14[50] = "./Imagenes/PNeptune.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[14]);
	loadImageFile(im14);

	char im15[50] = "./Imagenes/PUranus.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[15]);
	loadImageFile(im15);

	char im16[50] = "./Imagenes/PVenusAtmos.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[16]);
	loadImageFile(im16);

	char im17[50] = "./Imagenes/PSaturn.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[17]);
	loadImageFile(im17);

	char im18[50] = "./Imagenes/PSun.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[18]);
	loadImageFile(im18);

	char im19[50] = "./Imagenes/h.jpg";
	glBindTexture(GL_TEXTURE_2D, tex[19]);
	loadImageFile(im19);
}

float getAlignment(Vec3 initVec) {
	float res = initVec.normalize().dot((sist.getw() * -1));
	if (res < 0.65) {
		res = 0.65;
	}
	else {
		res *= -1;
	}
	return res;
}

void drawMalla() {
	glPushMatrix();
	glTranslatef(posMalla.x, posMalla.y, posMalla.z);
	glCallList(malla);
	glPopMatrix();
}

void checkPos() {
	Vec3 auxO = sist.geto();
	//cout << auxO.z << " " << posMalla.z << endl;
	if (auxO.x > posMalla.x - (DIM_ESPACIO / 2) * 0.9 &&
		auxO.x < posMalla.x + (DIM_ESPACIO / 2) * 0.9 &&
		auxO.y > posMalla.y - (DIM_ESPACIO / 2) * 0.9 &&
		auxO.y < posMalla.y + (DIM_ESPACIO / 2) * 0.9 &&
		auxO.z >= posMalla.z + 2 &&
		auxO.z < posMalla.z + 30) {
		movimiento = true;
		if (parked) {
			glutTimerFunc(1000 / tasaFPS, despegar, 0);
		}
		else {
			glutTimerFunc(1000 / tasaFPS, aterizar, 0);
		}
		parked = !parked;
		//cout << "inside " << endl;
	}
	else {
		cout << "acercate a la zona de aterizaje" << endl;
	}
}

// este codigo esta modificado a partir de http://www.songho.ca/opengl/gl_sphere.html
void BackgroudSphere(float rad, int sectors, int stacks, int repeat) {
	float x, y, z;
	float secStep = 2 * PI / sectors;
	float staStep = PI / stacks;
	float secAngle, staAngle;
	float numIndices = 0;

	vector<float> vertices;
	vector<float>normals;
	vector<float>texCoords;
	float lengthInv = 1.0f / rad;

#pragma region vertices
	//for sta == 0
	x = 0;
	y = 0;
	z = rad * sin(PI / 2);

	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);

	normals.push_back(x * lengthInv);
	normals.push_back(y * lengthInv);
	normals.push_back(z * lengthInv);

	numIndices += 1;

	//for 1 <= sta <= stacks-1
	for (int sta = 1; sta < stacks; sta++) {
		staAngle = PI / 2 - sta * staStep;
		z = rad * sin(staAngle);

		for (int sec = 0; sec < sectors; sec++)
		{
			secAngle = 1.0 * sec * secStep;

			y = rad * cos(staAngle) * sin(secAngle);
			x = rad * cos(staAngle) * cos(secAngle);

			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			normals.push_back(x * lengthInv);
			normals.push_back(y * lengthInv);
			normals.push_back(z * lengthInv);

			numIndices += 1;
		}
	}

	//for sta == stacks
	x = 0;
	y = 0;
	z = rad * sin(-PI / 2);

	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);

	normals.push_back(x * lengthInv);
	normals.push_back(y * lengthInv);
	normals.push_back(z * lengthInv);

	numIndices += 1;

#pragma endregion

	vector<int> indices;
#pragma region indices

	indices.push_back(0);
	for (int sec = 1; sec <= sectors + 1; sec++) {
		indices.push_back(((sec - 1) % sectors) + 1);
	}

	for (int sta = 0; sta < stacks - 2; sta++) {
		for (int sec = 0; sec <= sectors; sec++) {
			indices.push_back(1 + (sta * sectors) + (sec % sectors));
			indices.push_back(1 + ((sta + 1) * sectors) + (sec % sectors));
		}
	}

	indices.push_back(numIndices - 1);
	for (int sec = 1; sec <= sectors + 1; sec++) {
		indices.push_back(((stacks - 2) * sectors) + ((sec - 1) % sectors) + 1);
	}
#pragma endregion

	int i;
	float curStaStep = staStep;
#pragma region draw
	glPushMatrix();
	glBegin(GL_QUAD_STRIP);

	for (i = 1; i <= sectors + 1; i++) {
		if (i != sectors) {
			glNormal3f(
				normals[3 * indices[i] + 0],
				normals[3 * indices[i] + 1],
				normals[3 * indices[i] + 2]
			);
		}
		glTexCoord2f(repeat * (i - 1) * 1.0 / sectors, repeat * curStaStep / PI);
		glVertex3f(
			vertices[3 * indices[i] + 0],
			vertices[3 * indices[i] + 1],
			vertices[3 * indices[i] + 2]
		);
		if (i == 1) {
			glNormal3f(
				normals[3 * indices[0] + 0],
				normals[3 * indices[0] + 1],
				normals[3 * indices[0] + 2]
			);
		}
		glTexCoord2f(repeat * (i - 1) * 1.0 / sectors, repeat * 0);
		glVertex3f(
			vertices[3 * indices[0] + 0],
			vertices[3 * indices[0] + 1],
			vertices[3 * indices[0] + 2]
		);
	}
	glEnd();

	i = sectors + 2;
	while (i < indices.size() - 1 - (sectors + 1)) {
		glBegin(GL_QUAD_STRIP);
		for (int sec = 0; sec <= sectors; sec++) {
			i += 1;
			glTexCoord2f(repeat * sec * 1.0 / sectors, repeat * (curStaStep + staStep) / PI);
			if (sec != sectors) {
				glNormal3f(
					normals[3 * indices[i] + 0],
					normals[3 * indices[i] + 1],
					normals[3 * indices[i] + 2]
				);
			}
			glVertex3f(
				vertices[3 * indices[i] + 0],
				vertices[3 * indices[i] + 1],
				vertices[3 * indices[i] + 2]
			);
			i -= 1;
			glTexCoord2f(repeat * sec * 1.0 / sectors, repeat * curStaStep / PI);
			glVertex3f(
				vertices[3 * indices[i] + 0],
				vertices[3 * indices[i] + 1],
				vertices[3 * indices[i] + 2]
			);
			i += 2;
		}
		curStaStep += staStep;
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glNormal3f(
		normals[3 * indices[indices.size() - 1 - sectors - 1] + 0],
		normals[3 * indices[indices.size() - 1 - sectors - 1] + 1],
		normals[3 * indices[indices.size() - 1 - sectors - 1] + 2]
	);
	for (int i = indices.size() - 1 - sectors - 1, sec = 0; i < indices.size() - 1; i++, sec++) {
		glTexCoord2f(repeat * sec * 1.0 / sectors, repeat * 1);
		glVertex3f(
			vertices[3 * indices[indices.size() - 1 - sectors - 1] + 0],
			vertices[3 * indices[indices.size() - 1 - sectors - 1] + 1],
			vertices[3 * indices[indices.size() - 1 - sectors - 1] + 2]
		);
		glTexCoord2f(repeat * sec * 1.0 / sectors, repeat * ((stacks - 1) * 1.0 / stacks));
		glVertex3f(
			vertices[3 * indices[i + 1] + 0],
			vertices[3 * indices[i + 1] + 1],
			vertices[3 * indices[i + 1] + 2]
		);
	}
	glEnd();
	glPopMatrix();
#pragma endregion

	/*
	cout << numIndices << endl;
	cout << "=============" << endl;
	for (int i = 0; i < indices.size(); i++) {
		cout << indices[i] << endl;
	}

	cout << "=============" << endl;
	for (int i = 0; i < vertices.size(); i+= 3) {
		cout << "(" << vertices[i + 0] << ", " << vertices[i + 1]
			<< ", " << vertices[i + 2] << ")\n" << endl;
	}
	*/

}

void ViewSphere(float rad, int sectors, int stacks, int repeatX, int repeatY, float offsetX, float offsetY) {
	float x, y, z;
	float secStep = 2 * PI / sectors;
	float staStep = PI / stacks;
	float secAngle, staAngle;
	float numIndices = 0;

	vector<float> vertices;
	vector<float>normals;
	vector<float>texCoords;
	float lengthInv = 1.0f / rad;

#pragma region vertices
	//for sta == 0
	x = 0;
	y = 0;
	z = rad * sin(PI / 2);

	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);

	normals.push_back(x * lengthInv);
	normals.push_back(y * lengthInv);
	normals.push_back(z * lengthInv);

	numIndices += 1;

	//for 1 <= sta <= stacks-1
	for (int sta = 1; sta < stacks; sta++) {
		staAngle = PI / 2 - sta * staStep;
		z = rad * sin(staAngle);

		for (int sec = 0; sec < sectors; sec++)
		{
			secAngle = 1.0 * sec * secStep;

			y = rad * cos(staAngle) * sin(secAngle);
			x = rad * cos(staAngle) * cos(secAngle);

			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			normals.push_back(x * lengthInv);
			normals.push_back(y * lengthInv);
			normals.push_back(z * lengthInv);

			numIndices += 1;
		}
	}

	//for sta == stacks
	x = 0;
	y = 0;
	z = rad * sin(-PI / 2);

	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);

	normals.push_back(x * lengthInv);
	normals.push_back(y * lengthInv);
	normals.push_back(z * lengthInv);

	numIndices += 1;

#pragma endregion

	vector<int> indices;
#pragma region indices

	indices.push_back(0);
	for (int sec = 1; sec <= sectors + 1; sec++) {
		indices.push_back(((sec - 1) % sectors) + 1);
	}

	for (int sta = 0; sta < stacks - 2; sta++) {
		for (int sec = 0; sec <= sectors; sec++) {
			indices.push_back(1 + (sta * sectors) + (sec % sectors));
			indices.push_back(1 + ((sta + 1) * sectors) + (sec % sectors));
		}
	}

	indices.push_back(numIndices - 1);
	for (int sec = 1; sec <= sectors + 1; sec++) {
		indices.push_back(((stacks - 2) * sectors) + ((sec - 1) % sectors) + 1);
	}
#pragma endregion

	int i;
	float curStaStep = staStep;
#pragma region draw
	glPushMatrix();
	glBegin(GL_QUAD_STRIP);

	for (i = 1; i <= sectors + 1; i++) {
		if (i != sectors) {
			glNormal3f(
				normals[3 * indices[i] + 0],
				normals[3 * indices[i] + 1],
				normals[3 * indices[i] + 2]
			);
		}
		glTexCoord2f(offsetX + (repeatX * ((i - 1) * 1.0 / sectors)), offsetY + (repeatY * (curStaStep / PI)));
		glVertex3f(
			vertices[3 * indices[i] + 0],
			vertices[3 * indices[i] + 1],
			vertices[3 * indices[i] + 2]
		);
		if (i == 1) {
			glNormal3f(
				normals[3 * indices[0] + 0],
				normals[3 * indices[0] + 1],
				normals[3 * indices[0] + 2]
			);
		}
		glTexCoord2f(offsetX + (repeatX * ((i - 1) * 1.0 / sectors)), offsetY + (repeatY * 0));
		glVertex3f(
			vertices[3 * indices[0] + 0],
			vertices[3 * indices[0] + 1],
			vertices[3 * indices[0] + 2]
		);
	}
	glEnd();

	i = sectors + 2;
	while (i < indices.size() - 1 - (sectors + 1)) {
		glBegin(GL_QUAD_STRIP);
		for (int sec = 0; sec <= sectors; sec++) {
			i += 1;
			glTexCoord2f(offsetX + (repeatX * (sec * 1.0 / sectors)), offsetY + (repeatY * ((curStaStep + staStep) / PI)));
			if (sec != sectors) {
				glNormal3f(
					normals[3 * indices[i] + 0],
					normals[3 * indices[i] + 1],
					normals[3 * indices[i] + 2]
				);
			}
			glVertex3f(
				vertices[3 * indices[i] + 0],
				vertices[3 * indices[i] + 1],
				vertices[3 * indices[i] + 2]
			);
			i -= 1;
			glTexCoord2f(offsetX + (repeatX * (sec * 1.0 / sectors)), offsetY + (repeatY * (curStaStep / PI)));
			glVertex3f(
				vertices[3 * indices[i] + 0],
				vertices[3 * indices[i] + 1],
				vertices[3 * indices[i] + 2]
			);
			i += 2;
		}
		curStaStep += staStep;
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glNormal3f(
		normals[3 * indices[indices.size() - 1 - sectors - 1] + 0],
		normals[3 * indices[indices.size() - 1 - sectors - 1] + 1],
		normals[3 * indices[indices.size() - 1 - sectors - 1] + 2]
	);
	for (int i = indices.size() - 1 - sectors - 1, sec = 0; i < indices.size() - 1; i++, sec++) {
		glTexCoord2f(offsetX + (repeatX * (sec * 1.0 / sectors)), offsetY + (repeatY * 1));
		glVertex3f(
			vertices[3 * indices[indices.size() - 1 - sectors - 1] + 0],
			vertices[3 * indices[indices.size() - 1 - sectors - 1] + 1],
			vertices[3 * indices[indices.size() - 1 - sectors - 1] + 2]
		);
		glTexCoord2f(offsetX + (repeatX * (sec * 1.0 / sectors)), offsetY + (repeatY * ((stacks - 1) * 1.0 / stacks)));
		glVertex3f(
			vertices[3 * indices[i + 1] + 0],
			vertices[3 * indices[i + 1] + 1],
			vertices[3 * indices[i + 1] + 2]
		);
	}
	glEnd();
	glPopMatrix();
#pragma endregion

	/*
	cout << numIndices << endl;
	cout << "=============" << endl;
	for (int i = 0; i < indices.size(); i++) {
		cout << indices[i] << endl;
	}

	cout << "=============" << endl;
	for (int i = 0; i < vertices.size(); i+= 3) {
		cout << "(" << vertices[i + 0] << ", " << vertices[i + 1]
			<< ", " << vertices[i + 2] << ")\n" << endl;
	}
	*/
}

void init()
{
	cout << "p: despegas/aterizas" << endl;
	cout << "w: aceleras" << endl;
	cout << "s: frenas" << endl;
	cout << "c: quitar/poner cabina" << endl;
	cout << "v: entrar/salir cabina" << endl;
	cout << "t: luces" << endl;
	cout << "m: activar mapa (mostrar mundos)" << endl;
	cout << "j: salto al mundo" << endl;
	cout << "h: regreso a la plataforma" << endl;
	cout << "click izquierdo: disparo" << endl;
	cout << "click derecho: menu para selecionar nave" << endl;
	loadTexture();

	windCenterX = screenWidth / 2;
	windCenterY = screenHeight / 2;

#pragma region lightsAndRender
	GLfloat A[] = { 1, 1, 1, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, A);

	GLfloat dirColor[] = { 0.5, 0.5, 0, 1.0 };
	GLfloat focoColor[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat lazerColor[] = { 0.5,0,0, 1 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dirColor);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, focoColor);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, focoColor);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, lazerColor);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, ROJO);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 2);
	glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, 10);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_NORMALIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

#pragma endregion

#pragma region malla
	malla = glGenLists(1);
	glNewList(malla, GL_COMPILE);
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	GLfloat Dm_TV[] = { 0.5,0.5,0.5,1.0 };
	GLfloat Sm_TV[] = { 0.1,0.1,0.1,1.0 };
	GLfloat s_TV = 2;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Dm_TV);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Sm_TV);
	glMaterialf(GL_FRONT, GL_SHININESS, s_TV);

	glPushMatrix();

	GLfloat v0[3] = { DIM_ESPACIO / 2, -DIM_ESPACIO / 2, 0 };
	GLfloat v1[3] = { DIM_ESPACIO / 2, DIM_ESPACIO / 2, 0 };
	GLfloat v2[3] = { -DIM_ESPACIO / 2, DIM_ESPACIO / 2, 0 };
	GLfloat v3[3] = { -DIM_ESPACIO / 2, -DIM_ESPACIO / 2, 0 };
	//2a. Seleccionar el objeto textura
	glBindTexture(GL_TEXTURE_2D, tex[19]);
	//2b. Definir como se aplicar  la textura en ese objeto
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//2c. Definir la forma de combinar
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//Combina con la iluminacion
	quadtex(v0, v1, v2, v3, 0, 1, 0, 1, NUM_X, NUM_Y);

	glTranslatef(0, 0, 0.25);
	quadtex(v3, v2, v1, v0, 0, 1, 0, 1, NUM_X, NUM_Y);


	glPopMatrix();

	glPushMatrix();
	GLfloat v10[3] = { DIM_ESPACIO / 2, -DIM_ESPACIO / 2,         -1 };
	GLfloat v11[3] = { DIM_ESPACIO / 2, -(DIM_ESPACIO / 2) - 2,     -1 };
	GLfloat v12[3] = { -DIM_ESPACIO / 2, -(DIM_ESPACIO / 2) - 2,  -1 };
	GLfloat v13[3] = { -DIM_ESPACIO / 2, -DIM_ESPACIO / 2,        -1 };

	GLfloat v14[3] = { DIM_ESPACIO / 2, -DIM_ESPACIO / 2,          1 };
	GLfloat v15[3] = { DIM_ESPACIO / 2, -(DIM_ESPACIO / 2) - 2,    1 };
	GLfloat v16[3] = { -DIM_ESPACIO / 2, -(DIM_ESPACIO / 2) - 2,   1 };
	GLfloat v17[3] = { -DIM_ESPACIO / 2, -DIM_ESPACIO / 2,         1 };

	//2a. Seleccionar el objeto textura
	glBindTexture(GL_TEXTURE_2D, tex[0]);
	//2b. Definir como se aplicar  la textura en ese objeto
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//2c. Definir la forma de combinar
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//Combina con la iluminacion
	quadtex(v15, v14, v17, v16, 0, 1, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, NUM_X, NUM_Y);
	quadtex(v11, v15, v16, v12, 0, 1, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, NUM_X, NUM_Y);
	quadtex(v10, v11, v12, v13, 0, 1, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, NUM_X, NUM_Y);
	quadtex(v14, v10, v13, v17, 0, 1, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, NUM_X, NUM_Y);
	quadtex(v11, v15, v14, v10, 0, 1, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, NUM_X, NUM_Y);
	quadtex(v13, v17, v16, v12, 0, 1, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, NUM_X, NUM_Y);

	glTranslatef(0, DIM_ESPACIO + 2, 0);
	quadtex(v15, v14, v17, v16, 0, 1, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, NUM_X, NUM_Y);
	quadtex(v11, v15, v16, v12, 0, 1, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, NUM_X, NUM_Y);
	quadtex(v10, v11, v12, v13, 0, 1, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, NUM_X, NUM_Y);
	quadtex(v14, v10, v13, v17, 0, 1, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, NUM_X, NUM_Y);
	quadtex(v11, v15, v14, v10, 0, 1, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, NUM_X, NUM_Y);
	quadtex(v13, v17, v16, v12, 0, 1, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, NUM_X, NUM_Y);
	glPopMatrix();

	/////////////////////////////////////////////////////////////////

	glPushMatrix();
	GLfloat v20[3] = { DIM_ESPACIO / 2, (DIM_ESPACIO / 2) + 2,   -1 };
	GLfloat v21[3] = { (DIM_ESPACIO / 2) + 2, (DIM_ESPACIO / 2) + 2,   -1 };
	GLfloat v22[3] = { (DIM_ESPACIO / 2) + 2, (-DIM_ESPACIO / 2) - 2,  -1 };
	GLfloat v23[3] = { DIM_ESPACIO / 2, (-DIM_ESPACIO / 2) - 2,        -1 };

	GLfloat v24[3] = { DIM_ESPACIO / 2, (DIM_ESPACIO / 2) + 2,   1 };
	GLfloat v25[3] = { (DIM_ESPACIO / 2) + 2, (DIM_ESPACIO / 2) + 2,   1 };
	GLfloat v26[3] = { (DIM_ESPACIO / 2) + 2, (-DIM_ESPACIO / 2) - 2,  1 };
	GLfloat v27[3] = { DIM_ESPACIO / 2, (-DIM_ESPACIO / 2) - 2,        1 };

	//2a. Seleccionar el objeto textura
	glBindTexture(GL_TEXTURE_2D, tex[0]);
	//2b. Definir como se aplicar  la textura en ese objeto
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//2c. Definir la forma de combinar
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//Combina con la iluminacion
	quadtex(v27, v24, v20, v23, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, 0, 1, NUM_X, NUM_Y);
	quadtex(v23, v20, v21, v22, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, 0, 1, NUM_X, NUM_Y);
	quadtex(v26, v25, v24, v27, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, 0, 1, NUM_X, NUM_Y);
	quadtex(v26, v22, v21, v25, 0, 1, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, NUM_X, NUM_Y);
	quadtex(v22, v26, v27, v23, 0, 1, 0, 1, NUM_X, NUM_Y);
	quadtex(v20, v24, v25, v21, 0, 1, 0, 1, NUM_X, NUM_Y);

	glTranslatef(-DIM_ESPACIO - 2, 0, 0);
	quadtex(v27, v24, v20, v23, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, 0, 1, NUM_X, NUM_Y);
	quadtex(v23, v20, v21, v22, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, 0, 1, NUM_X, NUM_Y);
	quadtex(v26, v25, v24, v27, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, 0, 1, NUM_X, NUM_Y);
	quadtex(v26, v22, v21, v25, 0, 1, -DIM_ESPACIO / 4, DIM_ESPACIO / 4, NUM_X, NUM_Y);
	quadtex(v22, v26, v27, v23, 0, 1, 0, 1, NUM_X, NUM_Y);
	quadtex(v20, v24, v25, v21, 0, 1, 0, 1, NUM_X, NUM_Y);

	glPopMatrix();


	glPopAttrib();
	glEndList();
#pragma endregion

#pragma region asteroid
	astField.createField();
#pragma endregion

#pragma region fondo
	fondo = glGenLists(1);
	glNewList(fondo, GL_COMPILE);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();

	//2a. Seleccionar el objeto textura
	glBindTexture(GL_TEXTURE_2D, tex[5]);
	//2b. Definir como se aplicar  la textura en ese objeto
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//2c. Definir la forma de combinar
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glRotatef(180, 1, 0, 0);
	BackgroudSphere(camReach, 20, 20, 1);

	glPopMatrix();
	glPopAttrib();
	glEndList();
#pragma endregion

#pragma region cabina1
	cabina1 = glGenLists(1);
	glNewList(cabina1, GL_COMPILE);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, tex[6]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glPushMatrix();
	glRotatef(-10, 1, 0, 0);
	ViewSphere(2, 20, 20, 2, -2, 0, -0.5);
	glPopMatrix();

	glPopAttrib();
	glEndList();
#pragma endregion

#pragma region cabina1Inside
	cabina1Inside = glGenLists(1);
	glNewList(cabina1Inside, GL_COMPILE);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, tex[6]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glPushMatrix();
	glRotatef(170, 1, 0, 0);
	glRotatef(90, 0, 0, 1);
	ViewSphere(2, 20, 20, 1, 1, 0, 0);
	glPopMatrix();

	glPopAttrib();
	glEndList();
#pragma endregion

#pragma region cabina2
	cabina2 = glGenLists(1);
	glNewList(cabina2, GL_COMPILE);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, tex[7]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glPushMatrix();
	glRotatef(-5, 1, 0, 0);
	ViewSphere(2, 20, 20, 2, -2, 0, -0.5);
	glPopMatrix();

	glPopAttrib();
	glEndList();
#pragma endregion

#pragma region cabina2Inside
	cabina2Inside = glGenLists(1);
	glNewList(cabina2Inside, GL_COMPILE);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, tex[7]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glPushMatrix();
	glRotatef(170, 1, 0, 0);
	glRotatef(90, 0, 0, 1);
	ViewSphere(2, 20, 20, 1, 1, 0, 0);
	glPopMatrix();

	glPopAttrib();
	glEndList();
#pragma endregion

#pragma region cabina3
	cabina3 = glGenLists(3);
	glNewList(cabina3, GL_COMPILE);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, tex[8]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glPushMatrix();
	glRotatef(-5, 1, 0, 0);
	ViewSphere(2, 20, 20, 2, -2, 0, -0.5);
	glPopMatrix();

	glPopAttrib();
	glEndList();
#pragma endregion

#pragma region cabina3Inside
	cabina3Inside = glGenLists(3);
	glNewList(cabina3Inside, GL_COMPILE);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, tex[8]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glPushMatrix();
	glRotatef(170, 1, 0, 0);
	glRotatef(90, 0, 0, 1);
	ViewSphere(2, 20, 20, 1, 1, 0, 0);
	glPopMatrix();

	glPopAttrib();
	glEndList();
#pragma endregion

#pragma region orbitObject

	float radOrbPlanet = 40;
	float radPlacedPlanet = 75;

	float secStep = rad(360.0 / 9);
	float secAngle = 0;
	float rad = camReach - (radOrbPlanet * 3);

	//earth
	secAngle += secStep;
	orb.push_back(new OrbitObject(
		Vec3(
			rad * cos(secAngle),
			rad * sin(secAngle),
			0
		), true, radPlacedPlanet, radOrbPlanet, 10, 0, 0, 0.25, 1, 1
	)
	);

	//jupiter
	secAngle += secStep;
	orb.push_back(new OrbitObject(
		Vec3(
			rad * cos(secAngle),
			rad * sin(secAngle),
			0
		), true, radPlacedPlanet, radOrbPlanet, 11, 0.01, 0.01, 0.25, 1, 1
	)
	);

	//mars
	secAngle += secStep;
	orb.push_back(new OrbitObject(
		Vec3(
			rad * cos(secAngle),
			rad * sin(secAngle),
			0
		), true, radPlacedPlanet, radOrbPlanet, 12, 0.25, 0.01, 0.25, 1, 0.048
	)
	);

	//nerptuno
	secAngle += secStep;
	orb.push_back(new OrbitObject(
		Vec3(
			rad * cos(secAngle),
			rad * sin(secAngle),
			0
		), true, radPlacedPlanet, radOrbPlanet, 14, 0.15, 0.15, 0.30, 1, 0.347
	)
	);

	//mercurio
	secAngle += secStep;
	orb.push_back(new OrbitObject(
		Vec3(
			rad * cos(secAngle),
			rad * sin(secAngle),
			0
		), true, radPlacedPlanet, radOrbPlanet, 13, 0.15, 0.15, 0.30, 1, 0.034
	)
	);

	//urano
	secAngle += secStep;
	orb.push_back(new OrbitObject(
		Vec3(
			rad * cos(secAngle),
			rad * sin(secAngle),
			0
		), true, radPlacedPlanet, radOrbPlanet, 15, 0.15, 0.15, 0.30, 1, 0.358
	)
	);

	//venus
	secAngle += secStep;
	orb.push_back(new OrbitObject(
		Vec3(
			rad * cos(secAngle),
			rad * sin(secAngle),
			0
		), true, radPlacedPlanet, radOrbPlanet, 16, 0.15, 0.15, 0.30, 1, 0.085
	)
	);

	//saturn
	secAngle += secStep;
	orb.push_back(new OrbitObject(
		Vec3(
			rad * cos(secAngle),
			rad * sin(secAngle),
			0
		), true, radPlacedPlanet, radOrbPlanet, 17, 0.15, 0.15, 0.30, 1, 0.760
	)
	);

	//sun
	secAngle += secStep;
	orb.push_back(new OrbitObject(
		Vec3(
			rad * cos(secAngle),
			rad * sin(secAngle),
			0
		), true, radPlacedPlanet, radOrbPlanet, 18, 0.15, 0.15, 0.30, 1, 0.760
	)
	);

	for (auto& orbitobj : orb) {
		//cout << orbitobj->texture << endl;
		orbitobj->create();
	}

#pragma endregion

	initMenu();
}

// Funcion de atencion al evento de dibujo
void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLfloat PL0[] = { 1,1,1,0 };
	glLightfv(GL_LIGHT0, GL_POSITION, PL0);

	Vec3 temp = (Vec3(2, 0, 0));
	GLfloat posicion1[] = { temp.x, temp.y, temp.z, 1.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, posicion1);
	GLfloat posicion2[] = { -temp.x, -temp.y, -temp.z, 1.0 };
	glLightfv(GL_LIGHT2, GL_POSITION, posicion2);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, cutoff);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, cutoff);

	GLfloat posicion4[] = { 0, 0, 0, 1.0 };
	glLightfv(GL_LIGHT4, GL_POSITION, posicion4);
	glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, 2);

	if (!inside) {
		u = sist.getu();
		v = sist.getv();
		w = sist.getw();
		o = sist.geto();

		gluLookAt(o.x, o.y, o.z,
			(o.x + -w.x), (o.y + -w.y), (o.z + -w.z),
			u.x * cos(rad(rollAngle)), u.y * cos(rad(rollAngle)), sin(rad(rollAngle)));
	}
	else {
		u = insideSist.getu();
		v = insideSist.getv();
		w = insideSist.getw();
		o = insideSist.geto();

		gluLookAt(o.x, o.y, o.z,
			(o.x + -w.x), (o.y + -w.y), (o.z + -w.z),
			u.x * cos(rad(rollAngle)), u.y * cos(rad(rollAngle)), sin(rad(rollAngle)));
	}

	astField.changeFieldPos(o);

	if (shots.alive) {
		shots.draw();
	}

	if (planetSelected == -1) {
		drawMalla();
	}

	astField.drawAst();

	glPushMatrix();
	glTranslatef(o.x, o.y, o.z);
	glCallList(fondo);
	glPopMatrix();

	for (int i = 0; i < orb.size(); i++) {
		if (orb[i]->orbit) {
			if (map) {
				orb[i]->draw();
			}
		}
		else {
			orb[i]->draw();
		}
	}

	//////////////////TRANSPARENT/////////////////////

#pragma region alpha
	glDepthMask(GL_FALSE);

	astField.drawDust();


	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glPushMatrix();
	if (inside) {
		glTranslatef(o.x + (w.x * -1), o.y + (w.y * -1), o.z + (w.z * -1));
	}
	else {
		glTranslatef(o.x, o.y, o.z);
	}
	float rotAng = (90 - rollAngle) * 2.0;
	glRotatef(xrot, 0, 0, 1);
	glRotatef(zrot, 1, 0, 0);
	glRotatef(rotAng, 0, 1, 0);


	if (inside) {
		if (nave == 2) { glCallList(cabina1Inside); }
		else if (nave == 3) { glCallList(cabina2Inside); }
		else if (nave == 4) { glCallList(cabina3Inside); }
	}
	else {
		if (!zoomed) {
			if (nave == 2) { glCallList(cabina1); }
			else if (nave == 3) { glCallList(cabina2); }
			else if (nave == 4) { glCallList(cabina3); }
		}
	}
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopAttrib();

	glDepthMask(GL_TRUE);

#pragma endregion

	glutSwapBuffers();
}

// Funcion de atencion al redimensionamiento
void reshape(GLint w, GLint h)
{
	// Usamos toda el area de dibujo
	glViewport(0, 0, w, h);
	// Definimos la camara (matriz de proyeccion)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	screenWidth = w;
	screenHeight = h;
	windCenterX = screenWidth / 2;
	windCenterY = screenHeight / 2;
	razon = (float)w / h;
	gluPerspective(camAngle, razon, 1, camReach);
}

void mouseWheel(int b, int d, int x, int y)
{
	if (d > 0 && cutoff < 30) {
		cutoff += 1;
	}
	else if (d < 0 && cutoff > 6) {
		cutoff -= 1;
	}
	cout << cutoff << endl;
	return;
}

void onMenu(int opcion)
// Funcion de atencion al menu de popup
{
	nave = opcion;
	glutPostRedisplay();
}

void initMenu()
// Construye el menu de popup
{
	int menucolores = glutCreateMenu(onMenu);
	glutAddMenuEntry("Nave 1", 2);
	glutAddMenuEntry("Nave 2", 3);
	glutAddMenuEntry("Nave 3", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON); // Ya no pasa por onClick
}

void onKey(unsigned char tecla, int x, int y)
{
	switch (tecla) {
	case 'p':
		//cout << movimiento << endl;
		if (!jumping && !movimiento) {
			checkPos();
		}
		break;
	case 'h':
		if (!jumping && !movimiento) {
			if (planetSelected == -1) { cout << "ya estas en casa" << endl; }
			else {
				glutTimerFunc(1000 / tasaFPS, zoomACasa, 0);
				jumping = true;
			}
		}
		break;
	case 'j':
		if (map) {
			if (!jumping && !movimiento) {
				for (int i = 0; i < orb.size(); i++) {
					if (orb[i]->orbit) {
						float aux = orb[i]->alignment;
						if (aux < -0.99) {
							if (planetSelected != -1) { orb[planetSelected]->orbit = true; }
							//cout << planetSelected << " " << i << endl;
							planetSelected = i;
							glutTimerFunc(1000 / tasaFPS, zoom, 0);
							jumping = true;
							break;
						}
					}
					
				}
			}
			if (!jumping) { cout << "mira a un planeta para teletransportarse a el" << endl; }
		}
		else {
			cout << "activa el mapa con 'm'" << endl;
		}
		break;
	case 'w':
		if (!inside && !parked) {
			moveFoward();
		}
		else { cout << "estas dentro de copilot o estas parqueado" << endl; }
		break;
	case 's':
		if (!inside && !parked) {
			moveBackward();
		}
		else { cout << "estas dentro de copilot o estas parqueado" << endl; }
		break;
	case 'm':
		if (!jumping) {
			//cout << "map: " << map << endl;
			map = !map;
		}
		break;
	case 'c':
		//cout << "zoomed: " << zoomed << endl;
		zoomed = !zoomed;
		break;
	case 'v':
		//cout << "inside: " << inside << endl;
		inside = !inside;
		if (inside) {
			insideSist = sist;
			glEnable(GL_LIGHT4);
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT2);
			insideRotx = 0;
			insideRoty = 0;
			lights = false;
		}
		else {
			glDisable(GL_LIGHT4);
		}
		break;
	case 't':
		//cout << lights << endl;
		if (!inside) {
			if (lights) {
				glEnable(GL_LIGHT1);
				glEnable(GL_LIGHT2);
			}
			else {
				glDisable(GL_LIGHT1);
				glDisable(GL_LIGHT2);
			}
		}
		lights = !lights;
		break;
	case 27: // Pulso escape
		exit(0);
	}
	glutPostRedisplay();
}

void select()
{
	// Dibuja la escena en el backbuffer sin mostrar
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (inside) {
		u = insideSist.getu();
		v = insideSist.getv();
		w = insideSist.getw();
		o = insideSist.geto();
	}
	else {
		u = sist.getu();
		v = sist.getv();
		w = sist.getw();
		o = sist.geto();
	}


	// Situacion y orientacion de la camara
	gluLookAt(o.x, o.y, o.z,
		(o.x + -w.x), (o.y + -w.y), (o.z + -w.z),
		u.x * cos(rad(rollAngle)), u.y * cos(rad(rollAngle)), sin(rad(rollAngle)));

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();
	glTranslatef(o.x, o.y, o.z);
	glCallList(fondo);
	glPopMatrix();

	astField.detect();

	glPopAttrib();
}

void onClick(int boton, int estado, int x, int y)
{
	GLdouble modelview[16];
	GLdouble projMatrix[16];
	GLint    viewport[4];
	GLdouble objx[1];
	GLdouble objy[1];
	GLdouble objz[1];
	float pixel[4];
	float zCord;
	if (boton == GLUT_LEFT_BUTTON && estado == GLUT_DOWN) {
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
		glGetIntegerv(GL_VIEWPORT, viewport);
		select();
		glReadPixels(x, (viewport[3] - y), 1, 1, GL_BLUE, GL_FLOAT, &pixel);
		glReadPixels(x, (viewport[3] - y), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zCord);
		gluUnProject(x, (viewport[3] - y), zCord, modelview, projMatrix, viewport, objx, objy, objz);

		if (!shots.alive && !jumping) {
			tempx = objx[0];
			tempy = objy[0];
			tempz = objz[0];
			shots.create(nave, Vec3(tempx, tempy, tempz), pixel[0]);
		}
		else {
			cout << "espera para disparar" << endl;
		}
	}
}

int main(int argc, char** argv)
{
	// Inicializaciones
	FreeImage_Initialise();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(500, 100);

	// Crear ventana
	glutCreateWindow(PROYECTO);

	// Registrar callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(onKey);
	glutMouseWheelFunc(mouseWheel);
	glutMouseFunc(onClick);

	glutPassiveMotionFunc(onPassiveMotion);
	glutTimerFunc(1000 / tasaFPS, apuntarNave, 0);
	glutTimerFunc(1000 / tasaFPS, desplazarNave, 0);
	glutWarpPointer(screenWidth / 2, screenHeight / 2);
	init();

	// Bucle de atencion a eventos
	glutMainLoop();
	FreeImage_DeInitialise();
}