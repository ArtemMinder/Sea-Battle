#include "Field.h"

Field::Field(){
    clear();
}

Field::~Field(){
}

Cell Field::getCell( int x, int y ){
    int n = y * 10 + x;
    if( x < 0 || y < 0 || x >= 10 || y >= 10 )
        return CLEAR;
    if( n >= 0 && n < field.size() )
        return field[n];
    return CLEAR;
}

void Field::setCell( int x, int y, Cell cell ){
    int n = y * 10 + x;
    if( x >= 0 && y >= 0 && x < 10 && y < 10 &&
        n >= 0 && n < field.size() ) {
        field[n] = cell;
        return;
    }}

QString Field::getField(){
   QString result;
    for( QVector<Cell>::iterator i = field.begin(); i != field.end(); i++ )
        if( *i == CLEAR ){ result += "0";}
        else {result += "1";}
    return result;
}

void Field::clear()
{
    field.fill( CLEAR, 100 );
}

QPoint Field::getMyFieldCoord( const QPoint& pos ){
    QPoint res;
    res.setX( -1 );
    res.setY( -1 );

    if(  pos.x() < leftFieldX || pos.x() > (leftFieldX + fieldWidth) ||
        pos.y() < leftFieldY || pos.y() > (leftFieldY + fieldHeight)){ return res;}

    res.setX( 1.0 * (pos.x() - leftFieldX) / (0.1 * fieldWidth) );

    qDebug() << "X: " << pos.x() - leftFieldX;
    qDebug() << "Y: " << pos.y() - leftFieldY;

    res.setY( 1.0 * (pos.y() - leftFieldY) / (0.1 * fieldHeight) );
    return res;
}

QPoint Field::getEnemyFieldCoord( const QPoint& pos ){
    QPoint res;
    res.setX( -1 );
    res.setY( -1 );
    if(  pos.x() < rightFieldX || pos.x() > (rightFieldX + fieldWidth) ||
      pos.y() < rightFieldY || pos.y() > (rightFieldY + fieldHeight)){
        return res;}
    res.setX( 1.0 * (pos.x() - rightFieldX) / (0.1 * fieldWidth) );
    res.setY( 1.0 * (pos.y() - rightFieldY) / (0.1 * fieldHeight) );
    return res;
}

