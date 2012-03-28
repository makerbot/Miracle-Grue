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
    currentLayer = 0;

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
    static int frameCount = 0;
    cout << frameCount << endl;
    frameCount ++;

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
            point a = model.points[i-1];
            point b = model.points[i];

            float alpha = 0;

            if (model.map.heightLessThanLayer(currentLayer, b.z)) {
                alpha = .20;
            }
            else if (model.map.heightInLayer(currentLayer, b.z)) {
                alpha = 1;
            }
            else {
                alpha = .02;
            }


            // scale the feedrate to the bounds of what this job contains;
//            float val = (b.feedrate - model.feedrateBounds.getMin())/(model.feedrateBounds.getMax() - model.feedrateBounds.getMin());
            float val = (b.flowrate - model.flowrateBounds.getMin())/(model.flowrateBounds.getMax() - model.flowrateBounds.getMin());

            glColor4f(val,1-val,0,alpha);

            if (!b.toolEnabled) {
                glColor4f(0,0,255,alpha);
            }

            glVertex3f(a.x, a.y, a.z); // origin of the line
            glVertex3f(b.x, b.y, b.z); // ending point of the line
    }

    glEnd( );

    glPopMatrix();
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

void GcodeView::setCurrentLayer(int layer) {
    if (layer < model.map.size()) {
        currentLayer = layer;
        std::cout << "Current layer: " << layer << ", height: " << model.map.getLayerHeight(layer) << std::endl;
        updateGL();
    }
}
