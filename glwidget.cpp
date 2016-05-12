#include "glwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <QDir>
#include <math.h>
#include <iostream>
#include <string>

using namespace std;

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_xRot(0),
      m_yRot(0),
      m_zRot(0),
      m_program(0),
      m_elementBuffer(QOpenGLBuffer::IndexBuffer)
{

    QSurfaceFormat fmt = format();
    fmt.setAlphaBufferSize(8);
    setFormat(fmt);
}

GLWidget::~GLWidget()
{
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        emit xRotationChanged(angle); //tämä liikuttaa slidereita
        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        emit yRotationChanged(angle);
        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        emit zRotationChanged(angle);
        update();
    }
}

void GLWidget::cleanup()
{
    makeCurrent();
    m_vertexBuffer.destroy();
    m_normalBuffer.destroy();
    m_elementBuffer.destroy();
    delete m_program;
    m_program = 0;
    doneCurrent();
}



void GLWidget::initializeGL()
{
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.
    cerr << "initializeGL...\n";

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0.7f, 0.9f, 1.0f, 0);

    m_program = new QOpenGLShaderProgram;

    QString vertexShaderFile;
    QString fragmentShaderFile;
    QString filesPath = QCoreApplication::applicationDirPath() + "/files/";
    QString objectFile = "olento_testi.obj";

    QDir::setCurrent(filesPath);

    //Luodaan objekti
    if(m_obj.isReady() ) {
        cerr << "   Objekti on jo luotu\n";
    }
    else if(!m_obj.loadFromFile((filesPath + objectFile).toStdString() )) {
        cerr << "   Objektin luominen tiedostosta epäonnistui! " << (filesPath + objectFile).toStdString() << "\n";
        cerr << "   InitializeGL failed!\n";
        return;
    }

    cerr << "Vertices " << m_obj.vertices.size() << "\n";

    vertexShaderFile = "olento.vertexshader";
    fragmentShaderFile = "olento.fragmentshader";

    cerr << "   Käytetään shadereita: \n";
    cerr << "   " << vertexShaderFile.toStdString() << ",\n";
    cerr << "   " << fragmentShaderFile.toStdString() << "\n";

    if(!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderFile)) {
        //cerr << "Couldn't add vertex shader: " << vertexShaderFile.toStdString() << "\n";
        cerr << "InitializeGL failed!\n";
        return;
    }

    if(!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderFile)) {
        //cerr << "Couldn't add fragment shader: " << fragmentShaderFile.toStdString() << "\n";
        cerr << "InitializeGL failed!\n";
        return;
    }

    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->link();

    m_program->bind();

    //vertex shader uniforms: MVP, V, M, LightPos
    //fragment uniforms: LightPos, diffuseColor, specularity, hardness, alpha

    m_MVPMatrixID = m_program->uniformLocation("MVP");
    m_ViewMatrixID = m_program->uniformLocation("V");
    m_ModelMatrixID = m_program->uniformLocation("M");
    m_lightPosID = m_program->uniformLocation("LightPos");

    m_diffuseColorID = m_program->uniformLocation("diffuseColor");
    m_specularityID = m_program->uniformLocation("specularity");
    m_hardnessID = m_program->uniformLocation("hardness");
    m_alphaID = m_program->uniformLocation("alpha");

    // Create a vertex array object. In OpenGL ES 2.0 and OpenGL 2.x
    // implementations this is optional and support may not be present
    // at all. Nonetheless the below code works in all cases and makes
    // sure there is a VAO when one is needed.
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // Setup our vertex buffer object.
    m_vertexBuffer.create();
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(m_obj.getVertexData().data, m_obj.getVertexData().length);

    m_normalBuffer.create();
    m_normalBuffer.bind();
    m_normalBuffer.allocate(m_obj.getNormalData().data, m_obj.getNormalData().length);

    m_elementBuffer.create();
    m_elementBuffer.bind();
    m_elementBuffer.allocate(m_obj.getElementData().data, m_obj.getElementData().length);

    // Store the vertex attribute bindings for the program.
    setupVertexAttribs();

    // Our camera never changes in this example.
    m_camera.setToIdentity();

    QVector3D eye(8,3,3);
    QVector3D center(0,0,0);
    QVector3D up(0,1,0);

    m_camera.lookAt(eye, center, up);

    m_obj.sortElementsByDistance( glm::vec3(eye.x(), eye.y(), eye.z() ) );

    // Set fixed properties: light pos & material
    m_program->setUniformValue(m_lightPosID, QVector3D(11,6,11));

    m_program->setUniformValue(m_diffuseColorID, QVector3D(1,1,1));
    m_program->setUniformValue(m_specularityID, 0.9f);
    m_program->setUniformValue(m_hardnessID, 7.0f);
    m_program->setUniformValue(m_alphaID, 1.0f);

    m_program->release();

    cerr << "initializeGL ok\n";
}


void GLWidget::updateVertices() {
    m_obj.calculateAllNormals();

    m_vertexBuffer.bind();
    m_vertexBuffer.write(0, m_obj.getVertexData().data, m_obj.getVertexData().length);

    m_normalBuffer.bind();
    m_normalBuffer.write(0, m_obj.getNormalData().data, m_obj.getNormalData().length);

    m_obj.sortElementsByDistance();

    m_elementBuffer.bind();
    m_elementBuffer.write(0, m_obj.getElementData().data, m_obj.getElementData().length);
}


void GLWidget::setupVertexAttribs()
{
    m_vertexBuffer.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(
                0,              //index
                3,              //size
                GL_FLOAT,       //type
                GL_FALSE,       //normalized?
                0,              //stride
                0 );            //array buffer offset

    m_vertexBuffer.release();
    m_normalBuffer.bind();

    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(
                1,              //index
                3,              //size
                GL_FLOAT,       //type
                GL_FALSE,       //normalized?
                0,              //stride
                0 );            //array buffer offset


    m_normalBuffer.release();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    updateVertices();

    m_world.setToIdentity();
    /*m_world.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
    m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
    m_world.rotate(m_zRot / 16.0f, 0, 0, 1);*/

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();

    m_program->setUniformValue(m_MVPMatrixID, m_proj * m_camera * m_world);
    m_program->setUniformValue(m_ViewMatrixID, m_camera);
    m_program->setUniformValue(m_ModelMatrixID, m_world);

    glDrawElements(GL_TRIANGLES, m_obj.elements.size(), GL_UNSIGNED_INT, (void*)0);

    m_program->release();
}

void GLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(m_xRot + 8 * dy);
        setZRotation(m_zRot + 8 * dx);
    }
    m_lastPos = event->pos();
}
