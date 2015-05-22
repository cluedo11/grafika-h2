//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2013-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization fÃ¼ggvÃ©nyt kivÃ©ve, a lefoglalt adat korrekt felszabadÃ­tÃ¡sa nÃ©lkÃ¼l
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D,
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi,
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : BÁLINT FERENC
// Neptun : EGUGHN
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...
struct Vector {
    float x, y, z, w;
    
    Vector( ) {
        x = y = z = w = 0;
    }
    Vector(float x0, float y0, float z0 = 0, float w0 = 0) {
        x = x0; y = y0; z = z0; w = w0;
    }
    Vector operator*(float a) {
        return Vector(x * a, y * a, z * a, w * a);
    }
    Vector operator+(const Vector& v) {
        return Vector(x + v.x, y + v.y, z + v.z, w + v.w);
    }
    Vector operator-(const Vector& v) {
        return Vector(x - v.x, y - v.y, z - v.z, w - v.w);
    }
    float operator*(const Vector& v) {
        return (x * v.x + y * v.y + z * v.z + w * v.w);
    }
    float Length() { return sqrt(x * x + y * y + z * z + w * w); }
};

struct Color {
    float r, g, b;
    
    Color( ) {
        r = g = b = 0;
    }
    Color(float r0, float g0, float b0) {
        r = r0; g = g0; b = b0;
    }
    Color operator*(float a) {
        return Color(r * a, g * a, b * a);
    }
    Color operator*(const Color& c) {
        return Color(r * c.r, g * c.g, b * c.b);
    }
    Color operator+(const Color& c) {
        return Color(r + c.r, g + c.g, b + c.b);
    }
};

class ControlPoint {
public:
    Vector center;
    float time;
    
    ControlPoint() {
        center.x = center.y = center.z = 0;
        time = 0;
    }
    
    ControlPoint(float x, float y) {
        center.x = x;
        center.y = y;
        center.z = 0;
        center.w = 0;
    }
    
    void setCenter(Vector newCenter) {
        center.x = newCenter.x;
        center.y = newCenter.y;
    }
    
    void drawPoint(float r, float g, float b, float radius) {
        glColor3f(r, g, b);
        glBegin(GL_TRIANGLE_FAN);
        
        glVertex2f(center.x, center.y);
        for (int i = 0; i < 360; i++) {
            glVertex2f((radius / 50) * cos((float)i) + center.x, (radius / 50) * sin((float)i) + center.y);
        }
        
        glEnd();
    }
};

const int TENZIO = -0.5;

float time, oTime, dTime;

ControlPoint bezierYellowPoint, catmullYellowPoint;

float bezierYellowCounter, catmullYellowCounter;
int catmullYellowSectionPoint;

int factorial(int num)
{
    if (num < 1) return 1;
    return num * factorial(num - 1);
}

class BezierCurve {
public:
    ControlPoint controlPoints[10];
    int controlPointNumber;
    
    int canDrawSelf;
    
    BezierCurve() {
        controlPointNumber = 0;
        canDrawSelf = 0;
        for (int i = 0; i < 10; i++) {
            controlPoints[i].setCenter(Vector(0, 0, 0));
        }
    }
    
    void addPoint(Vector newPoint) {
        controlPoints[controlPointNumber].setCenter(newPoint);
        controlPoints[controlPointNumber].time = time;
        controlPointNumber++;
        
    }
    
    float calculateOneBezier(float t, int j) {
        float r = 0;
        
        r = powf(1.0 - t, controlPointNumber - 1 - j) * powf(t, j) * (factorial(controlPointNumber - 1) / (factorial(j) * factorial(controlPointNumber - 1 - j)));
        
        return r;
    }
    
    Vector calculateBezier(float t) {
        Vector nextPoint = Vector(0, 0, 0);
        
        for (int j = 0; j < controlPointNumber; j++) {
            nextPoint = nextPoint + controlPoints[j].center * calculateOneBezier(t, j);
        }
        
        return nextPoint;
    }
    
