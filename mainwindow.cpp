#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
bool setSize = false;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    designMainWindow();

    subtitleTimer = new QTimer;

    //============== Connects ===================================
    //gets signal from a specific metadata
    connect(player, SIGNAL(metaDataChanged(QString,QVariant)),this, SLOT(metaDataChanged(QString,QVariant)));
    //Connect timer signal
    connect(subtitleTimer,SIGNAL(timeout()),this, SLOT(timerOntimesignal()));
    //==========================================================
    //TESTING ADJUSTMENTS
//    player->setMedia(QUrl::fromLocalFile("/home/igormacedo/Downloads/The.Big.Bang.Theory.S09E03.720p.HDTV.X264-DIMENSION[rarbg]/The.Big.Bang.Theory.S09E03.720p.HDTV.X264-DIMENSION.mkv"));

//    //============================

//    mysubs = getsubtitles("/home/igormacedo/Downloads/The.Big.Bang.Theory.S09E03.720p.HDTV.X264-DIMENSION[rarbg]/The.Big.Bang.Theory.S09E03.720p.HDTV.X264-DIMENSION.srt");


//    int c = 0;

//    for (list<subtitle*>::iterator it=mysubs.begin(); it != mysubs.end(); ++it)
//    {
//        cout << "timebegin: " << (*it)->timerange.timeBegin << endl;
//        cout << "timeend: " << (*it)->timerange.timeEnd << endl;
//        cout << "first word: " << (*it)->firstline[0].at(0).toStdString() << endl;

//        if(c==10)break;
//        c++;
//    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::designMainWindow()
{
    createMenu();

    player = new QMediaPlayer;
    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(playerPositionChanged(qint64)));
    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(playerDurationChanged(qint64)));
    videoWidget = new QVideoWidget;
    graphicsView = new QGraphicsView;
    videoItem = new QGraphicsVideoItem;
    player->setVideoOutput(videoItem);
    scene = new QGraphicsScene;
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setScene(scene);
    graphicsView->scene()->addItem(videoItem);
    graphicsView->setAlignment(Qt::AlignLeft);
    //graphicsView->setGeometry(0,0,1280,720);


    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    playPauseButton = new QPushButton;
    playPauseButton->setText("Play");
    connect(playPauseButton, SIGNAL(clicked(bool)), this, SLOT(playPauseButtonClicked()));

    stopButton = new QPushButton;
    stopButton->setText("Stop");
    connect(stopButton, SIGNAL(clicked(bool)), this, SLOT(stopButtonClicked()));

    progressSlider = new QSlider(Qt::Horizontal);
    progressSlider->setTracking(false);
    //connect(progressSlider, SIGNAL(valueChanged(int)), this, SLOT()

    QHBoxLayout* ButtonsLayout = new QHBoxLayout;
    ButtonsLayout->addWidget(playPauseButton);
    ButtonsLayout->addWidget(stopButton);
    ButtonsLayout->addWidget(progressSlider);

    subtitleLayout = new QVBoxLayout;
    subtitleLayout->setContentsMargins(0,0,0,30);

    videoLayout = new QGridLayout;
    videoLayout->addWidget(graphicsView,0,0);
    videoLayout->addLayout(subtitleLayout,0,0,Qt::AlignBottom);
    //videoLayout->addWidget(videoWidget, 0,0,Qt::AlignCenter);


    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(videoLayout);
    layout->addLayout(ButtonsLayout);

    centralWidget->setLayout(layout);
}

void MainWindow::metaDataChanged(QString key, QVariant value)
{
    if(key == "Resolution")
    {
        QSize resolution = value.toSize();
        cout << resolution.width() << " " << resolution.height() << endl;
        this->setGeometry(100,100,resolution.width()+35,resolution.height()+100);
        videoItem->setSize(QSize(graphicsView->size().width(),graphicsView->size().height()));
        videoHeight = resolution.height();
        videoWidth = resolution.width();
        setSize = true;
    }
}

