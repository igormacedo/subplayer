/** @file mainwindow.h
@author Macedo:Igor
@version Revision 1.1
@brief Este é o Subplayer. Um player de video para linux baseado em Qt que
funciona com os codecs que você já tenha instalado no seu computador e te ajudar a aprender linguas estrangeiras.
@details Com o Subplayer, você pode tocar seus vídeos legendados e clicar em cada palavra da legenda
para ter o significado tirado diretamente de um dicionário online.
@date 6 de Dezembro de 2015
*/

/**
@mainpage
Este é o Subplayer. Um player de video baseado em Qt que
funciona com os codecs que você já tenha instalado no seu computador
*/

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

/**
 * @brief Esta timeRange struct auxilia na definição de intervalos para cada legenda encontrada no arquivo.
 */
struct timeRange
{
    qint64 timeBegin; /**< Tempo de inicio da legenda*/
    qint64 timeEnd; /**< Tempo final da legenda*/
};

/**
 * @brief Esta subtitle struct é utilizada na contrução de uma estrutura de dados para representar as legendas.
 */

struct subtitle
{
    bool played; /**< Indica se a legenda já foi tocada ou nao*/
    timeRange timerange; /**< Tempo de inicio e final da legenda*/
    vector<vector<QString> > firstline; /**< Armazena as Strings que compoem a legenda*/
};


namespace Ui {
class MainWindow;
}

/**
 * @brief A MainWindow class e a classe principal do programa, responsável por tocar o vídeo e organizar as legendas.
 */

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


/// ESSAS FUNCǑES E OBJETOS DEVERIAM SER PRIVATE, POREM PARA FINS DE DOCUMENTACAO SERAO
/// CONSIDERADOS PROTECTED.
protected:
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
    //! Lista de legendas
    list<subtitle*> mysubs;

    QMenu *fileMenu;
    QAction *openFile;

    QPushButton* playPauseButton;
    QPushButton* stopButton;
    QSlider* progressSlider;
    //! Temporizador das legendas
    QTimer* subtitleTimer;
    //QLabel* formerSub = NULL;

    QString filename;
    QString subtitleName;

    //! Cria os componentes da tela principal
    /*!
    Essa funcao criar os componentes que compoem a tela principal
    @return void
    */
    void designMainWindow();
    //! Cria o menu principal
    /*!
    Essa funcao cria o menu principal do programa
    @return void
    */
    void createMenu();
    //! Retorna duração em milisegundos
    /*!
    Essa funçao calcula a duraçao em milisegundos de um a string do formato hh:mm:ss,sss->hh:mm:ss,sss
    @return timeRange
    @param duration string contendo a duraçao.
    @pre
    -# duration no formato hh:mm:ss,sss->hh:mm:ss,sss.
    */
    timeRange getMilisecDuration(string duration);
    //! Cria lista que representa a legenda
    /*!
    Essa funçao cria a lista que será utilizada para exibir as legendas no video
    @return list<subtitle*>
    @param path caminho para o arquivo da legenda
    */
    list<subtitle*> getsubtitles(string path);
    //! Retorna a lista de strings de uma linha da legenda
    /*!
    essa função recebe uma string line que contém as palavras e retorna uma lista com cada string
    que representa as palavras da legenda.
    @return list<subtitle*>
    @param line linha de cada legenda
    */
    vector<QString> getStringList(string line);

    //! Redimensiona a tela de video
    /*!
    Essa funçao redimensiona a tela de video quando o usuário redimensionar a MainWindow
    @return void
    @param event evento de chamada
    */
    void resizeEvent(QResizeEvent* event);
};

#endif // MAINWINDOW_H