    void drawCurve() {
        float t = 0;
        
        glColor3f(0, 0, 1);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < 100; i++) {
            Vector nextPoint = calculateBezier(t);
            glVertex2f(nextPoint.x, nextPoint.y);
            
            t += 0.01;
        }
        
        glEnd();
        
        for (int i = 0; i < controlPointNumber; i++) {
            controlPoints[i].drawPoint(1.0, 0.0, 0.0, calculateOneBezier(bezierYellowCounter / (controlPoints[controlPointNumber - 1].time - controlPoints[0].time), i));
        }
    }
};

BezierCurve bezier;

class CatmullRomCurve {
public:
    ControlPoint controlPoints[12];
    int controlPointNumber;
    float pointParameters[12];
    
    CatmullRomCurve() {
        controlPointNumber = 0;
        for (int i = 0; i < 12; i++) {
            controlPoints[i].setCenter(Vector(0, 0, 0, 0));
        }
    }
    
    void addPoint(Vector newPoint) {
        controlPoints[controlPointNumber].setCenter(newPoint);
        controlPoints[controlPointNumber + 1].setCenter(newPoint);
        
        controlPoints[controlPointNumber].time = time;
        controlPoints[controlPointNumber + 1].time = time;
        
        if (controlPointNumber == 0) {
            pointParameters[controlPointNumber] = 0;
            pointParameters[controlPointNumber + 1] = time + 1;
            controlPointNumber += 2;
        } else {
            pointParameters[controlPointNumber] = time;
            pointParameters[controlPointNumber + 1] = time + pointParameters[controlPointNumber];
            controlPointNumber++;
        }
        
    }
    
    Vector calculateCatmullRom(float t, int i, int isRadius) {
        Vector c1, c2, c3, c4;
        if (isRadius == 0) {
            c1 = controlPoints[i].center;
            c2 = controlPoints[i + 1].center;
            c3 = controlPoints[i + 2].center;
            c4 = controlPoints[i + 3].center;
        } else {
            c1 = Vector(1, 0, 0, 0);
            c2 = Vector(0, 1, 0, 0);
            c3 = Vector(0, 0, 1, 0);
            c4 = Vector(0, 0, 0, 1);
        }
        
        Vector point_1 = (((c2 - c1) * (1 / (pointParameters[i + 1] - pointParameters[i]))) + ((c3 - c2) * (1 / (pointParameters[i + 2] - pointParameters[i + 1])))) * (1 - TENZIO) * 0.5;
        Vector point_2 = (((c3 - c2) * (1 / (pointParameters[i + 2] - pointParameters[i + 1]))) + ((c4 - c3) * (1 / (pointParameters[i + 3] - pointParameters[i + 2])))) * (1 - TENZIO) * 0.5;
            
        Vector a = c2;
        Vector b = point_1;
            
        Vector c = ((c3 - c2) * 3) * (1 / pow(pointParameters[i + 2] - pointParameters[i + 1], 2)) - (point_2 + point_1 * 2) * (1 / (pointParameters[i + 2] - pointParameters[i + 1]));
        Vector d = (point_2 + point_1) * (1 / pow(pointParameters[i + 2] - pointParameters[i + 1], 2)) - ((c3 - c2) * 2) * (1 / pow(pointParameters[i + 2] - pointParameters[i + 1], 3));
        
        Vector nextPoint =  d*pow(t - pointParameters[i + 1], 3) + c*pow(t - pointParameters[i + 1], 2) + b*(t - pointParameters[i + 1]) + a;
        
        return nextPoint;
    }
    
