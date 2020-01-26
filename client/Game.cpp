#include "Game.h"

Game::Game()
{
    myField = new Field;
    enemyField = new Field;
    state = ST_PLACING_SHIPS;
}

Game::~Game()
{
    delete myField;
    delete enemyField;
}

Cell Game::getMyCell( int x, int y ) const
{
    return myField->getCell( x, y );
}

void Game::setMyCell( int x, int y, Cell cell )
{
    myField->setCell( x, y, cell );
}

QString Game::getMyField() const
{
    return myField->getField();
}

Cell Game::getEnemyCell( int x, int y ) const
{
    return enemyField->getCell( x, y );
}

void Game::setEnemyCell( int x, int y, Cell cell )
{
    enemyField->setCell( x, y, cell );
}

QString Game::getEnemyField() const
{
    return enemyField->getField();
}

State Game::getState() const
{
    return state;
}

void Game::setState( State st )
{
    state = st;
}

void Game::setLogin( const QString& str )
{
    login = str;
}

void Game::setPassword( const QString& str )
{
    pass = str;
}

QString Game::getLogin() const
{
    return login;
}

QString Game::getPassword() const
{
    return pass;
}

bool Game::checkMyField() const
{
    // Check field for correct ship placement
    return (
        shipNum(1) == 4 &&
        shipNum(2) == 3 &&
        shipNum(3) == 2 &&
        shipNum(4) == 1
    );
}

int Game::shipNum( int size ) const
{
    int shipNumber = 0;

    for( int i = 0; i < 10; i++ )
        for( int j = 0; j < 10; j++ )
            if( isShip(size, i, j) )
                shipNumber++;

    return shipNumber;
}

bool Game::isShip( int size, int x, int y ) const {
    if( x > 0 && myField->getCell(x - 1, y) != CLEAR )  return false;
    if( y > 0 && myField->getCell(x, y - 1) != CLEAR )  return false;
    if( myField->getCell(x, y) == CLEAR ) return false;

    int tmp = x;
    int num = 0;

    while( myField->getCell(tmp, y) != CLEAR && tmp < 10 )
    {
        tmp++;
        num++;
    }

    if( num == size )
    {
        if( myField->getCell(x, y + 1) != CLEAR )return false;

        return true;
    }

    tmp = y;
    num = 0;

    // checking in down direction
    while( myField->getCell(x, tmp) != CLEAR && tmp < 10 ) {
        tmp++;
        num++;
    }

    if( num == size ) {
        if( myField->getCell(x + 1, y) != CLEAR ) return false;

        return true;
    }

    return false;
}

void Game::clearEnemyField()
{
    enemyField->clear();
}

void Game::clearMyField()
{
    myField->clear();
}