void MainWindow::playPauseButtonClicked()
{
    if(!filename.isEmpty())
    {
        if(playPauseButton->text() == "Pause")
        {
            player->pause();
            playPauseButton->setText("Play");
            subtitleTimer->stop();
        }
        else
        {
            player->play();
            playPauseButton->setText("Pause");
            subtitleTimer->start(0);
        }
    }
}

void MainWindow::stopButtonClicked()
{
    player->stop();
    playPauseButton->setText("Play");
    progressSlider->setValue(0);
    subtitleTimer->stop();

    //Reload subtitles
    mysubs = getsubtitles(subtitleName.toStdString());

    if ( subtitleLayout != NULL )
    {
        QLayoutItem* layoutItem;
        while((layoutItem = subtitleLayout->takeAt(0)) != NULL)
        {
            QLayoutItem* widgetItem;
            while((widgetItem = layoutItem->layout()->takeAt(0)) != NULL)
            {
                delete widgetItem->widget();
            }
            delete widgetItem;
            delete layoutItem->layout();
        }
        delete layoutItem;
    }
}

void MainWindow::createMenu()
{
    openFile = new QAction(tr("&Open"), this);

    fileMenu = menuBar()->addMenu(tr("&File"));
    connect(openFile, SIGNAL(triggered()), this, SLOT(openFileAction()));
    fileMenu->addAction(openFile);
}

list<subtitle*> MainWindow::getsubtitles(string path)
{
    list<subtitle*> mysubs;
    subtitle* newsub;
    int counter = 1;

    ifstream myReadFile;
    myReadFile.open(&path[0]);
    string line;

    if (myReadFile.is_open())
    {
        cout << "file is open" << endl;
        while (!myReadFile.eof())
        {
            cout << "Reading file" << endl;
            getline(myReadFile,line);
            cout << "line: " << line << " ";
            QString compareStr = QString::fromStdString(line);
            compareStr = compareStr.trimmed();
            cout << "number: " << compareStr.toInt() << endl;
            if(counter == compareStr.toInt())
            {
               cout << "Creating new sub" << endl;
               newsub = new subtitle;
               getline(myReadFile,line);
               timeRange timerange = getMilisecDuration(line);
               newsub->timerange = timerange;

               getline(myReadFile,line);
               newsub->firstline = vector<vector<QString> >();
               QString compareLine = QString::fromStdString(line);

               while((compareLine.compare(QString("\r")) != 0) && (compareLine.compare(QString("\n")) != 0))
               {
                   cout << "creating new line" << endl;
                   newsub->firstline.push_back(getStringList(line));
                   getline(myReadFile,line);
                   compareLine = QString::fromStdString(line);
               }

               newsub->played = false;
               mysubs.push_back(newsub);
               counter++;
            }

        }
   }
   myReadFile.close();

   return mysubs;
}

vector<QString> MainWindow::getStringList(string line)
{
    vector<QString>* stringList = new vector<QString>();

    QString convertedStr = QString::fromStdString(line);
    convertedStr = convertedStr.trimmed();
    QStringList splitList = convertedStr.split(" ",QString::SkipEmptyParts);

    for (int i = 0; i < splitList.size(); ++i)
    {
        cout << "Creating words" << endl;
        stringList->push_back(splitList.at(i));
    }

    return *stringList;
}

timeRange MainWindow::getMilisecDuration(string duration)
{
    QString dur = QString::fromStdString(duration);
    timeRange timerange;
    timerange.timeBegin = 0; timerange.timeEnd = 0;

    QStringList strList  = dur.split(" --> ", QString::SkipEmptyParts);

    QString begin = strList.first();
    QStringList beginList = begin.split(":", QString::SkipEmptyParts);
    QString lastFromBeginList = beginList.last();
    QStringList miliFromBegin = lastFromBeginList.split(",", QString::SkipEmptyParts);
    QString hour = beginList.first();
    QString minutes = beginList.at(1);
    QString secods = miliFromBegin.first();
    QString miliseconds = miliFromBegin.last();
    timerange.timeBegin = hour.toLongLong()*3600*1000;
    timerange.timeBegin += minutes.toLongLong()*60*1000;
    timerange.timeBegin += secods.toLongLong()*1000;
    timerange.timeBegin += miliseconds.toLongLong();

    QString end = strList.last();
    QStringList endList = end.split(":", QString::SkipEmptyParts);
    QString lastFromEndList = endList.last();
    QStringList miliFromEnd = lastFromEndList.split(",", QString::SkipEmptyParts);
    hour = endList.first();
    minutes = endList.at(1);
    secods = miliFromEnd.first();
    miliseconds = miliFromEnd.last();
    timerange.timeEnd = hour.toLongLong()*3600*1000;
    timerange.timeEnd += minutes.toLongLong()*60*1000;
    timerange.timeEnd += secods.toLongLong()*1000;
    timerange.timeEnd += miliseconds.toLongLong();

    return timerange;
}

