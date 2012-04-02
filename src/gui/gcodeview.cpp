#include "gcodeview.h"

#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include <iostream>

using namespace std;

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

GcodeView::GcodeView(QWidget *parent) :
    QGLWidget(parent)
{
    resetView();
    resizeGL(this->width(),this->height());

    animationTimer = new QTimer(this);
    connect(animationTimer, SIGNAL(timeout()), this, SLOT(animationUpdate()));

    arcball.set_damping(.05);
}

// TODO: This is very likely not thread safe, and this is probably detrimental to the event timer approach.
void GcodeView::animationUpdate() {
    if (arcball.is_spinning) {
        arcball.idle();
        *arcball.rot_ptr = *arcball.rot_ptr * arcball.rot_increment;
        updateGL();

        if (!arcball.is_spinning) {
            animationTimer->stop();
        }
    }
}

void GcodeView::resetView() {
    scale = .1;
    pan_x = 0.0;
    pan_y = 0.0;
    maxVisibleLayer = 0;
    minVisibleLayer = 0;
    // TODO: reset arcball
}

void GcodeView::initializeGL()
{
    // Set up the rendering context, define display lists etc.:
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glDisable(GL_DEPTH_TEST);

    glDisable(GL_LIGHTING);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GcodeView::setupViewport(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
}

void GcodeView::resizeGL(int width, int height)
{
    setupViewport(width, height);

    arcball.set_params( vec2( (float)(width / 2), (float)(height / 2)),
                       (float) 2*height );
}

void GcodeView::paintGL()
{
    paintGLgcode();
}


void GcodeView::paintGLgcode()
{
   // static int frameCount = 0;
   // cout << frameCount << endl;
   // frameCount ++;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    double widthRatio = (double)width()/(double)height();

    // do something to the projection?
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-widthRatio, widthRatio, -1, 1, 10, 100);
    glTranslatef(0.0, 0.0, -15.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, width(), height());

    glPushMatrix();

    glScalef(scale, scale, scale);

    glTranslatef(pan_x, pan_y, model.getModelZCenter());

    mat4 tmp_rot = arcball.rot;
    glMultMatrixf( (float*) &tmp_rot[0][0] );
    glTranslatef(0.0f, 0.0f,  -model.getModelZCenter());

    glLineWidth(2);
    glBegin(GL_LINE_STRIP);







    for (unsigned int i = 1; i < model.getPointCount(); i++) {

        float red=0;
        float green=0;
        float blue=0;

        point a = model.points[i-1];
        point b = model.points[i];

        float dark = 0.025;
        float clear = 0.4;

        float alpha= clear;

        if (model.map.heightLessThanLayer(minVisibleLayer, b.z))
        {
            // cout << "minlayer " << minVisibleLayer << endl;
            alpha = dark;
        }

        if (model.map.heightInLayer(maxVisibleLayer, b.z))
        {
            alpha = 1;
        }


        if (model.map.heightGreaterThanLayer(maxVisibleLayer, b.z))
        {
                // cout << "minlayer " << minVisibleLayer << endl;
                alpha = dark;

         }




        switch (b.kind) {
          case travel:
            blue = 0.5;
            break;
          case infill:
            red = 1;
            green = 1;
            blue = 0;
            break;
          case shell:
            if(b.nb ==0)
            {
                red = 1;
            }
            else
            {
                green = 1;
                blue = 1 ;
            }
            break;
          case perimeter:
            blue = 0.5;
            red = 0.5;
            green = 0.5;
            break;
          default:
            green = 1;

         }

        if(b.kind == travel)
        {
            alpha *= 0.25;
        }

        glColor4f(red,green,blue,alpha);
        glVertex3f(a.x, a.y, a.z); // origin of the line
        glVertex3f(b.x, b.y, b.z); // ending point of the line
    }

    glEnd( );

    glPopMatrix();
}

void GcodeView::loadSliceData(const std::vector<mgl::SliceData>&sliceData)
{
    model.loadSliceData(sliceData);
    resetView();

    updateGL();
}

void GcodeView::loadModel(QString filename) {
    model.loadGCode(filename);
    resetView();
    updateGL();
}

void GcodeView::exportModel(QString filename) {
    model.exportGCode(filename);
}

bool GcodeView::hasModel() {
    return !(model.points.empty());
}

void GcodeView::mousePressEvent(QMouseEvent *event)
{
    arcball.mouse_down(event->x(), height() - event->y());
//    lastPos = event->pos();
}

void GcodeView::mouseMoveEvent(QMouseEvent *event)
{
    // TODO: send key states too?
    arcball.mouse_motion(event->x(), height() - event->y());

    updateGL();


}

void GcodeView::mouseReleaseEvent(QMouseEvent *event) {
    arcball.mouse_up();

    // Start an animation timer if the arcball is still spinning
    if (arcball.is_spinning) {
        animationTimer->start(10); // 30fps?
    }
}

void GcodeView::wheelEvent(QWheelEvent *event)
{
    float newScale = scale*(1 + event->delta()/400.0);
    scale = newScale;

    updateGL();
}


void GcodeView::zoom(float amount){
    scale = scale * amount;
    updateGL();
}

void GcodeView::panX(float amount){
    pan_x = pan_x + amount;
    updateGL();
}

void GcodeView::panY(float amount){
    pan_y = pan_y + amount;
    updateGL();
}

void GcodeView::mouseDoubleClickEvent(QMouseEvent event) {
    // TODO: We never get here.
    resetView();
    updateGL();
}

void GcodeView::setMinimumVisibleLayer(int layer)
{
    if (layer >=0 && layer < model.map.size()) {
        minVisibleLayer = layer;
        std::cout << "Minimum visible layer: " << layer << ", height: " << model.map.getLayerHeight(layer) << std::endl;
        updateGL();
    }
}

void GcodeView::setMaximumVisibleLayer(int layer) {
    if (layer < model.map.size()) {
        maxVisibleLayer = layer;
        std::cout << "Maximum visible layer: " << layer << ", height: " << model.map.getLayerHeight(layer) << std::endl;
        updateGL();
    }
}