    void drawCurve() {
        float t = 0;
        
        glColor3f(1, 1, 1);
        glBegin(GL_LINE_STRIP);
        
        for (int i = 0; i < controlPointNumber - 2; i++) {
            float stepUnit = (pointParameters[i + 2] - pointParameters[i + 1]) / (100.0 / (controlPointNumber - 2));
            
            for (t = pointParameters[i + 1]; t < pointParameters[i + 2]; t += stepUnit) {
                Vector nextPoint = calculateCatmullRom(t, i, 0);
                glVertex2f(nextPoint.x, nextPoint.y);
            }
        }
        
        glEnd();
        
        for (int i = 0; i < controlPointNumber; i++) {
            if (bezier.canDrawSelf == 1) {
                Vector radius = calculateCatmullRom(catmullYellowCounter, catmullYellowSectionPoint, 1);
                if (i == catmullYellowSectionPoint) {
                    Color color1 = radius.x > 0 ? Color(1.0, 0.0, 0.0) : Color(0.25, 0.89, 0.96);
                    Color color2 = radius.y > 0 ? Color(1.0, 0.0, 0.0) : Color(0.25, 0.89, 0.96);
                    Color color3 = radius.z > 0 ? Color(1.0, 0.0, 0.0) : Color(0.25, 0.89, 0.96);
                    Color color4 = radius.w > 0 ? Color(1.0, 0.0, 0.0) : Color(0.25, 0.89, 0.96);
                    controlPoints[i].drawPoint(color1.r, color1.g, color1.b, fabs(radius.x));
                    controlPoints[i + 1].drawPoint(color2.r, color2.g, color2.b, fabs(radius.y));
                    controlPoints[i + 2].drawPoint(color3.r, color3.g, color3.b, fabs(radius.z));
                    controlPoints[i + 3].drawPoint(color4.r, color4.g, color4.b, fabs(radius.w));
                }
            } else {
                controlPoints[i].drawPoint(1.0, 0.0, 0.0, 1.0);
            }
        }
    }
};

Vector cameraCenter;


CatmullRomCurve catmullRom;

Vector transformCoordinates(float x, float y) {
    return Vector(((x - 300) / 300) / 2 + cameraCenter.x, (-(y - 300) / 300) / 2 + cameraCenter.y, 0, 0);
}

Vector calculateMassPoint() {
    Vector massPoint = Vector(0, 0, 0, 0);
    for (int i = 1; i < catmullRom.controlPointNumber; i++) {
        massPoint = massPoint + catmullRom.controlPoints[i].center;
    }
    
    massPoint.x /= (catmullRom.controlPointNumber - 1);
    massPoint.y /= (catmullRom.controlPointNumber - 1);
    
    return massPoint;
}


const int screenWidth = 600;
const int screenHeight = 600;


Color image[screenWidth*screenHeight];

void onInitialization( ) {
	glViewport(0, 0, screenWidth, screenHeight);
    
    time = dTime = oTime = 0;
    
    cameraCenter = Vector(-0.5, -0.5, 0, 0);
    
    catmullYellowSectionPoint = 0;
    
    bezierYellowCounter = catmullYellowCounter = 0;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(cameraCenter.x - 0.5, cameraCenter.x + 0.5, cameraCenter.y - 0.5, cameraCenter.y + 0.5);
    
    for(int Y = 0; Y < screenHeight; Y++)
		for(int X = 0; X < screenWidth; X++)
            image[Y*screenWidth + X] = Color(0, 0, 0);
    
    
}

void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    catmullRom.drawCurve();
    
    if (bezier.canDrawSelf == 1) {
        Vector massPoint = calculateMassPoint();
        catmullYellowPoint.drawPoint(1, 1, 0, 1.0);
        
        glTranslatef(massPoint.x, massPoint.y, 0);
        glRotatef(60, 0.0, 0.0, 1.0);
        glTranslatef(-massPoint.x, -massPoint.y, 0);
        bezier.drawCurve();
        bezierYellowPoint.drawPoint(1, 1, 0, 1.0);
        
    }
    
    glutSwapBuffers();
}

void onKeyboard(unsigned char key, int x, int y) {
    if (bezier.canDrawSelf == 0) {
        if (key == 's') {
            if (cameraCenter.x + 0.7 < 1 && cameraCenter.y + 0.9 < 1) {
                cameraCenter = cameraCenter + Vector(0.2, 0.4, 0, 0);
            } else {
                cameraCenter = Vector(-0.5, -0.5, 0, 0);
            }
            
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(cameraCenter.x - 0.5, cameraCenter.x + 0.5, cameraCenter.y - 0.5, cameraCenter.y + 0.5);
            
        }
        
        if (key == (char)32) {
            bezier.canDrawSelf = 1;
            
            bezierYellowPoint = bezier.controlPoints[0];
            catmullYellowPoint = catmullRom.controlPoints[1];
            
            bezierYellowCounter = 0;
            catmullYellowCounter = catmullRom.controlPoints[1].time;
        }

    }
    
    glutPostRedisplay( );
}