void MainWindow::playerPositionChanged(qint64 position)
{
    //cout << "progress: " << position;
    progressSlider->setValue(position);
}

void MainWindow::playerDurationChanged(qint64 duration)
{
    progressSlider->setMaximum(duration);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if(setSize)
    {
        videoItem->setSize(QSize(graphicsView->size().width(),graphicsView->size().height()));
        //graphicsView->scene()->sceneRect()
    }

}

void MainWindow::timerOntimesignal()
{
    subtitle* newsub = mysubs.front();

    if((player->position() >= newsub->timerange.timeBegin) && (newsub->played == false))
    {
        cout << "creating subtitle" << endl;
        for(int i = 0; i < (int) newsub->firstline.size(); ++i)
        {
            QHBoxLayout* newline = new QHBoxLayout;
            for(int j = 0; j < (int) newsub->firstline[i].size(); ++j)
            {
                QPushButton* subtitleWord = new QPushButton;
                subtitleWord->setFlat(true);
                subtitleWord->setText(newsub->firstline[i][j]);
                subtitleWord->setAttribute(Qt::WA_TranslucentBackground);
                subtitleWord->setFont(QFont( "Arial", 20, QFont::Bold));
                subtitleWord->setStyleSheet("color: rgb(220, 231, 97); padding: 1px;");
                connect(subtitleWord, SIGNAL(clicked(bool)), this, SLOT(subtitleButtonClicked()));
                newline->addWidget(subtitleWord);
            }
            subtitleLayout->addLayout(newline);
            newline->setAlignment(Qt::AlignCenter);
        }

        newsub->played = true;

    }
    else if(player->position() >= newsub->timerange.timeEnd)
    {
        cout << "time end detected" << endl;
        if ( subtitleLayout != NULL )
        {
            QLayoutItem* layoutItem;
            while((layoutItem = subtitleLayout->takeAt(0)) != NULL)
            {
                QLayoutItem* widgetItem;
                while((widgetItem = layoutItem->layout()->takeAt(0)) != NULL)
                {
                    delete widgetItem->widget();
                }
                delete widgetItem;
                delete layoutItem->layout();
            }
            delete layoutItem;
        }
        mysubs.pop_front();
    }
}

void MainWindow::subtitleButtonClicked()
{
    QPushButton* subtitleButton = (QPushButton*) sender();

    if(subwindow == NULL)
    {
        subwindow = new QMainWindow(this,Qt::WindowCloseButtonHint);
        //subwindow->setAttribute(Qt::WA_DeleteOnClose);
        webview = new QWebView;
        subwindow->layout()->addWidget(webview);
        QString s = subtitleButton->text();
        webview->load(QUrl("http://www.thefreedictionary.com/"+subtitleButton->text()));
        //subwindow->setMinimumHeight(webview->height());
        subwindow->setMinimumSize(webview->size());
        subwindow->show();
    }
    else
    {
        webview->load(QUrl("http://www.thefreedictionary.com/"+subtitleButton->text()));
        //subwindow->setMinimumHeight(webview->height());
        subwindow->setMinimumSize(webview->size());
        subwindow->show();
    }
}

void MainWindow::openFileAction()
{
    filename = QFileDialog::getOpenFileName(this,tr("Open File"));

    player->setMedia(QUrl::fromLocalFile(filename));
    filename.chop(3);
    subtitleName = filename+"srt";

    cout << subtitleName.toStdString() << endl;
    mysubs = getsubtitles(subtitleName.toStdString());

}
