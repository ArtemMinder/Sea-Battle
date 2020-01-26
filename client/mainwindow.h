#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPainter>
#include <QPoint>
#include <QMouseEvent>
#include <QDebug>
#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QTime>
#include "Textures.h"
#include "Game.h"
#include "Logic.h"
#include "Field.h"


namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( QWidget* parent = 0 );
    ~MainWindow();

protected:
    void paintEvent( QPaintEvent* event );
    void mousePressEvent( QMouseEvent* ev );
    void closeEvent( QCloseEvent* event );

private slots:
    void redraw();
    void showGameResult( GameResult result );
    void showGameError( GameErrorMessage message );
    void changeGameOpponent( const QString& name );

    void on_actionLeave_activated();

    void on_connectButton_clicked();

    void on_randomButton_clicked();

    void on_clearButton_clicked();

    void on_exitButton_clicked();

private:
    void setStatus( const QString& status );

private:
    QImage myFieldImage();
    QImage enemyFieldImage();
    QImage getFieldImage( char );

private:
    Ui::MainWindow* ui;
    Textures* pictures;
    State state;
    Field field;
    Game* game;
    Logic* logic;
};

#endif // MAINWINDOW_H
