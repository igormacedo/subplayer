#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QDebug>
#include <QtWidgets>
#include <QGraphicsVideoItem>
#include <QTimer>
#include <vector>
#include <fstream>
#include <QBoxLayout>
#include <QMediaMetaData>
#include <QWebView>
#include <iostream>
#include <QFileDialog>

using namespace std;

struct timeRange
{
    qint64 timeBegin;
    qint64 timeEnd;
};

struct subtitle
{
    bool played;
    timeRange timerange;
    vector<vector<QString> > firstline;
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void metaDataChanged(QString,QVariant);
    void playPauseButtonClicked();
    void stopButtonClicked();
    void playerPositionChanged(qint64);
    void playerDurationChanged(qint64);
    void timerOntimesignal();
    void subtitleButtonClicked();
    void openFileAction();

private:
    Ui::MainWindow *ui;
    QMediaPlayer* player;
    QMainWindow* subwindow = NULL;
    QWebView* webview;
    int videoHeight;
    int videoWidth;
    QVideoWidget* videoWidget;
    QGridLayout* videoLayout;
    QVBoxLayout* subtitleLayout;
    QWidget *centralWidget;

    QGraphicsVideoItem* videoItem;
    QGraphicsScene *scene;
    QGraphicsView *graphicsView;
    list<subtitle*> mysubs;

    QMenu *fileMenu;
    QAction *openFile;

    QPushButton* playPauseButton;
    QPushButton* stopButton;
    QSlider* progressSlider;

    QTimer* subtitleTimer;
    //QLabel* formerSub = NULL;

    QString filename;
    QString subtitleName;

    void designMainWindow();
    void createMenu();
    timeRange getMilisecDuration(string duration);
    list<subtitle*> getsubtitles(string path);
    vector<QString> getStringList(string line);

protected:
    void resizeEvent(QResizeEvent* event);
};

#endif // MAINWINDOW_H
