#include <QDebug>
#include <QString>
#include <qiterator.h>
#include "Field.h"

const quint16 MAX_SHIP_SIZE = 10;

Field::Field( int shipSize ):
    shipSize_( shipSize ),
    fieldLength_( 0 )
{
    if( shipSize > MAX_SHIP_SIZE )
        return;

    for( int i = 1; i <= shipSize; i++ )
        fieldLength_ += i;

    field_.fill( CI_CLEAR, getFieldSize() );
    killedShips_.fill( 0, shipSize_ );
}

void Field::convertField()
{
    Cell cell;
    bool bLeft, bRight, bTop, bBottom;
    for( int i = 0; i < fieldLength_; i++ )
        for( int j = 0; j < fieldLength_; j++ )
        {
            cell = getCell( i, j );
            if( cell == CI_CLEAR || cell > CI_CENTER )
                continue;

            bLeft = getCell( i - 1, j ) != CI_CLEAR;
            bRight = getCell( i + 1, j ) != CI_CLEAR;
            bTop = getCell( i, j - 1 ) != CI_CLEAR;
            bBottom = getCell( i, j + 1 ) != CI_CLEAR;

            cell = bLeft && bRight
                ? CI_HMIDDLE : !bLeft && !bRight
                ? CI_CENTER : bLeft
                ? CI_RIGHT : CI_LEFT;

            if( cell == CI_CENTER )
                cell = bTop && bBottom
                    ? CI_VMIDDLE : !bTop && !bBottom
                    ? CI_CENTER : bTop
                    ? CI_BOTTOM : CI_TOP;

            setCell( i, j, cell );
        }
}

void Field::initField( const QString& stringField )
{
    field_.clear();

    int fieldSize = getFieldSize();
    int sSize = qMin( stringField.length(), fieldSize );
    int nSize = qMax( stringField.length(), fieldSize ) - sSize;

    Cell value;
    for(
        QString::const_iterator it = stringField.constBegin();
        it < stringField.constEnd();
        it++
    )
    {
        value = ( Cell ) QString( *it ).toInt();
        if( value >= CellShipsTypeCount )
            value = CI_CLEAR;
        field_.push_back( value );
    }

    for( int i = 0; i < nSize; i++ )
        field_.push_back( CI_CLEAR );

    convertField();
}

void Field::showField() const
{
    QString str;
    for( int y = 0; y < fieldLength_; y++ )
    {
        str = "";
        for( int x = 0; x < fieldLength_; x++ )
        {
            str += QString( "%1" ).arg( getCell(x, y) );
        }

        qDebug() << str;
    }
}

Field::Cell Field::getCell( int x, int y ) const
{
    return getCellPrivate( x, y, field_ );
}

void Field::setCell( int x, int y, Cell cell )
{
    setCellPrivate( x, y, cell, field_ );
}

Field::Cell Field::getCellPrivate(
    int x,
    int y,
    const Field::Cells& cells
) const
{
    int n = y * fieldLength_ + x;

    if( x < 0 || y < 0 || x >= getFieldLength() || y >= getFieldLength() )
        return CI_CLEAR;

    if( n < 0 || (quint32) n > getFieldSize() )
        return CI_CLEAR;

    return cells[n];
}

void Field::setCellPrivate(
    int x,
    int y,
    Cell cell,
    Field::Cells& cells
) const
{
    int n = y * fieldLength_ + x;

    if( x < 0 || y < 0 || x >= getFieldLength() || y >= getFieldLength() )
        return;

    if( n < 0 || (quint32) n > getFieldSize() )
        return;

    cells[n] = cell;
}