void onKeyboardUp(unsigned char key, int x, int y) {
    
}

void onMouse(int button, int state, int x, int y) {
    Vector point = transformCoordinates(x, y);
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && bezier.canDrawSelf == 0) {
        if (catmullRom.controlPointNumber <= 10) {
            bezier.addPoint(point);
            catmullRom.addPoint(point);
        }
    }
    
    glutPostRedisplay( );
}

void onMouseMotion(int x, int y)
{
    
}

void onIdle( ) {
    time = glutGet(GLUT_ELAPSED_TIME);
    dTime = time - oTime;
    oTime = time;
    
    if (bezier.canDrawSelf == 1) {
        if (dTime > 100) {
            for (int i = 100; i < dTime; i += 100) {
                if (bezierYellowCounter + dTime <= bezier.controlPoints[bezier.controlPointNumber - 1].time - bezier.controlPoints[0].time) {
                    bezierYellowCounter += dTime;
                    bezierYellowPoint.setCenter(bezier.calculateBezier(bezierYellowCounter / (bezier.controlPoints[bezier.controlPointNumber - 1].time - bezier.controlPoints[0].time)));
                } else {
                    bezierYellowCounter = 0;
                    bezierYellowPoint.setCenter(bezier.controlPoints[0].center);
                }
                
                if (catmullYellowCounter + dTime <= catmullRom.controlPoints[catmullRom.controlPointNumber].time) {
                    catmullYellowCounter += dTime;
                    catmullYellowPoint.setCenter(catmullRom.calculateCatmullRom(catmullYellowCounter, catmullYellowSectionPoint, 0));
                    if (catmullYellowCounter >= catmullRom.controlPoints[catmullYellowSectionPoint + 2].time) {
                        catmullYellowSectionPoint++;
                    }
                } else {
                    catmullYellowPoint.setCenter(catmullRom.controlPoints[1].center);
                    catmullYellowCounter = catmullRom.controlPoints[1].time;
                    catmullYellowSectionPoint = 0;
                }
            }
        }
        
        if (dTime < 100) {
            if (bezierYellowCounter + dTime <= bezier.controlPoints[bezier.controlPointNumber - 1].time - bezier.controlPoints[0].time) {
                bezierYellowCounter += dTime;
                bezierYellowPoint.setCenter(bezier.calculateBezier(bezierYellowCounter / (bezier.controlPoints[bezier.controlPointNumber - 1].time - bezier.controlPoints[0].time)));
            } else {
                bezierYellowCounter = 0;
                bezierYellowPoint.setCenter(bezier.controlPoints[0].center);
            }
            
            if (catmullYellowCounter + dTime <= catmullRom.controlPoints[catmullRom.controlPointNumber].time) {
                catmullYellowCounter += dTime;
                catmullYellowPoint.setCenter(catmullRom.calculateCatmullRom(catmullYellowCounter, catmullYellowSectionPoint, 0));
                if (catmullYellowCounter >= catmullRom.controlPoints[catmullYellowSectionPoint + 2].time) {
                    catmullYellowSectionPoint++;
                }
            } else {
                catmullYellowPoint.setCenter(catmullRom.controlPoints[1].center);
                catmullYellowCounter = catmullRom.controlPoints[1].time;
                catmullYellowSectionPoint = 0;
            }
        }
    }
    
    glutPostRedisplay( );
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer
    
    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon
    
    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    
    onInitialization();					// Az altalad irt inicializalast lefuttatjuk
    
    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);
    
    glutMainLoop();					// Esemenykezelo hurok
    
    return 0;
}
