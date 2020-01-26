#pragma once
#include <QVector>
#include <QDebug>
#include <QString>
#include <QPoint>

enum Cell
{
    CLEAR = 0,
    SHIP,
    DOT,
    HALF
};

class Field
{
public:
    Field();
    ~Field();
    Cell getCell( int x, int y );
    void setCell( int x, int y, Cell cell );
    QString getField();
    void clear();
    QPoint getMyFieldCoord( const QPoint& pos );
    QPoint getEnemyFieldCoord( const QPoint& pos );

    const int leftFieldX = 230;
    const int leftFieldY = 210;
    const int rightFieldX = 567;
    const int rightFieldY = 210;
    const int fieldWidth = 221;
    const int fieldHeight = 218;
private:
    QVector<Cell> field;
};