bool Field::checkField() const
{
    if( field_.size() < getFieldSize() || field_.size() == 0 )
        return false;

    Cells field( field_ );

    QVector<int> shipsCount;
    shipsCount.fill( 0, shipSize_ );

    Cell cell;
    int n;
    int k;
    bool b;
    for( int y = 0; y < fieldLength_; y++ )
        for( int x = 0; x < fieldLength_; x++ )
        {
            cell = getCellPrivate( x, y, field );

            if( cell == CI_CENTER )
            {
                shipsCount[0]++;
                setCellPrivate( x, y, CI_CLEAR, field );
                continue;
            }

            if( cell == CI_TOP || cell == CI_LEFT )
            {
                n = 1;
                k = 0;
                b = cell == CI_LEFT;

                do
                {
                    cell = getCellPrivate(
                        x + b*(1 + k),
                        y + !b*(1 + k),
                        field
                    );
                    setCellPrivate(
                         x + b*(1 + k),
                         y + !b*(1 + k),
                         CI_CLEAR,
                         field
                    );

                    if(
                        (cell == CI_VMIDDLE && !b) ||
                        (cell == CI_HMIDDLE && b)
                    )
                    {
                        k++;
                        continue;
                    }

                    if(
                        (cell == CI_BOTTOM && !b) ||
                        (cell == CI_RIGHT && b)
                    )
                    {
                        n++;
                        break;
                    }
                } while( cell != CI_CLEAR );

                n += k;

                if( n < 2 || n > shipSize_ )
                    return false;

                shipsCount[n - 1]++;
                setCellPrivate( x, y, CI_CLEAR, field );
                continue;
            }
        }

    for( int i = 1, shipSize = shipSize_; i <= shipSize_; i++, shipSize-- )
        for( int j = 0; j < i; j++ )
            if( shipsCount[shipSize - 1] != i )
                return false;

    return true;
}

int Field::getShipSize() const
{
    return shipSize_;
}

int Field::getFieldLength() const
{
    return fieldLength_;
}

int Field::getFieldSize() const
{
    return fieldLength_ * fieldLength_;
}

bool Field::isAllKilled() const
{
    int killedCount = 0;
    for( int i = 0; i < shipSize_; i++ )
        killedCount += killedShips_[i];

    return killedCount >= getShipsCount();
}

int Field::getShipsCount() const
{
    return shipSize_ * (shipSize_ + 1) / 2;
}

void Field::addKilledShip( int shipSize )
{
    if( shipSize < 1 || shipSize > shipSize_ )
        return;

    killedShips_[ shipSize - 1 ]++;
}


bool Field::makeShot( int x, int y, Shots& killShots )
{
    killShots.clear();
    Cell cell = getCell( x, y );
    if( cell == CI_CLEAR || cell >= CellShipsTypeCount )
        return false;

    if( cell == CI_CENTER )
    {
        killShots.push_back( QPoint(x, y) );
        addKilledShip( CI_CENTER );
        return true;
    }

    bool vertical = cell == CI_VMIDDLE || cell == CI_TOP || cell == CI_BOTTOM;
    int shipSize = 0;
    int k = 1;
    int p, q;

    do
    {
        p = x + !vertical * k;
        q = y + vertical * k;
        cell = getCell( p, q );
        if(
            cell == CI_CLEAR ||
            (cell >= CellShipsTypeCount && cell != CI_DAMAGED)
        )
            break;

        if( cell == CI_DAMAGED )
            killShots.push_back( QPoint(p, q) );

        shipSize++;
        k++;
    }
    while( cell != CI_CLEAR || cell < CellShipsTypeCount );

    k = 1;

    do
    {
        p = x - !vertical * k;
        q = y - vertical * k;
        cell = getCell( p, q );
        if(
            cell == CI_CLEAR ||
            (cell >= CellShipsTypeCount && cell != CI_DAMAGED)
        )
            break;

        if( cell == CI_DAMAGED )
            killShots.push_back( QPoint(p, q) );

        k++;
        shipSize++;
    }
    while( cell != CI_CLEAR || cell < CellShipsTypeCount );

    shipSize++;
    killShots.push_back( QPoint(x, y) );

    if( shipSize != killShots.size() )
        return false;

    addKilledShip( shipSize );
    return true;
}
