#include "Textures.h"

void Textures::load(){
    images.insert( "field", QImage(":/textures/background.png") );
    images.insert( "dot", QImage(":/textures/shot.png") );
    images.insert( "full", QImage(":/textures/ship.png") );
    images.insert( "half", QImage(":/textures/damaged.png") );
    images.insert( "redhalf", QImage(":/textures/killed.png") );
    images.insert( "redfull", QImage(":/textures/killed.png") );
    images.insert( "about", QImage(":/about.png") );
}

QImage& Textures::get( const QString& imgName ){
    QMap<QString, QImage>::iterator i = images.find( imgName );
    if( i == images.end() ){
        throw 1;}
    return i.value();
}
